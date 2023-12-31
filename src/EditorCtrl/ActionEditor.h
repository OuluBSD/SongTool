#ifndef _EditorCtrl_ActionEditor_h_
#define _EditorCtrl_ActionEditor_h_


class ActionEditor : public SongToolCtrl {
	Splitter hsplit, vsplit, mainsplit, main_vsplit, sug_hsplit, sug_vsplit;
	ArrayCtrl parts, thrds, thrd_actions, actions, args, song_actions;
	ArrayCtrl colors, attrs;
	
	int def_thread_count = 8;
	
	int ds_i = 0;
	VectorMap<String, VectorMap<String, int>> uniq_acts;
	
	enum {ANY, TRANSITION, PARALLEL, T_AND_P};
	struct Sug : Moveable<Sug> {
		int src;
		bool skip_attr;
		ActionAttrs attr;
		ActionTransition trans;
		ActionParallel paral;
	};
	VectorMap<String, VectorMap<String, Sug>> suggestions;
	
public:
	typedef ActionEditor CLASSNAME;
	ActionEditor();
	
	void Data() override;
	void DataPart();
	void DataThread();
	void DataSong();
	void DataSuggestions();
	void DataAction();
	void ToolMenu(Bar& bar) override;
	void AddAction();
	void RemoveAction();
	
};


#endif
