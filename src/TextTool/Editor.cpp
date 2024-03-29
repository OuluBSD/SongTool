#include "TextTool.h"


BEGIN_TEXTLIB_NAMESPACE


ToolEditor::ToolEditor(TextTool* app) : app(*app) {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << menusplit << base;
	hsplit.SetPos(1000);
	
	menusplit.Vert() << appmode_list << page_group_list << page_list << entities << subsplit;
	int top_total = 10000 * 3 / 5;
	menusplit.SetPos(top_total * 1 / 4, 0);
	menusplit.SetPos(top_total * 2 / 4, 1);
	menusplit.SetPos(top_total * 3 / 4, 2);
	menusplit.SetPos(top_total, 3);
	componentsplit.Vert() << snaps << components;
	scriptssplit.Vert() << typeclasses << contents << scripts;
	
	
	scripts.WhenBar << THISBACK(ScriptMenu);
	entities.WhenBar << THISBACK(EntityMenu);
	snaps.WhenBar << THISBACK(SnapshotMenu);
	components.WhenBar << THISBACK(SongMenu);
	
	page_group_list.AddColumn(t_("Page group"));
	page_group_list <<= THISBACK(ViewPageGroup);
	
	page_list.AddColumn(t_("Page"));
	page_list <<= THISBACK(ViewPage);
	
	entities.AddColumn(t_("Entity"));
	entities <<= THISBACK(DataEntity);
	
	snaps.AddColumn(t_("Snapshot"));
	snaps.ColumnWidths("3 2");
	snaps <<= THISBACK(DataSnapshot);
	
	components.AddColumn(t_("Song"));
	components <<= THISBACK(DataComponent);
	
	typeclasses.AddColumn(t_("Typeclass"));
	typeclasses.AddColumn(t_("Count"));
	typeclasses.ColumnWidths("3 1");
	typeclasses <<= THISBACK(DataTypeclass);
	typeclasses.AddIndex("IDX");
	
	contents.AddColumn(t_("Content"));
	contents.AddColumn(t_("Count"));
	contents.ColumnWidths("3 1");
	contents <<= THISBACK(DataContent);
	contents.AddIndex("IDX");
	
	scripts.AddColumn(t_("Script"));
	scripts.AddIndex("IDX");
	scripts <<= THISBACK(DataScript);
	
	entity_info.editor = this;
	snap_info.editor = this;
	comp_info.editor = this;
	script_info.editor = this;
	
	SetSubMenu(0);
	
	for(int i = 0; i < DB_COUNT; i++) {
		MetaPtrs::Single().db[i].editor = this;
	}
}

void ToolEditor::InitAppModes(const Index<int>& appmodes) {
	appmode_list.AddColumn(t_("Appmode"));
	appmode_list.AddIndex("IDX");
	for(int i = 0; i < appmodes.GetCount(); i++) {
		int am = appmodes[i];
		appmode_list.Set(i, 0, GetAppModeString(am));
		appmode_list.Set(i, "IDX", am);
	}
	appmode_list.SetCursor(0);
	appmode_list <<= THISBACK(SwitchAppMode);
	PostCallback(THISBACK(SwitchAppMode));
}

void ToolEditor::SetSubMenu(int i) {
	subsplit.RemoveChild(&componentsplit);
	subsplit.RemoveChild(&scriptssplit);
	
	if (i == 0)
		subsplit.Add(componentsplit.SizePos());
	else
		subsplit.Add(scriptssplit.SizePos());
}

void ToolEditor::AddItem(String g, String i, ToolAppCtrl& c) {
	ListItem& it = items.GetAdd(g).Add();
	it.item = i;
	it.ctrl = &c;
}

