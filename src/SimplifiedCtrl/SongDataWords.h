#ifndef _SimplifiedCtrl_SongDataWords_h_
#define _SimplifiedCtrl_SongDataWords_h_


class SongDataWords : public SongToolCtrl {
	Splitter hsplit, vsplit;
	ArrayCtrl datasets, wordgroups, artists, words;
	bool disabled = false;
	int per_batch = 30;
	Vector<int> tmp_ds_i;
	Index<String> tmp_words;
	
public:
	typedef SongDataWords CLASSNAME;
	SongDataWords();
	
	void EnableAll();
	void DisableAll();
	void Data() override;
	void DataMain();
	void DataDataset();
	void DataWordgroup();
	void DataArtist();
	void ToolMenu(Bar& bar) override;
	void UpdateWords();
	void UpdateWordsProcess();
	void UpdateWordFlagGroups();
	void UpdateWordFlags();
	void DumpWordGroups();
	void GetSyllables(int batch_i, bool start_next);
	void GetDetails(int batch_i, bool start_next);
	void OnSyllables(String res, int batch_i, bool start_next);
	void OnDetails(String res, int batch_i, bool start_next);
	
};


#endif
