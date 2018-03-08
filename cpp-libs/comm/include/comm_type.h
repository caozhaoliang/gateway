#ifndef _COMM_TYPE_H_
#define _COMM_TYPE_H_

#include <string>
#include <sstream>
#include <arpa/inet.h>


using namespace std ;


// 协议头标志为10101010
const unsigned char protocol_label = 170 ;   

#pragma pack(1) 

//消息头定义
struct CommHead
{
   //固定为170 ,  二进制10101010
   unsigned char protocol_label;   

   //整个消息包长度,从前面的protocol_label算起
   int msg_len;        

   //数据校验和,对后面所有部分计算
   unsigned short check_sum;   

   //API业务类型
   unsigned int api_cmd;

   //消息ID，此消息所有服务唯一
   unsigned int msg_id;

   CommHead()
   {
       protocol_label = protocol_label;
       msg_len        = 0;
       check_sum      = 0;
       api_cmd        = 0;
       msg_id         = 0;
   }
};

#pragma pack()

#endif

