#include "TextDataCtrl.h"


BEGIN_TEXTLIB_NAMESPACE


LeadWebsites::LeadWebsites() {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << vsplit << mainsplit;
	hsplit.SetPos(1500);
	
	vsplit.Vert() << websites << payouts << prices;
	
	mainsplit.Vert() << list << bsplit << bssplit;
	mainsplit.SetPos(4500,0);
	mainsplit.SetPos(9000,1);
	
	bsplit.Horz() << attrs << bools << strings << bvsplit;
	bvsplit.Vert() << list_names << list_values;
	//bvsplit.SetPos(2500);
	bssplit.Horz() << song_typecasts << lyrics_ideas << music_styles;
	
	websites.AddColumn(t_("Website"));
	websites.AddColumn(t_("Count"));
	websites.AddIndex("IDX");
	websites.ColumnWidths("3 1");
	websites.WhenCursor << THISBACK(DataWebsite);
	
	payouts.AddColumn(t_("Payout"));
	payouts.AddColumn(t_("Count"));
	payouts.AddIndex("IDX");
	payouts.AddIndex("MIN");
	payouts.AddIndex("MAX");
	payouts.ColumnWidths("3 1");
	payouts.WhenCursor << THISBACK(DataPayout);
	
	prices.AddColumn(t_("Submission price"));
	prices.AddColumn(t_("Count"));
	prices.AddIndex("IDX");
	prices.AddIndex("MIN");
	prices.AddIndex("MAX");
	prices.ColumnWidths("3 1");
	prices.WhenCursor << THISBACK(DataPrice);
	
	
	list.AddColumn(t_("Website"));
	list.AddColumn(t_("Name"));
	list.AddColumn(t_("Price"));
	list.AddColumn(t_("Payout"));
	list.AddColumn(t_("Description"));
	list.AddColumn(t_("Money-score"));
	list.AddColumn(t_("Money-score-rank"));
	list.AddColumn(t_("Opp-score"));
	list.AddColumn(t_("Opp-score-rank"));
	list.AddColumn(t_("Weighted-rank"));
	list.AddColumn(t_("Chance %"));
	list.AddColumn(t_("Av. Payout"));
	list.AddIndex("IDX");
	list.ColumnWidths("2 4 1 1 10 1 1 1 1 1 1 1");
	list.WhenCursor << THISBACK(DataOpportunity);
	
	attrs.AddColumn(t_("Key"));
	attrs.AddColumn(t_("Value"));
	attrs.ColumnWidths("2 3");
	
	bools.AddColumn(t_("Key"));
	bools.AddColumn(t_("Value"));
	bools.ColumnWidths("5 1");
	
	strings.AddColumn(t_("Key"));
	strings.AddColumn(t_("Value"));
	strings.ColumnWidths("2 3");
	
	list_names.AddColumn(t_("Key"));
	list_names.WhenCursor << THISBACK(DataAnalyzedList);
	list_values.AddColumn(t_("Value"));
	
	song_typecasts.AddColumn(t_("#"));
	song_typecasts.AddColumn(t_("Typecast"));
	song_typecasts.AddColumn(t_("Content"));
	song_typecasts.ColumnWidths("1 3 3");
	
	lyrics_ideas.AddColumn(t_("#"));
	lyrics_ideas.AddColumn(t_("Lyrics idea"));
	lyrics_ideas.ColumnWidths("1 10");
	
	music_styles.AddColumn(t_("#"));
	music_styles.AddColumn(t_("Music style"));
	music_styles.ColumnWidths("1 10");
}

void LeadWebsites::Data() {
	MetaDatabase& db = MetaDatabase::Single();
	LeadData& ld = db.lead_data;
	LeadDataAnalysis& sda = db.lead_data.a;
	
	
	int counts[LEADSITE_COUNT];
	for(int i = 0; i < LEADSITE_COUNT; i++) counts[i] = 0;
	for (LeadOpportunity& o : ld.opportunities) {
		counts[o.leadsite]++;
	}
	int total = 0;
	for(int i = 0; i < LEADSITE_COUNT; i++) total += counts[i];
	
	
	websites.Set(0, 0, t_("All"));
	websites.Set(0, 1, total);
	websites.Set(0, "IDX", -1);
	
	for(int i = 0; i < LEADSITE_COUNT; i++) {
		websites.Set(1+i, 0, GetLeadWebsiteKey(i));
		websites.Set(1+i, 1, counts[i]);
		websites.Set(1+i, "IDX", i);
	}
	INHIBIT_CURSOR(websites);
	websites.SetCount(1+LEADSITE_COUNT);
	websites.SetSortColumn(1, true);
	if (!websites.IsCursor() && websites.GetCount())
		websites.SetCursor(0);
	
	
	DataWebsite();
}

