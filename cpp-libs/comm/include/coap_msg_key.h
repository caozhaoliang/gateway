#ifndef _COAP_MSG_KEY_H_
#define _COAP_MSG_KEY_H_

#include <string>

using namespace std ;


//标示coap消息的唯一key
struct CoapMsgKey
{
    unsigned short msg_id ;     // coap的msg id
    string device_id ;          // 设备id
    string user_id ;

    CoapMsgKey()
    {
        msg_id = 0 ;
    }

    CoapMsgKey(unsigned short id,string d_id,string u_id)
    {
        msg_id = id;
        device_id = d_id ;
        user_id = u_id ;
    }

    CoapMsgKey(const CoapMsgKey& key)
    {
        msg_id = key.msg_id;
        device_id = key.device_id ;
        user_id = key.user_id ;
    }

    CoapMsgKey& operator=(const CoapMsgKey & key)
    {
        msg_id = key.msg_id;
        device_id = key.device_id ;
        user_id = key.user_id ;

        return *this;
    }

    bool operator==(const CoapMsgKey & key) const
    {
        if( msg_id != key.msg_id )
        {
            return false ;
        }

        if( device_id != key.device_id)
        {
            return false ;
        }

        if(user_id != key.user_id)
        {
            return false ;
        }

        return true;
    }


    bool operator < ( const CoapMsgKey & key) const
    {
        if( msg_id < key.msg_id )
        {
            return true ;
        }

        if(  msg_id > key.msg_id )
        {
            return false ;
        }

        if( device_id < key.device_id )
        {
            return true ;
        }

        if(  device_id > key.device_id )
        {
            return false ;
        }

        if( user_id < key.user_id )
        {
            return true ;
        }

        if(  user_id > key.user_id )
        {
            return false ;
        }

        return false ;
    }
};

#endif



