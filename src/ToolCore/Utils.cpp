#include "ToolCore.h"
#include <Database/Database.h>



Song& SongToolCtrl::GetSong() {
	Database& db = Database::Single();
	EditorPtrs& p = db.ctx.ed;
	if(!p.song || !p.artist)
		throw NoPointerExc("no song");
	Song& song = *p.song;
	return song;
}

void SongToolCtrl::GetAttrs(const VectorMap<String,String>& data, VectorMap<String,String>& v) {
	for(int i = 0; i < Attr::ATTR_COUNT; i++) {
		const char* key = Attr::AttrKeys[i][0];
		int value = StrInt(data.Get(key, "0"));
		
		if (value) {
			if (value > 0) {
				v.GetAdd(key) = Attr::AttrKeys[i][2];
			}
			else {
				v.GetAdd(key) = Attr::AttrKeys[i][3];
			}
		}
	}
}


String Capitalize(String s) {
	return ToUpper(s.Left(1)) + s.Mid(1);
}

String GetSnapGroupString(PatternSnap& snap, int group_i, Index<String>& skip_list) {
	Database& db = Database::Single();
	Attributes& g = *snap.pipe;
	Attr::Group& gg = g.attr_groups[group_i];
	
	String s;
	for(const SnapAttrStr& a : snap.attributes.GetKeys()) {
		a.RealizeId(*snap.pipe);
		if (a.group_i != group_i)
			continue;
		
		String item = db.Translate(gg.values[a.item_i]);
		
		if (skip_list.Find(item) < 0) {
			if (!s.IsEmpty())
				s << ", ";
			skip_list.Add(item);
			s << item;
		}
	}
	return s;
}

bool HasSnapGroupString(PatternSnap& snap, int group_i) {
	Attributes& g = *snap.pipe;
	Attr::Group& gg = g.attr_groups[group_i];
	
	for(const SnapAttrStr& a : snap.attributes.GetKeys()) {
		a.RealizeId(*snap.pipe);
		if (a.group_i != group_i)
			continue;
		
		return true;
	}
	return false;
}
