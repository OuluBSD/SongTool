#include "TextTool.h"


BEGIN_TEXTLIB_NAMESPACE


LeadsCtrl::LeadsCtrl(TextTool* app) : ToolEditorBase("leads", *app) {
	MetaPtrs::Single().leads = this;
	
	Add(hsplit.SizePos());
	
	hsplit.Horz() << menusplit << base;
	hsplit.SetPos(1000);
	
	menusplit.Vert() << page_group_list << page_list << owners << profiles;
	
	owners.AddColumn(t_("Owner"));
	owners <<= THISBACK(DataOwner);
	owners.WhenBar << THISBACK(OwnerMenu);
	
	profiles.AddColumn(t_("Profile"));
	profiles <<= THISBACK(DataProfile);
	profiles.WhenBar << THISBACK(ProfileMenu);
	
}

void LeadsCtrl::InitSimplified() {
	AddItem(t_("Tools"), t_("AI Image Generator"), image_gen);
	AddItem(t_("Database"), t_("Lead Websites"), lead_sites);
	AddItem(t_("Profile"), t_("Owner Info"), owner_info);
	AddItem(t_("Profile"), t_("Profile Info"), profile_info);
	AddItem(t_("Social Media"), t_("Content"), some_content);
	
}

void LeadsCtrl::Init() {
	UpdatePageList();
	
	ToolEditorBase::Init();
	
	
}

void LeadsCtrl::Data() {
	MetaDatabase& mdb = MetaDatabase::Single();
	MetaPtrs& p = MetaPtrs::Single();
	
	for(int i = 0; i < mdb.owners.GetCount(); i++) {
		owners.Set(i, 0, mdb.owners[i].name);
	}
	INHIBIT_CURSOR(owners);
	owners.SetCount(mdb.owners.GetCount());
	if (!owners.IsCursor() && owners.GetCount())
		owners.SetCursor(0);
	
	
	DataPage();
}

void LeadsCtrl::DataOwner() {
	MetaDatabase& mdb = MetaDatabase::Single();
	MetaPtrs& p = MetaPtrs::Single();
	if (!owners.IsCursor())
		return;
	
	int owner_i = owners.GetCursor();
	Owner& owner = mdb.owners[owner_i];
	p.owner = owners.IsCursor() ? &mdb.owners[owners.GetCursor()] : 0;
	
	for(int i = 0; i < owner.profiles.GetCount(); i++) {
		profiles.Set(i, 0, owner.profiles[i].name);
	}
	INHIBIT_CURSOR(profiles);
	profiles.SetCount(owner.profiles.GetCount());
	if (!profiles.IsCursor() && profiles.GetCount())
		profiles.SetCursor(0);
	
	DataProfile();
}

void LeadsCtrl::DataProfile() {
	MetaDatabase& mdb = MetaDatabase::Single();
	MetaPtrs& p = MetaPtrs::Single();
	if (!owners.IsCursor())
		return;
	
	int owner_i = owners.GetCursor();
	Owner& owner = mdb.owners[owner_i];
	p.profile = profiles.IsCursor() ? &owner.profiles[profiles.GetCursor()] : 0;
	
	DataPage();
}

void LeadsCtrl::OwnerMenu(Bar& bar) {
	bar.Add(t_("Add Owner"), THISBACK(AddOwner));
	
	if (owners.IsCursor()) {
		bar.Add(t_("Delete Owner"), THISBACK(RemoveOwner));
	}
}

void LeadsCtrl::ProfileMenu(Bar& bar) {
	bar.Add(t_("Add Profile"), THISBACK(AddProfile));
	
	if (profiles.IsCursor()) {
		bar.Add(t_("Delete Profile"), THISBACK(RemoveProfile));
	}
}

void LeadsCtrl::AddOwner() {
	MetaDatabase& mdb = MetaDatabase::Single();
	MetaPtrs& p = MetaPtrs::Single();
	
	String name;
	bool b = EditTextNotNull(
		name,
		t_("Add Owner"),
		t_("Owner's name"),
		0
	);
	if (!b) return;
	
	String t = MakeTitle(name);
	int artist_i = -1;
	for(int i = 0; i < mdb.owners.GetCount(); i++) {
		Owner& a = mdb.owners[i];
		if (a.file_title == t) {
			artist_i = i;
			break;
		}
	}
	if (artist_i >= 0) {
		/*if (!PromptYesNo(DeQtf(Format(t_("Do you want to replace file for artist '%s'"), name)))
			return;*/
		PromptOK(DeQtf(t_("Owner exist already")));
		return;
	}
	
	Owner& a = mdb.owners.Add();
	a.file_title = t;
	a.name = name;
	p.owner = &a;
	
	Data();
}

void LeadsCtrl::AddProfile() {
	MetaDatabase& mdb = MetaDatabase::Single();
	MetaPtrs& p = MetaPtrs::Single();
	if (!owners.IsCursor())
		return;
	
	int owner_i = owners.GetCursor();
	Owner& owner = mdb.owners[owner_i];
	
	String name;
	bool b = EditTextNotNull(
		name,
		t_("Add Profile"),
		t_("Profile's name"),
		0
	);
	if (!b) return;
	
	int artist_i = -1;
	for(int i = 0; i < owner.profiles.GetCount(); i++) {
		Profile& a = owner.profiles[i];
		if (a.name == name) {
			artist_i = i;
			break;
		}
	}
	if (artist_i >= 0) {
		/*if (!PromptYesNo(DeQtf(Format(t_("Do you want to replace file for artist '%s'"), name)))
			return;*/
		PromptOK(DeQtf(t_("Profile exist already")));
		return;
	}
	
	Profile& a = owner.profiles.Add();
	a.name = name;
	p.profile = &a;
	
	DataOwner();
}

void LeadsCtrl::RemoveOwner() {
	MetaDatabase& mdb = MetaDatabase::Single();
	MetaPtrs& p = MetaPtrs::Single();
	if (owners.IsCursor()) {
		mdb.owners.Remove(owners.GetCursor());
	}
	Data();
}

void LeadsCtrl::RemoveProfile() {
	if (!owners.IsCursor())
		return;
	
	MetaDatabase& mdb = MetaDatabase::Single();
	int owner_i = owners.GetCursor();
	Owner& owner = mdb.owners[owner_i];
	
	MetaPtrs& p = MetaPtrs::Single();
	if (profiles.IsCursor()) {
		owner.profiles.Remove(profiles.GetCursor());
	}
	DataOwner();
}

void LeadsCtrl::SetSubMenu(int i) {
	
}


END_TEXTLIB_NAMESPACE
	