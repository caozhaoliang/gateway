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

//��λ������
void CHttpParser::Reset()
{
    ResetHttpBuf(http_buf.req_http_buf, http_buf.http_buf_size, 0);
    clear();
}

//����http���󻺳���ֵ
int CHttpParser::ResetHttpBuf(char * buf, int buf_size, int buf_len)
{
    http_buf.req_http_buf  = buf;
    http_buf.http_buf_size = buf_size;
    http_buf.http_buf_len  = buf_len;

    return 0;
}

// �������                               
void CHttpParser::clear()
{
    http_message.http_version.clear();
    http_message.http_uri.clear();
    http_message.uri_paramter.clear();
    http_message.header_item.clear();
    http_message.http_body.clear();
}

// ����Http����
int CHttpParser::ParseHttp(char* buff, int len)
{
    return I_RET_CODE_SUCCESS;
}

//����ڴ�ռ�
int CHttpParser::CheckMem(char* buff, int len)
{
    //���Ԥ������ڴ治����ô����Ԥ������ڴ�Ĵ�С
    //�����ڴ���䲻�ܳ���Ԥ������ֵ
    int nleft = http_buf.http_buf_size - http_buf.http_buf_len;
    if ((nleft <= len) && (http_buf.http_buf_size < HTTP_BUFF_MAX_ALLOC_LEN))
    {
        //У���Ƿ񳬹��ڴ�����޶�
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

//����Ƿ�������
int CHttpParser::CheckIsComplete(char* buff, int len)
{
    char* pBuf = buff;

    //���ͷ�Ƿ��������
    //char* pEnd = strstr(pBuf, HTTP_HEADER_END_FLAG);
    char* pEnd = (char*)memmem(pBuf, len, HTTP_HEADER_END_FLAG, sizeof(HTTP_HEADER_END_FLAG) - 1);
    if (NULL == pEnd) 
    {
        return I_RET_CODE_INCOMPLETE;
    }

    //��ȡ���峤��
    int head_Len = 0;
    //char* pBegin = strstr(pBuf, "Content-Length");
    char* pBegin = (char*)memmem(pBuf, pEnd - pBuf, "Content-Length", sizeof("Content-Length") - 1);
    if(NULL == pBegin)
    {
        //û�ҵ����峤��, �����Ƿ�ΪTransfer-Encoding: chunked
        //pBegin = strstr(pBuf, "Transfer-Encoding");
        pBegin = (char*)memmem(pBuf, pEnd - pBuf, "Transfer-Encoding", sizeof("Transfer-Encoding") - 1);
        if(NULL == pBegin)
        {
            //û����Ϣ��
            return 0;
        }
    }
    else
    {
        //�ҵ����峤��
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

    // �����ո�
    while(*pBegin == ' ') pBegin++;

    int content_len = 0;
    if(0 == strncasecmp(pBegin, "chunked", 7))
    {
        //chunked����, У���Ƿ������(���ڱ���������ܴ�����ͬ���ַ���, �ڽ���������Ҫ����Ƿ���������)
        //char* pBegin = strstr(pBuf, HTTP_CHUNKED_END_FLAG);
        pBegin = (char*)memmem(pBuf, len, HTTP_CHUNKED_END_FLAG, sizeof(HTTP_CHUNKED_END_FLAG) - 1);
        if(NULL == pBegin)
        {
            return I_RET_CODE_INCOMPLETE;
        }

        //��ɽ���
        return 0;
    }
    else
    {
        *pEnd = '\0';
        content_len = atoi(pBegin);
        *pEnd = '\r';
    }

    //��� head + content_len > buf �ܳ��� , ��ʾ��������δ����
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

//����http������uri�����Ĳ���
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

    //�������һ��
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

//����chunked���͵ı���
int CHttpParser::ParseHttpChunked(int chunk_pos)
{
    int chunk_size=0;
	char* begin_pos;
	char* end_pos;
	
	//http_pos must is begin of a chunk
	//���ʱ��_chunk_posӦ��ָ��\r\nλ��
	for(;;){
		chunk_size = 0;
		begin_pos = http_buf.req_http_buf + chunk_pos;

		//if begin_pos points to a invalid address out of _http_buf, continue receive data
		if (begin_pos > (http_buf.req_http_buf + http_buf.http_buf_len - 1))
		{
			//����δ�������
			return I_RET_CODE_INCOMPLETE;
		}

		if ((begin_pos + 1) > (http_buf.req_http_buf + http_buf.http_buf_len - 1))
		{
			return I_RET_CODE_INCOMPLETE;
		}

        //У����β
		if ( memcmp(begin_pos,"\r\n", 2) != 0 )
		{
			return I_RET_CODE_INCOMPLETE;
		}

        //����\r\n
		begin_pos += 2;
		if (begin_pos > (http_buf.req_http_buf + http_buf.http_buf_len - 1))
		{
			return I_RET_CODE_INCOMPLETE;
		}

        //������β
		end_pos = strstr(begin_pos,"\r\n");
		if ( !end_pos ) 
		{
			return I_RET_CODE_INCOMPLETE;
		}

        //ȡ���öε�chunk����
		*end_pos = '\0';
		//chunk-size is hex string,should change to int
		chunk_size = StringUtility::HexStr2long(begin_pos);
		*end_pos = '\r';
		
		//chunk���ݽ������
		if ( chunk_size == 0 )
		{	//receive all chunk over
			return I_RET_CODE_SUCCESS;
		}

        //������β\r\nΪ4���ֽ�
		if ((http_buf.http_buf_len - chunk_pos) < (end_pos - begin_pos + 4 + chunk_size) )
		{
			//�ö�chunk������δ������
			return I_RET_CODE_INCOMPLETE;
		}

        //����chunk����
        http_message.http_body.insert(http_content_len, end_pos + 2, chunk_size);

        //�ۼ�chunk����
		http_content_len += chunk_size;

		//this chunk-size chunk have receive complete
        //������һ��chunk
		chunk_pos += (end_pos - begin_pos + 4 + chunk_size);
	}

	return I_RET_CODE_INCOMPLETE;
}


//�Խ�����ı������ݽ��н���
int CHttpParser::ContentDecode()
{
    std::string content_encodeing("Content-Encoding");
    map<string, string>::iterator iter = http_message.header_item.find(content_encodeing);
    if(iter == http_message.header_item.end())
    {
        //û�ҵ�Ĭ�ϲ�����
        return 0;
    }

    //�ж��Ƿ�Ϊgzip����ͷ
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