void ToolEditor::InitSimplified() {
	AddItem(t_("Tools"), t_("AI Image Generator"), image_gen);
	
	AddItem(t_("Database"), "Components", src_data);
	AddItem(t_("Database"), t_("Tokens"), tokens_data);
	AddItem(t_("Database"), t_("Token phrases"), token_phrases);
	AddItem(t_("Database"), t_("Words"), component_words);
	AddItem(t_("Database"), t_("Ambiguous word pairs"), ambiguous_word_pairs);
	AddItem(t_("Database"), t_("Virtual phrases"), virtual_phrases);
	AddItem(t_("Database"), t_("Virtual phrase parts"), virtual_phrase_parts);
	AddItem(t_("Database"), t_("Virtual phrase structs"), virtual_phrase_structs);
	AddItem(t_("Database"), t_("Phrase parts"), phrase_parts);
	AddItem(t_("Database"), t_("Phrase part analysis 1"), phrase_part_analysis);
	AddItem(t_("Database"), t_("Phrase part analysis 2"), phrase_part_analysis2);
	AddItem(t_("Database"), t_("Rhyme container"), rhyme_container);
	AddItem(t_("Database"), t_("Action attributes"), action_attrs);
	AddItem(t_("Database"), t_("Action parallels"), action_parallels);
	AddItem(t_("Database"), t_("Action transitions"), action_transitions);
	AddItem(t_("Database"), t_("Wordnet"), component_wordnet);
	AddItem(t_("Database"), t_("Attributes"), db_attrs);
	AddItem(t_("Database"), t_("Diagnostics"), db_diagnostics);
	
	AddItem("Entity", t_("Info"), entity_info);
	
	AddItem("Snapshot", t_("Info"), snap_info);
	AddItem("Snapshot", t_("Briefing"), snap_briefing);
	AddItem("Snapshot", t_("Idea notepad"), snap_ideas);
	
	AddItem("Component", t_("Info"), comp_info);
	
	AddItem("Script", t_("Info"), script_info);
	AddItem("Script", t_("Structure"), comp_struct);
	AddItem("Script", t_("Pool"), script_pool);
	AddItem("Script", t_("Script solver"), scripts_solver);
	
}

void ToolEditor::Init() {
	INHIBIT_ACTION_(page_group_list, 0);
	INHIBIT_ACTION_(page_list, 1);
	LoadLast();
	page_group_list.SetCursor(page_group);
	int page = this->page.GetAdd(page_group, 0);
	page_list.SetCursor(page);
	SetView(page_group, page);
	Data();
	app.SetBar(); // requires Data();
}

void ToolEditor::SetView(int i, int j) {
	for (const auto& v : items)
		for (const ListItem& it : v)
			it.ctrl->Hide();
	
	parts.Enable();
	
	WhenStopUpdating();
	
	if (i >= 0 && i < items.GetCount() && j >= 0 && j < items[i].GetCount())
		items[i][j].ctrl->Show();
	
	// Save 'cookie' about last viewed page
	page_group = i;
	page.GetAdd(i) = j;
	
	if (page_group == items.Find("Script"))
		SetSubMenu(1);
	else
		SetSubMenu(0);
	
	DataPage();
	
	app.SetBar();
}

void ToolEditor::DataPage() {
	if (app.skip_data || !appmode_list.IsCursor()) return;
	
	StoreLast();
	
	int appmode = appmode_list.Get("IDX");
	EnterAppMode(appmode);
	
	int page = this->page.GetAdd(page_group, 0);
	try {
		if (page_group >= 0 && page_group < items.GetCount() && page >= 0 && page < items[page_group].GetCount())
			items[page_group][page].ctrl->Data();
	}
	catch (NoPointerExc e) {
		LOG("error: " << e);
	}
	
	LeaveAppMode();
}

void ToolEditor::ToolMenu(Bar& bar) {
	int page = this->page.GetAdd(page_group, 0);
	if (page_group >= 0 && page_group < items.GetCount() && page >= 0 && page < items[page_group].GetCount())
		items[page_group][page].ctrl->ToolMenu(bar);
}

