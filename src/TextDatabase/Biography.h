#ifndef _TextDatabase_Biography_h_
#define _TextDatabase_Biography_h_


BEGIN_TEXTLIB_NAMESPACE

#define BIOCATEGORY_LIST \
	BIOCATEGORY(HOME) \
	BIOCATEGORY(HOME_KITCHEN) \
	BIOCATEGORY(HOME_WORKDESK) \
	BIOCATEGORY(HOME_BEDROOM) \
	BIOCATEGORY(HOME_COMPUTER) \
	BIOCATEGORY(PEOPLE_FRIENDS) \
	BIOCATEGORY(PEOPLE_CLOSE_FRIENDS) \
	BIOCATEGORY(PEOPLE_ENEMIES) \
	BIOCATEGORY(PEOPLE_PUBLIC_IN_GENERAL) \
	BIOCATEGORY(PEOPLE_SYSTEMIC_ENEMIES) \
	BIOCATEGORY(SEXUALITY_ADULT_ENTERNTAINMENT) \
	BIOCATEGORY(SEXUALITY_EXPERIENCES) \
	BIOCATEGORY(EDUCATION_CAMPUS) \
	BIOCATEGORY(EDUCATION_HOME) \
	BIOCATEGORY(EDUCATION_INTERNET) \
	BIOCATEGORY(EDUCATION_MUSIC_PRODUCTION) \
	BIOCATEGORY(FAMILY_PARENTAL) \
	BIOCATEGORY(FAMILY_OWN) \
	BIOCATEGORY(EVENTS_MUSIC) \
	BIOCATEGORY(EVENTS_SPORT) \
	BIOCATEGORY(EVENTS_FOOD) \
	BIOCATEGORY(EVENTS_OTHER) \
	BIOCATEGORY(POLITICAL) \
	BIOCATEGORY(POLITICAL_PIRATISM) \
	BIOCATEGORY(PERSONAL_TASTE_FOOD) \
	BIOCATEGORY(PERSONAL_TASTE_ENTERNTAINMENT) \
	BIOCATEGORY(PERSONAL_TASTE_VIDEOGAMES) \
	BIOCATEGORY(PERSONAL_TASTE_MOVIES) \
	BIOCATEGORY(PERSONAL_TASTE_TV) \
	BIOCATEGORY(PERSONAL_TASTE_BOOKS) \
	BIOCATEGORY(PERSONAL_TASTE_WORLD_INFORMATION) \
	BIOCATEGORY(PERSONAL_TASTE_HOBBIES) \
	BIOCATEGORY(PERSONAL_TASTE_PHYSICAL_EXERCISE) \
	BIOCATEGORY(PERSONAL_TASTE_ADVENTURES) \
	BIOCATEGORY(PERSONAL_TASTE_YOUTUBE) \
	BIOCATEGORY(TRADING_BUYING) \
	BIOCATEGORY(TRADING_SELLING) \
	BIOCATEGORY(HEALTH) \
	BIOCATEGORY(HEALTH_SUPPLEMENTS) \
	BIOCATEGORY(HEALTH_MEDICATION) \
	BIOCATEGORY(HEALTH_DOCTOR) \
	BIOCATEGORY(HEALTH_OPERATIONS) \
	BIOCATEGORY(ANIMALS_PETS) \
	BIOCATEGORY(ANIMALS_WILD) \
	BIOCATEGORY(WORK_PROGRAMMING) \
	BIOCATEGORY(WORK_MUSIC_COLLABORATION) \
	BIOCATEGORY(WORK_MUSIC_PRODUCTION) \
	BIOCATEGORY(WORK_SONGWRITING) \
	BIOCATEGORY(BADGE_OF_HONOR) \
	BIOCATEGORY(BADGE_OF_SHAME) \
	BIOCATEGORY(BADGE_OF_JUDGEMENT) \



enum {
	#define BIOCATEGORY(x) BIOCATEGORY_##x,
	BIOCATEGORY_LIST
	#undef BIOCATEGORY
	
	BIOCATEGORY_COUNT
};

String KeyToName(String s);
String GetBiographyCategoryEnum(int i);
String GetBiographyCategoryKey(int i);


enum {
	TIME_ACCURACY_NONE,
	TIME_ACCURACY_DATE,
	TIME_ACCURACY_TIME,
	
	TIME_ACCURACY_COUNT,
};

struct BioImage {
	String keywords, text, native_text;
	String image_keywords, image_text;
	String location;
	String people;
	Time time;
	int time_accuracy = TIME_ACCURACY_NONE;
	
	void Jsonize(JsonIO& json) {
		json
			("keywords", keywords)
			("text", text)
			("native_text", native_text)
			("image_keywords", image_keywords)
			("image_text", image_text)
			("location", location)
			("people", people)
			("time", time)
			("time_accuracy", time_accuracy)
			;
	}
};

struct BioYear {
	int year = 0;
	String keywords, text, native_text;
	Array<BioImage> images;
	
	void Jsonize(JsonIO& json) {
		json
			("year", year)
			("keywords", keywords)
			("native_text", native_text)
			("text", text)
			("images", images)
			;
	}
};

struct BiographyCategory {
	Array<BioYear> years;
	
	void Jsonize(JsonIO& json) {
		json
			("years", years)
			;
	}
	BioYear& GetAdd(int year);
	int GetFilledCount() const;
	int GetFilledImagesCount() const;
};

struct Biography {
	ArrayMap<String, BiographyCategory> categories;
	
	
	
	void Jsonize(JsonIO& json) {
		json
			("categories", categories)
			;
	}
	BiographyCategory& GetAdd(Owner& o, int enum_);
	
};



END_TEXTLIB_NAMESPACE


#endif
