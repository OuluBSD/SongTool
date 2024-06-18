#ifndef _TextDatabase_DatabaseClass_h_
#define _TextDatabase_DatabaseClass_h_


BEGIN_TEXTLIB_NAMESPACE



struct Translation : Moveable<Translation> {
	VectorMap<String, String> data;
	
	Translation& Add(String key, String value) {data.GetAdd(key) = value; return *this;}
	void Serialize(Stream& s) {
		s % data;
	}
	void Jsonize(JsonIO& json) {
		if (json.IsStoring())
			SortByKey(data, StdLess<String>());
		json
			("data", data)
			;
		if (json.IsLoading()) {
			VectorMap<String, String> tmp;
			for(int i = 0; i < data.GetCount(); i++)
				tmp.Add(ToLower(data.GetKey(i)), ToLower(data[i]));
			Swap(tmp, data);
		}
	}
};


struct StructuredScript : Moveable<StructuredScript> {
	String entity, title, structure_str;
	VectorMap<String, Vector<String>> script;
	
	void Jsonize(JsonIO& json) {
		json
			("entity", entity)
			("title", title)
			("structure_str", structure_str)
			("script", script)
			;
	}
};


struct TextDatabase {
	// Share
	Array<Entity>	entities;
	VectorMap<String, Translation> translation;
	Vector<StructuredScript> structured_scripts;
	
	// Source
	SourceData		src_data;
	
	// Temp
	RWMutex			lock;
	
	TextDatabase();
	Array<Entity>& GetSub() {return entities;}
	const Array<Entity>& GetSub() const {return entities;}
	void Clear() {entities.Clear(); }
	void Store();
	void Load();
	void FindOrphaned();
	/*void Serialize(Stream& s) {
		s	% entities
			% translation;
	}*/
	void Jsonize(JsonIO& json) {
		json ("structured_scripts", structured_scripts);
	}
	String GetEntitiesDir() const;
	String GetSnapshotsDir() const;
	String GetComponentsDir() const;
	
	String Translate(const String& s);
	//static int trans_i; // active language translation index
	Entity& GetAddEntity(Profile& p);
	
};


struct Belief {
	struct Attr : Moveable<Attr> {
		String positive, negative;
		void Jsonize(JsonIO& json) {json("positive",positive)("negative",negative);}
	};
	String name, reference;
	Vector<Attr> attrs;
	Vector<String> user;
	
	void Jsonize(JsonIO& json) {
		json("name", name)("reference", reference)("attrs", attrs)("user", user);
	}
};


struct MetaDatabase {
	TextDatabase db[DB_COUNT];
	
	//Array<MetaEntity>	meta_entities;
	Array<Owner>				owners;
	Array<Belief>				beliefs;
	
	// Source
	LeadData					lead_data;
	
	// Temp
	String						dir, share;
	
private:
	ArrayMap<String, PlatformAnalysis>		platforms;
	ArrayMap<String, SocietyRoleAnalysis>	roles;
	
public:
	
	void Store();
	void Load();
	void Jsonize(JsonIO& json);
	static MetaDatabase& Single();
	static String GetDirectory();
	
	
	// Dummy temporary solution
	int GetLanguageIndex() const;
	int GetOtherLanguageIndex() const;
	String GetLanguage() const;
	String GetOtherLanguage() const;
	const Array<PlatformAnalysis>& GetPlatforms() const {return platforms.GetValues();}
	PlatformAnalysis& GetAdd(int plat_i);
	PlatformAnalysis& GetAdd(const Platform& plat);
	SocietyRoleAnalysis& GetAddRole(int role_i);
	
};


TextDatabase& GetAppModeDatabase();
TextDatabase& GetAppModeDatabase(int appmode);
EditorPtrs& GetAppModePointers();
EditorPtrs& GetAppModePointers(int appmode);
void EnterAppMode(int i);
void LeaveAppMode();
ToolEditor& GetAnyEditor();
LeadsCtrl& GetAnyLeads();
int GetAppModeGlobal();














END_TEXTLIB_NAMESPACE


#endif