String ToolEditor::GetStatusText() {
	int page = this->page.GetAdd(page_group, 0);
	if (page_group >= 0 && page_group < items.GetCount() && page >= 0 && page < items[page_group].GetCount())
		return items[page_group][page].ctrl->GetStatusText();
	else
		return String();
}

void ToolEditor::MovePageGroup(int d) {
	if (page_group_list.IsCursor()) {
		int c = page_group_list.GetCursor();
		c += d;
		if (c >= 0 && c < page_group_list.GetCount())
			page_group_list.SetCursor(c);
	}
}

void ToolEditor::MovePage(int d) {
	if (page_list.IsCursor()) {
		int c = page_list.GetCursor();
		c += d;
		if (c >= 0 && c < page_list.GetCount())
			page_list.SetCursor(c);
	}
}

void ToolEditor::MovePart(int d) {
	if (parts.IsCursor()) {
		int c = parts.GetCursor();
		c += d;
		if (c >= 0 && c < parts.GetCount())
			parts.SetCursor(c);
	}
}

void ToolEditor::LoadLast() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	p.Zero();
	for (Entity& a : db.entities) {
		for (Typeclass& a : a.typeclasses) {
			if (a.file_title == app.last_entity) {
				p.typecast = &a;
				for (Content& r : a.contents) {
					if (r.file_title == app.last_snapshot) {
						p.archetype = &r;
						for (Script& s : r.scripts) {
							if (s.file_title == app.last_component) {
								p.script = &s;
								for (StaticPart& part : s.parts) {
									if (part.name == app.last_part) {
										p.part = &part;
										break;
									}
								}
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}
		if (a.file_title == app.last_entity) {
			p.entity = &a;
			for (Snapshot& r : a.snaps) {
				if (r.file_title == app.last_snapshot) {
					p.release = &r;
					for (Component& s : r.components) {
						if (s.file_title == app.last_component) {
							p.component = &s;
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}
}

void ToolEditor::StoreLast() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	app.last_typeclass = p.typecast ? p.typecast->file_title : String();
	app.last_content = p.archetype ? p.archetype->file_title : String();
	app.last_scripts = p.script ? p.script->file_title : String();
	app.last_part = p.part ? p.part->name : String();
	app.last_entity = p.entity ? p.entity->file_title : String();
	app.last_snapshot = p.release ? p.release->file_title : String();
	app.last_component = p.component ? p.component->file_title : String();
	app.Store();
}

void ToolEditor::SwitchAppMode() {
	EnterAppMode(GetAppMode());
	
	page_group_list.Clear();
	
	for(int i = 0; i < items.GetCount(); i++) {
		String group = items.GetKey(i);
		if      (group == "Entity") group = GetAppModeKeyCap(AM_ENTITY);
		else if (group == "Snapshot") group = GetAppModeKeyCap(AM_SNAPSHOT);
		else if (group == "Component") group = GetAppModeKeyCap(AM_COMPONENT);
		else if (group == "Script") group = GetAppModeKeyCap(AM_SCRIPT);
		page_group_list.Add(group);
	}
	INHIBIT_ACTION(page_group_list);
	if (page_group_list.GetCount() && !page_group_list.IsCursor())
		page_group_list.SetCursor(0);
	
	
	entities	.ColumnAt(0).HeaderTab().SetText(GetAppModeKeyCapN(AM_ENTITY));
	snaps		.ColumnAt(0).HeaderTab().SetText(GetAppModeKeyCapN(AM_SNAPSHOT));
	components	.ColumnAt(0).HeaderTab().SetText(GetAppModeKeyCapN(AM_COMPONENT));
	
	LeaveAppMode();
	
	ViewPageGroup();
}

void ToolEditor::ViewPageGroup() {
	int page_group = page_group_list.GetCursor();
	int page = this->page.GetAdd(page_group, 0);
	
	if (!page_group_list.IsCursor() || page_group < 0 || page_group >= items.GetCount()) {
		return;
	}
	
	EnterAppMode(GetAppMode());
	
	const auto& v = items[page_group];
	for(int j = 0; j < v.GetCount(); j++) {
		const ListItem& it = v[j];
		
		// Rename app-mode keyed pages
		String s = it.item;
		if (s == "Components") s = GetAppModeKeyCapN(AM_COMPONENT);
		
		page_list.Set(j, 0, it.item);
		base.Add(it.ctrl->SizePos());
	}
	INHIBIT_ACTION(page_list);
	page_list.SetCount(v.GetCount());
	page_list.SetCursor(page);
	
	if (page >= v.GetCount())
		page = v.GetCount()-1;
	
	LeaveAppMode();
	
	SetView(page_group, page);
	DataPage();
}

void ToolEditor::ViewPage() {
	SetView(page_group_list.GetCursor(), page_list.GetCursor());
	//DataPage(); // Duplicate
}

void ToolEditor::Data() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	
	for(int i = 0; i < db.entities.GetCount(); i++) {
		Entity& a = db.entities[i];
		entities.Set(i, 0, a.native_name);
	}
	INHIBIT_ACTION(entities);
	entities.SetCount(db.entities.GetCount());
	
	int cursor = max(0, p.GetActiveEntityIndex());
	if (cursor >= 0 && cursor < entities.GetCount())
		entities.SetCursor(cursor);
	
	DataEntity();
}

void ToolEditor::DataEntity() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!entities.IsCursor()) {
		p.entity = 0;
		p.release = 0;
		p.component = 0;
		p.part = 0;
		DataPage();
		return;
	}
	
	
	// Song part of the artist
	p.entity = &db.entities[entities.GetCursor()];
	Entity& a = *p.entity;
	
	for(int i = 0; i < a.snaps.GetCount(); i++) {
		Snapshot& r = a.snaps[i];
		snaps.Set(i, 0, r.native_title);
		snaps.Set(i, 1, r.date);
	}
	INHIBIT_ACTION(snaps);
	snaps.SetCount(a.snaps.GetCount());
	
	int cursor = max(0, p.GetActiveSnapshotIndex());
	if (cursor >= 0 && cursor < snaps.GetCount())
		snaps.SetCursor(cursor);
	
	
	// Script part of the artist
	a.RealizeTypeclasses();
	const auto& tcs = GetTypeclasses(GetAppMode());
	for(int i = 0; i < tcs.GetCount(); i++) {
		const String& tc = tcs[i];
		typeclasses.Set(i, "IDX", i);
		typeclasses.Set(i, 0, tc);
		typeclasses.Set(i, 1, a.typeclasses[i].GetScriptCount());
	}
	INHIBIT_ACTION_(typeclasses, tc);
	typeclasses.SetCount(tcs.GetCount());
	typeclasses.SetSortColumn(1, true);
	
	cursor = max(0, p.GetActiveTypeclassIndex());
	if (cursor >= 0 && cursor < typeclasses.GetCount())
		SetIndexCursor(typeclasses, cursor);


	DataSnapshot();
	DataTypeclass();
}

void ToolEditor::DataSnapshot() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!snaps.IsCursor() || !p.entity) {
		p.release = 0;
		p.component = 0;
		p.part = 0;
		DataPage();
		return;
	}
	
	p.release = &p.entity->snaps[snaps.GetCursor()];
	Entity& a = *p.entity;
	Snapshot& r = *p.release;
	
	for(int i = 0; i < r.components.GetCount(); i++) {
		Component& s = r.components[i];
		components.Set(i, 0, s.GetAnyTitle(a));
		/*components.Set(i, 0, s.native_title);
		components.Set(i, 1, s.prj_name);
		components.Set(i, 2, s.artist);*/
	}
	INHIBIT_ACTION(components);
	components.SetCount(r.components.GetCount());
	
	int cursor = max(0, p.GetActiveComponentIndex());
	if (cursor >= 0 && cursor < components.GetCount())
		components.SetCursor(cursor);
	
	DataComponent();
}

void ToolEditor::DataComponent() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!components.IsCursor() || !p.entity || !p.release) {
		p.component = 0;
		DataPage();
		return;
	}
	
	p.component = &p.release->components[components.GetCursor()];
	Entity& a = *p.entity;
	Snapshot& r = *p.release;
	Component& s = *p.component;
	
	
	/*
	for(int i = 0; i < s.parts.GetCount(); i++) {
		String k;
		int c = 0;
		Color clr = White();
		StaticPart& p = s.parts[i];
		k = p.name;
		clr = GetComponentPartPaperColor(p.type);
		c = p.rhymes.GetCount();
		parts.Set(i, 0, AttrText(k).NormalPaper(clr));
		parts.Set(i, 1, c);
	}
	INHIBIT_ACTION(parts);
	parts.SetCount(s.parts.GetCount());
	
	int cursor = 0;
	if (cursor >= 0 && cursor < parts.GetCount() && !parts.IsCursor())
		parts.SetCursor(cursor);*/
	
	DataPage();
}

void ToolEditor::DataTypeclass() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!typeclasses.IsCursor()) {
		p.typecast = 0;
		p.archetype = 0;
		p.script = 0;
		DataPage();
		return;
	}
	
	Entity& a = *p.entity;
	a.RealizeTypeclasses();
	p.typecast = &a.typeclasses[typeclasses.Get("IDX")];
	Typeclass& t = *p.typecast;
	
	const auto& cons = GetContents(GetAppMode());
	for(int i = 0; i < cons.GetCount(); i++) {
		const auto& con = cons[i];
		contents.Set(i, "IDX", i);
		contents.Set(i, 0, con.key);
		contents.Set(i, 1, t.contents[i].scripts.GetCount());
	}
	INHIBIT_CURSOR(contents);
	contents.SetSortColumn(1, true);
	
	int cursor = max(0, p.GetActiveContentIndex());
	if (cursor >= 0 && cursor < contents.GetCount())
		SetIndexCursor(contents, cursor);

	DataContent();
}

