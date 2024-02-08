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
			ClearLyrics();
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
		else if (phase == LS_SECONDARY_WORD_CLASS) {
			ProcessSecondaryWordClass();
		}
		else if (phase == LS_SECONDARY_FILTER) {
			ProcessSecondaryFilter();
		}
		else if (phase == LS_SECONDARY) {
			ProcessSecondary();
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

void LyricsSolver::ClearLyrics() {
	for(int i = 0; i < song->parts.GetCount(); i++) {
		StaticPart& sp = song->parts[i];
		auto& lines = sp.nana.Get();
		for(int j = 0; j < lines.GetCount(); j++) {
			auto& line = lines[j];
			line.pp_i = -1;
			line.end_pp_i = -1;
		}
		sp.phrase_parts.Clear();
	}
	song->picked_phrase_parts.Clear();
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
	Song& song = *this->song;
	
	#if !PRIMARY_STATIC_PART
	if ((skip_ready && song.picked_phrase_parts.GetCount())) {
		NextBatch();
		return;
	}
	
	Vector<int> prob_phrase;
	prob_phrase.Reserve(1000000);
	for(int i = 0; i < song.parts.GetCount(); i++) {
		const StaticPart& sp = song.parts[i];
		
		for(int i = 0; i < sp.phrase_parts.GetCount(); i++) {
			int pp_i = sp.phrase_parts[i];
			int slots = 1 + (sp.phrase_parts.GetCount()-1-i)*10 / sp.phrase_parts.GetCount();
			for(int j = 0; j < slots; j++)
				prob_phrase << pp_i;
		}
	}
	
	int primary_count = min(this->primary_count, prob_phrase.GetCount());
	Index<int> primary;
	while (true) {
		int j = Random(prob_phrase.GetCount());
		int pp_i = prob_phrase[j];
		primary.FindAdd(pp_i);
		if (primary.GetCount() >= primary_count)
			break;
	}
	
	song.picked_phrase_parts <<= primary.GetKeys();
	
	SetWaiting(1);
	SongLib::TaskManager& tm = SongLib::TaskManager::Single();
	tm.DoNana(ds_i, 0, song, THISBACK(OnProcessPrimary), -1, -1);
	#else
	if (batch >= song->parts.GetCount()) {
		NextPhase();
		return;
	}
	
	StaticPart& sp = song->parts[batch];
	if (sp.part_type == StaticPart::SKIP ||
		sp.phrase_parts.GetCount() == 0 ||
		(skip_ready && sp.picked_phrase_parts.GetCount())) {
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
	
	int primary_count = min(this->primary_count, prob_phrase.GetCount());
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
	tm.DoNana(ds_i, 0, song, THISBACK(OnProcessPrimary), -1, batch);
	#endif
}

void LyricsSolver::OnProcessPrimary() {
	SetWaiting(0);
	
	#if !PRIMARY_STATIC_PART
	NextPhase();
	#else
	NextBatch();
	#endif
}

void LyricsSolver::ProcessSecondaryWordClass() {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	LyricsSolverArgs args;
	args.fn = 2;
	
	for(int i = 0; i < song->parts.GetCount(); i++) {
		const StaticPart& sp = song->parts[i];
		const auto& lines = sp.nana.Get();
		for(int j = 0; j < lines.GetCount(); j++) {
			const auto& line = lines[j];
			if (line.pp_i >= 0) {
				const PhrasePart& pp = da.phrase_parts[line.pp_i];
				args.parts << da.GetWordString(pp.words);
			}
		}
	}
	
	int wc_count = 30;
	for(int i = 0; i < da.word_classes.GetCount() && args.attrs.GetCount() < wc_count; i++) {
		const auto& wc = da.word_classes[i];
		if (!wc.IsEmpty())
			args.attrs << wc;
	}
	
	SetWaiting(1);
	RealizePipe();
	TaskMgr& m = *pipe;
	m.GetLyricsSolver(args, THISBACK(OnProcessSecondaryWordClass));
}

void LyricsSolver::OnProcessSecondaryWordClass(String res) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	res = "2. " + res;
	
	VectorMap<int, Index<int>> results;
	
	RemoveEmptyLines3(res);
	Vector<String> lines = Split(res, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		
		int a = line.Find(".");
		if (a < 0) continue;
		int idx = ScanInt(line.Left(a)) - 2;
		
		line = line.Mid(a+1);
		Vector<String> parts = Split(line, ",");
		
		Index<int>& idx_results = results.GetAdd(idx);
		for (String& wc : parts) {
			wc = TrimBoth(wc);
			int wc_i = da.word_classes.Find(wc);
			if (wc_i < 0)
				continue;
			idx_results.FindAdd(wc_i);
			
			int a = wc.Find(" phrase");
			if (a >= 0) {
				wc = wc.Left(a);
				int wc_i = da.word_classes.Find(wc);
				if (wc_i < 0)
					continue;
				idx_results.FindAdd(wc_i);
			}
		}
	}
	
	int idx = 0;
	for(int i = 0; i < song->parts.GetCount(); i++) {
		StaticPart& sp = song->parts[i];
		auto& lines = sp.nana.Get();
		for(int j = 0; j < lines.GetCount(); j++) {
			auto& line = lines[j];
			if (line.pp_i >= 0) {
				const PhrasePart& pp = da.phrase_parts[line.pp_i];
				int k = results.Find(idx);
				if (k >= 0) {
					line.sub_next_wc_i <<= results[k].GetKeys();
				}
				else {
					line.sub_next_wc_i.Clear();
				}
				
				idx++;
			}
		}
	}
	
	SetWaiting(0);
	NextPhase();
}

void LyricsSolver::ProcessSecondaryFilter() {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	
	Vector<Tuple2<int,int>> part_lines;
	for(int i = 0; i < song->parts.GetCount(); i++) {
		const StaticPart& sp = song->parts[i];
		const auto& l = sp.nana.Get();
		for(int j = 0; j < l.GetCount(); j++)
			part_lines.Add(Tuple2<int,int>(i, j));
	}
	
	if (batch >= part_lines.GetCount()) {
		NextPhase();
		return;
	}
	
	int part_i = part_lines[batch].a;
	int line_i = part_lines[batch].b;
	StaticPart& sp = song->parts[part_i];
	auto& line = sp.nana.Get()[line_i];
	
	if (/*(skip_ready && line.sub_pp_i.GetCount()) ||*/
		sp.part_type == StaticPart::SKIP) {
		NextBatch();
		return;
	}
	
	bool check_rhyming = (line_i % 2) == 1;
	const ExportWord* cmp_ew = 0;
	if (check_rhyming) {
		auto& prev_line = sp.nana.Get()[line_i - 1];
		if (prev_line.end_pp_i >= 0) {
			const PhrasePart& pp = da.phrase_parts[prev_line.end_pp_i];
			if (pp.words.GetCount())
				cmp_ew = &da.words[pp.words.Top()];
		}
	}
	
	Vector<int> compatible_phrase_parts;
	for (int tries = 0; tries < 3; tries++) {
		double try_dist_limit = this->dist_limit + tries * 0.005;
		
		compatible_phrase_parts.SetCount(0);
		compatible_phrase_parts.Reserve(sp.phrase_parts.GetCount());
		for(int i = 0; i < sp.phrase_parts.GetCount(); i++) {
			int pp_i = sp.phrase_parts[i];
			const PhrasePart& pp = da.phrase_parts[pp_i];
			if (pp.words.IsEmpty()) continue;
			
			// Check that phrase's first word matches possible continuations
			if (line.sub_next_wc_i.GetCount()) {
				int first_word = pp.words[0];
				const ExportWord& ew = da.words[first_word];
				bool found = false;
				for(int j = 0; j < ew.class_count; j++) {
					int wc0 = ew.classes[j];
					for (int wc1 : line.sub_next_wc_i) {
						if (wc0 == wc1) {
							found = true;
							break;
						}
					}
					if (found) break;
				}
				if (!found) continue;
			}
			
			// Check rhyming
			if (cmp_ew) {
				int last_word = pp.words.Top();
				const ExportWord& ew = da.words[last_word];
				if (&ew == cmp_ew) // skip same word
					continue;
				int len = min(5, min(cmp_ew->phonetic.GetCount(), ew.phonetic.GetCount()));
				WString a = cmp_ew->phonetic.Right(len);
				WString b = ew.phonetic.Right(len);
				double dist = GetSpellingRelativeDistance(a, b);
				if (dist > try_dist_limit)
					continue;
			}
			
			compatible_phrase_parts.Add(pp_i);
		}
		
		if (compatible_phrase_parts.GetCount() >= rhyming_list_count)
			break;
	}
	
	if (compatible_phrase_parts.GetCount() < primary_count) {
		line.sub_pp_i <<= compatible_phrase_parts;
	}
	else {
		Vector<int> prob_phrase;
		prob_phrase.Reserve(compatible_phrase_parts.GetCount() * 10);
		
		for(int i = 0; i < compatible_phrase_parts.GetCount(); i++) {
			int pp_i = compatible_phrase_parts[i];
			int slots = 1 + (compatible_phrase_parts.GetCount()-1-i)*10 / compatible_phrase_parts.GetCount();
			for(int j = 0; j < slots; j++)
				prob_phrase << pp_i;
		}
		
		Index<int> primary;
		while (true) {
			int j = Random(prob_phrase.GetCount());
			int pp_i = prob_phrase[j];
			primary.FindAdd(pp_i);
			if (primary.GetCount() >= primary_count)
				break;
		}
	
		line.sub_pp_i <<= primary.GetKeys();
	}
	
	
	NextBatch();
}

void LyricsSolver::ProcessSecondary() {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	Vector<Tuple2<int,int>> part_lines;
	for(int i = 0; i < song->parts.GetCount(); i++) {
		const StaticPart& sp = song->parts[i];
		const auto& l = sp.nana.Get();
		for(int j = 0; j < l.GetCount(); j++)
			part_lines.Add(Tuple2<int,int>(i, j));
	}
	
	if (batch >= part_lines.GetCount()) {
		NextPhase();
		return;
	}
	
	
	
	const auto& part_line = part_lines[batch];
	int part = part_line.a;
	int line = part_line.b;
	
	SetWaiting(1);
	SongLib::TaskManager& tm = SongLib::TaskManager::Single();
	tm.DoNana(ds_i, 1, *song, THISBACK(OnProcessSecondary), line, part);
}

void LyricsSolver::OnProcessSecondary() {
	SetWaiting(0);
	NextBatch();
}

}