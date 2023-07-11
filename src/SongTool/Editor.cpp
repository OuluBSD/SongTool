#include "SongTool.h"


Editor::Editor() {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << menusplit << base;
	hsplit.SetPos(1500);
	
	menusplit.Vert() << tablist << artists << releases << songs << parts;
	artists.WhenBar << THISBACK(ArtistMenu);
	releases.WhenBar << THISBACK(ReleaseMenu);
	songs.WhenBar << THISBACK(SongMenu);
	parts.WhenBar << THISBACK(PartMenu);
	tablist <<= THISBACK(UpdateView);
	importer.WhenStructureChange << THISBACK(DataSong);
	
	tablist.NoHeader();
	tablist.AddColumn("");
	tablist.AddColumn("");
	tablist.ColumnWidths("1 3");
	tablist.Add(t_("All"), t_("Info"));
	tablist.Add(t_("Release"), t_("Recruitment"));
	tablist.Add(t_("Release"), t_("Social media campaign"));
	tablist.Add(t_("Release"), t_("Calendar"));
	tablist.Add(t_("Release"), t_("Tasks"));
	tablist.Add(t_("Song"), t_("Importer"));
	tablist.Add(t_("Song"), t_("Pattern mask"));
	tablist.Add(t_("Song"), t_("Pattern"));
	tablist.Add(t_("Song"), t_("Attribute scoring"));
	tablist.Add(t_("Song"), t_("Scoring"));
	tablist.Add(t_("Song"), t_("Reverse"));
	tablist.Add(t_("Song"), t_("Composition"));
	tablist.Add(t_("Song"), t_("Analysis"));
	tablist.Add(t_("Song"), t_("Production"));
	tablist.Add(t_("Song"), t_("Rhymes"));
	tablist.SetCursor(0);
	
	artists.AddColumn(t_("Artist"));
	artists <<= THISBACK(DataArtist);
	
	releases.AddColumn(t_("Title"));
	releases.AddColumn(t_("Date"));
	releases.ColumnWidths("3 1");
	releases <<= THISBACK(DataRelease);
	
	songs.AddColumn(t_("Artist"));
	songs.AddColumn(t_("Title"));
	songs.AddColumn(t_("Project name"));
	songs <<= THISBACK(DataSong);
	
	parts.AddColumn(t_("Part"));
	parts.AddColumn(t_("Lines"));
	parts.ColumnWidths("3 1");
	parts <<= THISBACK(DataPart);
	
	info.editor = this;
	
	base.Add(info.SizePos());
	base.Add(recru.SizePos());
	base.Add(social.SizePos());
	base.Add(cal.SizePos());
	base.Add(task.SizePos());
	base.Add(importer.SizePos());
	base.Add(patmask.SizePos());
	base.Add(pattern.SizePos());
	base.Add(attrscore.SizePos());
	base.Add(scoring.SizePos());
	base.Add(composition.SizePos());
	base.Add(analysis.SizePos());
	base.Add(production.SizePos());
	base.Add(rhymes.SizePos());
}

void Editor::Init() {
	tablist.SetCursor(page);
	SetView(page);
}

void Editor::SetView(int i) {
	info.Hide();
	recru.Hide();
	social.Hide();
	cal.Hide();
	task.Hide();
	importer.Hide();
	patmask.Hide();
	pattern.Hide();
	attrscore.Hide();
	scoring.Hide();
	reverse.Hide();
	composition.Hide();
	analysis.Hide();
	production.Hide();
	rhymes.Hide();
	
	switch (i) {
		default: i = 0;
		case 0: info.Show(); break;
		case 1: recru.Show(); break;
		case 2: social.Show(); break;
		case 3: cal.Show(); break;
		case 4: task.Show(); break;
		case 5: importer.Show(); break;
		case 6: patmask.Show(); break;
		case 7: pattern.Show(); break;
		case 8: attrscore.Show(); break;
		case 9: scoring.Show(); break;
		case 10: reverse.Show(); break;
		case 11: composition.Show(); break;
		case 12: analysis.Show(); break;
		case 13: production.Show(); break;
		case 14: rhymes.Show(); break;
	}
	page = i;
	DataPage();
}

void Editor::DataPage() {
	switch (page) {
		case 0: info.Data(); break;
		case 1: recru.Data(); break;
		case 2: social.Data(); break;
		case 3: cal.Data(); break;
		case 4: task.Data(); break;
		case 5: importer.Data(); break;
		case 6: patmask.Data(); break;
		case 7: pattern.Data(); break;
		case 8: attrscore.Data(); break;
		case 9: scoring.Data(); break;
		case 10: reverse.Data(); break;
		case 11: composition.Data(); break;
		case 12: analysis.Data(); break;
		case 13: production.Data(); break;
		case 14: rhymes.Data(); break;
		default: break;
	}
}