void ToolEditor::DataContent() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!contents.IsCursor()) {
		p.archetype = 0;
		p.script = 0;
		DataPage();
		return;
	}
	
	Typeclass& t = *p.typecast;
	p.archetype = &t.contents[contents.Get("IDX")];
	Content& a = *p.archetype;
	
	for(int i = 0; i < a.scripts.GetCount(); i++) {
		const Script& l = a.scripts[i];
		scripts.Set(i, "IDX", i);
		scripts.Set(i, 0, l.GetAnyTitle());
	}
	INHIBIT_CURSOR(scripts);
	scripts.SetCount(a.scripts.GetCount());
	
	int cursor = max(0, p.GetActiveScriptIndex());
	if (cursor >= 0 && cursor < scripts.GetCount())
		SetIndexCursor(scripts, cursor);

	DataScript();
}

void ToolEditor::DataScript() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!scripts.IsCursor()) {
		p.script = 0;
		DataPage();
		return;
	}
	
	Typeclass& t = *p.typecast;
	Content& a = *p.archetype;
	p.script = &a.scripts[scripts.GetCursor()];
	Script& l = *p.script;
	
	
	DataPage();
}

#if 0
void ToolEditor::DataPart() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!parts.IsCursor() || !p.entity || !p.release || !p.component /*|| !p.component->pipe*/) {
		DataPage();
		return;
	}
	
	// OLD!!
	/*
	PipePtrs& pp = p.component->pipe->p;
	
	int cursor = parts.GetCursor();
	if (!cursor) {
		db.ctx.active_wholesong = true;
		pp.part = 0;
	}
	else {
		db.ctx.active_wholesong = false;
		pp.part = &e.parts[cursor-1];
	}
	
	int part_i = pp.GetActivePartIndex();
	if (part_i >= 0 && part_i < parts.GetCount() && !parts.IsCursor())
		parts.SetCursor(1+part_i);
	*/
	
	Component& song = *p.component;
	int cursor = parts.GetCursor();
	
	db.ctx.active_wholesong = false;
	p.part = &song.parts[cursor];
	
	int part_i = p.GetActivePartIndex();
	if (part_i >= 0 && part_i < parts.GetCount() && !parts.IsCursor())
		parts.SetCursor(part_i);
	
	
	DataPage();
}
#endif