void LeadWebsites::DataWebsite() {
	MetaDatabase& db = MetaDatabase::Single();
	LeadData& ld = db.lead_data;
	LeadDataAnalysis& sda = db.lead_data.a;
	
	if (!websites.IsCursor())
		return;
	
	int leadsite_i = websites.Get("IDX");
	bool filter_leadsite = leadsite_i >= 0;
	
	VectorMap<Tuple2<double,double>,int> counts;
	counts.Add(Tuple2<double,double>(0,1), 0);
	counts.Add(Tuple2<double,double>(1,10), 0);
	counts.Add(Tuple2<double,double>(10,100), 0);
	counts.Add(Tuple2<double,double>(100,1000), 0);
	counts.Add(Tuple2<double,double>(10000,10000), 0);
	counts.Add(Tuple2<double,double>(10000,100000000), 0);
	
	
	for(int i = 0; i < LEADSITE_COUNT; i++) counts[i] = 0;
	for (LeadOpportunity& o : ld.opportunities) {
		if (filter_leadsite && o.leadsite != leadsite_i)
			continue;
		int range_i = 0;
		for(int j = 0; j < counts.GetCount(); j++) {
			const auto& range = counts.GetKey(j);
			if (o.min_compensation >= range.a && o.min_compensation < range.b) {
				range_i = j;
				break;
			}
		}
		counts[range_i]++;
	}
	int total = 0;
	for(int i = 0; i < LEADSITE_COUNT; i++) total += counts[i];
	
	
	
	payouts.Set(0,0,t_("All"));
	payouts.Set(0,1,total);
	payouts.Set(0,"IDX",-1);
	payouts.Set(0,"MIN",0);
	payouts.Set(0,"MAX",INT_MAX);
	for(int i = 0; i < counts.GetCount(); i++) {
		const auto& range = counts.GetKey(i);
		String str = IntStr(range.a) + " - " + IntStr(range.b);
		payouts.Set(1+i, 0, str);
		payouts.Set(1+i, 1, counts[i]);
		payouts.Set(1+i, "IDX", i);
		payouts.Set(1+i, "MIN", range.a);
		payouts.Set(1+i, "MAX", range.b);
	}
	
	INHIBIT_CURSOR(payouts);
	payouts.SetCount(1+counts.GetCount());
	//payouts.SetSortColumn(1, true);
	if (!payouts.IsCursor() && payouts.GetCount())
		payouts.SetCursor(0);
	
	DataPayout();
}

void LeadWebsites::DataPayout() {
	MetaDatabase& db = MetaDatabase::Single();
	LeadData& ld = db.lead_data;
	LeadDataAnalysis& sda = db.lead_data.a;
	
	if (!websites.IsCursor() || !payouts.IsCursor())
		return;
	
	int leadsite_i = websites.Get("IDX");
	bool filter_leadsite = leadsite_i >= 0;
	int payout_min = payouts.Get("MIN");
	int payout_max = payouts.Get("MAX");
	
	VectorMap<Tuple2<double,double>,int> counts;
	counts.Add(Tuple2<double,double>(0,1), 0);
	counts.Add(Tuple2<double,double>(1,10), 0);
	counts.Add(Tuple2<double,double>(10,100), 0);
	counts.Add(Tuple2<double,double>(100,1000), 0);
	counts.Add(Tuple2<double,double>(10000,10000), 0);
	counts.Add(Tuple2<double,double>(10000,100000000), 0);
	
	
	for(int i = 0; i < LEADSITE_COUNT; i++) counts[i] = 0;
	for (LeadOpportunity& o : ld.opportunities) {
		if (filter_leadsite && o.leadsite != leadsite_i)
			continue;
		if (o.min_compensation < payout_min || o.min_compensation >= payout_max)
			continue;
		double price = 0.01 * o.min_entry_price_cents;
		int range_i = 0;
		for(int j = 0; j < counts.GetCount(); j++) {
			const auto& range = counts.GetKey(j);
			if (price >= range.a && price < range.b) {
				range_i = j;
				break;
			}
		}
		counts[range_i]++;
	}
	int total = 0;
	for(int i = 0; i < LEADSITE_COUNT; i++) total += counts[i];
	
	
	
	prices.Set(0,0,t_("All"));
	prices.Set(0,1,total);
	prices.Set(0,"IDX",-1);
	prices.Set(0,"MIN",0);
	prices.Set(0,"MAX",INT_MAX);
	for(int i = 0; i < counts.GetCount(); i++) {
		const auto& range = counts.GetKey(i);
		String str = IntStr(range.a) + " - " + IntStr(range.b);
		prices.Set(1+i, 0, str);
		prices.Set(1+i, 1, counts[i]);
		prices.Set(1+i, "IDX", i);
		prices.Set(1+i, "MIN", range.a);
		prices.Set(1+i, "MAX", range.b);
	}
	
	
	INHIBIT_CURSOR(prices);
	prices.SetCount(1+counts.GetCount());
	//prices.SetSortColumn(1, true);
	if (!prices.IsCursor() && prices.GetCount())
		prices.SetCursor(0);
	
	
	DataPrice();
}

