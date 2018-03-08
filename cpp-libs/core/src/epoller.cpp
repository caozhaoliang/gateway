#include "epoller.h"

CObjectMap::CObjectMap()
{
    hash_table_size = 0;
    hash_table = NULL;
}

CObjectMap::~CObjectMap()
{
    if ( hash_table )
    {
        DropMap();
    }
    hash_table = NULL ;
}

int CObjectMap::CreateMap(int map_size)
{
    hash_table = new struct OBJECTMAP[map_size];
    if ( !hash_table ) return -1;

    for(int i=0; i<map_size; i++)
    {
        hash_table[i].id = 0;
        hash_table[i].objaddr = NULL;
        INIT_LIST_HEAD(&hash_table[i].list_item);
    }
    hash_table_size = map_size;

    return 0;
}

int CObjectMap::AddMapObj(int id,void* obj)
{
    int idx = id%hash_table_size;

    struct OBJECTMAP* item;
    item = new struct OBJECTMAP();
    if ( !item )
        return -1;

    item->id = id;
    item->objaddr = obj;
    list_add_tail(&item->list_item,&hash_table[idx].list_item);

    return 0;
}

int CObjectMap::DelMapObj(int id)
{
    int idx = id%hash_table_size;

    struct list_head* _head;
    struct list_head* _tmp1;
    struct list_head* _tmp2;
    struct OBJECTMAP*  item;

    _head = &hash_table[idx].list_item;
    list_for_each_safe(_tmp1,_tmp2,_head)
    {
        item = list_entry(_tmp1,struct OBJECTMAP,list_item);
        if ( item->id == id )
        {
            list_del(_tmp1);
            delete item;
            return 0;
        }
    }

    return -1;
}

void* CObjectMap::GetMapObj(int id)
{
    int idx = id%hash_table_size;

    struct list_head* _head;
    struct list_head* _tmp;
    struct OBJECTMAP*  item;

    _head = &hash_table[idx].list_item;
    list_for_each(_tmp,_head)
    {
        item = list_entry(_tmp,struct OBJECTMAP,list_item);
        if ( item->id == id )
        {
            return item->objaddr;
        }
    }

    return NULL;
}

int CObjectMap::DropMap()
{
    struct list_head* _head;
    struct list_head* _tmp1;
    struct list_head* _tmp2;
    struct OBJECTMAP*  item;

    for( int i=0; i<hash_table_size; i++ )
    {
        _head = &hash_table[i].list_item;
        list_for_each_safe(_tmp1,_tmp2,_head)
        {
            item = list_entry(_tmp1,struct OBJECTMAP,list_item);
            list_del(_tmp1);
            delete item;
        }
    }

    delete[] hash_table;
    hash_table = NULL;
    hash_table_size = 0;

    return 0;
}



CEPoller::CEPoller()
{
    _epoll_fd = -1;
}

CEPoller::~CEPoller()
{
}

int CEPoller::create(int maxfd)
{
    _maxfd = maxfd ;
    _epoll_fd = epoll_create(_maxfd);
    if ( _epoll_fd <= 0 )
    {
        memset(_err_msg,0,NET_ERRMSG_SIZE);
        snprintf(_err_msg,NET_ERRMSG_SIZE,"epoller create size:%d error:%s\n",maxfd,strerror(errno));
        return -1;
    }

    if ( _obj_map.CreateMap(_maxfd) < 0 )
    {
        memset(_err_msg,0,NET_ERRMSG_SIZE);
        snprintf(_err_msg,NET_ERRMSG_SIZE,"epoller create obj-map:%d error\n",maxfd);
        return -1;
    }

    return 0;
}

void CEPoller::attachSocket(CEpollSocket* sock)
{
    int fd = sock->getSockHandle();
    if ( fd > 0 )
        _obj_map.AddMapObj(fd,(void*)sock);

    return ;
}

void CEPoller::detachSocket(CEpollSocket* sock)
{
    int fd = sock->getSockHandle();
    if ( fd > 0 )
    {
        delEpollIO(fd);
        _obj_map.DelMapObj(fd);
    }

    return ;
}

int CEPoller::loopForEvent(int timeout)
{
    for(;;)
    {
        int wait_result = waitForEvent(timeout);
        if( wait_result <= 0 )
        {
            return wait_result;
        }
    }
}


