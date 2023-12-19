#ifndef _SongDataCtrl_RhymeContainer_h_
#define _SongDataCtrl_RhymeContainer_h_


class RhymeContainerPage : public SongToolCtrl {
	ProgressIndicator prog;
	Splitter hsplit, vsplit;
	ArrayCtrl datasets, syl_counts, colors, attrs, actions, data;
	bool running = false, stopped = true;
	
	VectorMap<String, VectorMap<String, int>> uniq_acts;
	VectorMap<String, String> mirror_words;
	
	void Start();
	void Stop();
	void Process();
	void MakeSplit(Vector<String>& parts, Vector<Index<String>>& wrds);
	int FindSplit(const Vector<String>& part_words);
	
public:
	typedef RhymeContainerPage CLASSNAME;
	RhymeContainerPage();
	~RhymeContainerPage();
	
	void EnableAll();
	void DisableAll();
	void Data() override;
	void MainData();
	void ManualData();
	void ToolMenu(Bar& bar) override;
	void ProcessData();
	void SetProgress(int a, int t) {prog.Set(a,t);}
	void PostProgress(int a, int t) {PostCallback(THISBACK2(SetProgress, a, t));}
	
};


#endif