void ToolEditor::EntityMenu(Bar& bar) {
	bar.Add(t_("Add Entity"), THISBACK(AddEntity));
	
	if (entities.IsCursor()) {
		bar.Add(t_("Rename Entity"), THISBACK(RenameEntity));
		bar.Add(t_("Delete Entity"), THISBACK(RemoveEntity));
	}
}

void ToolEditor::SnapshotMenu(Bar& bar) {
	bar.Add(t_("Add Snapshot"), THISBACK(AddSnapshot));
	
	if (snaps.IsCursor()) {
		bar.Add(t_("Rename Snapshot"), THISBACK(RenameSnapshot));
		bar.Add(t_("Delete Snapshot"), THISBACK(RemoveSnapshot));
	}
}

void ToolEditor::SongMenu(Bar& bar) {
	bar.Add(t_("Add Song"), THISBACK(AddSong));
	
	if (components.IsCursor()) {
		bar.Add(t_("Rename Song"), THISBACK(RenameSong));
		bar.Add(t_("Delete Song"), THISBACK(RemoveSong));
	}
}

void ToolEditor::ScriptMenu(Bar& bar) {
	bar.Add(t_("Add Script"), THISBACK(AddScript));
	
	if (scripts.IsCursor()) {
		bar.Add(t_("Delete Script"), THISBACK(RemoveScript));
	}
}