int CEPoller::waitForEvent(int timeout)
{
    int fd;
    int nfds;
    CEpollSocket*  sock;
    unsigned ev;

    nfds = epoll_wait(_epoll_fd, _events, EPOLL_FD_MAX, timeout);
    if (nfds < 0)
    {
        snprintf(_err_msg,NET_ERRMSG_SIZE,"epoll-wait rtn:%d error:%s\n",nfds,strerror(errno));
        return -1;
    }
    else if( nfds == 0 )
    {
        return 0 ;
    }

    for( int i=0; i<nfds; i++ )
    {
        fd = _events[i].data.fd;
        sock = (CEpollSocket*)_obj_map.GetMapObj(fd);
        if ( sock == NULL )
        {
            continue;
        }

        ev = _events[i].events;
        if ( ev & EPOLLIN )
        {
            sock->onRecv();
        }
        else if ( ev& EPOLLOUT )
        {
            sock->onSend();
        }
        else if ( ev & EPOLLHUP )
        {
            sock->onClose();
        }
        else if ( ev & EPOLLERR )
        {
            sock->onError();
        }
        else
        {
            sock->onError();
        }
    }

    return nfds ;
}


char * CEPoller::getErrMsg()
{
    return _err_msg;
}

int CEPoller::setEpollIO(int fd,unsigned flag)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = flag|EPOLLHUP|EPOLLERR;

    if ( epoll_ctl(_epoll_fd, EPOLL_CTL_MOD , fd, &ev) < 0 )
    {
        if ( epoll_ctl(_epoll_fd, EPOLL_CTL_ADD , fd, &ev) < 0 )
        {
            snprintf(_err_msg,NET_ERRMSG_SIZE,"epoll_ctl fd:%d err:%s\n",fd,strerror(errno));
            return -1;
        }
    }

    return 0;
}

int CEPoller::addEpollIO(int fd,unsigned flag)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = flag;

    if ( epoll_ctl(_epoll_fd, EPOLL_CTL_ADD , fd, &ev) < 0 )
        return -1;

    return 0;
}

int CEPoller::modEpollIO(int fd,unsigned flag)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = flag;

    if ( epoll_ctl(_epoll_fd, EPOLL_CTL_MOD , fd, &ev) < 0 )
    {
        return -1;
    }

    return 0;
}


// 有数据不在通知应用层处理
int CEPoller::delEpollIO(int fd)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = 0;
    if ( epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &ev) < 0 )
        return -1;

    return 0;
}

CEpollSocket::CEpollSocket()
{
    _sock_fd = -1;
    _epoller = NULL;
}

CEpollSocket::~CEpollSocket()
{
    dropSocket();
}

int CEpollSocket::getSockHandle()
{
    return _sock_fd;
}

int CEpollSocket::setSockHandle(int fd)
{
    _sock_fd = fd;
    return 0;
}

int CEpollSocket::attachEpoller(CEPoller* epoller)
{
    _epoller = epoller;
    if ( _epoller )
    {
        _epoller->attachSocket(this);
    }

    return 0;
}
int CEpollSocket::detachEpoller()
{
    if ( _epoller )
    {
        _epoller->detachSocket(this);
    }

    _epoller = NULL;

    return 0;
}

int CEpollSocket::setEvent(unsigned event)
{
    if ( !_epoller )
    {
        return -1;
    }

    if ( _epoller->modEpollIO(_sock_fd,event) < 0 )
    {
        return _epoller->addEpollIO(_sock_fd,event);
    }

    return 0;
}

int CEpollSocket::dropSocket()
{
    if ( _sock_fd )
    {
        detachEpoller();
        close(_sock_fd);
    }
    _sock_fd = -1;
    _epoller = NULL;

    return 0;
}

int CEpollSocket::sendTcpMsg(char* buff, int len)
{
    int ret;
    int _snd=0;
    int _len = len;
    errno = 0;
    while(_len>0)
    {
        ret = ::send(_sock_fd,(void*)((char*)buff+_snd),_len,0);
        if ( ret <= 0 )
        {
            if ( errno == EINTR || errno == EAGAIN )
            {
                usleep(10);
                continue;
            }
            //TRACE_LOG("send fail,ret:%d errno:%d,desc:%s\n", ret, errno,strerror(errno) );
            break;
        }
        _len -= ret;
        _snd += ret;
    }

    return _snd;
}

