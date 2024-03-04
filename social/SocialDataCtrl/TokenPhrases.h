#ifndef _SocialDataCtrl_TokenPhrases_h_
#define _SocialDataCtrl_TokenPhrases_h_


class TokenPhrases : public ToolAppCtrl {
	Splitter vsplit, hsplit;
	ArrayCtrl datasets, texts;
	
public:
	typedef TokenPhrases CLASSNAME;
	TokenPhrases();
	
	void Data() override;
	void DataDataset();
	void ToolMenu(Bar& bar) override;
	void GetUnknownPairs();
	
	
};


#endif
