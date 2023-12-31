#ifndef _EditorCtrl_AttributeDistribution_h_
#define _EditorCtrl_AttributeDistribution_h_


class AttributeDistribution : public SongToolCtrl {
	bool disabled = false;
	Splitter vsplit, hsplit;
	ArrayCtrl attrs, active, positive, negative;
	
	VectorMap<String,Index<String>> active_attrs, free_attrs;
	
public:
	typedef AttributeDistribution CLASSNAME;
	AttributeDistribution();
	
	void DisableAll();
	void EnableAll();
	void Data() override;
	void DataSuggestions();
	void ToolMenu(Bar& bar) override;
	void AddAttribute(bool negative);
	void AddRandom();
	void RemoveActiveAttribute();
	void AddRandomCombination();
	
};


#endif
