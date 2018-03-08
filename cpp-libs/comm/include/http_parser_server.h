#ifndef _HTTP_PARSER_SERVER_H_
#define _HTTP_PARSER_SERVER_H_

#include <map>
#include <string>
#include <vector>
#include "http_parser.h"

using namespace std;



class CHttpParserServer: public CHttpParser
{
public:
	CHttpParserServer();
	~CHttpParserServer();

public:
    // 解析Http报文
    int ParseHttp(char* buff, int len);
    
protected:
    //解析http报文头
    int ParseHttpMsg(char* buff, int len);

};


#endif


