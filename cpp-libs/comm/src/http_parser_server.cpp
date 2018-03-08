#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "http_parser_server.h"



CHttpParserServer::CHttpParserServer()
{
    
}

CHttpParserServer::~CHttpParserServer()
{

}

// 解析Http请求
int CHttpParserServer::ParseHttp(char* buff, int len)
{
    //INFO_LOG("enter into CAmHttpHandler ParseReqHttp\n");
    if((NULL == buff) || (0 > len))
    {
        return I_RET_CODE_INPUT_ERR;
    }

    //检查缓冲区
    int iRet = CheckMem(buff, len);
    if(0 != iRet)
    {
    	return iRet;
    }

    //查看是否接收完整
    iRet = CheckIsComplete(http_buf.req_http_buf, http_buf.http_buf_len);
    if(0 != iRet)
    {
    	return iRet;
    }

    //开始解析
    clear();
    iRet = ParseHttpMsg(http_buf.req_http_buf, http_buf.http_buf_len);
    if(0 != iRet)
    {
    	return iRet;
    }

    //查看内容是要解码
    iRet = ContentDecode();
    if(0 != iRet)
    {
    	return iRet;
    }

    return I_RET_CODE_SUCCESS;
}


int CHttpParserServer::ParseHttpMsg(char* buff, int len)
{
    //INFO_LOG("CHttpParser::ParseReqHttpHead() enter.\n");

    char* pBuf = buff;
    //char* head_end = strstr(pBuf, "\r\n\r\n");
    //if (NULL == head_end) return 0;

    //skip first line, find content-len
    char* begin_pos = strstr(pBuf, "\r\n");
    if (NULL == begin_pos) return 0;

    //get the request method and if method=1[GET] then fetch the need info
    http_message.header_item.clear();
    char item_value[4096];
    char* pszPos;
    char* pszStartPos = pBuf;

    memset(item_value, 0x00, sizeof(item_value));
    pszPos = strchr(pszStartPos, ' ');
    if (pszPos)
    {
        char* pParamStart = NULL;

        if (0 == strncasecmp(pszStartPos, "GET", 3)|| 0 == strncasecmp(pszStartPos, "DELETE", 6))
        {
            if( 0 == strncasecmp(pszStartPos, "GET", 3))
            {
                 http_message.http_method = HTTP_METHOD_GET;
            }
            else if(0 == strncasecmp(pszStartPos, "DELETE", 6))
            {
                 http_message.http_method = HTTP_METHOD_DELETE;
            }

            pszStartPos = pszPos + 1;
            pszPos = strchr(pszStartPos, ' ');
            if (pszPos)
            {
                char *pOldszPos = pszPos;

                //pszPos = strchr(pszStartPos, '?');
                pszPos = (char*)memmem(pszStartPos, begin_pos - pszStartPos, "?", sizeof("?") - 1);
                if (pszPos)
                {
                    //获取URI
                    memcpy(item_value, pszStartPos, pszPos - pszStartPos);
                    http_message.http_uri = item_value;

                    //获取参数
                    memset(item_value, 0x00, sizeof(item_value));
                    memcpy(item_value, ++pszPos, pOldszPos - pszPos);
                    ParseHttpParamter(item_value, http_message.uri_paramter);
                    
                }
                else
                {
                    memcpy(item_value, pszStartPos, pOldszPos - pszStartPos);
                    http_message.http_uri = item_value;

                }
            }
            else
            {
                //TRACE_LOG("2 request line without method=%s", pszStartPos);
            }
        }
        else if (0 == strncasecmp(pszStartPos, "POST", 4) || 0 == strncasecmp(pszStartPos, "PUT", 3))
        {
            if(0 == strncasecmp(pszStartPos, "POST", 4))
            {
                http_message.http_method = HTTP_METHOD_POST;
            }
            else if( 0 == strncasecmp(pszStartPos, "PUT", 3))
            {
                 http_message.http_method = HTTP_METHOD_PUT;
            }

            pszStartPos = pszPos + 1;
            //pszPos = strchr(pszStartPos, '?');
            pszPos = (char*)memmem(pszStartPos, begin_pos - pszStartPos, "?", sizeof("?") - 1);
            if (pszPos)
            {
                pParamStart = pszPos + 1;

                //获取URI
                memcpy(item_value, pszStartPos, pszPos - pszStartPos);
                http_message.http_uri = item_value;

                pszPos = strchr(pszStartPos, ' ');
                if (pszPos)
                {
                    //获取参数
                    memset(item_value, 0x00, sizeof(item_value));
                    memcpy(item_value, pParamStart, pszPos - pParamStart);
                    ParseHttpParamter(item_value, http_message.uri_paramter);
  
                }
            }
            else
            {
                pszPos = strchr(pszStartPos, ' ');
                if (pszPos)
                {
                    memcpy(item_value, pszStartPos, pszPos - pszStartPos);
                    http_message.http_uri = item_value;
                }
                else
                {
                   // ERROR_LOG("request line without method=%s", pszStartPos);
                   return I_RET_CODE_FORMAT_ERR;
                }
            }
        }
        else
        {
            //ERROR_LOG("request method is not get or post");
            return I_RET_CODE_METHOD_ERR;
        }
    }
    else
    {
        //ERROR_LOG("request line parse error, request line=%s", pszStartPos);
        return I_RET_CODE_FORMAT_ERR;
    }


    begin_pos += 2;

    char* end_pos = NULL;
    char item_name[256];

    for(;;)
    {
        // 又来一个\r\n表示header结束
        if ( memcmp(begin_pos,"\r\n", 2) == 0 )
        {
            // parse http-head complete
            http_head_len = (begin_pos - pBuf) + 2;

            map<string, string>::iterator iter = http_message.header_item.find(string("Transfer-Encoding"));
            if(iter != http_message.header_item.end())
            {
                //chunked编码
			    if(strcasecmp(iter->second.c_str(), "chunked") == 0)
                {
				    int chunk_pos    = http_head_len - 2;
                    http_content_len = 0;

				    return ParseHttpChunked(chunk_pos);
			    }
            }

            // 如果 head + content - len <= buf总长度 , 表示整个请求收完
            if ( (http_head_len + http_content_len) <= len )
            {
                begin_pos += 2;
                http_message.http_body.insert(0, begin_pos, http_content_len);   
                return  I_RET_CODE_SUCCESS;
            }

            //return 0;
        }

        // 寻找header头部的下一个item
        while(*begin_pos == ' ' ) begin_pos++;
        end_pos = strchr(begin_pos,':');
        if ( !end_pos )
            return 0;

        // 保存item_name
        memset(item_name, 0x00, sizeof(item_name));
        memcpy(item_name, begin_pos, end_pos - begin_pos);

        // 寻找item_value
        // 跳过 : 号
        begin_pos = end_pos + 1; 

        // 跳过空格
        while(*begin_pos == ' ' ) begin_pos++;

        end_pos = strstr(begin_pos,"\r\n");
        if (!end_pos) return 0;

         memset(item_value, 0x00, sizeof(item_value));

        do
        {
            if (strncasecmp(item_name, "Content-Length", HTTP_HEADER_CONTENT_LENGTH_LEN) == 0 )
            {
                memcpy(item_value, begin_pos, end_pos - begin_pos);
                http_content_len = atoi(item_value);
                http_message.header_item.insert(pair<string, string>(item_name, item_value));
                break;
            }

            if ( strncasecmp(item_name, "Transfer-Encoding", HTTP_HEADER_TRANSFER_ENCODING_LEN) == 0 )
            {
                memcpy(item_value, begin_pos, end_pos - begin_pos);
                http_message.header_item.insert(pair<string, string>(item_name, item_value));
                break;
            }

            if ( strncasecmp(item_name, "User-Agent", HTTP_HEADER_USER_AGENT_LEN) == 0 )
            {
                 memcpy(item_value, begin_pos, end_pos - begin_pos);
			     http_message.header_item.insert(pair<string, string>(item_name, item_value));
                 break;
            }

            if ( strncasecmp(item_name, "Content-Type", HTTP_HEADER_CONTENT_TYPE_LEN) == 0 )
            {
                memcpy(item_value, begin_pos, end_pos - begin_pos);
                http_message.header_item.insert(pair<string, string>(item_name, item_value));
                break;
            }

            if ( strncasecmp(item_name, "Connection", HTTP_HEADER_CONNECTION_LEN) == 0 )
            {
                 memcpy(item_value, begin_pos, end_pos - begin_pos);
                 http_message.header_item.insert(pair<string, string>(item_name, item_value));
                 break;
            }

            if ( strncasecmp(item_name, "Accept-Encoding", HTTP_HEADER_ACCEPT_ENCODING_LEN) == 0 )
            {
                 memcpy(item_value, begin_pos, end_pos - begin_pos);
                 http_message.header_item.insert(pair<string, string>(item_name, item_value));
                 break;
            }

            if ( strncasecmp(item_name, "Accept", HTTP_HEADER_ACCEPT_LEN) == 0 )
            {
                memcpy(item_value, begin_pos, end_pos - begin_pos);
                http_message.header_item.insert(pair<string, string>(item_name, item_value));
                break;
            }

            if ( strncasecmp(item_name, "Accept-Language", HTTP_HEADER_ACCEPT_LANGUAGE_LEN) == 0 )
            {
                memcpy(item_value, begin_pos, end_pos - begin_pos);
                http_message.header_item.insert(pair<string, string>(item_name, item_value));
                break;
            }

            if ( strncasecmp(item_name, HTTP_HEADER_X_AUTHENTI_SCOPE_NAME, HTTP_HEADER_X_AUTHENTI_SCOPE_LEN) == 0 )
            {
                memcpy(item_value, begin_pos, end_pos - begin_pos);
                http_message.header_item.insert(pair<string, string>(item_name, item_value));
                break;
            }

        }while(0);

        begin_pos  = end_pos + 2;
    }


	return 0;
}