void Editor::UpdateView() {
	SetView(tablist.GetCursor());
	DataPage();
}

void Editor::Data() {
	Database& db = Database::Single();
	
	for(int i = 0; i < db.artists.GetCount(); i++) {
		Artist& a = db.artists[i];
		artists.Set(i, 0, a.name);
	}
	artists.SetCount(db.artists.GetCount());
	
	int cursor = max(0, db.GetActiveArtistIndex());
	if (cursor >= 0 && cursor < artists.GetCount())
		artists.SetCursor(cursor);
	
	DataArtist();
}

void Editor::DataArtist() {
	Database& db = Database::Single();
	if (!artists.IsCursor())
		return;
	
	db.active_artist = &db.artists[artists.GetCursor()];
	Artist& a = *db.active_artist;
	
	for(int i = 0; i < a.releases.GetCount(); i++) {
		Release& r = a.releases[i];
		releases.Set(i, 0, r.title);
		releases.Set(i, 1, r.date);
	}
	releases.SetCount(a.releases.GetCount());
	
	int cursor = max(0, db.GetActiveReleaseIndex());
	if (cursor >= 0 && cursor < releases.GetCount())
		releases.SetCursor(cursor);
	
	DataRelease();
}

void Editor::DataRelease() {
	Database& db = Database::Single();
	if (!releases.IsCursor() || !db.active_artist)
		return;
	
	db.active_release = &db.active_artist->releases[releases.GetCursor()];
	Artist& a = *db.active_artist;
	Release& r = *db.active_release;
	
	for(int i = 0; i < r.songs.GetCount(); i++) {
		Song& s = r.songs[i];
		songs.Set(i, 0, s.artist);
		songs.Set(i, 1, s.title);
		songs.Set(i, 2, s.prj_name);
	}
	songs.SetCount(r.songs.GetCount());
	
	int cursor = max(0, db.GetActiveSongIndex());
	if (cursor >= 0 && cursor < songs.GetCount())
		songs.SetCursor(cursor);
	
	DataSong();
}

void Editor::DataSong() {
	Database& db = Database::Single();
	if (!songs.IsCursor() || !db.active_artist || !db.active_release)
		return;
	
	db.active_song = &db.active_release->songs[songs.GetCursor()];
	Artist& a = *db.active_artist;
	Release& r = *db.active_release;
	Song& s = *db.active_song;
	
	for(int i = 0; i < s.parts.GetCount(); i++) {
		Part& p = s.parts[i];
		String k = s.parts.GetKey(i);
		parts.Set(i, 0, k);
		parts.Set(i, 1, p.lines.GetCount());
	}
	parts.SetCount(s.parts.GetCount());
	
	int cursor = db.GetActivePartIndex();
	if (cursor >= 0 && cursor < songs.GetCount() && !songs.IsCursor())
		songs.SetCursor(cursor);
	
	DataPage();
	DataPart();
}

void Editor::DataPart() {
	Database& db = Database::Single();
	if (!parts.IsCursor() || !db.active_artist || !db.active_release || !db.active_song)
		return;
	
	db.active_part = &db.active_song->parts[parts.GetCursor()];
	
	int cursor = db.GetActivePartIndex();
	if (cursor >= 0 && cursor < parts.GetCount() && !parts.IsCursor())
		parts.SetCursor(cursor);
	
}

void Editor::ArtistMenu(Bar& bar) {
	bar.Add(t_("Add Artist"), THISBACK(AddArtist));
	
	if (artists.IsCursor()) {
		bar.Add(t_("Rename Artist"), THISBACK(RenameArtist));
		bar.Add(t_("Delete Artist"), THISBACK(RemoveArtist));
	}
}

void Editor::ReleaseMenu(Bar& bar) {
	bar.Add(t_("Add Release"), THISBACK(AddRelease));
	
	if (releases.IsCursor()) {
		bar.Add(t_("Rename Release"), THISBACK(RenameRelease));
		bar.Add(t_("Delete Release"), THISBACK(RemoveRelease));
	}
}

void Editor::SongMenu(Bar& bar) {
	bar.Add(t_("Add Song"), THISBACK(AddSong));
	
	if (songs.IsCursor()) {
		bar.Add(t_("Rename Song"), THISBACK(RenameSong));
		bar.Add(t_("Delete Song"), THISBACK(RemoveSong));
	}
}

