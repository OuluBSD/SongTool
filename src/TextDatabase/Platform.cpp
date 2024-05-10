#include "TextDatabase.h"


BEGIN_TEXTLIB_NAMESPACE


int PlatformComment::GetTotalComments() const {
	int t = 1;
	for (const PlatformComment& pc : responses)
		t += pc.GetTotalComments();
	return t;
}

int Entry::GetTotalComments() const {
	int t = 0;
	for (const PlatformComment& pc : comments)
		t += pc.GetTotalComments();
	return t;
}






const Vector<Platform>& GetPlatforms() {
	static Vector<Platform> a;
	if (!a.IsEmpty())
		return a;
	a.SetCount(PLATFORM_COUNT);
	{
		Platform& p = a[PLATFORM_EMAIL];
		p.name = "E-Mail";
		p.has_title = true;
		p.has_message = true;
		p.has_link_promotion = true;
		p.AddRole(SOCIETYROLE_MATERNAL_AUNT);
		p.AddRole(SOCIETYROLE_MATERNAL_UNCLE);
		p.AddRole(SOCIETYROLE_PATERNAL_AUNT);
		p.AddRole(SOCIETYROLE_PATERNAL_UNCLE);
		p.AddRole(SOCIETYROLE_MATERNAL_FEMALE_COUSIN);
		p.AddRole(SOCIETYROLE_MATERNAL_MALE_COUSIN);
		p.AddRole(SOCIETYROLE_PATERNAL_FEMALE_COUSIN);
		p.AddRole(SOCIETYROLE_PATERNAL_MALE_COUSIN);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE);
	}
	{
		Platform& p = a[PLATFORM_TWITTER];
		p.name = "Twitter";
		p.has_comments = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_video = true;
		p.has_reel = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE); // Twitter
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE); // LinkedIn
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE);
	}
	{
		Platform& p = a[PLATFORM_THREADS];
		p.name = "Threads";
		p.has_comments = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_video = true;
		p.has_reel = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE); // Twitter
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MATERNAL_GRANDMOTHER);
		p.AddRole(SOCIETYROLE_PATERNAL_GRANDMOTHER);
		p.AddRole(SOCIETYROLE_MATERNAL_GRANDFATHER);
		p.AddRole(SOCIETYROLE_PATERNAL_GRANDFATHER);
	}
	{
		Platform& p = a[PLATFORM_FACEBOOK];
		p.name = "Facebook";
		p.has_comments = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_video = true;
		p.has_reel = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE);
		p.AddRole(SOCIETYROLE_WIFE);
		p.AddRole(SOCIETYROLE_BEST_MAN);
		p.AddRole(SOCIETYROLE_MOTHER);
		p.AddRole(SOCIETYROLE_FATHER);
		p.AddRole(SOCIETYROLE_MATERNAL_GRANDMOTHER);
		p.AddRole(SOCIETYROLE_PATERNAL_GRANDMOTHER);
		p.AddRole(SOCIETYROLE_MATERNAL_GRANDFATHER);
		p.AddRole(SOCIETYROLE_PATERNAL_GRANDFATHER);
	}
	{
		Platform& p = a[PLATFORM_INSTAGRAM];
		p.name = "Instagram";
		p.has_comments = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_video = true;
		p.has_reel = true;
		p.has_image = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_IMAGE_SITE); // Instagram
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_IMAGE_SITE); // Instagram
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE); // TikTok,_Instagram
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE); // Twitter
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE);
		p.AddRole(SOCIETYROLE_WIFE);
		p.AddRole(SOCIETYROLE_MOTHER);
		p.AddRole(SOCIETYROLE_BIG_SISTER);
		p.AddRole(SOCIETYROLE_LITTLE_SISTER);
	}
	{
		Platform& p = a[PLATFORM_TIKTOK];
		p.name = "TikTok";
		p.has_comments = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_reel = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE); // TikTok,_Instagram
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_IMAGE_SITE); // Instagram
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_IMAGE_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE); // Twitter
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE); // Twitch
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
	}
	{
		Platform& p = a[PLATFORM_GETTR];
		p.name = "Gettr";
		p.has_comments = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_reel = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_REPUBLICAN_PUBLIC_MESSAGE_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_REPUBLICAN_PUBLIC_MESSAGE_SITE);
		p.AddRole(SOCIETYROLE_MATERNAL_AUNT);
		p.AddRole(SOCIETYROLE_PATERNAL_AUNT);
		p.AddRole(SOCIETYROLE_MATERNAL_UNCLE);
		p.AddRole(SOCIETYROLE_PATERNAL_UNCLE);
		p.AddRole(SOCIETYROLE_MATERNAL_FEMALE_COUSIN);
		p.AddRole(SOCIETYROLE_PATERNAL_FEMALE_COUSIN);
	}
	{
		Platform& p = a[PLATFORM_LINKEDIN];
		p.name = "LinkedIn";
		p.has_comments = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_video = true;
		p.has_image = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE); // LinkedIn
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_RECRUITER_FOR_WORK);
		p.AddRole(SOCIETYROLE_MALE_RECRUITER_FOR_WORK);
		p.AddRole(SOCIETYROLE_FEMALE_SUPERIOR_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_MALE_SUPERIOR_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_FEMALE_PEER_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_MALE_PEER_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_MATERNAL_GRANDMOTHER);
		p.AddRole(SOCIETYROLE_PATERNAL_GRANDMOTHER);
		p.AddRole(SOCIETYROLE_MATERNAL_GRANDFATHER);
		p.AddRole(SOCIETYROLE_PATERNAL_GRANDFATHER);
	}
	{
		Platform& p = a[PLATFORM_SOUNDCLOUD];
		p.name = "Soundcloud";
		p.has_comments = true;
		p.has_title = true;
		p.has_hashtags = true;
		p.has_audio = true;
		p.has_music = true;
		p.has_image = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER);
	}
	{
		Platform& p = a[PLATFORM_MUSIC_DISTRIBUTOR];
		p.name = "Music Distributor";
		p.has_title = true;
		p.has_music = true;
		p.has_image = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
	}
	{
		Platform& p = a[PLATFORM_YOUTUBE];
		p.name = "YouTube";
		p.has_comments = true;
		p.has_message = true;
		p.has_title = true;
		p.has_hashtags = true;
		p.has_video = true;
		p.has_reel = true;
		p.has_image = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER);
	}
	{
		Platform& p = a[PLATFORM_VK];
		p.name = "VK";
		p.has_comments = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_video = true;
		p.has_reel = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
	}
	{
		Platform& p = a[PLATFORM_REDDIT];
		p.name = "Reddit";
		p.has_comments = true;
		p.has_message = true;
		p.has_title = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER);
	}
	{
		Platform& p = a[PLATFORM_FORUM];
		p.name = "Forum";
		p.has_comments = true;
		p.has_message = true;
		p.has_title = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_FEMALE_PEER_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_MALE_PEER_IN_MILITARY_SERVICE);
	}
	{
		Platform& p = a[PLATFORM_BLOGGER];
		p.name = "Blogger";
		p.has_comments = true;
		p.has_message = true;
		p.has_title = true;
		p.has_hashtags = true;
		p.has_audio = true;
		p.has_music = true;
		p.has_video = true;
		p.has_reel = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_WIFE);
		p.AddRole(SOCIETYROLE_MOTHER);
		p.AddRole(SOCIETYROLE_LITTLE_SISTER);
		p.AddRole(SOCIETYROLE_BIG_SISTER);
		p.AddRole(SOCIETYROLE_MATERNAL_GRANDMOTHER);
		p.AddRole(SOCIETYROLE_PATERNAL_GRANDMOTHER);
		p.AddRole(SOCIETYROLE_MATERNAL_AUNT);
		p.AddRole(SOCIETYROLE_PATERNAL_AUNT);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
	}
	{
		Platform& p = a[PLATFORM_WEBSITE];
		p.name = "Website";
		p.has_message = true;
		p.has_title = true;
		p.has_hashtags = true;
		p.has_audio = true;
		p.has_music = true;
		p.has_video = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE); // LinkedIn
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE); // LinkedIn
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
	}
	{
		Platform& p = a[PLATFORM_TWITCH];
		p.name = "Twitch";
		p.has_comments = true;
		p.has_video = true;
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE); // Twitch
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE); // Twitch
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE); // TikTok,_Instagram
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE); // TikTok,_Instagram
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_INFLUENCER_FOR_ART_AND_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_INFLUENCER_FOR_ART_AND_MUSIC);
	}
	{
		Platform& p = a[PLATFORM_STUMBLE];
		p.name = "Stumble";
		p.has_link_promotion = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
	}
	{
		Platform& p = a[PLATFORM_GITHUB];
		p.name = "GitHub";
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // Soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // Soundcloud
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
	}
	{
		Platform& p = a[PLATFORM_MYSPACE];
		p.name = "MySpace";
		p.has_comments = true;
		p.has_title = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_music = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_IMAGE_SITE); // Instagram
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_IMAGE_SITE); // Instagram
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE); // TikTok,_Instagram
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_REEL_VIDEO_SITE); // TikTok,_Instagram
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE); // Twitter
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE); // Twitter
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE); // Twitch
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_VIDEO_STREAMING_SITE); // Twitch
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
	}
	{
		Platform& p = a[PLATFORM_MIKSERINET];
		p.name = "Mikseri.net";
		p.has_comments = true;
		p.has_title = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_music = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
	}
	{
		Platform& p = a[PLATFORM_IRCGALLERIA];
		p.name = "Irc-Galleria.net";
		p.has_comments = true;
		p.has_hashtags = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_SUPERIOR_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_MALE_SUPERIOR_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_FEMALE_INFERIOR_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_MALE_INFERIOR_IN_MILITARY_SERVICE);
	}
	{
		Platform& p = a[PLATFORM_DISCORD];
		p.name = "Discord";
		p.has_comments = true;
		p.has_hashtags = true;
		p.has_audio = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_PROGRAMMING);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE); // Twitter
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SHALLOW_PUBLIC_MESSAGE_SITE); // Twitter
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
	}
	{
		Platform& p = a[PLATFORM_BANDMIX];
		p.name = "Bandmix";
		p.has_comments = true;
		p.has_title = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_music = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_FORUM_MUSIC);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_WEBSITE_READER);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_WEBSITE_READER);
	}
	{
		Platform& p = a[PLATFORM_BANDCAMP];
		p.name = "Bandcamp";
		p.has_comments = true;
		p.has_title = true;
		p.has_message = true;
		p.has_hashtags = true;
		p.has_music = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_MUSIC_SITE); // soundcloud
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_MUSIC_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_WIFE);
		p.AddRole(SOCIETYROLE_BEST_MAN);
		p.AddRole(SOCIETYROLE_MATERNAL_AUNT);
		p.AddRole(SOCIETYROLE_PATERNAL_AUNT);
		p.AddRole(SOCIETYROLE_MATERNAL_UNCLE);
		p.AddRole(SOCIETYROLE_PATERNAL_UNCLE);
	}
	{
		Platform& p = a[PLATFORM_REMOTEMORE];
		p.name = "RemoteMore";
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_VIDEO_SITE); // YouTube
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_VIDEO_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE); // LinkedIn
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROFSSIONAL_INDUSTRY_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE); // github
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_SOCIAL_PROGRAMMING_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
	}
	{
		Platform& p = a[PLATFORM_KUVAKENET];
		p.name = "Kuvake.net";
		p.has_comments = true;
		p.has_hashtags = true;
		p.has_image = true;
		p.has_link_promotion = true;
		p.has_comment_self_promotion_hack = true;
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MODERATE_FULL_PROFILE_SITE); // Facebook
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_MUSIC_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_MALE_IN_INTERNET_PROGRAMMING_COOPERATION_SITE);
		p.AddRole(SOCIETYROLE_FEMALE_SUPERIOR_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_MALE_SUPERIOR_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_FEMALE_INFERIOR_IN_MILITARY_SERVICE);
		p.AddRole(SOCIETYROLE_MALE_INFERIOR_IN_MILITARY_SERVICE);
	}
	
	
	
	return a;
}

