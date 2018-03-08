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
    // 解析Http报文
    int ParseHttp(char* buff, int len);
    
protected:
    //解析http报文
    int ParseHttpMsg(char* buff, int len);

};


#endif


