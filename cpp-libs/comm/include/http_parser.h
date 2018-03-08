#ifndef _HTTP_PARSER_H_
#define _HTTP_PARSER_H_

#include <map>
#include <string>
#include <vector>
#include <string.h>
#include "string_utility.h"

using namespace std;



const int  I_RET_CODE_SUCCESS            = 0;    // �����ɹ�
const int  I_RET_CODE_INCOMPLETE         = 1;    // δ�����걨��
const int  I_RET_CODE_METHOD_ERR         = -1;   // ��֧�ֵ����󷽷�
const int  I_RET_CODE_FORMAT_ERR         = -2;   // ����ͷ��ʽ����
const int  I_RET_CODE_BODY_ERR           = -3;   // �������ʽ����(�����ǽ�ѹʧ��)
const int  I_RET_CODE_INPUT_ERR          = -6;   // ��������ָ��Ϊ��
const int  I_RET_CODE_MAXMEM_ERR         = -10;  // ���յı��ĵĴ�С���������ڴ��С(��Ҫ�鿴�Ƿ��ظ����ݻ��ж�������)
const int  I_RET_CODE_ALLOCMEM_ERR       = -11;  // ϵͳ�����ڴ����(��Ҫ����ڴ��Ƿ��пռ�), �ⲿ���ÿ�������


typedef enum ENUM_HTTP_METHOD
{
    HTTP_METHOD_GET    = 1,
    HTTP_METHOD_POST   = 2,
    HTTP_METHOD_DELETE = 3,
    HTTP_METHOD_PUT    = 4
} HTTP_METHOD;


struct STHttpMsg
{
	// http���󷽷�, �磺GET��POST(���ֶ�ֻ�з���˽����ͻ��˵������ĲŻ��õ�)
	HTTP_METHOD    http_method;

    // http����汾, �磺HTTP/1.1
	std::string    http_version;

    // ״̬����(���ֶ�ֻ�пͻ��˽�������˷��صı��ĲŻ��õ�),
    // �磺200 OK; 301 Moved Permanently; 400 Bad Request
    std::string    http_status_desc;

	// �������URI(��������), ��: /main/index.html
	std::string    http_uri;

	// http�����uri�Ĳ����б�
	std::map<std::string, std::string> uri_paramter;

	// http����ͷ���б�, �磺map<Accept-Language, zh-cn>
	std::map<std::string, std::string> header_item;

	// ��ȡ��Ϣ��
	std::string    http_body;
};



#define HTTP_HEADER_END_FLAG               "\x0d\x0a\x0d\x0a"              //HTTPЭ��ͷ������ʶ
#define HTTP_CHUNKED_END_FLAG              "\x0d\x0a\x30\x0d\x0a"          //HTTPЭ����chunked���ͽ�����ʶ


#define HTTP_BUFF_LEN                      (100 * 1024)                    //һ������http���ջ���
#define HTTP_BUFF_MAX_ALLOC_LEN            (100 * HTTP_BUFF_LEN)           //��������HTTP�Ľ��ջ���

typedef struct HTTPBUFtag
{
    int         http_buf_size;              // buf�ܴ�С
    int         http_buf_len;               // buf��ʹ�õĳ���
    char*       req_http_buf;               // ���ݻ�����ָ��
} HTTPBUF;


//HTTPЭ��ͷ�ĸ�����
#define HTTP_HEADER_X_AUTHENTI_SCOPE_NAME     ("X-Authenticated-Scope")




//HTTPЭ��ͷ�ĸ������ĳ���
#define HTTP_HEADER_CONTENT_LENGTH_LEN        (sizeof("Content-Length") - 1)
#define HTTP_HEADER_CONTENT_ENCODING_LEN      (sizeof("Content-Encoding") - 1)
#define HTTP_HEADER_TRANSFER_ENCODING_LEN     (sizeof("Transfer-Encoding") - 1)
#define HTTP_HEADER_USER_AGENT_LEN            (sizeof("User-Agent") - 1)
#define HTTP_HEADER_CONTENT_TYPE_LEN          (sizeof("Content-Type") - 1)
#define HTTP_HEADER_CONNECTION_LEN            (sizeof("Connection") - 1)
#define HTTP_HEADER_ACCEPT_ENCODING_LEN       (sizeof("Accept-Encoding") - 1)
#define HTTP_HEADER_ACCEPT_LEN                (sizeof("Accept") - 1)
#define HTTP_HEADER_ACCEPT_LANGUAGE_LEN       (sizeof("Accept-Language") - 1)
#define HTTP_HEADER_X_AUTHENTI_SCOPE_LEN      (sizeof(HTTP_HEADER_X_AUTHENTI_SCOPE_NAME) - 1)


class CHttpParser
{
public:
	CHttpParser();
	virtual ~CHttpParser();

public:
    // ����Http����
    virtual int ParseHttp(char* buff, int len);
    
    // ��ȡ�������http��Ϣ
    virtual STHttpMsg & GetHttpMessage()
    {
        return http_message;
    }
    
    // ��ȡURI�Ĳ����б�
    virtual std::map<std::string, std::string> & GetUriParamterList()
	{
        return http_message.uri_paramter;
	}
    
     // ��ȡHTTP��Ӧ��(�˽ӿ����ڿͻ��˽�������˷��صı���), ��Ӧ��(3λ) + �ո� + ״̬����, ��:200 OK
    virtual std::string & GetHttpRspCode()
    {
        return http_message.http_status_desc;
    }

     // ��ȡURI(��������)
    virtual std::string & GetHttpUri()
    {
        return http_message.http_uri;
    }
    
    // ��ȡ��Ϣ��
    virtual std::string & GetHttpBody()
    {
        return http_message.http_body;
    }

    //��λ������
    virtual void Reset();

protected:
    //����http����
    virtual int ParseHttpMsg(char* buff, int len);

    //����ڴ�ռ�
    virtual int CheckMem(char* buff, int len);

    //����http������uri�����Ĳ���
    virtual int ParseHttpParamter(char* param_buff, std::map<std::string, std::string> & paramter_map);
	
    //����chunked���͵ı���
    virtual int ParseHttpChunked(int chunk_pos);

    //����Ƿ�������
    virtual int CheckIsComplete(char* buff, int len);
	
    //�Խ�����ı��Ľ��н���
    virtual int ContentDecode();

    //����http���󻺳���ֵ
    virtual int ResetHttpBuf(char * buf, int buf_size, int buf_len);

    // �������              
    virtual void clear();

	
protected:
    //ͷ����
    int            http_head_len;
    
    //���ݳ���
    int            http_content_len;
    
    //�������http����
    STHttpMsg      http_message;

    //ԭʼ���ݴ洢��
    HTTPBUFtag     http_buf;

};


#endif


