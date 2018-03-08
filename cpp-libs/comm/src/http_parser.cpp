#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "ZLibWrapper.h"
#include "http_parser.h"



CHttpParser::CHttpParser() : http_head_len(0), http_content_len(0)
{
    memset(&http_buf, 0x00, sizeof(HTTPBUF));
	http_buf.http_buf_size = HTTP_BUFF_LEN;
	http_buf.http_buf_len  = 0;

    http_buf.req_http_buf = (char *)malloc(http_buf.http_buf_size * sizeof(char));
    if (NULL == http_buf.req_http_buf )
    {
        //ERROR_LOG("CHttpParser::CHttpParser() alloc size:%d error:%s\n",
        //    http_buf.http_buf_size, strerror(errno));
    }
    else
    {
        *(http_buf.req_http_buf + http_buf.http_buf_size - 1) = '\0';
    }
    
}

CHttpParser::~CHttpParser()
{
    if(NULL != http_buf.req_http_buf)
    {
        free(http_buf.req_http_buf);
        http_buf.req_http_buf = NULL;
    }
}

//复位解析器
void CHttpParser::Reset()
{
    ResetHttpBuf(http_buf.req_http_buf, http_buf.http_buf_size, 0);
    clear();
}

//重置http请求缓冲区值
int CHttpParser::ResetHttpBuf(char * buf, int buf_size, int buf_len)
{
    http_buf.req_http_buf  = buf;
    http_buf.http_buf_size = buf_size;
    http_buf.http_buf_len  = buf_len;

    return 0;
}

// 清空数据                               
void CHttpParser::clear()
{
    http_message.http_version.clear();
    http_message.http_uri.clear();
    http_message.uri_paramter.clear();
    http_message.header_item.clear();
    http_message.http_body.clear();
}

// 解析Http请求
int CHttpParser::ParseHttp(char* buff, int len)
{
    return I_RET_CODE_SUCCESS;
}

//检查内存空间
int CHttpParser::CheckMem(char* buff, int len)
{
    //如果预分配的内存不够那么增长预分配的内存的大小
    //最大的内存分配不能超过预设的最大值
    int nleft = http_buf.http_buf_size - http_buf.http_buf_len;
    if ((nleft <= len) && (http_buf.http_buf_size < HTTP_BUFF_MAX_ALLOC_LEN))
    {
        //校验是否超过内存最大限度
        if((len + http_buf.http_buf_size) > HTTP_BUFF_MAX_ALLOC_LEN)
        {
            return I_RET_CODE_MAXMEM_ERR;
        }

        int realloc_size =  0;
        if(len > HTTP_BUFF_LEN)
        {
            realloc_size   =  http_buf.http_buf_size + len + HTTP_BUFF_LEN;
        }
        else
        {
            realloc_size   =  http_buf.http_buf_size + HTTP_BUFF_LEN;
        }
       
        http_buf.req_http_buf = (char*)realloc(http_buf.req_http_buf, realloc_size);
        if (http_buf.req_http_buf == NULL)
        {
            //WARN_LOG("CAmHttpHandler::OnRecv() can't realloc memory for http buffer, size=%d\n", realloc_size);
            return I_RET_CODE_ALLOCMEM_ERR;
        }

        ResetHttpBuf(http_buf.req_http_buf, realloc_size, http_buf.http_buf_len);
    }
    else if ((nleft <= len) && (http_buf.http_buf_size >= HTTP_BUFF_MAX_ALLOC_LEN))
    {
        //ERROR_LOG("CAmHttpHandler::OnRecv() reach the httpbuf size limit buf-size:%d recv-len:%d\n",
        //    http_buf.http_buf_size, http_buf.http_buf_len);
        return I_RET_CODE_MAXMEM_ERR;
    }


    memcpy(http_buf.req_http_buf + http_buf.http_buf_len, buff, len);
     http_buf.http_buf_len += len;
    *(http_buf.req_http_buf + http_buf.http_buf_len) = '\0';

    return 0;
}

//检查是否接收完成
int CHttpParser::CheckIsComplete(char* buff, int len)
{
    char* pBuf = buff;

    //检查头是否接收完整
    //char* pEnd = strstr(pBuf, HTTP_HEADER_END_FLAG);
    char* pEnd = (char*)memmem(pBuf, len, HTTP_HEADER_END_FLAG, sizeof(HTTP_HEADER_END_FLAG) - 1);
    if (NULL == pEnd) 
    {
        return I_RET_CODE_INCOMPLETE;
    }

    //获取包体长度
    int head_Len = 0;
    //char* pBegin = strstr(pBuf, "Content-Length");
    char* pBegin = (char*)memmem(pBuf, pEnd - pBuf, "Content-Length", sizeof("Content-Length") - 1);
    if(NULL == pBegin)
    {
        //没找到包体长度, 查找是否为Transfer-Encoding: chunked
        //pBegin = strstr(pBuf, "Transfer-Encoding");
        pBegin = (char*)memmem(pBuf, pEnd - pBuf, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1);
        if(NULL == pBegin)
        {
            //没有消息体
            return 0;
        }
    }
    else
    {
        //找到包体长度
        head_Len = (pEnd - pBuf) + 4;
    }

    pEnd = strstr(pBegin, "\r\n");
    if (NULL == pEnd)
    {
        return I_RET_CODE_FORMAT_ERR;
    }

    pBegin = strchr(pBegin, ':');
    if (NULL == pBegin)
    {
        return I_RET_CODE_FORMAT_ERR;
    }

    ++pBegin;

    // 跳过空格
    while(*pBegin == ' ') pBegin++;

    int content_len = 0;
    if(0 == strncasecmp(pBegin, "chunked", 7))
    {
        //chunked类型, 校验是否接收完(由于报文体里可能存在相同的字符组, 在解析报文需要检查是否真接收完成)
        //char* pBegin = strstr(pBuf, HTTP_CHUNKED_END_FLAG);
        pBegin = (char*)memmem(pBuf, len, HTTP_CHUNKED_END_FLAG, sizeof(HTTP_CHUNKED_END_FLAG) - 1);
        if(NULL == pBegin)
        {
            return I_RET_CODE_INCOMPLETE;
        }

        //完成接收
        return 0;
    }
    else
    {
        *pEnd = '\0';
        content_len = atoi(pBegin);
        *pEnd = '\r';
    }

    //如果 head + content_len > buf 总长度 , 表示整个请求未收完
    if((head_Len + content_len) > len)
    {
        return  I_RET_CODE_INCOMPLETE;
    }

    return 0;
}

