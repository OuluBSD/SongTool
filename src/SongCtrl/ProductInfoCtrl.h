#ifndef _SongCtrl_ProductInfoCtrl_h_
#define _SongCtrl_ProductInfoCtrl_h_


class Editor;

class ProductInfoCtrl : public WithProductInfo<ToolAppCtrl> {
	
	
public:
	typedef ProductInfoCtrl CLASSNAME;
	ProductInfoCtrl();
	
	void Data();
	void Clear();
	void OnValueChange();
	
	Editor* editor = 0;
	
};


#endif
