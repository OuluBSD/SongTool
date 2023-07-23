#ifndef _SongTool_Attributes_h_
#define _SongTool_Attributes_h_


struct Attributes : DataFile {
	struct Translation : Moveable<Translation> {
		VectorMap<String, String> data;
		
		Translation& Add(String key, String value) {data.GetAdd(key) = value; return *this;}
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
	
	struct Group : Moveable<Group> {
		Vector<String> values;
		Color clr;
		String description;
		String type;
		
		// temp values
		int type_i = -1;
		
		Group& SetDescription(String s) {description = s; return *this;}
		Group& SetColor(Color c) {clr = c; return *this;}
		Group& SetColor(int r, int g, int b) {clr = Color(r,g,b); return *this;}
		Group& operator<<(String s) {ASSERT(s.Find(",") < 0); values.Add(s); return *this;}
		void Jsonize(JsonIO& json) {
			json
				("description", description)
				("type", type)
				("color", clr)
				("values", values)
				;
		}
		bool HasValue(String v) const {
			for (const String& s : values)
				if (s == v)
					return true;
			return false;
		}
		String ToString() const {return description + "(" + type + ")";}
	};
	struct ScoringType : Moveable<ScoringType> {
		String klass;
		String axes0, axes1;
		
		void Jsonize(JsonIO& json) {
			json
				("class", klass)
				("axes0", axes0)
				("axes1", axes1)
				;
		}
	};
	struct GroupType : Moveable<GroupType> {
		String name;
		String ai_txt;
		Color clr;
		void Jsonize(JsonIO& json) {json
			("name", name)
			("ai_txt", ai_txt)
			("clr", clr);
			//if (json.IsLoading()) clr = Color(Random(256),Random(256),Random(256));
		}
	};
	Vector<GroupType> group_types;
	Vector<Group> groups;
	VectorMap<String, Translation> translation;
	Vector<ScoringType> scorings;
	VectorMap<String, Vector<String>> analysis;
	
	
	Attributes();
	
	void Store();
	void Load();
	void LoadDefaultGroups();
	void LoadDefaultAnalysis();
	void LoadDefaultAttrGroups();
	void RealizeAttrIds();
	void FindGroupTypes(const Vector<int>& groups, Index<int>& group_types) const;
	GroupType& AddGroupType(String type, String ai_txt);
	GroupType& GetGroupType(String type);
	Group& AddGroup(String type, String desc);
	int GetCount() const {return groups.GetCount();}
	int GetItemCount() const {
		int i = 0;
		for (const Group& g : groups)
			i += g.values.GetCount();
		return i;
	}
	String Translate(const String& s);
	SnapAttr GetAddAttr(String group, String item);
	bool FindAttr(String group, String item, SnapAttr& sa) const;
	void Clear() {groups.Clear(); translation.Clear();}
	void Jsonize(JsonIO& json) {
		json
			("group_types", group_types)
			("groups", groups)
			("translation", translation)
			("scorings", scorings)
			("analysis", analysis)
			;
		if (json.IsLoading()) {
			String lng = GetCurrentLanguageString().Left(5);
			trans_i = translation.Find(lng);
			scorings.Clear();
			Realize();
		}
	}
	void Realize() {
		if (groups.IsEmpty() || group_types.IsEmpty())
			LoadDefaultGroups();
		if (analysis.IsEmpty())
			LoadDefaultAnalysis();
		if (scorings.IsEmpty())
			LoadDefaultAttrGroups();
	}
	void AddScoring(String s, Vector<Attributes::ScoringType>& scorings);
	int FindGroup(String group_name);
	
	static const int group_limit = 1024;
	static int trans_i; // active language translation index
	
};













#endif