void ToolEditor::AddEntity() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	
	String name;
	bool b = EditTextNotNull(
		name,
		t_("Add Entity"),
		t_("Entity's English name"),
		0
	);
	if (!b) return;
	
	int artist_i = -1;
	for(int i = 0; i < db.entities.GetCount(); i++) {
		Entity& a = db.entities[i];
		if (a.english_name == name) {
			artist_i = i;
			break;
		}
	}
	if (artist_i >= 0) {
		/*if (!PromptYesNo(DeQtf(Format(t_("Do you want to replace file for artist '%s'"), name)))
			return;*/
		PromptOK(DeQtf(t_("Entity exist already")));
		return;
	}
	
	Entity& a = db.entities.Add();
	a.file_title = MakeTitle(name);
	a.english_name = name;
	p.entity = &a;
	
	Data();
}

void ToolEditor::RenameEntity() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.entity)
		return;
	
	String name;
	bool b = EditTextNotNull(
		name,
		t_("Rename Entity"),
		t_("Entity's English name"),
		0
	);
	if (!b) return;
	
	p.entity->english_name = name;
	
	Data();
}

void ToolEditor::RemoveEntity() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.entity)
		return;
	int idx = p.GetActiveEntityIndex();
	if (idx < 0) return;
	db.entities.Remove(idx);
	Data();
}

void ToolEditor::AddSnapshot() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.entity)
		return;
	Entity& a = *p.entity;
	
	String title;
	bool b = EditTextNotNull(
		title,
		t_("Add Snapshot"),
		t_("Snapshot's English title"),
		0
	);
	if (!b) return;
	
	int rel_i = -1;
	for(int i = 0; i < a.snaps.GetCount(); i++) {
		Snapshot& r = a.snaps[i];
		if (r.english_title == title) {
			rel_i = i;
			break;
		}
	}
	if (rel_i >= 0) {
		PromptOK(DeQtf(t_("Snapshot exist already")));
		return;
	}
	
	Snapshot& r = a.snaps.Add();
	r.file_title = MakeTitle(title);
	r.english_title = title;
	p.release = &r;
	
	DataEntity();
}

