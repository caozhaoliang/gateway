
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "http_parser_client.h"



CHttpParserClient::CHttpParserClient()
{
    
}

CHttpParserClient::~CHttpParserClient()
{

}


// 解析Http请求
int CHttpParserClient::ParseHttp(char* buff, int len)
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

    //查看内容是要gzip等解码
    iRet = ContentDecode();
    if(0 != iRet)
    {
    	return iRet;
    }

    return I_RET_CODE_SUCCESS;
}


int CHttpParserClient::ParseHttpMsg(char* buff, int len)
{
    //INFO_LOG("CHttpParser::ParseReqHttpHead() enter.\n");

    char* pBuf = buff;
    //char* head_end = strstr(pBuf, "\r\n\r\n");
    //if (NULL == head_end) return 0;

    //skip first line, find content-len
    char* begin_pos = strstr(pBuf, "\r\n");
    if (NULL == begin_pos) return 0;

    //获取状态行
    http_message.header_item.clear();
    char item_value[4096];
    char* status_end;
    char* status_begin = pBuf;

    status_end = strchr(status_begin, ' ');
    if(status_end)
    {
        //版本号
        memset(item_value, 0x00, sizeof(item_value));
        memcpy(item_value, status_begin, status_end - status_begin);
        http_message.http_version = item_value;

        status_begin = status_end;
        status_end   = begin_pos;

        // 跳过空格
        while(*status_begin == ' ') status_begin++;

        //HTTP响应码
        memset(item_value, 0x00, sizeof(item_value));
        memcpy(item_value, status_begin, status_end - status_begin);
        http_message.http_status_desc = item_value;
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

            std::string transfer_encodeing("Transfer-Encoding");
            map<string, string>::iterator iter = http_message.header_item.find(transfer_encodeing);
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

        // 保存 item_name
        memset(item_name, 0x00, sizeof(item_name));
        memcpy(item_name, begin_pos, end_pos - begin_pos);

        // 寻找 item_value
        // 跳过 : 号
        begin_pos = end_pos + 1; 

        // 跳过空格
        while(*begin_pos == ' ' ) begin_pos++;

        end_pos = strstr(begin_pos,"\r\n");
        if (!end_pos) return 0;

         memset(item_value, 0x00, sizeof(item_value));
        if (strncasecmp(item_name, "Content-Length", HTTP_HEADER_CONTENT_LENGTH_LEN) == 0 )
        {
            memcpy(item_value, begin_pos, end_pos - begin_pos);
            http_content_len = atoi(item_value);
            http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        if (strncasecmp(item_name, "Content-Encoding", HTTP_HEADER_CONTENT_LENGTH_LEN) == 0 )
        {
            memcpy(item_value, begin_pos, end_pos - begin_pos);
            //http_content_len = atoi(item_value);
            http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        if ( strncasecmp(item_name, "Transfer-Encoding", HTTP_HEADER_TRANSFER_ENCODING_LEN) == 0 )
        {
            memcpy(item_value, begin_pos, end_pos - begin_pos);
            http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        if ( strncasecmp(item_name, "User-Agent", HTTP_HEADER_USER_AGENT_LEN) == 0 )
        {
             memcpy(item_value, begin_pos, end_pos - begin_pos);
			 http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        if ( strncasecmp(item_name, "Content-Type", HTTP_HEADER_CONTENT_TYPE_LEN) == 0 )
        {
            memcpy(item_value, begin_pos, end_pos - begin_pos);
            http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        if ( strncasecmp(item_name, "Connection", HTTP_HEADER_CONNECTION_LEN) == 0 )
        {
             memcpy(item_value, begin_pos, end_pos - begin_pos);
             http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        if ( strncasecmp(item_name, "Accept-Encoding", HTTP_HEADER_ACCEPT_ENCODING_LEN) == 0 )
        {
             memcpy(item_value, begin_pos, end_pos - begin_pos);
             http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        if ( strncasecmp(item_name, "Accept", HTTP_HEADER_ACCEPT_LEN) == 0 )
        {
            memcpy(item_value, begin_pos, end_pos - begin_pos);
            http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        if ( strncasecmp(item_name, "Accept-Language", HTTP_HEADER_ACCEPT_LANGUAGE_LEN) == 0 )
        {
            memcpy(item_value, begin_pos, end_pos - begin_pos);
            http_message.header_item.insert(pair<string, string>(item_name, item_value));
        }

        begin_pos  = end_pos + 2;
    }

	return 0;
}


/*           
int main(int argc, char ** argv)
{
	  char sBuf[2048] = "HTTP/1.1 200 OK\r\n" \
                        "Server: Apache-Coyote/1.1\r\n" \
                        "Content-Type: text/html;charset=UTF-8\r\n" \
                        "Transfer-Encoding: chunked\r\n" \
                        "Date: Wed, 10 Nov 2010 07:10:05 GMT\r\n\r\n" \
                        "d\r\n" \
                        "hello world! \r\n" \
                        "9\r\n" \
                        "shenzhen \r\n" \
                        "C\r\n" \
                        "wondershare!\r\n" \
                        "0\r\n\r\n";
	  
    STHttpMsg http_msg;
    
    CHttpParserClient http_parser;
    int iRet = http_parser.ParseHttp(sBuf, strlen(sBuf));
    printf("iRet: %d\n", iRet);
    
    if(I_RET_CODE_SUCCESS == iRet)
    {
        http_msg = http_parser.GetHttpMessage();
        printf("http_msg.http_version: %s\n", http_msg.http_version.c_str());
        printf("http_msg.http_status_desc: %s\n", http_msg.http_status_desc.c_str());
        
        
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