void Editor::PartMenu(Bar& bar) {
	
}

void Editor::AddArtist() {
	Database& db = Database::Single();
	
	String name;
	bool b = EditTextNotNull(
		name,
		t_("Add Artist"),
		t_("Artist's name"),
		0
	);
	if (!b) return;
	
	int artist_i = -1;
	for(int i = 0; i < db.artists.GetCount(); i++) {
		Artist& a = db.artists[i];
		if (a.name == name) {
			artist_i = i;
			break;
		}
	}
	if (artist_i >= 0) {
		/*if (!PromptYesNo(DeQtf(Format(t_("Do you want to replace file for artist '%s'"), name)))
			return;*/
		PromptOK(DeQtf(t_("Artist exist already")));
		return;
	}
	
	Artist& a = db.artists.Add();
	a.file_title = MakeTitle(name);
	a.name = name;
	db.active_artist = &a;
	
	Data();
}

void Editor::RenameArtist() {
	Database& db = Database::Single();
	if (!db.active_artist)
		return;
	
	String name;
	bool b = EditTextNotNull(
		name,
		t_("Rename Artist"),
		t_("Artist's name"),
		0
	);
	if (!b) return;
	
	db.active_artist->name = name;
	
	Data();
}

void Editor::RemoveArtist() {
	Database& db = Database::Single();
	if (!db.active_artist)
		return;
	int idx = db.GetActiveArtistIndex();
	if (idx < 0) return;
	db.artists.Remove(idx);
	Data();
}

void Editor::AddRelease() {
	Database& db = Database::Single();
	if (!db.active_artist)
		return;
	Artist& a = *db.active_artist;
	
	String title;
	bool b = EditTextNotNull(
		title,
		t_("Add Release"),
		t_("Release's title"),
		0
	);
	if (!b) return;
	
	int rel_i = -1;
	for(int i = 0; i < a.releases.GetCount(); i++) {
		Release& r = a.releases[i];
		if (r.title == title) {
			rel_i = i;
			break;
		}
	}
	if (rel_i >= 0) {
		PromptOK(DeQtf(t_("Release exist already")));
		return;
	}
	
	Release& r = a.releases.Add();
	r.file_title = MakeTitle(title);
	r.title = title;
	db.active_release = &r;
	
	DataArtist();
}

void Editor::RenameRelease() {
	Database& db = Database::Single();
	if (!db.active_release)
		return;
	
	String title;
	bool b = EditTextNotNull(
		title,
		t_("Rename Release"),
		t_("Release's title"),
		0
	);
	if (!b) return;
	
	db.active_release->title = title;
	
	DataArtist();
}

void Editor::RemoveRelease() {
	Database& db = Database::Single();
	if (!db.active_artist || !db.active_release)
		return;
	int idx = db.GetActiveReleaseIndex();
	if (idx < 0) return;
	db.active_artist->releases.Remove(idx);
	DataArtist();
}

void Editor::AddSong() {
	Database& db = Database::Single();
	if (!db.active_artist)
		return;
	Artist& a = *db.active_artist;
	Release& r = *db.active_release;
	
	String title;
	bool b = EditTextNotNull(
		title,
		t_("Add Song"),
		t_("Song's title"),
		0
	);
	if (!b) return;
	
	int rel_i = -1;
	for(int i = 0; i < r.songs.GetCount(); i++) {
		Song& s = r.songs[i];
		if (s.title == title) {
			rel_i = i;
			break;
		}
	}
	if (rel_i >= 0) {
		PromptOK(DeQtf(t_("Song exist already")));
		return;
	}
	
	Song& s = r.songs.Add();
	s.file_title = MakeTitle(title);
	s.title = title;
	db.active_song = &s;
	
	DataArtist();
}

void Editor::RenameSong() {
	Database& db = Database::Single();
	if (!db.active_song)
		return;
	
	String title;
	bool b = EditTextNotNull(
		title,
		t_("Rename Song"),
		t_("Song's title"),
		0
	);
	if (!b) return;
	
	db.active_song->title = title.ToString();
	
	DataRelease();
}

void Editor::RemoveSong() {
	Database& db = Database::Single();
	if (!db.active_song || !db.active_release)
		return;
	int idx = db.GetActiveSongIndex();
	if (idx < 0) return;
	db.active_release->songs.Remove(idx);
	db.active_song = 0;
	DataRelease();
}
