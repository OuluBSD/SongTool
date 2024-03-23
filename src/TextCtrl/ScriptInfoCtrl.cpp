#include "TextCtrl.h"
#include <TextTool/TextTool.h>


BEGIN_TEXTLIB_NAMESPACE


ScriptInfoCtrl::ScriptInfoCtrl() {
	CtrlLayout(*this);
	
	copyright <<= THISBACK(OnValueChange);
	native_title <<= THISBACK(OnValueChange);
	english_title <<= THISBACK(OnValueChange);
	content_vision <<= THISBACK(OnValueChange);
	//typecast <<= THISBACK(OnValueChange);
	//archetype <<= THISBACK(OnValueChange);
	is_rapper <<= THISBACK(OnValueChange);
	
	typecast.Disable();
	archetype.Disable();
	
}

void ScriptInfoCtrl::Clear() {
	this->copyright			.Clear();
	this->native_title		.Clear();
	this->english_title		.Clear();
	this->content_vision	.Clear();
	this->typecast			.SetIndex(0);
	this->archetype			.SetIndex(0);
	this->is_rapper			.SetIndex(0);
}

void ScriptInfoCtrl::Data() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	
	lbl_script.SetLabel(GetAppModeKeyCap(AM_SCRIPT));
	lbl_typeclass.SetLabel(GetAppModeLabel(AM_TYPECLASS) + ":");
	lbl_content.SetLabel(GetAppModeLabel(AM_CONTENT) + ":");
	lbl_is_unsafe.SetLabel("Is " + GetAppModeKey(AM_UNSAFE) + ":");
	
	is_rapper.Clear();
	is_rapper.Add(GetAppModeLabel(AML_SAFE_DESC));
	is_rapper.Add(GetAppModeLabel(AML_UNSAFE_DESC));
	
	// Requires ToolEditor ptr (can't move to ctor)
	if (typecast.GetCount() == 0) {
		const auto& tcs = GetTypeclasses();
		ASSERT(tcs.GetCount());
		for(int i = 0; i < tcs.GetCount(); i++) {
			typecast.Add(tcs[i]);
		}
		
		const auto& archs = GetContents();
		ASSERT(archs.GetCount());
		for(int i = 0; i < archs.GetCount(); i++) {
			archetype.Add(archs[i].key);
		}
	}
	
	Clear();
	
	
	if (p.scripts) {
		Script& l = *p.scripts;
		
		copyright.SetData(l.copyright);
		native_title.SetData(l.native_title);
		english_title.SetData(l.english_title);
		content_vision.SetData(l.content_vision);
		typecast.SetIndex(l.typecast);
		archetype.SetIndex(l.archetype);
		is_rapper.SetIndex(l.is_rapper);
	}
}

void ScriptInfoCtrl::OnValueChange() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	
	if (p.scripts && editor->scripts.IsCursor()) {
		Script& l = *p.scripts;
		
		l.copyright = copyright.GetData();
		l.native_title = native_title.GetData();
		l.english_title = english_title.GetData();
		l.content_vision = content_vision.GetData();
		l.typecast = typecast.GetIndex();
		l.archetype = archetype.GetIndex();
		l.is_rapper = is_rapper.GetIndex();
		
		int c = editor->scripts.GetCursor();
		editor->scripts.Set(c, 0, l.GetAnyTitle());
	}
}


END_TEXTLIB_NAMESPACE