void LeadWebsites::DataPrice() {
	MetaDatabase& db = MetaDatabase::Single();
	MetaPtrs& p = MetaPtrs::Single();
	LeadData& ld = db.lead_data;
	LeadDataAnalysis& sda = db.lead_data.a;
	
	if (!websites.IsCursor() || !payouts.IsCursor() || !prices.IsCursor())
		return;
	
	int leadsite_i = websites.Get("IDX");
	bool filter_leadsite = leadsite_i >= 0;
	int payout_min = payouts.Get("MIN");
	int payout_max = payouts.Get("MAX");
	int price_min = prices.Get("MIN");
	int price_max = prices.Get("MAX");
	
	
	int row = 0;
	int i = -1;
	for (LeadOpportunity& o : ld.opportunities) {
		i++;
		if (filter_leadsite && o.leadsite != leadsite_i)
			continue;
		if (o.min_compensation < payout_min || o.min_compensation >= payout_max)
			continue;
		double price = 0.01 * o.min_entry_price_cents;
		if (price < price_min || price >= price_max)
			continue;
		
		list.Set(row, 0, GetLeadWebsiteKey(o.leadsite));
		list.Set(row, 1, o.name);
		list.Set(row, 2, price);
		list.Set(row, 3, o.min_compensation);
		if (o.request_opportunity_description.GetCount()) {
			String s;
			s	<< o.request_opportunity_description << "\n"
				<< o.request_band_description << "\n"
				<< o.request_selection_description;
			list.Set(row, 4, s);
		}
		else {
			list.Set(row, 4, o.request_description);
		}
		list.Set(row, 5, o.money_score);
		list.Set(row, 6, o.money_score_rank);
		list.Set(row, 7, o.opp_score);
		list.Set(row, 8, o.opp_score_rank);
		list.Set(row, 9, o.weighted_rank);
		list.Set(row, 10, o.chance_of_acceptance);
		list.Set(row, 11, o.average_payout_estimation);
		list.Set(row, "IDX", i);
		
		row++;
	}
	
	INHIBIT_CURSOR(list);
	list.SetCount(row);
	list.SetSortColumn(11, true);
	if (!list.IsCursor() && list.GetCount())
		list.SetCursor(0);
	
	DataOpportunity();
}

