#include "SongLib.h"


namespace SongLib {


LyricsSolver::LyricsSolver() {
	
}

LyricsSolver& LyricsSolver::Get(Artist& a, Release& r, Song& s) {
	String t = a.english_name + " - " + r.english_title + " - " + s.english_title;
	hash_t h = t.GetHashValue();
	static ArrayMap<hash_t, LyricsSolver> map;
	int i = map.Find(h);
	if (i >= 0)
		return map[i];
	
	LyricsSolver& ls = map.Add(h);
	ls.song = &s;
	ls.release = &r;
	ls.artist = &a;
	return ls;
}

void LyricsSolver::RealizePipe() {
	Database& db = Database::Single();
	if (!pipe) {
		TaskManager::Single().RealizePipe();
		pipe = TaskManager::Single().GetPipe();
		ASSERT(pipe);
	}
}

void LyricsSolver::Process() {
	
	while (running && !Thread::IsShutdownThreads()) {
		if (waiting) {
			Sleep(10);
			continue;
		}
		
		if (phase == LS_BEGIN) {
			time_started = GetSysTime();
			//skip_ready = false;
			NextPhase();
		}
		else if (phase == LS_COLOR) {
			ProcessColor();
		}
		else if (phase == LS_ATTR) {
			ProcessAttr();
		}
		else if (phase == LS_ACTION) {
			ProcessAction();
		}
		else if (phase == LS_FILTER) {
			ProcessFilter();
		}
		else if (phase == LS_PRIMARY) {
			ProcessPrimary();
		}
		
		
		else /*if (phase == LS_COUNT)*/ {
			time_stopped = GetSysTime();
			phase = LS_BEGIN;
			break;
		}
		
		
		PostProgress();
		Sleep(1);
	}
	
	running = false;
	stopped = true;
}

void LyricsSolver::ProcessColor() {
	if (batch >= song->parts.GetCount()) {
		NextPhase();
		return;
	}
	
	StaticPart& sp = song->parts[batch];
	if (sp.part_type == StaticPart::SKIP ||
		sp.name.IsEmpty() ||
		(skip_ready && sp.clr_list.GetCount())) {
		NextBatch();
		return;
	}
	
	
	LyricsSolverArgs args;
	args.fn = 0;
	
	// Artist information
	args.artist.Add("year of birth", IntStr(artist->year_of_birth));
	args.artist.Add("year of beginning of career", IntStr(artist->year_of_career_begin));
	args.artist.Add("biography", artist->biography);
	args.artist.Add("musical style", artist->musical_style);
	args.artist.Add("vocalist visually", artist->vocalist_visual);
	
	// Release information
	args.release.Add("title of release", release->english_title);
	args.release.Add("year of content", IntStr(release->year_of_content));
	
	// Song information
	args.song.Add("title of song", song->english_title);
	args.song.Add("artist's content vision", song->data.Get("ATTR_CONTENT_VISION", ""));
	
	// Parts
	for(int i = 0; i < song->parts.GetCount(); i++)
		args.parts << song->parts[i].name;
	args.part = sp.name; // active part
	
	SetWaiting(1);
	RealizePipe();
	TaskMgr& m = *pipe;
	m.GetLyricsSolver(args, THISBACK(OnProcessColor));
}

void LyricsSolver::OnProcessColor(String result) {
	//LOG(result);
	StaticPart& sp = song->parts[batch];
	
	result = "- RGB(" + result;
	
	Index<int> no_clr_list;
	RemoveEmptyLines3(result);
	Vector<String> lines = Split(result, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		
		int a = line.Find("RGB(");
		if (a < 0) continue;
		a += 4;
		int b = line.Find(")");
		String clr_str = line.Mid(a,b-a);
		Vector<String> clr_parts = Split(clr_str, ",");
		if (clr_parts.GetCount() != 3) continue;
		int R = StrInt(TrimBoth(clr_parts[0]));
		int G = StrInt(TrimBoth(clr_parts[1]));
		int B = StrInt(TrimBoth(clr_parts[2]));
		Color clr(R,G,B);
		
		int clr_group = GetColorGroup(clr);
		no_clr_list.FindAdd(clr_group);
	}
	
	sp.clr_list.Clear();
	int c = GetColorGroupCount();
	for(int i = 0; i < c; i++)
		if (no_clr_list.Find(i) < 0)
			sp.clr_list.Add(i);
	
	NextBatch();
	SetWaiting(0);
}

void LyricsSolver::ProcessAttr() {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	
	LyricsSolverArgs args;
	args.fn = 1;
	
	// Artist information
	args.artist.Add("year of birth", IntStr(artist->year_of_birth));
	args.artist.Add("year of beginning of career", IntStr(artist->year_of_career_begin));
	args.artist.Add("biography", artist->biography);
	args.artist.Add("musical style", artist->musical_style);
	args.artist.Add("vocalist visually", artist->vocalist_visual);
	
	// Release information
	args.release.Add("title of release", release->english_title);
	args.release.Add("year of content", IntStr(release->year_of_content));
	
	// Song information
	args.song.Add("title of song", song->english_title);
	args.song.Add("artist's content vision", song->data.Get("ATTR_CONTENT_VISION", ""));
	
	// Parts
	for(int i = 0; i < song->parts.GetCount(); i++)
		args.parts << song->parts[i].name;
	
	
	per_batch = 50;
	int begin = batch * per_batch;
	int end = begin + per_batch;
	end = min(end, da.simple_attrs.GetCount());
	
	if (skip_ready && end < song->simple_attrs.GetCount()) {
		NextBatch();
		return;
	}
	if (skip_ready && end == song->simple_attrs.GetCount()) {
		NextPhase();
		return;
	}
	
	
	for(int i = begin; i < end; i++) {
		const ExportSimpleAttr& t = da.simple_attrs[i];
		String group = da.simple_attrs.GetKey(i);
		String attr_s0 = da.attrs.GetKey(t.attr_i0).value;
		String attr_s1 = da.attrs.GetKey(t.attr_i1).value;
		args.attrs << (group + ": +" + attr_s0 + "/-" + attr_s1);
	}
	batch_count = da.simple_attrs.GetCount() / per_batch;
	
	if (args.attrs.IsEmpty()) {
		NextPhase();
		return;
	}
	
	SetWaiting(1);
	RealizePipe();
	TaskMgr& m = *pipe;
	m.GetLyricsSolver(args, THISBACK(OnProcessAttr));
}

void LyricsSolver::OnProcessAttr(String result) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	int begin = batch * per_batch;
	int end = begin + per_batch;
	end = min(end, da.simple_attrs.GetCount());
	