void ProfileData::Jsonize(JsonIO& json) {
	json
		("platforms", platforms)
		;
}

void ProfileData::Load() {
	String dir = AppendFileName(MetaDatabase::Single().dir, "share-common");
	String fname = IntStr64(hash) + ".json";
	String path = AppendFileName(dir, fname);
	
	LoadFromJsonFileStandard(*this, path);
}

void ProfileData::Store() {
	String dir = AppendFileName(MetaDatabase::Single().dir, "share-common");
	String fname = IntStr64(hash) + ".json";
	String path = AppendFileName(dir, fname);
	
	RealizeDirectory(dir);
	StoreAsJsonFileStandard(*this, path);
}

Array<ProfileData>& ProfileData::GetAll() {
	static Array<ProfileData> a;
	return a;
}

ProfileData& ProfileData::Get(Profile& p) {
	Array<ProfileData>& a = GetAll();
	CombineHash ch;
	ch.Do(p.owner->name);
	ch.Do(p.name);
	hash_t h = ch;
	for (ProfileData& pd : a) {
		if (pd.hash == h) {
			ASSERT(pd.profile == &p);
			pd.platforms.SetCount(PLATFORM_COUNT);
			return pd;
		}
	}
	ProfileData& pd = a.Add();
	pd.hash = h;
	pd.Load();
	pd.platforms.SetCount(PLATFORM_COUNT);
	pd.profile = &p;
	return pd;
}

void ProfileData::StoreAll() {
	for (ProfileData& pd : GetAll())
		pd.Store();
}


END_TEXTLIB_NAMESPACE

