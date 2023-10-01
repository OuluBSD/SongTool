#ifndef _SongTool_PatternMask_h_
#define _SongTool_PatternMask_h_

struct PatternMask : PipePtrs {
	Index<SnapAttrStr>	mask;
	Vector<int>			maskscore;
	
	
	void Clear() {
		mask.Clear();
		maskscore.Clear();
	}
	void Add(const SnapAttrStr& sa) {
		mask.FindAdd(sa);
	}
	void Serialize(Stream& s) {
		s	% mask
			% maskscore;
	}
	/*void Jsonize(JsonIO& json) {
		json
			("mask", mask)
			("maskscore", maskscore)
			;
	}*/
	void GetGroups(Index<int>& groups) {
		for (const SnapAttrStr& sa : mask.GetKeys()) {
			sa.RealizeId(*pipe);
			groups.FindAdd(sa.group_i);
		}
	}
	void GetGroupItems(VectorMap<int, Vector<int>>& group_items) {
		for (const SnapAttrStr& sa : mask.GetKeys()) {
			sa.RealizeId(*pipe);
			group_items.GetAdd(sa.group_i).Add(sa.item_i);
		}
	}
	void ResolveId() {
		#if 0
		for (const SnapAttrStr& sa : mask.GetKeys())
			sa.RealizeId();
		#else
		for(int i = 0; i < mask.GetCount(); i++) {
			if (!mask[i].RealizeId(*pipe))
				mask.Remove(i--);
		}
		#endif
	}
};

#endif