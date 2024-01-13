#include "SongLib.h"

namespace SongLib {

void TaskManager::GetTokenData(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[t->ds_i];
	
	TokenArgs& args = token_args;
	args.fn = 0;
	args.words.Clear();
	
	int per_action_task = 100;
	int begin = t->batch_i * per_action_task;
	int end = begin + per_action_task;
	end = min(end, da.tokens.GetCount());
	int count = end - begin;
	if (count <= 0) {
		RemoveTask(*t);
		return;
	}
	
	for(int i = begin; i < end; i++) {
		const String& tk = da.tokens.GetKey(i);
		args.words << tk;
	}
	
	RealizePipe();
	TaskMgr& m = *pipe;
	if (t->fn == 0)
		m.GetTokenData(args, THISBACK1(OnTokenData, t));
	
}

void TaskManager::GetAmbiguousWordPairs(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[t->ds_i];
	
	TokenArgs& args = token_args;
	args.fn = 1;
	args.words.Clear();
	
	int per_action_task = 100;
	int begin = t->batch_i * per_action_task;
	int end = begin + per_action_task;
	end = min(end, da.ambiguous_word_pairs.GetCount());
	int count = end - begin;
	if (count <= 0) {
		RemoveTask(*t);
		return;
	}
	
	for(int i = begin; i < end; i++) {
		const auto& wp = da.ambiguous_word_pairs[i];
		if (wp.from >= 0 && wp.to >= 0) {
			const String& from = da.words.GetKey(wp.from);
			const String& to = da.words.GetKey(wp.to);
			args.words << (from + " " + to);
		}
	}
	
	RealizePipe();
	TaskMgr& m = *pipe;
	if (t->fn == 1)
		m.GetTokenData(args, THISBACK1(OnAmbiguousWordPairs, t));
	
}

void TaskManager::GetVirtualPhrases(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[t->ds_i];
	
	if (t->fn == 0) {
		Vector<int> word_is, word_classes;
		for(int i = 0; i < da.token_texts.GetCount(); i++) {
			TokenText& txt = da.token_texts[i];
			
			bool succ = true;
			word_is.SetCount(0);
			word_classes.SetCount(0);
			for(int tk_i : txt.tokens) {
				const Token& tk = da.tokens[tk_i];
				int w_i = tk.word_;
				if (w_i < 0) {
					String key = ToLower(da.tokens.GetKey(tk_i));
					w_i = da.words.Find(key);
					tk.word_ = w_i;
				}
				word_is << w_i;
			}
			
			int prev_w_i = -1;
			for(int j = 0; j < word_is.GetCount(); j++) {
				int w_i = word_is[j];
				int next_w_i = j+1 < word_is.GetCount() ? word_is[j] : -1;
				succ = succ && GetTypePhrase(word_classes, da, next_w_i, w_i, prev_w_i);
				prev_w_i = w_i;
			}
			
			if (word_classes.IsEmpty())
				succ = false;
			
			if (succ) {
				CombineHash ch;
				for (int wc_i : word_classes)
					ch.Do(wc_i);
				hash_t h = ch;
				
				int vp_i = -1;
				VirtualPhrase& vp = da.virtual_phrases.GetAdd(h, vp_i);
				Swap(word_classes, vp.word_classes);
				
				txt.virtual_phrase = vp_i;
			}
		}
		
		//int punctuation_mark_i = da.word_classes.FindAdd("punctuation mark");
		//int punctuation_i = da.word_classes.FindAdd("punctuation");
		for(int i = 0; i < da.virtual_phrase_parts.GetCount(); i++)
			da.virtual_phrase_parts[i].count = 0;
		
		for(int i = 0; i < da.virtual_phrases.GetCount(); i++) {
			VirtualPhrase& vp = da.virtual_phrases[i];
			Vector<Vector<int>> tmps;
			Vector<int> tmp;
			
			// NOTE: see duplicate in fn 3
			
			for (int wc_i : vp.word_classes) {
				String wc = da.word_classes[wc_i];
				int a = wc.Find("punctuation");
				int b = wc.Find("conjunction");
				//if (type == punctuation_mark_i || type == punctuation_i) {
				if (a >= 0 || b >= 0) {
					if (tmp.GetCount()) {
						Swap(tmps.Add(), tmp);
						tmp.SetCount(0);
					}
					if (b >= 0)
						tmp << wc_i;
				}
				else
					tmp << wc_i;
			}
			if (tmp.GetCount()) {
				Swap(tmps.Add(), tmp);
				tmp.SetCount(0);
			}
			CombineHash struct_ch;
			Vector<int> vpp_is;
			for (const Vector<int>& tmp : tmps) {
				CombineHash ch;
				for (int type : tmp)
					ch.Do(type).Put(1);
				hash_t h = ch;
				
				int vpp_i = -1;
				VirtualPhrasePart& vpp = da.virtual_phrase_parts.GetAdd(h, vpp_i);
				if (vpp.word_classes.IsEmpty())
					vpp.word_classes <<= tmp;
				vpp.count++;
				vpp_is << vpp_i;
				struct_ch.Do(vpp_i).Put(1);
			}
			hash_t vps_h = struct_ch;
			int vps_i = -1;
			VirtualPhraseStruct& vps = da.virtual_phrase_structs.GetAdd(vps_h, vps_i);
			//if (vps.parts.IsEmpty())
				vps.virtual_phrase_parts <<= vpp_is;
			vps.count = 0;
			vp.virtual_phrase_struct = vps_i;
		}
		LOG(da.virtual_phrase_parts.GetCount());
		LOG(da.virtual_phrase_parts.GetCount() * 100.0 / da.virtual_phrases.GetCount());
		RemoveTask(*t);
	}
	else if (t->fn == 1) {
		TokenArgs& args = token_args;
		args.fn = 2;
		args.words.Clear();
		
		int per_action_task = 75;
		int begin = t->batch_i * per_action_task;
		int end = begin + per_action_task;
		end = min(end, da.virtual_phrase_parts.GetCount());
		int count = end - begin;
		if (count <= 0) {
			RemoveTask(*t);
			return;
		}
		
		for(int i = begin; i < end; i++) {
			const VirtualPhrasePart& vpp = da.virtual_phrase_parts[i];
			String s;
			int punct_count = 0;
			bool fail = false;
			for(int j = 0; j < vpp.word_classes.GetCount(); j++) {
				if (j) s << ",";
				int wc_i = vpp.word_classes[j];
				if (wc_i >= da.word_classes.GetCount()) {fail = true; break;}
				String wc = da.word_classes[wc_i];
				
				int a = wc.Find("(");
				if (a >= 0) wc = wc.Left(a);
				a = wc.Find(",");
				if (a >= 0) wc = wc.Left(a);
				
				if (wc.Find("punctuation") >= 0)
					punct_count++;
				
				s << wc;
			}
			if (punct_count > 8 || fail)
				args.words << "error";
			else
				args.words << s;
		}
		
		RealizePipe();
		TaskMgr& m = *pipe;
		m.GetTokenData(args, THISBACK1(OnVirtualPhrases, t));
	}
	else if (t->fn == 2) {
		TokenArgs& args = token_args;
		args.fn = 3;
		args.words.Clear();
		
		int per_action_task = 75;
		int begin = t->batch_i * per_action_task;
		int end = begin + per_action_task;
		end = min(end, da.virtual_phrase_structs.GetCount());
		int count = end - begin;
		if (count <= 0) {
			RemoveTask(*t);
			return;
		}
		
		t->tmp.Clear();
		for(int i = begin; i < end; i++) {
			const VirtualPhraseStruct& vps = da.virtual_phrase_structs[i];
			String s;
			bool fail = false;
			for(int j = 0; j < vps.virtual_phrase_parts.GetCount(); j++) {
				if (j) s << " + ";
				int vpp_i = vps.virtual_phrase_parts[j];
				
				const VirtualPhrasePart& vpp = da.virtual_phrase_parts[vpp_i];
				if (vpp.struct_part_type < 0) {
					fail = false;
					continue;
				}
				
				String type_str = da.struct_part_types[vpp.struct_part_type];
				s << type_str;
			}
			if (fail)
				args.words << "error";
			else
				args.words << s;
			t->tmp << i;
		}
		
		RealizePipe();
		TaskMgr& m = *pipe;
		m.GetTokenData(args, THISBACK1(OnVirtualPhraseTypes, t));
	}
	else if (t->fn == 3) {
		// NOTE: see duplicate in fn 0
		// TODO reduce duplicate code
		Vector<int> word_is, word_classes;
		
		for(int i = 0; i < da.token_texts.GetCount(); i++) {
			const TokenText& txt = da.token_texts[i];
			if (txt.virtual_phrase < 0)
				continue;
			
			// NOTE: see duplicate in fn 0
			bool succ = true;
			word_is.SetCount(0);
			word_classes.SetCount(0);
			for(int tk_i : txt.tokens) {
				const Token& tk = da.tokens[tk_i];
				int w_i = tk.word_;
				if (w_i < 0) {
					String key = ToLower(da.tokens.GetKey(tk_i));
					w_i = da.words.Find(key);
					tk.word_ = w_i;
				}
				word_is << w_i;
			}
			
			const VirtualPhrase& vp = da.virtual_phrases[txt.virtual_phrase];
			if (word_is.GetCount() != vp.word_classes.GetCount())
				continue;
			
			
			
			Vector<Vector<int>> w_isv, wc_isv;
			Vector<int> w_is, wc_is;
			
			// NOTE: see duplicate in fn 0
			int c = word_is.GetCount();
			for(int j = 0; j < vp.word_classes.GetCount(); j++) {
				int w_i = word_is[j];
				int wc_i = vp.word_classes[j];
				
				String wc = da.word_classes[wc_i];
				int a = wc.Find("punctuation");
				int b = wc.Find("conjunction");
				//if (type == punctuation_mark_i || type == punctuation_i) {
				if (a >= 0 || b >= 0) {
					if (w_is.GetCount()) {
						Swap(w_isv.Add(), w_is);
						Swap(wc_isv.Add(), wc_is);
						w_is.SetCount(0);
						wc_is.SetCount(0);
					}
					if (b >= 0) {
						wc_is << wc_i;
						w_is << w_i; // NOTE: this is NOT duplicate
					}
				}
				else {
					wc_is << wc_i;
					w_is << w_i;
				}
			}
			if (w_is.GetCount()) {
				Swap(wc_isv.Add(), wc_is);
				Swap(w_isv.Add(), w_is);
				wc_is.SetCount(0);
				w_is.SetCount(0);
			}
			
			
			for(int j = 0; j < w_isv.GetCount(); j++) {
				const Vector<int>& wc_is = wc_isv[j];
				const Vector<int>& w_is = w_isv[j];
				
				hash_t wc_h, w_h;
				{
					CombineHash ch;
					for (int wc_i : wc_is)
						ch.Do(wc_i).Put(1);
					wc_h = ch;
				}
				{
					CombineHash ch;
					for (int w_i : w_is)
						ch.Do(w_i).Put(1);
					w_h = ch;
				}
				
				int pp_i = -1;
				PhrasePart& pp = da.phrase_parts.GetAdd(w_h, pp_i);
				pp.words <<= w_is;
				pp.tt_i = i;
				pp.virtual_phrase_part = da.virtual_phrase_parts.Find(wc_h);
				
			}
		}
		
		
	}
}

void TaskManager::GetPhrases(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[t->ds_i];
	
	PhraseArgs& args = phrase_args;
	args.fn = t->fn;
	args.phrases.Clear();
	
	int per_action_task = 100;
	if (args.fn == 2 || args.fn == 3)
		per_action_task = 35;
	
	int begin = t->batch_i * per_action_task;
	int end = begin + per_action_task;
	end = min(end, da.ambiguous_word_pairs.GetCount());
	int count = end - begin;
	if (count <= 0) {
		if (t->fn < 3) {
			t->fn++;
			t->batch_i = 0;
			t->running = false;
		}
		else RemoveTask(*t);
		return;
	}
	
	t->tmp.SetCount(0);
	for(int i = begin; i < end; i++) {
		const PhrasePart& pp = da.phrase_parts[i];
		String phrase = da.GetWordString(pp.words);
		args.phrases << phrase;
		t->tmp << i;
	}
	
	RealizePipe();
	TaskMgr& m = *pipe;
	
	if (args.fn == 0)
		m.GetPhraseData(args, THISBACK1(OnPhraseColors, t));
	else if (args.fn == 1)
		m.GetPhraseData(args, THISBACK1(OnPhraseAttrs, t));
	else if (args.fn == 2)
		m.GetPhraseData(args, THISBACK1(OnPhraseActions, t));
	else if (args.fn == 3)
		m.GetPhraseData(args, THISBACK1(OnPhraseScores, t));
	
}

void TaskManager::GetActionlist(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[t->ds_i];
	
	ActionAnalysisArgs args;
	args.fn = t->fn;
	
	int per_action_task = 0;
	if (t->fn == 0)	per_action_task = per_action_clrs;
	if (t->fn == 1)	per_action_task = per_action_attrs;
	
	int begin = t->batch_i * per_action_task;
	int end = begin + per_action_task;
	
	int iter = 0;
	for(int i = 0; i < uniq_acts.GetCount(); i++) {
		const VectorMap<String,int>& idx = uniq_acts[i];
		for(int j = 0; j < idx.GetCount(); j++) {
			if (iter >= begin && iter < end) {
				String s = uniq_acts.GetKey(i) + "(" + idx.GetKey(j) + ")";
				args.actions << s;
			}
			iter++;
		}
	}
	if (args.actions.IsEmpty()) {
		RemoveTask(*t);
		return; // ready
	}
	
	RealizePipe();
	TaskMgr& m = *pipe;
	if (t->fn == 0)
		m.GetActionAnalysis(args, THISBACK1(OnActionlistColors, t));
	else if (t->fn == 1)
		m.GetActionAnalysis(args, THISBACK1(OnActionlistAttrs, t));
}

void TaskManager::GetActionParallels(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[t->ds_i];
	
	TimeStop ts;
	
	// Loop from previous to 'current' to get score at the 'current'
	// (Instead of 'current' to next)
	
	for(const ExportDepActionPhrase& prev : da.action_phrases.GetValues()) {
		for(int k = 0; k < prev.next_phrases.GetCount(); k++) {
			int next_i = prev.next_phrases[k];
			const ExportDepActionPhrase& ap = da.action_phrases[next_i];
			
			for(int i = 0; i < ap.actions.GetCount(); i++) {
				int aa0_i = ap.actions[i];
				const ActionHeader& aa0 = da.actions.GetKey(aa0_i);
				for(int j = 0; j < ap.actions.GetCount(); j++) {
					if (i == j) continue;
					
					int aa1_i = ap.actions[j];
					const ActionHeader& aa1 = da.actions.GetKey(aa1_i);
					
					// we have a transition
					ActionHeader ah0, ah1;
					ah0.action = aa0.action;
					ah0.arg = aa0.arg;
					ah1.action = aa1.action;
					ah1.arg = aa1.arg;
					
					int ap_i = -1;
					ExportParallel& ap = da.parallel.GetAdd(aa0_i, aa1_i, ap_i);
					ap.count++; // increase count
					
					if (k < prev.next_scores.GetCount()) {
						const Vector<int>& sc = prev.next_scores[k];
						int total_score = 0;
						for(int j = 0; j < sc.GetCount(); j++) {
							int v = sc[j];
							total_score += v;
						}
						ap.score_sum += max(0, min(
							(int)SCORE_COUNT,
							total_score));
					}
				}
			}
		}
	}
	
	LOG("TaskManager::GetActionParallels took " << ts.ToString());
}

void TaskManager::GetActionTransitions(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[t->ds_i];
	
	TimeStop ts;
	
	for(const ExportDepActionPhrase& ap : da.action_phrases.GetValues()) {
		int ap_i0 = 0;
		for(int next_i : ap.next_phrases) {
			if (ap_i0 < ap.next_scores.GetCount()) {
				const ExportDepActionPhrase& next = da.action_phrases[next_i];
				
				for (const auto& aa0 : ap.actions) {
					for (const auto& aa1 : next.actions) {
						if (aa0 == aa1) {
							// we have a transition
							#if 0
							const ActionHeader& ah0 = da.actions.GetKey(aa0);
							const ActionHeader& ah1 = da.actions.GetKey(aa1);
							#endif
							
							ExportTransition& at = da.trans.GetAdd(aa0, aa1);
							at.count++; // increase count
							
							const auto& sc = ap.next_scores[ap_i0];
							int total_score = 0;
							for(int v : sc)
								total_score += v;
							at.score_sum += max(0, min(
								(int)SCORE_COUNT,
								total_score));
						}
					}
				}
			}
			ap_i0++;
		}
	}
	
	LOG("TaskManager::GetActionTransitions took " << ts.ToString());
}

void TaskManager::GetContainer(Task* t) {
	
	if (t->fn == 0) {
		GetRhymeContainers(t);
	}
	else if (t->fn == 1) {
		MakeNana(t);
	}
	
}

void TaskManager::MakeNana(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	EnglishPronounciation ep;
	
	int ds_i = 0;
	DatasetAnalysis& da = sda.datasets[ds_i];
	
	PhoneticNanaAnalyser anal;
	
	int total = 0;
	for(int i = 0; i < da.packed_rhymes.GetCount(); i++)
		total += da.packed_rhymes[i].GetCount();
	
	int actual = 0;
	for(int i = 0; i < da.packed_rhymes.GetCount(); i++) {
		const PackedRhymeHeader& h = da.packed_rhymes.GetKey(i);
		Vector<PackedRhymeContainer>& v = da.packed_rhymes[i];
		
		for(int j = 0; j < v.GetCount(); j++) {
			PackedRhymeContainer& c = v[j];
			c.ZeroNana();
			
			if (!anal.Parse(c.pron, PackedRhymeContainer::MAX_PRON_LEN))
				continue;
			
			int len = anal.WritePackedNana(c.nana, PackedRhymeContainer::MAX_NANA_LEN);
			c.nana_len = len;
			
			actual++;
			
			if (actual % 1000 == 0)
				t->update(actual, total);
		}
	}
	
	t->update(0, 1);
}

void TaskManager::GetRhymeContainers(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	EnglishPronounciation ep;
	
	int ds_i = 0;
	int pp_i = 0;
	
	
	Vector<int> comb;
	Vector<Index<String>> words;
	Vector<String> parts;
	int phrase_count = 0;
	
	while (running) {
		
		if (ds_i >= sda.datasets.GetCount())
			break;
		DatasetAnalysis& da = sda.datasets[ds_i];
		if (pp_i == 0) {
			da.packed_rhymes.Clear();
		}
		if (pp_i >= da.phrase_parts.GetCount()) {
			SortByKey(da.packed_rhymes, PackedRhymeHeader());
			pp_i = 0;
			ds_i++;
			continue;
		}
		
		const PhrasePart& pp = da.phrase_parts[pp_i];
		if (pp.words.IsEmpty() || pp.attr < 0) {
			pp_i++;
			continue;
		}
		
		int act_count = min(PackedRhymeContainer::MAX_ACTIONS, pp.actions.GetCount());
		int acts[PackedRhymeContainer::MAX_ACTIONS];
		for(int i = 0; i < act_count; i++)
			acts[i] = pp.actions[i];
		
		// Make sentences from templates
		String phrase = da.GetWordString(pp.words);
		
		if (ep.Parse(phrase, da)) {
			//const Color& clr = ep.GetColor();
			Color clr = pp.clr;
			
			PackedRhymeHeader prh;
			prh.syllable_count = ep.GetSyllableCount();
			prh.color_group = GetColorGroup(clr);
			prh.attr = pp.attr;
			
			bool sort = da.packed_rhymes.Find(prh) < 0;
			
			if (phrase.GetCount() <= PackedRhymeContainer::MAX_TXT_LEN) {
				//LOG(phrase_count << ": " << phrase);
				PackedRhymeContainer& pcr = da.packed_rhymes.GetAdd(prh).Add();
				pcr.Zero();
				
				strncpy(pcr.txt, phrase.Begin(), PackedRhymeContainer::MAX_TXT_LEN);
				
				const WString& pron = ep.GetPronounciation();
				int pron_len = pron.GetCount();
				int pron_sz = sizeof(wchar) * pron_len;
				memcpy(pcr.pron, pron.Begin(), PackedRhymeContainer::MAX_PRON_SZ);
				
				pcr.clr[0] = clr.GetR();
				pcr.clr[1] = clr.GetG();
				pcr.clr[2] = clr.GetB();
				
				for(int i = 0; i < act_count; i++) {
					pcr.actions[i] = acts[i];
				}
				
				phrase_count++;
			}
		}
		
		//Sleep(1);
		pp_i++;
		
		if ((pp_i % 100) == 0)
			t->update(pp_i, da.phrase_parts.GetCount());
	}
	
	for(int i = 0; i < sda.datasets.GetCount(); i++)
		SortByKey(sda.datasets[i].packed_rhymes, PackedRhymeHeader());
	
	t->update(0,1);
	RemoveTask(*t);
}

void TaskManager::GetRhymeContainersFromTemplates(Task* t) {
	TODO
	#if 0
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	EnglishPronounciation ep;
	
	int ds_i = 0;
	int at_i = 0;
	
	
	Vector<int> comb;
	Vector<Index<String>> words;
	Vector<String> parts;
	int phrase_count = 0;
	
	while (running) {
		
		if (ds_i >= sda.datasets.GetCount())
			break;
		DatasetAnalysis& da = sda.datasets[ds_i];
		if (at_i == 0) {
			da.packed_rhymes.Clear();
			da.dynamic_actions.Clear();
			da.dynamic_attrs.Clear();
		}
		if (at_i >= da.action_tmpls.GetCount()) {
			SortByKey(da.packed_rhymes, PackedRhymeHeader());
			at_i = 0;
			ds_i++;
			continue;
		}
		
		const ActionTemplate& at = da.action_tmpls[at_i];
		const int tmpl_i = at.tp_i;
		
		int act_count = min(PackedRhymeContainer::MAX_ACTIONS, at.actions.GetCount());
		int16 act_groups[PackedRhymeContainer::MAX_ACTIONS];
		int16 act_values[PackedRhymeContainer::MAX_ACTIONS];
		for(int i = 0; i < act_count; i++) {
			const ActionHeader& aa = at.actions[i];
			da.RealizeAction(aa.action, aa.arg, act_groups[i], act_values[i]);
		}
		
		// Make sentences from templates
		TemplatePhrase& tp = da.tmpl_phrases[tmpl_i];
		if (tp.group.IsEmpty() || tp.value.IsEmpty()) {
			at_i++;
			continue;
		}
		int attr_group, attr_value;
		if (da.RealizeAttribute(tp.group, tp.value, attr_group, attr_value)) {
			tp.group = Attr::AttrKeys[attr_group][1]; // backport fix
		}
		
		if (tp.words.GetCount()) {
			words.Clear();
			for (const auto& wv : tp.words) {
				auto& wvd = words.Add();
				for (const auto& s : wv)
					wvd.Add(s);
			}
			
			parts <<= tp.parts;
			MakeSplit(parts, words);
			
			// Make extra sentences using wordnets
			for (Index<String>& part_words : words) {
				int wc = part_words.GetCount();
				for(int i = 0; i < wc; i++) {
					String w = part_words[i];
					String lw = ToLower(w);
					for (const ColorWordnet& wa : da.clr_wordnets) {
						if (wa.src_word == w || wa.src_word == lw) {
							for (const String& s : wa.words)
								part_words.FindAdd(s);
							break;
						}
					}
					int j = mirror_words.Find(lw);
					if (j >= 0)
						part_words.FindAdd(mirror_words[j]);
				}
			}
			
			// Get combination count
			int comb_count = 1;
			for (const auto& v : words)
				comb_count *= v.GetCount();
			int limit = min(8*1024, comb_count);
			
			// Reset combination vector
			comb.SetCount(words.GetCount());
			for (auto& v : comb) v = 0;
			
			for (int iter = 0; iter < limit; iter++) {
				bool roll_over = false;
				for (int i = comb.GetCount()-1; i >= 0; i--) {
					int& j = comb[i];
					j++;
					if (j < words[i].GetCount())
						break;
					else {
						j = 0;
						roll_over = i == 0;
					}
				}
				if (roll_over)
					break;
				
				ep.Clear();
				
				String phrase;
				ep.SetBaseColor(tp.clr); // modify color based on words (which has color value too)
				
				int word_i = 0;
				bool fail = false;
				for (const auto& part : parts) {
					if (part[0] == '{') {
						if (word_i >= comb.GetCount()) {
							fail = true;
							break;
						}
						int comb_i = comb[word_i];
						const auto& v = words[word_i];
						if (comb_i >= v.GetCount()) {
							fail = true;
							break;
						}
						const String& wrd = v[comb_i];
						if (!phrase.IsEmpty())
							phrase.Cat('\n');
						phrase << wrd;
						if (!ep.ParseMore(wrd, da)) {
							fail = true;
							break;
						}
						word_i++;
					}
					else {
						//if (part.GetCount() == 1 && (part[0] == ',' || part[1] == '.' || part[1] == '!' || part[1] == '?' || part[1] == ':'))
						//	;
						//else
						if (!phrase.IsEmpty())
							phrase.Cat('\n');
						phrase << part;
						if (!ep.ParseMore(part, da)) {
							fail = true;
							break;
						}
					}
				}
				
				if (!fail && !ep.ParseFinish(da))
					fail = true;
				
				if (!fail) {
					const Color& clr = ep.GetColor();
					
					PackedRhymeHeader prh;
					prh.syllable_count = ep.GetSyllableCount();
					prh.color_group = GetColorGroup(clr);
					prh.attr_group = attr_group;
					prh.attr_value = attr_value;
					
					bool sort = da.packed_rhymes.Find(prh) < 0;
					
					ASSERT(phrase.GetCount() <= PackedRhymeContainer::MAX_TXT_LEN);
					//LOG(phrase_count << ": " << phrase);
					PackedRhymeContainer& pcr = da.packed_rhymes.GetAdd(prh).Add();
					pcr.Zero();
					
					strncpy(pcr.txt, phrase.Begin(), PackedRhymeContainer::MAX_TXT_LEN);
					
					const WString& pron = ep.GetPronounciation();
					int pron_len = pron.GetCount();
					int pron_sz = sizeof(wchar) * pron_len;
					memcpy(pcr.pron, pron.Begin(), PackedRhymeContainer::MAX_PRON_SZ);
					
					pcr.clr[0] = clr.GetR();
					pcr.clr[1] = clr.GetG();
					pcr.clr[2] = clr.GetB();
					
					for(int i = 0; i < act_count; i++) {
						pcr.action_groups[i] = act_groups[i];
						pcr.action_values[i] = act_values[i];
					}
					
					if (sort)
						SortByKey(da.packed_rhymes, PackedRhymeHeader());
					
					phrase_count++;
				}
			}
		}
		
		//Sleep(1);
		at_i++;
		
		if ((at_i % 100) == 0)
			PostProgress(at_i, da.action_tmpls.GetCount());
	}
	
	PostProgress(0,1);
	running = false;
	stopped = true;
	#endif
}

void TaskManager::GetLineActions(Task* t) {
	if (Thread::IsShutdownThreads())
		return;
	
	int per_batch = 15;
	
	if (t->batches.IsEmpty())
		t->UpdateBatches(per_batch);
	
	if (t->batch_i < 0 || t->batch_i >= t->batches.GetCount()) {
		RemoveTask(*t);
		return;
	}
	Task::Batch& batch = t->batches[t->batch_i];
	
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	
	SongDataAnalysisArgs args;
	args.fn = 10;
	args.phrases <<= Split(batch.txt, "\n");
	
	TaskMgr& m = SongLib::TaskManager::Single().MakePipe();
	m.GetSongDataAnalysis(args, THISBACK1(OnLineActions, t));
}

void TaskManager::GetSyllables(Task* t) {
	if (Thread::IsShutdownThreads())
		return;
	
	int per_batch = 30;
	
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	
	int begin = t->batch_i * per_batch;
	int end = (t->batch_i+1) * per_batch;
	
	
	SongDataAnalysisArgs args;
	t->tmp_map_ds_i.Clear();
	
	int iter = 0;
	for(int ds_i = 0; ds_i < sd.GetCount(); ds_i++) {
		String ds_key = sd.GetKey(ds_i);
		DatasetAnalysis& ds = sda.datasets.GetAdd(ds_key);
		
		for(int i = 0; i < ds.words.GetCount(); i++) {
			if (iter >= begin) {
				String wrd = ds.words.GetKey(i);
				
				// hotfix
				HotfixReplaceWord(wrd);
				
				args.words << wrd;
				t->tmp_map_ds_i.Add(wrd, ds_i);
			}
			iter++;
			if (iter >= end) break;
		}
		if (iter >=  end) break;
	}
	
	if (args.words.IsEmpty()) {
		RemoveTask(*t);
		return;
	}
	
	
	args.fn = 4;
	
	TaskMgr& m = SongLib::TaskManager::Single().MakePipe();
	m.GetSongDataAnalysis(args, THISBACK1(OnSyllables, t));
}

void TaskManager::GetDetails(Task* t) {
	if (Thread::IsShutdownThreads())
		return;
	
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	
	int per_batch = 30;
	
	int begin = t->batch_i * per_batch;
	int end = (t->batch_i+1) * per_batch;
	
	if (t->batch_i < 0) {
		begin = 0;
		end = 1;
	}
	
	t->tmp_ds_i.Clear();
	t->tmp_words.Clear();
	
	SongDataAnalysisArgs args;
	
	int iter = 0;
	for(int ds_i = 0; ds_i < sd.GetCount(); ds_i++) {
		String ds_key = sd.GetKey(ds_i);
		DatasetAnalysis& ds = sda.datasets.GetAdd(ds_key);
		
		for(int i = 0; i < ds.words.GetCount(); i++) {
			if (iter >= begin) {
				const String& wrd = ds.words.GetKey(i);
				
				#if 0
				int translation_i = ds.translations.Find(wrd);
				if (translation_i < 0)
					continue;
				#endif
				
				// Skip cyrillic words
				bool is_cyrillic = false;
				WString ws = wrd.ToWString();
				for(int j = 0; j < ws.GetCount(); j++) {
					int chr = ws[j];
					if (chr >= 0x0400 && chr < 0x052F) {
						is_cyrillic = true;
						break;
					}
				}
				if (is_cyrillic)
					continue;
				
				args.words << wrd;
				t->tmp_ds_i << ds_i;
				t->tmp_words.Add(wrd);
			}
			iter++;
			if (iter >= end) break;
		}
		if (iter >= end) break;
	}
	
	if (args.words.IsEmpty()) {
		RemoveTask(*t);
		return;
	}
	
	args.fn = 5;
	
	TaskMgr& m = SongLib::TaskManager::Single().MakePipe();
	m.GetSongDataAnalysis(args, THISBACK1(OnDetails, t));
}

void TaskManager::GetLineChangeScores(Task* t) {
	if (Thread::IsShutdownThreads())
		return;
	
	int per_batch = 15;
	
	if (t->batches.IsEmpty())
		t->UpdateBatches(per_batch);
	
	if (t->batch_i < 0 || t->batch_i >= t->batches.GetCount()) {
		RemoveTask(*t);
		return;
	}
	
	Task::Batch& batch = t->batches[t->batch_i];
	
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	DatasetAnalysis& da = sda.datasets[batch.ds_i];
	
	SongDataAnalysisArgs args;
	args.fn = 11;
	//args.score_mode = score_mode;
	args.phrases <<= Split(batch.txt, "\n");
	
	Vector<int>& ap_is = t->tmp;
	ap_is.Clear();
	
	bool fail = false;
	for (String& l : args.phrases) {
		int ap_i = 0;
		bool found = false;
		for(int i = 0; i < da.action_phrases.GetCount(); i++) {
			const String& txt = da.action_phrases.GetKey(i);
			const ExportDepActionPhrase& ap = da.action_phrases[i];
			
			if (txt == l) {
				l.Clear();
				for (int a : ap.actions) {
					const ActionHeader& ah = da.actions.GetKey(a);
					if (!l.IsEmpty()) l << " + ";
					l << ah.action << "(" << ah.arg << ")";
				}
				found = true;
				break;
			}
			ap_i++;
		}
		ap_is << (found ? ap_i : -1);
		if (!found)
			fail = true;
	}
	
	/*if (fail) {
		PromptOK("Failed to finish line change scores");
		return;
	}*/
	
	
	TaskMgr& m = SongLib::TaskManager::Single().MakePipe();
	m.GetSongDataAnalysis(args, THISBACK1(OnLineChangeScores, t));
}

void TaskManager::GetWordData(Task* t) {
	
	
	/*bar.Add(t_("Get details"), AppImg::BlueRing(), THISBACK2(GetDetails, -1, false)).Key(K_CTRL_W);
	bar.Add(t_("Get all details"), AppImg::RedRing(), THISBACK2(GetDetails, 0, true)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Get word syllables"), AppImg::BlueRing(), THISBACK2(GetSyllables, -1, false)).Key(K_CTRL_Q);
	bar.Add(t_("Get all syllables"), AppImg::RedRing(), THISBACK2(GetSyllables, 0, true)).Key(K_F7);*/
	
	/*void GetEverything();
	void GetSyllables(int batch_i, bool start_next);
	void GetDetails(int batch_i, bool start_next);
	void OnSyllables(String res, int batch_i, bool start_next);
	void OnDetails(String res, int batch_i, bool start_next);*/
	
}

void TaskManager::GetWordnet(Task* t) {
	if (t->fn == 0)
		MakeWordnetsFromTemplates(t);
	else if (t->fn == 1)
		GetColorAlternatives(t);
}

void TaskManager::GetColorAlternatives(Task* t) {
	if (Thread::IsShutdownThreads())
		return;
	
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	
	int per_batch = 25;
	int begin = t->batch_i * per_batch;
	int end = (t->batch_i+1) * per_batch;
	
	if (t->batch_i < 0) {
		begin = 0;
		end = 1;
	}
	
	SongDataAnalysisArgs args;
	
	VectorMap<String, int>& word_ds = t->tmp_map_ds_i;
	VectorMap<String, Color>& word_clr = t->word_clr;
	word_ds.Clear();
	word_clr.Clear();
	
	int iter = 0;
	for(int ds_i = 0; ds_i < sd.GetCount(); ds_i++) {
		String ds_key = sd.GetKey(ds_i);
		DatasetAnalysis& ds = sda.datasets.GetAdd(ds_key);
		
		for(int i = 0; i < ds.words.GetCount(); i++) {
			if (iter >= begin) {
				const String& txt = ds.words.GetKey(i);
				ExportWord& wa = ds.words[i];
				
				String wc = wa.class_count > 0 ? ds.word_classes[wa.classes[0]] : String();
				String header = wc + ": " + txt;
				String key = header;
				key << ", RGB(" << (int)wa.clr.GetR() << "," << (int)wa.clr.GetG() << "," << (int)wa.clr.GetB() << ")";
				args.words << key;
				
				word_ds.Add(txt, ds_i);
				word_clr.Add(txt, wa.clr);
				
				if (args.words.GetCount() == 1) {
					t->tmp_str = header;
				}
			}
			iter++;
			if (iter >=  end) break;
		}
		if (iter >=  end) break;
	}
	
	if (args.words.IsEmpty()) {
		RemoveTask(*t);
		return;
	}
	
	args.fn = 7;
	
	TaskMgr& m = SongLib::TaskManager::Single().MakePipe();
	m.GetSongDataAnalysis(args, THISBACK1(OnColorAlternatives, t));
}

void TaskManager::MakeWordnetsFromTemplates(Task* t) {
	Database& db = Database::Single();
	SongData& sd = db.song_data;
	SongDataAnalysis& sda = db.song_data.a;
	
	TimeStop ts;
	
	PromptOK("TODO");
	#if 0
	for(int i = 0; i < sda.datasets.GetCount(); i++) {
		DatasetAnalysis& da = sda.datasets[i];
		for(int j = 0; j < da.tmpl_phrases.GetCount(); j++) {
			TemplatePhrase& tp = da.tmpl_phrases[j];
			String group = tp.group;
			String value = tp.value;
			Color clr = tp.clr;
			int clr_group = GetColorGroup(clr);
			Vector<String> main_classes;
			for (const String& part : tp.parts) {
				if (part.IsEmpty()) continue;
				if (part[0] == '{')
					main_classes << part.Mid(1,part.GetCount()-2);
			}
			for(int k = 0; k < tp.words.GetCount(); k++) {
				if (k >= main_classes.GetCount()) break;
				const String& main_class = main_classes[k];
				auto& words = tp.words[k];
				
				// Find wordnet
				bool found = false;
				for (ExportWordnet& wn : da.wordnets) {
					if (wn.clr_group == clr_group &&
						wn.group == group &&
						wn.value == value &&
						wn.main_class == main_class) {
						bool any_match = false;
						for (const String& dst : wn.words) {
							for (const String& src : words) {
								if (dst == src) {
									any_match = true;
									break;
								}
							}
							if (any_match) break;
						}
						if (any_match) {
							
							// Append wordnet
							for (const String& src : words)
								VectorFindAdd(wn.words, src);
							
							found = true;
							break;
						}
					}
				}
				if (!found) {
					// New wordnet
					ExportWordnet& wn = da.wordnets.Add();
					wn.main_class = main_class;
					wn.group = group;
					wn.value = value;
					wn.clr = clr;
					wn.clr_group = clr_group;
					wn.words <<= words;
				}
			}
		}
	}
	
	LOG("MakeWordnetsFromTemplates took: " << ts.ToString());
	#endif
}

}
