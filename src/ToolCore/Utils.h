#ifndef _SongTool_Utils_h_
#define _SongTool_Utils_h_


typedef String NoPointerExc;
struct Song;


class SongToolCtrl : public Ctrl {
	
	
public:
	virtual ~SongToolCtrl() {}
	virtual void Data() = 0;
	virtual void ToolMenu(Bar& bar) {bar.Add("", AppImg::placeholder16(), Callback());}
	virtual String GetStatusText() {return String();}
	
	Song& GetSong();
	void GetAttrs(const VectorMap<String,String>& data, VectorMap<String,String>& v);
	
};


String Capitalize(String s);


template <int I, class K=int, class V=double>
struct FixedTopValueSorter {
	inline static const int size = I;
	
	K key[size];
	V value[size];
	int count = 0;
	
	
	FixedTopValueSorter() {Reset();}
	void Reset() {
		count = 0;
		for(int i = 0; i < size; i++) {
			value[i] = -DBL_MAX;
			key[i] = -1;
		}
	}
	void Add(const K& key, const V& value) {
		if (value <= this->value[size-1])
			return;
		for(int i = 0; i < size; i++) {
			if (value > this->value[i]) {
				for(int j = size-1; j > i; j--) {
					this->value[j] = this->value[j-1];
					this->key[j]   = this->key[j-1];
				}
				this->value[i] = value;
				this->key[i] = key;
				count = min(count+1, size);
				break;
			}
		}
	}
	void Serialize(Stream& s) {
		for(int i = 0; i < size; i++)
			s % value[i] % key[i];
	}
};

template <class T, class PTR>
int VectorFindPtr(PTR* p, T& arr) {
	int i = 0;
	for (auto& r : arr) {
		if (&r == p)
			return i;
		i++;
	}
	return -1;
}

inline String MakeTitle(String s) {
	WString ws = s.ToWString();
	ws = ToLower(ws);
	ws.Replace(" ", "_");
	ws.Replace("ä", "a");
	ws.Replace("ö", "o");
	return ws.ToString();
}

#endif
