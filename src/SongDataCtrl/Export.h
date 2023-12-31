#ifndef _SongDataCtrl_Export_h_
#define _SongDataCtrl_Export_h_


class ExportPage : public SongToolCtrl {
	DocEdit log;
	ProgressIndicator prog;
	bool running = 0, stopped = 1;
	
	void Export();
	
public:
	typedef ExportPage CLASSNAME;
	ExportPage();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void Start();
	void Stop();
	void SetProgress(int a, int t) {prog.Set(a,t);}
	void PostProgress(int a, int t) {PostCallback(THISBACK2(SetProgress, a, t));}
	
};


#endif
