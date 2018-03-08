#ifndef _HTTP_PARSER_H_
#define _HTTP_PARSER_H_

#include <map>
#include <string>
#include <vector>
#include <string.h>
#include "string_utility.h"

using namespace std;



const int  I_RET_CODE_SUCCESS            = 0;    // 解析成功
const int  I_RET_CODE_INCOMPLETE         = 1;    // 未接收完报文
const int  I_RET_CODE_METHOD_ERR         = -1;   // 不支持的请求方法
const int  I_RET_CODE_FORMAT_ERR         = -2;   // 报文头格式错误
const int  I_RET_CODE_BODY_ERR           = -3;   // 报文体格式错误(可以是解压失败)
const int  I_RET_CODE_INPUT_ERR          = -6;   // 输入数据指针为空
const int  I_RET_CODE_MAXMEM_ERR         = -10;  // 接收的报文的大小超出最大的内存大小(需要查看是否重复数据或有恶意请求)
const int  I_RET_CODE_ALLOCMEM_ERR       = -11;  // 系统分配内存出错(需要检查内存是否有空间), 外部调用可以重试


typedef enum ENUM_HTTP_METHOD
{
    HTTP_METHOD_GET    = 1,
    HTTP_METHOD_POST   = 2,
    HTTP_METHOD_DELETE = 3,
    HTTP_METHOD_PUT    = 4
} HTTP_METHOD;


struct STHttpMsg
{
	// http请求方法, 如：GET或POST(此字段只有服务端解析客户端的请求报文才会用到)
	HTTP_METHOD    http_method;

    // http请求版本, 如：HTTP/1.1
	std::string    http_version;

    // 状态描述(此字段只有客户端解析服务端返回的报文才会用到),
    // 如：200 OK; 301 Moved Permanently; 400 Bad Request
    std::string    http_status_desc;

	// 解析后的URI(不带参数), 如: /main/index.html
	std::string    http_uri;

	// http请求的uri的参数列表
	std::map<std::string, std::string> uri_paramter;

	// http请求头部列表, 如：map<Accept-Language, zh-cn>
	std::map<std::string, std::string> header_item;

	// 获取消息体
	std::string    http_body;
};



#define HTTP_HEADER_END_FLAG               "\x0d\x0a\x0d\x0a"              //HTTP协议头结束标识
#define HTTP_CHUNKED_END_FLAG              "\x0d\x0a\x30\x0d\x0a"          //HTTP协议体chunked类型结束标识


#define HTTP_BUFF_LEN                      (100 * 1024)                    //一次申请http接收缓存
#define HTTP_BUFF_MAX_ALLOC_LEN            (100 * HTTP_BUFF_LEN)           //最大可申请HTTP的接收缓存

typedef struct HTTPBUFtag
{
    int         http_buf_size;              // buf总大小
    int         http_buf_len;               // buf已使用的长度
    char*       req_http_buf;               // 数据缓冲区指针
} HTTPBUF;


//HTTP协议头的各项名
#define HTTP_HEADER_X_AUTHENTI_SCOPE_NAME     ("X-Authenticated-Scope")




//HTTP协议头的各项名的长度
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
    // 解析Http报文
    virtual int ParseHttp(char* buff, int len);
    
    // 获取解析后的http消息
    virtual STHttpMsg & GetHttpMessage()
    {
        return http_message;
    }
    
    // 获取URI的参数列表
    virtual std::map<std::string, std::string> & GetUriParamterList()
	{
        return http_message.uri_paramter;
	}
    
     // 获取HTTP响应码(此接口用在客户端解析服务端返回的报文), 响应码(3位) + 空格 + 状态描述, 如:200 OK
    virtual std::string & GetHttpRspCode()
    {
        return http_message.http_status_desc;
    }

     // 获取URI(不带参数)
    virtual std::string & GetHttpUri()
    {
        return http_message.http_uri;
    }
    
    // 获取消息体
    virtual std::string & GetHttpBody()
    {
        return http_message.http_body;
    }

    //复位解析器
    virtual void Reset();

protected:
    //解析http报文
    virtual int ParseHttpMsg(char* buff, int len);

    //检查内存空间
    virtual int CheckMem(char* buff, int len);

    //解析http报文中uri所带的参数
    virtual int ParseHttpParamter(char* param_buff, std::map<std::string, std::string> & paramter_map);
	
    //解析chunked类型的报文
    virtual int ParseHttpChunked(int chunk_pos);

    //检查是否接收完成
    virtual int CheckIsComplete(char* buff, int len);
	
    //对解析完的报文进行解码
    virtual int ContentDecode();

    //重置http请求缓冲区值
    virtual int ResetHttpBuf(char * buf, int buf_size, int buf_len);

    // 清空数据              
    virtual void clear();

	
protected:
    //头长度
    int            http_head_len;
    
    //内容长度
    int            http_content_len;
    
    //解析后的http数据
    STHttpMsg      http_message;

    //原始数据存储区
    HTTPBUFtag     http_buf;

};


#endif


