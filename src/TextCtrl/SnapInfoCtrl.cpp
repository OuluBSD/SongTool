#include "TextCtrl.h"
#include <TextTool/TextTool.h>


BEGIN_TEXTLIB_NAMESPACE


SnapInfoCtrl::SnapInfoCtrl() {
	CtrlLayout(*this);
	
	native_album_title <<= THISBACK(OnValueChange);
	english_album_title <<= THISBACK(OnValueChange);
	album_date <<= THISBACK(OnValueChange);
	year_of_content <<= THISBACK(OnValueChange);
	
}

void SnapInfoCtrl::Clear() {
	this->native_album_title		.Clear();
	this->english_album_title		.Clear();
	this->album_date				.Clear();
	this->year_of_content			.Clear();
}

void SnapInfoCtrl::Data() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	
	Clear();
	
	if (p.release) {
		Snapshot& r = *p.release;
		
		native_album_title.SetData(r.native_title);
		english_album_title.SetData(r.english_title);
		album_date.SetData(r.date);
		year_of_content.SetData(r.year_of_content);
	}
	
}

void SnapInfoCtrl::OnValueChange() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	
	if (p.release && editor->snaps.IsCursor()) {
		Snapshot& r = *p.release;
		
		r.native_title = native_album_title.GetData();
		r.english_title = english_album_title.GetData();
		r.date = album_date.GetData();
		r.year_of_content = year_of_content.GetData();
		
		int c = editor->snaps.GetCursor();
		editor->snaps.Set(c, 0, r.native_title);
	}
	
}


END_TEXTLIB_NAMESPACE