	if (end > song->simple_attrs.GetCount())
		song->simple_attrs.SetCount(end, 0);
	
	RemoveEmptyLines3(result);
	Vector<String> lines = Split(result, "\n");
	int c = min(lines.GetCount(), per_batch);
	for(int i = 0; i < c; i++) {
		String& line = lines[i];
		int pos = begin + i;
		
		int p = line.Find("+");
		int n = line.Find("-");
		int s = line.Find("/");
		
		bool positive = true;
		if (p >= 0 && n >= 0)
			positive = p < n;
		else if (s >= 0 && p >= 0)
			positive = p < s;
		else if (s >= 0 && n >= 0)
			positive = s < n;
		else if (n >= 0)
			positive = false;
		bool negative = !positive;
		
		song->simple_attrs[pos] = negative;
	}
	
	NextBatch();
	SetWaiting(0);
}

void LyricsSolver::ProcessAction() {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	if (batch >= song->parts.GetCount()) {
		NextPhase();
		return;
	}
	
	StaticPart& sp = song->parts[batch];
	if ((skip_ready && sp.actions_enabled.GetCount() == da.actions.GetCount()) ||
		sp.part_type == StaticPart::SKIP) {
		NextBatch();
		return;
	}
	
	sp.actions_enabled.SetCount(da.actions.GetCount(), false);
	
	Color no_clr(0,0,0);
	for(int i = 0; i < da.actions.GetCount(); i++) {
		const ExportAction& ea = da.actions[i];
		bool& enabled = sp.actions_enabled[i];
		
		// Filter by attribute
		if (ea.attr >= 0) {
			const ExportAttr* eat = &da.attrs[ea.attr];
			while (eat->link >= 0) {
				eat = &da.attrs[eat->link];
			}
			
			if (eat->simple_attr >= 0) {
				const ExportSimpleAttr& esa = da.simple_attrs[eat->simple_attr];
				bool song_positive = song->simple_attrs[eat->simple_attr];
				bool attr_positive = eat->positive;
				enabled = song_positive == attr_positive;
				continue;
			}
		}
		// Filter by color
		if (ea.clr != no_clr) {
			int clr_group = GetColorGroup(ea.clr);
			enabled = VectorFind(sp.clr_list, clr_group) >= 0;
			continue;
		}
		
		enabled = false;
	}
	
	NextBatch();
}

