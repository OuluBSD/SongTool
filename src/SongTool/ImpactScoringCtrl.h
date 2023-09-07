#ifndef _SongTool_ImpactScoringCtrl_h_
#define _SongTool_ImpactScoringCtrl_h_


class ImpactScoringCtrl : public SongToolCtrl {
	MArr<Plotter> plotter;
	MArr<One<ArrayCtrl>> list;
	MArr<Splitter> vsplit;
	Splitter mainsplit;
	Pipe* last_pipe = 0;
	
	void Realize(Pipe& p);
	void ListMenu(Bar& bar);
public:
	typedef ImpactScoringCtrl CLASSNAME;
	ImpactScoringCtrl();
	
	void Data();
	void DataListAll() {for(const SnapArg& a : ModeArgs()) DataList(a);}
	void DataList(const SnapArg& a);
	void ListValueChanged(const SnapArg& a, int pos, int scoring);
	
	static const int group_begin = 4;
	
};


#endif
