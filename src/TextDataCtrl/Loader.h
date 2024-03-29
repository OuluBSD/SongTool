#ifndef _TextDataCtrl_TextDataLoader_h_
#define _TextDataCtrl_TextDataLoader_h_


BEGIN_TEXTLIB_NAMESPACE


class TextDataLoader : public WithTextDataLoader<TopWindow> {
	int appmode = -1;
	
	void LoadHuggingArtists();
	void LoadHuggingFinn();
	void LoadHuggingTweets();
	TextDatabase& GetDatabase();
	
public:
	typedef TextDataLoader CLASSNAME;
	TextDataLoader(int appmode);
	
	void Process();
	void Start() {Thread::Start(THISBACK(Process));}
	void Stop() {Close();}
	void PostMessage(String s) {PostCallback(THISBACK1(SetMessage, s));}
	void PostProgress(int i, int t) {PostCallback(THISBACK2(SetProgress, i, t));}
	void SetMessage(String s) {message.SetLabel(s);}
	void SetProgress(int i, int t) {prog.Set(i,t);}
	
};


END_TEXTLIB_NAMESPACE


#endif