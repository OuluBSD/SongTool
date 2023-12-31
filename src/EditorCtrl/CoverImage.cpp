#include "EditorCtrl.h"



CoverImage::CoverImage() {
	user_natural_english_key = "NATURAL_ENGLISH_OF_AUTHOR";
	
}

void CoverImage::Init() {
	ImagePlayerBase::Init();
	
}

void CoverImage::ToolMenu(Bar& bar) {
	bar.Add(t_("Create suggestions for prompts"), AppImg::Part(), THISBACK(CreateSuggestionsForPrompts)).Key(K_F5);
	bar.Add(t_("Make single image"), AppImg::Part(), THISBACK(MakeSingleImage)).Key(K_F6);
	bar.Add(t_("Make all images"), AppImg::Part(), THISBACK(MakeAllImages)).Key(K_F7);
	
}

void CoverImage::CreateSuggestionsForPrompts() {
	Database& db = Database::Single();
	EditorPtrs& p = db.ctx.ed;
	if(!p.song || !p.artist ||!p.release)
		return;
	
	Release& rel = *p.release;
	
	
	
	if (items.GetCount()) {
		if (!PromptYesNo(DeQtf(t_("Are you sure that you want to replace all in the list?"))))
			return;
	}
	
	int row = 0;
	for(int i = 0; i < rel.songs.GetCount(); i++) {
		const Song& song = rel.songs[i];
		String lyrics = song.data.Get(user_natural_english_key, "");
		if (lyrics.IsEmpty())
			continue;
		
		String title = !song.english_title.IsEmpty() ? song.english_title : song.native_title;
		
		String raw_prompt;
		raw_prompt << "Artist: " << p.artist->english_name << "\n";
		raw_prompt << "Album: " << p.release->english_title << "\n";
		raw_prompt << "Year: " << (int)p.release->date.year << "\n";
		raw_prompt << "Genre: " << p.artist->musical_style << "\n";
		raw_prompt << "Description of singer: " << p.artist->vocalist_visual << "\n";
		raw_prompt << "Count of songs: " << p.release->songs.GetCount() << "\n";
		raw_prompt << "\n\n";
		
		raw_prompt << "Song: " << title << "\n\n";
		raw_prompt << lyrics << "\n\n";
		
		raw_prompt << "Short description of the album's cover image for the generative image AI:\n";
		
		String result_part = title + " based cover";
		
		TaskMgr& m = SongLib::TaskManager::Single().MakePipe();
		m.RawCompletion(raw_prompt, THISBACK1(PostOnResult, result_part));
	}
}

void CoverImage::OnResult(String res, String part) {
	Item& item = items.Add();
	item.part = part;
	item.prompt = res;
	
	DataList();
}

void CoverImage::MakeSingleImage() {
	ImagePlayerBase::MakeSingleImage();
}

void CoverImage::MakeAllImages() {
	ImagePlayerBase::MakeAllImages();
}