void ToolEditor::RenameSnapshot() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.release)
		return;
	
	String title;
	bool b = EditTextNotNull(
		title,
		t_("Rename Snapshot"),
		t_("Snapshot's English title"),
		0
	);
	if (!b) return;
	
	p.release->english_title = title;
	
	DataEntity();
}

void ToolEditor::RemoveSnapshot() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.entity || !p.release)
		return;
	int idx = p.GetActiveSnapshotIndex();
	if (idx < 0) return;
	p.entity->snaps.Remove(idx);
	DataEntity();
}

void ToolEditor::AddSong() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.entity)
		return;
	Entity& a = *p.entity;
	Snapshot& r = *p.release;
	
	String title;
	bool b = EditTextNotNull(
		title,
		t_("Add Song"),
		t_("Song's English title"),
		0
	);
	if (!b) return;
	
	TODO
	/*
	int rel_i = -1;
	for(int i = 0; i < r.components.GetCount(); i++) {
		Component& s = r.components[i];
		if (s.english_title == title) {
			rel_i = i;
			break;
		}
	}
	if (rel_i >= 0) {
		PromptOK(DeQtf(t_("Song exist already")));
		return;
	}
	
	Component& s = r.components.Add();
	s.file_title = MakeTitle(title);
	s.english_title = title;
	p.component = &s;
	*/
	DataEntity();
}

void ToolEditor::RenameSong() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.component)
		return;
	
	String title;
	bool b = EditTextNotNull(
		title,
		t_("Rename Song"),
		t_("Song's English title"),
		0
	);
	if (!b) return;
	
	
	TODO //p.component->english_title = title.ToString();
	
	DataSnapshot();
}

void ToolEditor::RemoveSong() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.component || !p.release)
		return;
	int idx = p.GetActiveComponentIndex();
	if (idx < 0) return;
	p.release->components.Remove(idx);
	p.component = 0;
	DataSnapshot();
}

void ToolEditor::AddScript() {
	TextDatabase& db = GetDatabase();
	EditorPtrs& p = GetPointers();
	if (!p.archetype)
		return;
	Typeclass& t = *p.typecast;
	Content& a = *p.archetype;
	
	int t_i = p.GetActiveTypeclassIndex();
	int a_i = p.GetActiveContentIndex();
	
	String title;
	for(int i = 0; i < 8; i++) {
		title.Cat('a' + Random('z' - 'a' + 1));
	}
	
	Script& l = a.scripts.Add();
	l.file_title = MakeTitle(title);
	l.typeclass = t_i;
	l.content = a_i;
	p.script = &l;
	
	Data();
}

void ToolEditor::RemoveScript() {
	
	
	
}

TextDatabase& ToolEditor::GetDatabase() {
	ASSERT(appmode_list.IsCursor());
	int appmode = this->appmode_list.Get("IDX");
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return MetaDatabase::Single().db[appmode];
}

EditorPtrs& ToolEditor::GetPointers() {
	ASSERT(appmode_list.IsCursor());
	int appmode = this->appmode_list.Get("IDX");
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return MetaPtrs::Single().db[appmode];
}

int ToolEditor::GetAppMode() const {
	ASSERT(appmode_list.IsCursor());
	int appmode = this->appmode_list.Get("IDX");
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return appmode;
}

TextLib::TaskManager& ToolAppCtrl::GetTaskManager() {
	EditorPtrs& p = GetPointers();
	int appmode = -1;
	if (p.editor)
		appmode = p.editor->GetAppMode();
	else
		appmode = GetAnyEditor().GetAppMode();
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return TextLib::TaskManager::Single(appmode);
}

int ToolAppCtrl::GetAppMode() const {
	EditorPtrs& p = GetPointers();
	if (p.editor)
		return p.editor->GetAppMode();
	return GetAnyEditor().GetAppMode();
}


END_TEXTLIB_NAMESPACE
