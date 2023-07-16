#include "SongTool.h"


void SnapAttrStr::RealizeId() const {
	if (has_id)
		return;
	SnapAttrStr& sa = const_cast<SnapAttrStr&>(*this);
	Database& db = Database::Single();
	sa.group_i = -1;
	sa.item_i = -1;
	for(int i = 0; i < db.attrs.groups.GetCount(); i++) {
		const Attributes::Group& gg = db.attrs.groups[i];
		if (gg.description == group) {
			sa.group_i = i;
			for(int j = 0; j < gg.values.GetCount(); j++) {
				if (gg.values[j] == item) {
					sa.item_i = j;
					return;
				}
			}
			DUMPC(gg.values);
			DUMP(sa.group);
			DUMP(sa.item);
			ASSERT_(0, "item not found");
		}
	}
	DUMP(sa.group);
	DUMP(sa.item);
	ASSERT_(0, "group and item not found");
}

void SnapAttrStr::Load(const SnapAttr& sa) {
	group_i = sa.group;
	item_i = sa.item;
	group.Clear();
	item.Clear();
	has_id = false;
	if (group_i < 0 || item_i < 0) {ASSERT(0); return;}
	Database& db = Database::Single();
	ASSERT(group_i < db.attrs.groups.GetCount());
	if (group_i >= db.attrs.groups.GetCount()) {ASSERT(0); return;}
	Attributes::Group& g = db.attrs.groups[group_i];
	if (item_i >= g.values.GetCount()) {ASSERT(0); return;}
	group = g.description;
	item = g.values[item_i];
	has_id = true;
}