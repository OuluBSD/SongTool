#include "TextDatabase.h"


BEGIN_TEXTLIB_NAMESPACE


int TextDatabase::trans_i = -1;


TextDatabase::TextDatabase() {
	
}

String TextDatabase::GetEntitiesDir() const {
	String& dir = MetaDatabase::Single().dir;
	String& share = MetaDatabase::Single().share;
	return dir + DIR_SEPS + share + DIR_SEPS + GetAppModeDir() + DIR_SEPS + "entities" + DIR_SEPS;
}

String TextDatabase::GetSnapshotsDir() const {
	String& dir = MetaDatabase::Single().dir;
	String& share = MetaDatabase::Single().share;
	return dir + DIR_SEPS + share + DIR_SEPS + GetAppModeDir() + DIR_SEPS + "snapshots" + DIR_SEPS;
}

String TextDatabase::GetComponentsDir() const {
	String& dir = MetaDatabase::Single().dir;
	String& share = MetaDatabase::Single().share;
	return dir + DIR_SEPS + share + DIR_SEPS + GetAppModeDir() + DIR_SEPS + "components" + DIR_SEPS;
}

void TextDatabase::Store() {
	String& dir = MetaDatabase::Single().dir;
	String& share = MetaDatabase::Single().share;
	StoreAsJsonFileStandard(*this, dir + DIR_SEPS + share + DIR_SEPS + GetAppModeDir() + DIR_SEPS + "db.json", true);
}

void TextDatabase::Load() {
	String& dir = MetaDatabase::Single().dir;
	String& share = MetaDatabase::Single().share;
	Clear();
	
	lock.EnterWrite();
	LoadFromJsonFileStandard(*this, dir + DIR_SEPS + share + DIR_SEPS + GetAppModeDir() + DIR_SEPS + "db.json");
	lock.LeaveWrite();
	
}

void TextDatabase::FindOrphaned() {
	{
		String dir = GetEntitiesDir();
		String search = dir + "*.json";
		
		FindFile ff;
		if (ff.Search(search)) do {
			if (ff.IsFile()) {
				String path = ff.GetPath();
				String title = GetFileTitle(path);
				bool found = false;
				for (Entity& a : entities)
					if (a.file_title == title)
						found = true;
				if (!found) {
					Entity& a = entities.Add();
					a.LoadTitle(title);
				}
			}
		}
		while (ff.Next());
	}
}

String TextDatabase::Translate(const String& s) {
	if (trans_i < 0)
		return s;
	Translation& t = this->translation[trans_i];
	int i = t.data.Find(s);
	String o;
	if (i >= 0)
		o = t.data[i];
	if (i < 0 || o.IsEmpty()) {
		i = t.data.Find(ToLower(s));
		if (i >= 0)
			o = t.data[i];
		else
			t.data.Add(ToLower(s));
	}
	return o.IsEmpty() ? s : o;
}






int EditorPtrs::GetActiveEntityIndex() const {return VectorFindPtr(entity, GetDatabase().entities);}
int EditorPtrs::GetActiveSnapshotIndex() const {if (!entity) return -1; return VectorFindPtr(release, entity->snaps);}
int EditorPtrs::GetActiveComponentIndex() const {if (!release) return -1; return VectorFindPtr(component, release->components);}
int EditorPtrs::GetActiveTypeclassIndex() const {return VectorFindPtr(typecast, entity->typeclasses);}
int EditorPtrs::GetActiveContentIndex() const {return VectorFindPtr(archetype, typecast->contents);}
int EditorPtrs::GetActiveScriptIndex() const {return VectorFindPtr(script, archetype->scripts);}


TextDatabase& EditorPtrs::GetDatabase() const {
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return MetaDatabase::Single().db[appmode];
}


template <>
void CheckSerialisationData<TextLib::Component>(const String& json) {
	TextLib::Component song;
	LoadFromJson(song, json);
	//ASSERT(song.native_title.GetCount() || song.english_title.GetCount());
}

int& __global_appmode() {
	thread_local int i;
	return i;
}

TextDatabase& GetAppModeDatabase() {
	int appmode = __global_appmode() - 1;
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return MetaDatabase::Single().db[appmode];
}

TextDatabase& GetAppModeDatabase(int appmode) {
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return MetaDatabase::Single().db[appmode];
}

int GetAppModeGlobal() {
	return __global_appmode() - 1;
}

void EnterAppMode(int i) {
	__global_appmode() = 1+i;
}

void LeaveAppMode() {
	__global_appmode() = 0;
}

EditorPtrs& GetAppModePointers() {
	int appmode = __global_appmode() - 1;
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return MetaPtrs::Single().db[appmode];
}

EditorPtrs& GetAppModePointers(int appmode) {
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return MetaPtrs::Single().db[appmode];
}

ToolEditor& GetAnyEditor() {
	for(int i = 0; i < DB_COUNT; i++) {
		auto* p = MetaPtrs::Single().db[i].editor;
		if (p) return *p;
	}
	Panic("No ToolEditor pointer");
	ToolEditor* p = 0;
	return *p;
}

LeadsCtrl& GetAnyLeads() {
	return *MetaPtrs::Single().leads;
}

String GetAppModeDir(int appmode) {
	if (appmode < 0)
		appmode = GetAppModeGlobal();
	ASSERT(appmode >= 0 && appmode < DB_COUNT);
	return ToLower(GetAppModeString(appmode));
}










MetaDatabase& MetaDatabase::Single() {
	static MetaDatabase db;
	return db;
}

void MetaDatabase::Store() {
	lead_data.Store();
	
	String& dir = MetaDatabase::Single().dir;
	String& share = MetaDatabase::Single().share;
	StoreAsJsonFileStandard(*this, dir + DIR_SEPS + share + DIR_SEPS + "db.json", true);
}

void MetaDatabase::Load() {
	String& dir = MetaDatabase::Single().dir;
	String& share = MetaDatabase::Single().share;
	ASSERT(dir.GetCount());
	LoadFromJsonFileStandard(*this, dir + DIR_SEPS + share + DIR_SEPS + "db.json");
	
	lead_data.Load();
}

void MetaDatabase::Jsonize(JsonIO& json) {
	json
		("owners", owners)
		;
}

int MetaDatabase::GetLanguageIndex() const {
	if (share == "share")
		return LNG_ENGLISH;
	if (share == "share-fi")
		return LNG_FINNISH;
	TODO
	return LNG_NATIVE;
}

int MetaDatabase::GetOtherLanguageIndex() const {
	if (share == "share-fi")
		return LNG_ENGLISH;
	else
		return LNG_FINNISH;
}

String MetaDatabase::GetLanguage() const {
	return GetLanguageKey(GetLanguageIndex());
}

String MetaDatabase::GetOtherLanguage() const {
	return GetLanguageKey(GetOtherLanguageIndex());
}



END_TEXTLIB_NAMESPACE

