#ifndef _EditorCtrl_PublicRelations_h_
#define _EditorCtrl_PublicRelations_h_


class PublicRelationsCtrl : public SongToolCtrl {
	
	
public:
	typedef PublicRelationsCtrl CLASSNAME;
	PublicRelationsCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	
};


#endif