int CHttpParser::ParseHttpMsg(char* buff, int len)
{
	return I_RET_CODE_SUCCESS;
}

//解析http报文中uri所带的参数
int CHttpParser::ParseHttpParamter(char* param_buff, std::map<std::string, std::string> & paramter_map)
{
    if(0 == strlen(param_buff)) return -1;
    
    char* pBegin = param_buff;
    char* pEnd   = NULL;
    char* pMid   = NULL;
    char  sName[2048];
    char  sValue[2048];

    paramter_map.clear();
    while(pEnd = strchr(pBegin, '&'))
    {
        memset(sName, 0x00, sizeof(sName));
        memset(sValue, 0x00, sizeof(sValue));

        if(pMid = strchr(pBegin, '='))
        {
            memcpy(sName, pBegin, pMid - pBegin);

            ++pMid;
            memcpy(sValue, pMid, pEnd - pMid);

            paramter_map.insert(pair<string, string>(sName, sValue));
        }

        pBegin = pEnd + 1;
    }

    //还有最后一个
    if(pMid = strchr(pBegin, '='))
    {
        memset(sName, 0x00, sizeof(sName));
        memset(sValue, 0x00, sizeof(sValue));

        memcpy(sName, pBegin, pMid - pBegin);
        ++pMid;
        memcpy(sValue, pMid, strlen(pMid));

        paramter_map.insert(pair<string, string>(sName, sValue));
    }

    return 0 ;
}

//解析chunked类型的报文
int CHttpParser::ParseHttpChunked(int chunk_pos)
{
    int chunk_size=0;
	char* begin_pos;
	char* end_pos;
	
	//http_pos must is begin of a chunk
	//这个时候_chunk_pos应该指向\r\n位置
	for(;;){
		chunk_size = 0;
		begin_pos = http_buf.req_http_buf + chunk_pos;

		//if begin_pos points to a invalid address out of _http_buf, continue receive data
		if (begin_pos > (http_buf.req_http_buf + http_buf.http_buf_len - 1))
		{
			//数据未接收完毕
			return I_RET_CODE_INCOMPLETE;
		}

		if ((begin_pos + 1) > (http_buf.req_http_buf + http_buf.http_buf_len - 1))
		{
			return I_RET_CODE_INCOMPLETE;
		}

        //校验行尾
		if ( memcmp(begin_pos,"\r\n", 2) != 0 )
		{
			return I_RET_CODE_INCOMPLETE;
		}

        //跳过\r\n
		begin_pos += 2;
		if (begin_pos > (http_buf.req_http_buf + http_buf.http_buf_len - 1))
		{
			return I_RET_CODE_INCOMPLETE;
		}

        //查找行尾
		end_pos = strstr(begin_pos,"\r\n");
		if ( !end_pos ) 
		{
			return I_RET_CODE_INCOMPLETE;
		}

        //取出该段的chunk长度
		*end_pos = '\0';
		//chunk-size is hex string,should change to int
		chunk_size = StringUtility::HexStr2long(begin_pos);
		*end_pos = '\r';
		
		//chunk数据接收完毕
		if ( chunk_size == 0 )
		{	//receive all chunk over
			return I_RET_CODE_SUCCESS;
		}

        //两个行尾\r\n为4个字节
		if ((http_buf.http_buf_len - chunk_pos) < (end_pos - begin_pos + 4 + chunk_size) )
		{
			//该段chunk的数据未接收完
			return I_RET_CODE_INCOMPLETE;
		}

        //拷贝chunk数据
        http_message.http_body.insert(http_content_len, end_pos + 2, chunk_size);

        //累加chunk长度
		http_content_len += chunk_size;

		//this chunk-size chunk have receive complete
        //跳到下一个chunk
		chunk_pos += (end_pos - begin_pos + 4 + chunk_size);
	}

	return I_RET_CODE_INCOMPLETE;
}


//对解析完的报文内容进行解码
int CHttpParser::ContentDecode()
{
    std::string content_encodeing("Content-Encoding");
    map<string, string>::iterator iter = http_message.header_item.find(content_encodeing);
    if(iter == http_message.header_item.end())
    {
        //没找到默认不处理
        return 0;
    }

    //判断是否为gzip数据头
    string::size_type nPos;
    string str_gzip = "gzip";
    nPos = StringUtility::FindNoCase(iter->second, str_gzip, 0);
    if (nPos != string::npos)
    {
	    unsigned char* unpress_data = NULL;
	    unsigned int   unpress_len = 0;
	    unsigned char* p_gzip_check = (unsigned char *) http_message.http_body.c_str();
	    if(ZLibWrapper::Unzip(p_gzip_check, http_message.http_body.size(), unpress_data, unpress_len, NULL, 0))
        {
            http_message.http_body.clear();
		    memcpy((char*)http_message.http_body.c_str(), unpress_data, unpress_len);
		    delete [] unpress_data;
        }
        else
        {
            return I_RET_CODE_BODY_ERR;
        }
    }

    return 0;
}



