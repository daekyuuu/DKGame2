#ifndef __S2_JSON_FILE_H__
#define __S2_JSON_FILE_H__

#include ".\ThirdParty/rapidjson/document.h"
#include ".\ThirdParty/rapidjson/error/en.h"
using namespace rapidjson;

typedef Document				S2Json;

class S2_EXPORT_BASE S2JsonFile
{
	S2Json					m_Json;

public:
	S2JsonFile();
	~S2JsonFile();

	S2Json*					Create( TCHAR* strFile );
	void					OnDestroy();
};

#endif
