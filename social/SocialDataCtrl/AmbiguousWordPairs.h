#ifndef _SocialDataCtrl_AmbiguousWordPairs_h_
#define _SocialDataCtrl_AmbiguousWordPairs_h_


class AmbiguousWordPairs : public ToolAppCtrl {
	Splitter vsplit, hsplit;
	ArrayCtrl datasets, texts;
	
public:
	typedef AmbiguousWordPairs CLASSNAME;
	AmbiguousWordPairs();
	
	void Data() override;
	void DataDataset();
	void ToolMenu(Bar& bar) override;
	void Process();
	
};


#endif
