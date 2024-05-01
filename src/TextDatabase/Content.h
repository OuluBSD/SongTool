#ifndef _TextDatabase_Content_h_
#define _TextDatabase_Content_h_


BEGIN_TEXTLIB_NAMESPACE


struct Content : DataFile {
	Array<Script>	scripts;
	
	
	Script& GetAddScript(String name);
};


END_TEXTLIB_NAMESPACE


#endif
