#include "TextDatabase.h"


BEGIN_TEXTLIB_NAMESPACE


#if 0
void Owner::CopyOld() {
	MetaDatabase& mdb = MetaDatabase::Single();
	
	int c = mdb.meta_entities.GetCount();
	
	mdb.owners.SetCount(c);
	for(int i = 0; i < mdb.owners.GetCount(); i++) {
		Owner& o = mdb.owners[i];
		const MetaEntity& e = mdb.meta_entities[i];
		
		o.name = e.name;
		o.year_of_birth = e.year_of_birth;
		o.year_of_hobbyist_begin = e.year_of_hobbyist_begin;
		o.year_of_career_begin = e.year_of_career_begin;
		o.is_guitarist = e.is_guitarist;
		o.electronic_tools = e.electronic_tools;
		
		o.profiles.SetCount(max(o.profiles.GetCount(), 1));
		Profile& p = o.profiles[0];
		p.name = e.name;
		p.begin = Date(o.year_of_hobbyist_begin,1,1);
		p.biography = e.biography;
		p.preferred_genres = e.preferred_genres;
		p.languages = e.languages;
	}
	
}
#endif


Owner& Owner::DatabaseUpdate() {
	static Owner e;
	return e;
}

void Owner::Store() {
	String dir = AppendFileName(MetaDatabase::GetUserDirectory(), "share");
	String path = AppendFileName(dir, name + ".json");
	RealizeDirectory(dir);
	StoreAsJsonFileStandard(*this, path);
}

void Owner::Load(String name) {
	String dir = AppendFileName(MetaDatabase::GetUserDirectory(), "share");
	String path = AppendFileName(dir, name + ".json");
	RealizeDirectory(dir);
	LoadFromJsonFileStandard(*this, path);
}

int Owner::FindRole(const String& name) const {
	for(int i = 0; i < roles.GetCount(); i++)
		if (roles[i].name == name)
			return i;
	return -1;
}

int Role::FindAction(const String& name) const {
	for(int i = 0; i < actions.GetCount(); i++)
		if (actions[i].name == name)
			return i;
	return -1;
}

int RoleAction::FindEvent(const String& event) const {
	for(int i = 0; i < events.GetCount(); i++)
		if (events[i].text == event)
			return i;
	return -1;
}



int Owner::GetOpportunityScore(const LeadOpportunity& opp) const {
	int score = 0;
	
	Date now = GetSysTime();
	int active_years = now.year - year_of_career_begin;
	bool is_beginner = active_years < 3;
	
	if (opp.analyzed_booleans.GetCount() != LISTING_SONG_BOOLEAN_COUNT) {
		score -= 10;
	}
	else {
		if (is_beginner) {
			score +=
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_IS_ALLOWED_MALE] +
				-2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_IS_ALLOWED_FEMALE] +
				5	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_ALLOWED_DEMO_QUALITY] +
				-1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MUST_BE_BROADCAST_QUALITY] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_SIGNIFICANT_INCOME] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_DIFFICULT_TO_DETERMINE] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_COULD_BE_ROYALTIES] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_FUTURE_COLLABS] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_IS_EXCLUSIVE] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_LISTING_WRITER_IS_DECISION_MAKER] +
				-1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_LISTING_WRITER_IS_COMPANY] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_LISTING_WRITER_IS_PERSON] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MUSIC_IS_SPECIFIC_GENRE] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MUSIC_IS_SPECIFIC_TEMPO] +
				3	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_REVIEWER_IS_GIVING_FEEDBACK] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_REVIEWER_IS_GIVING_RATING] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_REVIEWER_IS_HAVING_KNOWN_TIMELINE] +
				3	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_REVIEWER_IS_SPECIFIED] +
				3	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_ARTIST_IS_FEATURED] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_HIGHEST_PROFESSIONALISM] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_APPRECIATION_TOWARDS_BEGINNERS] +
				-5	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_LACK_OF_CARE_FOR_THE_SONGWRITER] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_EASY_ACCEPTANCE] +
				-5	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_DIFFICULT_ACCEPTANCE] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_ACCEPTED] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_SUCCESSFUL_LAUNCH] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_HIT_SONG] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_RECORD_DEAL] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_RADIO_ROTATION] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_TV_ROTATION] +
				5	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_RECEIVING_LEADS] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_COLLABORATION] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_PROMOTED] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_DISCOVERED_BY_PUBLISHERS] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_NETWORKING] +
				0
				;
		}
		else {
			score +=
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_IS_ALLOWED_MALE] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_IS_ALLOWED_FEMALE] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_ALLOWED_DEMO_QUALITY] +
				5	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MUST_BE_BROADCAST_QUALITY] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_SIGNIFICANT_INCOME] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_DIFFICULT_TO_DETERMINE] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_COULD_BE_ROYALTIES] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_FUTURE_COLLABS] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_IS_EXCLUSIVE] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_LISTING_WRITER_IS_DECISION_MAKER] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_LISTING_WRITER_IS_COMPANY] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_LISTING_WRITER_IS_PERSON] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MUSIC_IS_SPECIFIC_GENRE] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_MUSIC_IS_SPECIFIC_TEMPO] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_REVIEWER_IS_GIVING_FEEDBACK] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_REVIEWER_IS_GIVING_RATING] +
				1	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_REVIEWER_IS_HAVING_KNOWN_TIMELINE] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_REVIEWER_IS_SPECIFIED] +
				3	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_ARTIST_IS_FEATURED] +
				3	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_HIGHEST_PROFESSIONALISM] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_APPRECIATION_TOWARDS_BEGINNERS] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_LACK_OF_CARE_FOR_THE_SONGWRITER] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_EASY_ACCEPTANCE] +
				0	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_OPPORTUNITY_CONVEYING_DIFFICULT_ACCEPTANCE] +
				3	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_ACCEPTED] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_SUCCESSFUL_LAUNCH] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_HIT_SONG] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_RECORD_DEAL] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_RADIO_ROTATION] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_TV_ROTATION] +
				5	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_RECEIVING_LEADS] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_COLLABORATION] +
				10	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_PROMOTED] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_DISCOVERED_BY_PUBLISHERS] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_CHANCES_IS_NETWORKING] +
				0
				;
		}
		
		{
			score +=
				5	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_PLACEMENT_TV] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_PLACEMENT_RADIO] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_PLACEMENT_MOVIE] +
				5	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_PLACEMENT_AD] +
				2	* opp.analyzed_booleans[LISTING_SONG_BOOLEAN_PLACEMENT_PLAYLIST] +
				0
				;
		}
	}
	
	return score;
}


END_TEXTLIB_NAMESPACE