void LeadWebsites::DataOpportunity() {
	MetaDatabase& db = MetaDatabase::Single();
	LeadData& ld = db.lead_data;
	LeadDataAnalysis& sda = db.lead_data.a;
	
	if (!list.IsCursor())
		return;
	
	int idx = list.Get("IDX");
	LeadOpportunity& o = ld.opportunities[idx];
	
	{
		int row = 0;
		for(int i = 0; i < o.GetCount(); i++) {
			const char* key = o.GetKey(i);
			Value val = o[i];
			if (val.IsVoid() || val.IsNull())
				continue;
			attrs.Set(row, 0, key);
			attrs.Set(row, 1, val);
			row++;
		}
		INHIBIT_CURSOR(attrs);
		attrs.SetCount(row);
	}
	
	{
		for(int i = 0; i < o.analyzed_booleans.GetCount(); i++) {
			String key = GetSongListingBooleanKey(i);
			bool b = o.analyzed_booleans[i];
			bools.Set(i, 0, key);
			bools.Set(i, 1, b ? "true" : "false");
		}
		INHIBIT_CURSOR(bools);
		bools.SetCount(o.analyzed_booleans.GetCount());
	}
	
	{
		for(int i = 0; i < o.analyzed_string.GetCount(); i++) {
			String key = GetSongListingStringKey(i);
			const String& s = o.analyzed_string[i];
			strings.Set(i, 0, key);
			strings.Set(i, 1, s);
		}
		INHIBIT_CURSOR(strings);
		strings.SetCount(o.analyzed_string.GetCount());
	}
	
	{
		for(int i = 0; i < o.analyzed_lists.GetCount(); i++) {
			String key = GetSongListingListKey(i);
			list_names.Set(i, 0, key);
		}
		INHIBIT_CURSOR(list_names);
		list_names.SetCount(o.analyzed_lists.GetCount());
		if (list_names.GetCount() && !list_names.IsCursor())
			list_names.SetCursor(0);
		DataAnalyzedList();
	}
	
	{
		const auto& tc_list = TextLib::GetTypeclasses(DB_SONG);
		const auto& co_list = TextLib::GetContents(DB_SONG);
		int c = min(o.typeclasses.GetCount(), o.contents.GetCount());
		for(int i = 0; i < c; i++) {
			int tc_i = o.typeclasses[i];
			int co_i = o.contents[i];
			String tc = tc_i < tc_list.GetCount() ? tc_list[tc_i] : String();
			String co = co_i < co_list.GetCount() ? co_list[co_i].key : String();
			song_typecasts.Set(i, 0, 1+i);
			song_typecasts.Set(i, 1, tc);
			song_typecasts.Set(i, 2, co);
		}
		INHIBIT_CURSOR(song_typecasts);
		song_typecasts.SetCount(o.typeclasses.GetCount());
		if (song_typecasts.GetCount() && !song_typecasts.IsCursor())
			song_typecasts.SetCursor(0);
	}
	
	{
		for(int i = 0; i < o.lyrics_ideas.GetCount(); i++) {
			String idea = o.lyrics_ideas[i];
			lyrics_ideas.Set(i, 0, 1+i);
			lyrics_ideas.Set(i, 1, idea);
		}
		INHIBIT_CURSOR(lyrics_ideas);
		lyrics_ideas.SetCount(o.lyrics_ideas.GetCount());
		if (lyrics_ideas.GetCount() && !lyrics_ideas.IsCursor())
			lyrics_ideas.SetCursor(0);
	}
	
	{
		for(int i = 0; i < o.music_styles.GetCount(); i++) {
			String s = o.music_styles[i];
			music_styles.Set(i, 0, 1+i);
			music_styles.Set(i, 1, s);
		}
		INHIBIT_CURSOR(music_styles);
		music_styles.SetCount(o.music_styles.GetCount());
		if (music_styles.GetCount() && !music_styles.IsCursor())
			music_styles.SetCursor(0);
	}
	
}

void LeadWebsites::DataAnalyzedList() {
	MetaDatabase& db = MetaDatabase::Single();
	LeadData& ld = db.lead_data;
	LeadDataAnalysis& sda = db.lead_data.a;
	
	if (!list.IsCursor() || !list_names.IsCursor())
		return;
	
	int idx = list.Get("IDX");
	int list_idx = list_names.GetCursor();
	LeadOpportunity& o = ld.opportunities[idx];
	
	const Vector<String>& l = o.analyzed_lists[list_idx];
	
	{
		for(int i = 0; i < l.GetCount(); i++) {
			const String& v = l[i];
			list_values.Set(i, 0, v);
		}
		INHIBIT_CURSOR(list_values);
		list_values.SetCount(l.GetCount());
	}
	
}

void LeadWebsites::ToolMenu(Bar& bar) {
	bar.Add(t_("Refresh"), AppImg::BlueRing(), THISBACK(Data)).Key(K_CTRL_Q);
	bar.Separator();
	bar.Add(t_("Update website leads"), AppImg::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Update website leads (with MetaEntity)"), AppImg::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	
}

void LeadWebsites::Do(int fn) {
	if (fn == 0) {
		LeadSolver& tm = LeadSolver::Get(Owner::DatabaseUpdate());
		tm.Start();
	}
	else {
		MetaPtrs& p = MetaPtrs::Single();
		if (p.owner) {
			LeadSolver& tm = LeadSolver::Get(*p.owner);
			tm.Start();
		}
	}
}


END_TEXTLIB_NAMESPACE