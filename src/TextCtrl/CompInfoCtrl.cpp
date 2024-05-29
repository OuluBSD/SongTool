#include "TextCtrl.h"
#include <TextTool/TextTool.h>


BEGIN_TEXTLIB_NAMESPACE


CompInfoCtrl::CompInfoCtrl() {
	CtrlLayout(*this);
	
	comp_entity <<= THISBACK(OnValueChange);
	comp_prj_name <<= THISBACK(OnValueChange);
	reference <<= THISBACK(OnValueChange);
	origins <<= THISBACK(OnValueChange);
	music_style <<= THISBACK(OnValueChange);
	
	scripts.AddColumn(t_("Typeclass & Content"));
	scripts.AddColumn(t_("Script"));
	scripts.ColumnWidths("1 5");
	scripts.AddIndex("IDX");
	
	scripts.WhenCursor << THISBACK(DataScript);
	
	set.WhenAction << THISBACK(SetScript);
	
}

void CompInfoCtrl::Clear() {
	this->comp_entity				.Clear();
	this->comp_prj_name				.Clear();
	
}

void CompInfoCtrl::Data() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	
	lbl_component.SetLabel(GetAppModeKeyCap(AM_COMPONENT));
	lbl_script.SetLabel(GetAppModeKeyCap(AM_SCRIPT) + ":");
	lbl_entity.SetLabel(GetAppModeLabel(AML_ENTITY_IF_DIFFERENT));
	lbl_ref_comp.SetLabel(GetAppModeLabel(AML_REFERENCE_COMPONENT));
	lbl_origins.SetLabel(GetAppModeLabel(AML_COMPONENT_ORIGINS));
	
	scripts		.ColumnAt(0).HeaderTab().SetText(GetAppModeKeyCapN(AM_TYPECLASS) + " & " + GetAppModeKeyCapN(AM_CONTENT));
	scripts		.ColumnAt(1).HeaderTab().SetText(GetAppModeKeyCapN(AM_SCRIPT));
	
	Clear();
	
	if (p.component) {
		Component& s = *p.component;
		
		comp_entity.SetData(s.artist);
		comp_prj_name.SetData(s.prj_name);
		reference.SetData(s.reference);
		origins.SetData(s.origins);
		music_style.SetData(s.music_style);
	}
	
	
	if (!p.entity) {
		scripts.Clear();
		return;
	}
	
	Entity& a = *p.entity;
	if (p.component)
		focus_lyr = a.FindScript(p.component->scripts_file_title);
	
	if (focus_lyr < 0) {
		focus_lyr = p.GetActiveScriptIndex();
	}
	/*
	const auto& tcs = GetTypeclasses();
	for(int i = 0; i < a.typeclasses.GetCount(); i++) {
		const auto& t = tcs[i];
		const auto& tc = a.typeclasses[i];
		typeclasses.Set(i, "IDX", i);
		typeclasses.Set(i, 0, t);
		typeclasses.Set(i, 1, a.typeclasses[i].GetScriptCount());
	}
	INHIBIT_CURSOR(typeclasses);
	typeclasses.SetSortColumn(1, true);
	
	int cursor = max(0, focus_tc);
	if (cursor >= 0 && cursor < typeclasses.GetCount())
		SetIndexCursor(typeclasses, cursor);

	DataTypeclass();*/
	/*
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.entity || !typeclasses.IsCursor()) {
		contents.Clear();
		scripts.Clear();
		return;
	}
	
	Entity& a = *p.entity;
	Typeclass& t = a.typeclasses[typeclasses.Get("IDX")];
	const auto& cons = GetContents();
	for(int i = 0; i < t.contents.GetCount(); i++) {
		const auto& con = cons[i];
		const auto& at = t.contents[i];
		contents.Set(i, "IDX", i);
		contents.Set(i, 0, con.key);
		contents.Set(i, 1, at.scripts.GetCount());
	}
	INHIBIT_CURSOR(contents);
	contents.SetSortColumn(1, true);
	
	int cursor = max(0, focus_arch);
	if (cursor >= 0 && cursor < contents.GetCount())
		SetIndexCursor(contents, cursor);

	DataContent();
	

	
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.entity || !typeclasses.IsCursor() || !contents.IsCursor()) {
		scripts.Clear();
		return;
	}*/
	
	//Entity& a = *p.entity;
	
	int row = 0;
	const auto& tcs = GetTypeclasses();
	const auto& cons = GetContents();
	for(int i = 0; i < a.scripts.GetCount(); i++) {
		Script& sc = a.scripts[i];
		String g = tcs[sc.typeclass] + ": " + cons[sc.content].key;
		scripts.Set(row, "IDX", i);
		scripts.Set(row, 0, g);
		scripts.Set(row, 1, sc.GetAnyTitle());
		row++;
	}
	INHIBIT_CURSOR(scripts);
	scripts.SetCount(row);
	int cursor = max(0, focus_lyr);
	if (cursor >= 0 && cursor < scripts.GetCount())
		scripts.SetCursor(cursor);
	
	DataScript();
}

void CompInfoCtrl::DataScript() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.entity || !scripts.IsCursor()) {
		scripts_text.Clear();
		return;
	}
	
	Entity& a = *p.entity;
	Script& lyr = a.scripts[scripts.Get("IDX")];
	
	if (lyr.text.GetCount())
		scripts_text.SetData(lyr.text);
	else
		scripts_text.SetData("<no scripts>");
}

void CompInfoCtrl::OnValueChange() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	
	if (p.component && editor->components.IsCursor()) {
		Component& s = *p.component;
		
		s.artist = comp_entity.GetData();
		s.prj_name = comp_prj_name.GetData();
		s.reference = reference.GetData();
		s.origins = origins.GetData();
		s.music_style = music_style.GetData();
		
		int c = editor->components.GetCursor();
		editor->components.Set(c, 0, s.artist);
		editor->components.Set(c, 1, s.prj_name);
	}
}

void CompInfoCtrl::SetScript() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	Component& s = *p.component;
	
	if (!p.entity || !p.component || !scripts.IsCursor()) {
		return;
	}
	
	int l_i = scripts.Get("IDX");
	Script& l = p.entity->scripts[l_i];
	s.scripts_file_title = l.file_title;
}


END_TEXTLIB_NAMESPACE
