#include <string.h>
#include <time.h>

#ifndef _DEV_KEY_H_
#define _DEV_KEY_H_

//��dev_service����������ip�Ͷ˿ں���Ϊkey
struct DevKey
{
    unsigned int  device_ip ;    //�����ֽ���
    unsigned short device_port ; // �����ֽ���

    DevKey()
    {
        device_ip = 0 ;
        device_port = 0 ;
    }

    DevKey(unsigned int ip,unsigned short port)
    {
        device_ip = ip;
        device_port = port ;
    }

    DevKey(const DevKey& key)
    {
        device_ip = key.device_ip;
        device_port = key.device_port ;
    }

    DevKey& operator=(const DevKey & key)
    {
        device_ip = key.device_ip;
        device_port = key.device_port ;
        return *this;
    }

    bool operator==(const DevKey & key) const
    {
        if( device_ip != key.device_ip )
        {
            return false ;
        }

        if( device_port != key.device_port)
        {
            return false ;
        }

        return true;
    }


    bool operator < (DevKey const& key) const
    {
        if( device_ip < key.device_ip )
        {
            return true ;
        }

        if(  device_ip > key.device_ip )
        {
            return false ;
        }


        if( device_port < key.device_port)
        {
            return true;
        }

        return false ;
    }
};

#pragma pack(1)
struct DevRouteInfo
{
    char device_id[33];
    unsigned int device_ip ;
    unsigned short device_port ;
    unsigned char  device_status ;  // ����״̬0 ���� 1 ����
    time_t         active_time ;    //���һ������ʱ��

    DevRouteInfo()
    {
        memset(this,0,sizeof(DevRouteInfo));
    }
};
#pragma pack()

#endif