/*         
int main(int argc, char ** argv)
{
	
	 //char sBuf[2048] = "POST /search?hl=zh-CN&source=hp&q=domety&aq=f&oq=AA HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; TheWorld)\r\nConnection:Keep-Alive\r\nContent-Length: 11\r\n\r\nwondershare";
	 char sBuf[2048] = "POST /search?hl=zh-CN&source=hp&q=domety&aq=f&oq=AA HTTP/1.1\r\n"     \
                       "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n" \
                       "Transfer-Encoding: chunked\r\n\r\n" \
                       "3\r\n" \
                       "abc\r\n" \
                       "b\r\n" \
                       "helloworld!\r\n" \
                       "0\r\n\r\n";

	  
    STHttpMsg http_msg;
    
    CHttpParserServer http_parser;
    int iRet = http_parser.ParseHttp(sBuf, strlen(sBuf));
    printf("iRet: %d\n", iRet);
    
    if(I_RET_CODE_SUCCESS == iRet)
    {
        http_msg = http_parser.GetHttpMessage();
        printf("http_msg.http_method: %d\n", http_msg.http_method);
        printf("http_msg.http_uri: %s\n", http_msg.http_uri.c_str());
        
        
        std::map<std::string, std::string>::iterator iter = http_msg.uri_paramter.begin();
        for(;iter != http_msg.uri_paramter.end(); ++iter)
        {
            printf("%s -- %s\n", iter->first.c_str(), iter->second.c_str());
        }
        
        printf("\n\n");
        
        iter = http_msg.header_item.begin();
        for(;iter != http_msg.header_item.end(); ++iter)
        {
            printf("%s -- %s\n", iter->first.c_str(), iter->second.c_str());
        }
        
        printf("http_msg.http_body: %s\n", http_msg.http_body.c_str());
        
        printf("\n\n");
    }
    
    
    
    return 0;
}               
*/

