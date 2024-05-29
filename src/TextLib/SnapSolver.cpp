#include "TextLib.h"

BEGIN_TEXTLIB_NAMESPACE


SnapSolver::SnapSolver() {
	
}

int SnapSolver::GetPhaseCount() const {
	return SS_COUNT;
}

void SnapSolver::DoPhase() {
	ASSERT(snap);
	
	if (phase == SS_LYRICS_SUMMARIES) {
		if (batch >= snap->components.GetCount()) {
			NextPhase();
			return;
		}
		
		Component& comp = snap->components[batch];
		int script_i = snap->entity->FindScript(comp.scripts_file_title);
		if (script_i < 0) {
			NextBatch();
			LOG("warning: no script found for " << comp.scripts_file_title);
			return;
		}
		
		Script& script = snap->entity->scripts[script_i];
		
		SnapshotArgs args;
		args.text = script.text;
		args.entity = snap->entity->profile->name;
		args.title = script.native_title;
		args.fn = 0;
		
		SetWaiting(1);
		TaskMgr& m = TaskMgr::Single();
		m.GetSnapshot(args, [this](String res) {
			if (batch >= snap->song_analysis.GetCount())
				snap->song_analysis.SetCount(batch+1);
			String& s = snap->song_analysis[batch];
			s = TrimBoth(res);
			RemoveQuotes(s);
			SetWaiting(0);
			NextBatch();
		});
	}
	else if (phase == SS_LYRICS_SUMMARY ||
			 phase == SS_LYRICS_PSYCHOANALYSIS ||
			 phase == SS_LYRICS_SOCIAL_PSYCHOLOGY_ANALYSIS ||
			 phase == SS_MARKET_VALUE_ANALYSIS ||
			 phase == SS_MARKETING_SUGGESTION) {
		SnapshotArgs args;
		args.fn = 1 + phase - SS_LYRICS_SUMMARY;
		args.entity = snap->entity->profile->name;
		args.title = snap->native_title;
		for(int i = 0; i < snap->components.GetCount() && i < snap->song_analysis.GetCount(); i++) {
			Component& c = snap->components[i];
			int script_i = snap->entity->FindScript(c.scripts_file_title);
			if (script_i < 0) continue;
			Script& script = snap->entity->scripts[script_i];
			args.songs.Add(script.native_title, snap->song_analysis[i]);
		}
		
		SetWaiting(1);
		TaskMgr& m = TaskMgr::Single();
		m.GetSnapshot(args, [this](String res) {
			int mode = phase - SS_LYRICS_SUMMARY;
			if (mode >= snap->analysis.GetCount()) snap->analysis.SetCount(mode+1);
			String& s = snap->analysis[mode];
			s = TrimBoth(res);
			RemoveQuotes(s);
			SetWaiting(0);
			NextPhase();
		});
	}
	else if (phase == SS_ART_SUGGESTION) {
		SnapshotArgs args;
		args.fn = 6;
		args.entity = snap->entity->profile->name;
		args.title = snap->native_title;
		args.text = snap->analysis[SNAPANAL_MARKETING_SUGGESTION];
		
		SetWaiting(1);
		TaskMgr& m = TaskMgr::Single();
		m.GetSnapshot(args, [this](String res) {
			res = "- " + res;
			int mode = SNAPANAL_ART_SUGGESTION;
			if (mode >= snap->analysis.GetCount()) snap->analysis.SetCount(mode+1);
			String& s = snap->analysis[mode];
			s = TrimBoth(res);
			RemoveQuotes(s);
			SetWaiting(0);
			NextPhase();
		});
	}
	else if (phase == SS_COVER_SUGGESTION) {
		SnapshotArgs args;
		args.fn = 7;
		args.entity = snap->entity->profile->name;
		args.title = snap->native_title;
		args.text = snap->analysis[SNAPANAL_MARKETING_SUGGESTION];
		
		SetWaiting(1);
		TaskMgr& m = TaskMgr::Single();
		m.GetSnapshot(args, [this](String res) {
			res = "1. " + res;
			RemoveEmptyLines(res);
			int mode = SNAPANAL_COVER_SUGGESTION;
			if (mode >= snap->analysis.GetCount()) snap->analysis.SetCount(mode+1);
			String& s = snap->analysis[mode];
			s = TrimBoth(res);
			RemoveQuotes(s);
			
			
			RemoveEmptyLines2(res);
			Vector<String> lines = Split(res, "\n");
			for (String& l : lines) {
				int i = VectorFind(snap->cover_suggestions, l);
				if (i >= 0) continue;
				snap->cover_suggestions << l;
			}
			
			SetWaiting(0);
			NextPhase();
		});
	}
	else if (phase == SS_COVER_SUGGESTION_DALLE2) {
		if (batch >= snap->cover_suggestions.GetCount()) {
			NextPhase();
			return;
		}
		
		String prompt = snap->cover_suggestions[batch];
		
		String title = IntStr64(prompt.GetHashValue());
		String img_dir = AppendFileName(MetaDatabase::GetUserDirectory(), "images" DIR_SEPS "full");
		RealizeDirectory(img_dir);
		String path = AppendFileName(img_dir, title + "_" + IntStr(0) + ".jpg");
		
		if (skip_ready && FileExists(path)) {
			NextBatch();
			return;
		}
		
		SetWaiting(1);
		TaskMgr& m = TaskMgr::Single();
		m.CreateImage(prompt, 4, [this, prompt, title, img_dir](Array<Image>& img) {
			for(int i = 0; i < img.GetCount(); i++) {
				String path = AppendFileName(img_dir, title + "_" + IntStr(i) + ".jpg");
				JPGEncoder enc(100);
				enc.SaveFile(path, img[i]);
			}
			SetWaiting(0);
			NextBatch();
		});
	}
	else TODO
	
}

ArrayMap<hash_t, SnapSolver>& __SnapSolvers() {
	static ArrayMap<hash_t, SnapSolver> map;
	return map;
}

SnapSolver& SnapSolver::Get(Snapshot& e) {
	String t = e.entity->profile->owner->name + ": " + e.entity->file_title + ": " + e.file_title;
	hash_t h = t.GetHashValue();
	ArrayMap<hash_t, SnapSolver>& map = __SnapSolvers();
	int i = map.Find(h);
	if (i >= 0)
		return map[i];
	
	SnapSolver& ls = map.Add(h);
	ASSERT(e.entity);
	ls.entity = e.entity;
	ls.snap = &e;
	return ls;
}


END_TEXTLIB_NAMESPACE
