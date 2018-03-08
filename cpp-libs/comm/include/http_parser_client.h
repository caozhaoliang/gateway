#ifndef _HTTP_PARSER_CLIENT_H_
#define _HTTP_PARSER_CLIENT_H_

#include <map>
#include <string>
#include <vector>
#include "http_parser.h"

using namespace std;



class CHttpParserClient: public CHttpParser
{
public:
	CHttpParserClient();
	~CHttpParserClient();

public:
    // ����Http����
    int ParseHttp(char* buff, int len);
    
protected:
    //����http����
    int ParseHttpMsg(char* buff, int len);

};


#endif


