#ifndef _Database_SongData_h_
#define _Database_SongData_h_


struct LyricsDataset : Moveable<LyricsDataset> {
	String name;
	String text;
	
	void Serialize(Stream& s) {
		s % name % text;
	}
};

struct ArtistDataset : Moveable<ArtistDataset> {
	String name;
	Vector<LyricsDataset> lyrics;
	
	void Serialize(Stream& s) {
		s % name % lyrics;
	}
};

struct LyricsAnalysis : Moveable<LyricsAnalysis> {
	struct Rhyme : Moveable<Rhyme> {
		Vector<String> words;
		int score;
		
		void Serialize(Stream& s) {s % words % score;}
		void Jsonize(JsonIO& json) {
			json
				("words", words)
				("score", score)
				;
		}
	};
	struct Role : Moveable<Role> {
		String subject;
		int percent;
		
		void Serialize(Stream& s) {s % subject % percent;}
		void Jsonize(JsonIO& json) {
			json
				("subject", subject)
				("percent", percent)
				;
		}
	};
	struct RhymeLocationLine : Moveable<RhymeLocationLine> {
		Vector<String> words;
		Vector<bool> rhyming;
		
		void Serialize(Stream& s) {
			s % words % rhyming;
		}
		void Jsonize(JsonIO& json) {
			json
				("words", words)
				("rhyming", rhyming)
				;
		}
	};
	String name;
	Vector<Rhyme> rhymes;
	VectorMap<String, Vector<String>> word_groups;
	Vector<Vector<Role>> positive_roles, negative_roles;
	Vector<RhymeLocationLine> rhyme_locations;
	
	void Serialize(Stream& s){
		s % name % rhymes % word_groups % positive_roles % negative_roles % rhyme_locations;
	}
	
	void Jsonize(JsonIO& json) {
		json
			("name", name)
			("rhymes", rhymes)
			("word_groups", word_groups)
			("positive_roles", positive_roles)
			("negative_roles", negative_roles)
			("rhyme_locations", rhyme_locations)
			;
	}
	String AsString() const;
};

struct ArtistAnalysis : Moveable<ArtistAnalysis> {
	int total_words = 0;
	VectorMap<String, int> word_counts;
	Array<LyricsAnalysis> songs;
	
	void Jsonize(JsonIO& json) {
		json
			("total_words", total_words)
			("word_counts", word_counts)
			("songs", songs)
			;
	}
	void Serialize(Stream& s) {
		s % total_words % word_counts % songs;
	}
};

struct WordGroupAnalysis : Moveable<WordGroupAnalysis> {
	VectorMap<String, int> values;
	bool is_wordlist = false;
	
	void Jsonize(JsonIO& json) {
		json
			("values", values)
			("is_wordlist", is_wordlist)
			;
	}
	void Serialize(Stream& s) {
		s % values % is_wordlist;
	}
};

struct WordAnalysis : Moveable<WordAnalysis> {
	int count = 0;
	Index<int> group_is;
	String main_class;
	String translation;
	Color clr, main_class_clr = Black();
	
	void Jsonize(JsonIO& json) {
		json
			("count", count)
			("group_is", group_is)
			("main_class", main_class)
			("translation", translation)
			("clr", clr)
			("main_class_clr", main_class_clr)
			;
	}
	void Serialize(Stream& s) {
		/*if (s.IsLoading())
			s % count % group_is % main_class % translation % clr;
		else*/
			s % count % group_is % main_class % translation % clr % main_class_clr;
	}
};

struct SongId : Moveable<SongId> {
	byte dataset;
	uint16 artist;
	uint16 song;
	
	void Jsonize(JsonIO& json) {
		int dataset = this->dataset;
		int artist = this->artist;
		int song = this->song;
		json
			("dataset", dataset)
			("artist", artist)
			("song", song)
			;
		this->dataset = dataset;
		this->artist = artist;
		this->song = song;
	}
	void Serialize(Stream& s) {
		s % dataset % artist % song;
	}
};

struct PhraseAnalysis : Moveable<PhraseAnalysis> {
	Vector<SongId> songs;
	Vector<int> word_ids;
	Color clr;
	
	void Jsonize(JsonIO& json) {
		json
			("songs", songs)
			("word_ids", word_ids)
			("clr", clr)
			;
	}
	void Serialize(Stream& s) {
		s % songs % word_ids % clr;
	}
};

struct VirtualPhraseAnalysis : Moveable<VirtualPhraseAnalysis> {
	Vector<int> group_ids;
	Index<int> phrases;
	
	void Jsonize(JsonIO& json) {
		json
			("group_ids", group_ids)
			("phrases", phrases)
			;
	}
	void Serialize(Stream& s) {
		s % group_ids % phrases;
	}
};

struct DatasetAnalysis {
	VectorMap<String, ArtistAnalysis> artists;
	VectorMap<String, WordGroupAnalysis> groups;
	VectorMap<String, WordAnalysis> words;
	VectorMap<String, PhraseAnalysis> unique_phrases;
	VectorMap<String, VirtualPhraseAnalysis> virtual_phrases;
	
	void Jsonize(JsonIO& json) {
		json
			("artists", artists)
			("groups", groups)
			("word", words)
			("unique_phrases", unique_phrases)
			("virtual_phrases", virtual_phrases)
			;
	}
	
	
	void Serialize(Stream& s) {
		s % artists % groups % words % unique_phrases % virtual_phrases;
	}
};

struct SongDataAnalysis {
	ArrayMap<String, DatasetAnalysis> datasets;
	
	void Jsonize(JsonIO& json) {
		json
			("datasets", datasets)
			;
	}
	
	
	void Serialize(Stream& s) {
		s % datasets;
	}
	void StoreJson();
	void LoadJson();
	void Store();
	void Load();
};

struct SongData {
	// Binary data
	Vector<ArtistDataset> artists_en;
	Vector<ArtistDataset> artists_fi;
	
	SongDataAnalysis a;
	
	
	int GetCount() const {return 2;}
	Vector<ArtistDataset>& operator[](int i) {
		switch (i) {
			case 0: return artists_en;
			case 1: return artists_fi;
			default: Panic("error");
		}
		return Single<Vector<ArtistDataset>>();
	}
	String GetKey(int i) const {
		switch (i) {
			case 0: return "en";
			case 1: return "fi";
			default: Panic("error");
		}
		return "";
	}
	void Store();
	void Load();
	void Serialize(Stream& s);
	bool IsEmpty() const {return artists_en.IsEmpty() || artists_fi.IsEmpty();}
	
};


#endif
