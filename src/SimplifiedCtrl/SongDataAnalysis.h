#ifndef _SimplifiedCtrl_SongDataAnalysis_h_
#define _SimplifiedCtrl_SongDataAnalysis_h_


class SongDataAnalysis : public SongToolCtrl {
	struct Task : Moveable<Task> {
		String artist, song, text;
		LyricsDataset* ld = 0;
		LyricsAnalysis* analysis = 0;
		int progress = 0;
	};
	
	int txt_limit = 6000;
	
private:
	Splitter hsplit;
	ArrayCtrl list;
	DocEdit analysis;
	Vector<Task> tasks;
	ProgressIndicator prog;
	bool batch = false;
	bool disabled = false;
public:
	typedef SongDataAnalysis CLASSNAME;
	SongDataAnalysis();
	
	void DisableAll();
	void EnableAll();
	void Data() override;
	void DataTask();
	void StartBatch();
	void ToolMenu(Bar& bar) override;
	void UpdateTasks();
	void GetRhymes(int list_i, bool start_next);
	void GetWords(int list_i, bool start_next);
	void GetRoleDistributions(int list_i, bool start_next);
	void OnRhymes(String res, int list_i, bool start_next);
	void OnWords(String res, int list_i, bool start_next);
	void OnRoleDistributions(String res, int list_i, bool start_next);
	
};


#endif