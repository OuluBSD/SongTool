#include "SongTool.h"


PatternMaskCtrl::PatternMaskCtrl() {
	CtrlLayout(*this);
	data.AddColumn(t_("Key"));
	data.AddColumn(t_("Value"));
	data.ColumnWidths("1 4");
	
}

void PatternMaskCtrl::Data() {
	Database& db = Database::Single();
	if (!db.active_part) return;
	Attributes& a = db.attrs;
	Part& p = *db.active_part;
	
	lyrics.SetData(Join(p.lines, "\n"));
	
	for(int i = 0; i < p.mask.attrs.GetCount(); i++) {
		const SnapAttrStr& sa = p.mask.attrs[i];
		#if 0
		const Attributes::Group& gg = a.groups[sa.group];
		const String& item = gg.values[sa.item];
		#if 0
		data.Set(i, 0, Capitalize(gg.description));
		data.Set(i, 1, Capitalize(item));
		#else
		data.Set(i, 0, gg.description);
		data.Set(i, 1, item);
		#endif
		#endif
		data.Set(i, 0, sa.group);
		data.Set(i, 1, sa.item);
	}
	data.SetCount(p.mask.attrs.GetCount());
	
}
