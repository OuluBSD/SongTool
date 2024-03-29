#ifndef _TextDataCtrl_ActionAttrs_h_
#define _TextDataCtrl_ActionAttrs_h_


BEGIN_TEXTLIB_NAMESPACE


class ActionAttrsPage : public ToolAppCtrl {
	Splitter vsplit, hsplit;
	ArrayCtrl datasets, attrs, colors, actions;
	
public:
	typedef ActionAttrsPage CLASSNAME;
	ActionAttrsPage();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void DataMain();
	void DataDataset();
	void DataAttribute();
	void DataColor();
	void UpdateFromCache();
	void UpdateColors();
	void UpdateAttributes();
	
};


END_TEXTLIB_NAMESPACE


#endif
