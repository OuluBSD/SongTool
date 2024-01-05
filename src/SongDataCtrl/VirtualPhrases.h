#ifndef _SongDataCtrl_VirtualPhrases_h_
#define _SongDataCtrl_VirtualPhrases_h_


class VirtualPhrases : public SongToolCtrl {
	Splitter vsplit, hsplit;
	ArrayCtrl datasets, texts, parts;
	
public:
	typedef VirtualPhrases CLASSNAME;
	VirtualPhrases();
	
	void Data() override;
	void DataDataset();
	void ToolMenu(Bar& bar) override;
	void ProcessVirtualPhrases();
	void ProcessVirtualPhraseParts();
	
};



#endif