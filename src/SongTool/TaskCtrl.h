#ifndef _SongTool_TaskCtrl_h_
#define _SongTool_TaskCtrl_h_


class Tasks : public Ctrl {
	Splitter hsplit, vsplit;
	ArrayCtrl list;
	DocEdit input, output;
	ProgressIndicator prog;
	Label lbl;
	int data_cursor = -1;
	
public:
	typedef Tasks CLASSNAME;
	Tasks();
	
	void Data();
	void DataTask();
	void ToolMenu(Bar& bar) {}
	void ValueChange();
	void ProcessItem();
	void RetryItem(bool skip_prompt, bool skip_cache);
	void OutputMenu(Bar& bar);
	
};


#endif