void LyricsSolver::ProcessFilter() {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	if (batch >= song->parts.GetCount()) {
		NextPhase();
		return;
	}
	
	StaticPart& sp = song->parts[batch];
	sp.phrase_parts.Clear();
	if ((skip_ready && sp.phrase_parts.GetCount() == da.phrase_parts.GetCount()) ||
		sp.part_type == StaticPart::SKIP) {
		NextBatch();
		return;
	}
	
	Color no_clr(0,0,0);
	
	sp.phrase_parts.Reserve(da.phrase_parts.GetCount());
	
	VectorMap<int,int> phrase_parts;
	for(int i = 0; i < da.phrase_parts.GetCount(); i++) {
		const PhrasePart& pp = da.phrase_parts[i];
		
		// Check attr
		if (pp.attr >= 0) {
			const ExportAttr* ea = &da.attrs[pp.attr];
			if (ea->link >= 0)
				ea = &da.attrs[ea->link];
			if (ea->simple_attr >= 0) {
				bool song_enabled = song->simple_attrs[ea->simple_attr];
				if (!song_enabled)
					continue;
			}
			else continue;
		}
		
		
		// Check clr
		if (pp.clr != no_clr) {
			int clr_group = GetColorGroup(pp.clr);
			bool part_enabled = VectorFind(sp.clr_list, clr_group) >= 0;
			if (!part_enabled)
				continue;
		}
		else continue;
		
		
		// Phrase is enabled
		phrase_parts.Add(i);
	}
	
	
	double perc = 100 * phrase_parts.GetCount() / da.phrase_parts.GetCount();
	LOG("Part " << sp.name << ": " << phrase_parts.GetCount() << " parts (" << perc << "\%)");
	
	
	// Sort based on score
	for(int i = 0; i < phrase_parts.GetCount(); i++) {
		int pp_i = phrase_parts.GetKey(i);
		const PhrasePart& pp = da.phrase_parts[pp_i];
		
		int score_sum = 0;
		for(int j = 0; j < SCORE_COUNT; j++)
			score_sum += pp.scores[j];
		
		phrase_parts[i] = score_sum;
	}
	
	struct Sorter {
		bool operator()(int a, int b) const {
			return a < b;
		}
	};
	SortByValue(phrase_parts, Sorter());
	
	
	sp.phrase_parts <<= phrase_parts.GetKeys();
	
	
	NextBatch();
}

void LyricsSolver::ProcessPrimary() {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	if (batch >= song->parts.GetCount()) {
		NextPhase();
		return;
	}
	
	StaticPart& sp = song->parts[batch];
	if (sp.part_type == StaticPart::SKIP) {
		NextBatch();
		return;
	}
	
	
	Vector<int> prob_phrase;
	prob_phrase.Reserve(sp.phrase_parts.GetCount() * 10);
	
	for(int i = 0; i < sp.phrase_parts.GetCount(); i++) {
		int pp_i = sp.phrase_parts[i];
		int slots = 1 + (sp.phrase_parts.GetCount()-1-i)*10 / sp.phrase_parts.GetCount();
		for(int j = 0; j < slots; j++)
			prob_phrase << pp_i;
	}
	
	int primary_count = min(50, prob_phrase.GetCount());
	Index<int> primary;
	while (true) {
		int j = Random(prob_phrase.GetCount());
		int pp_i = prob_phrase[j];
		primary.FindAdd(pp_i);
		if (primary.GetCount() >= primary_count)
			break;
	}
	
	sp.picked_phrase_parts <<= primary.GetKeys();
	
	SetWaiting(1);
	SongLib::TaskManager& tm = SongLib::TaskManager::Single();
	tm.DoNana(ds_i, 0, *song, THISBACK(OnProcessPrimary), -1, batch);
}

void LyricsSolver::OnProcessPrimary() {
	
	SetWaiting(0);
	NextBatch();
}

}
