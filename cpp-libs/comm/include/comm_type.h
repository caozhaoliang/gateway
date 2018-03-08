#ifndef _COMM_TYPE_H_
#define _COMM_TYPE_H_

#include <string>
#include <sstream>
#include <arpa/inet.h>


using namespace std ;


// Э��ͷ��־Ϊ10101010
const unsigned char protocol_label = 170 ;   

#pragma pack(1) 

//��Ϣͷ����
struct CommHead
{
   //�̶�Ϊ170 ,  ������10101010
   unsigned char protocol_label;   

   //������Ϣ������,��ǰ���protocol_label����
   int msg_len;        

   //����У���,�Ժ������в��ּ���
   unsigned short check_sum;   

   //APIҵ������
   unsigned int api_cmd;

   //��ϢID������Ϣ���з���Ψһ
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

