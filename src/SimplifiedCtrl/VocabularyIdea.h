#ifndef _SimplifiedCtrl_VocabularyIdea_h_
#define _SimplifiedCtrl_VocabularyIdea_h_


class VocabularyIdeaCtrl : public SongToolCtrl {
	Splitter hsplit, vsplit;
	ArrayCtrl parts;
	DocEdit part_dialogue0, part_dialogue1;
	
public:
	typedef VocabularyIdeaCtrl CLASSNAME;
	VocabularyIdeaCtrl();
	
	void DisableAll();
	void EnableAll();
	void Data() override;
	void ToolMenu(Bar& bar) override;
	
};


#endif
