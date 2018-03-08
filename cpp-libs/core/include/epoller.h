#ifndef _EPOLLER_H_
#define _EPOLLER_H_
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <assert.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <queue>

#include "list.h"

using namespace std ;

#define NET_ERRMSG_SIZE	512
#define EPOLL_FD_MAX		10240

class CEPoller;
class CEpollSocket;
struct OBJECTMAP
{
    int 	id;
    void* 	objaddr;
    struct list_head list_item;
};

class CObjectMap
{
private:
    struct OBJECTMAP* hash_table;
    int hash_table_size;


public:
    CObjectMap();
    ~CObjectMap();

    int CreateMap(int map_size);
    int AddMapObj(int id,void* obj);
    int DelMapObj(int id);
    void* GetMapObj(int id);
    int DropMap();
};



class CEPoller
{
public:
    CEPoller();
    ~CEPoller();

    int create(int maxfd);
    int addEpollIO(int fd,unsigned flag);
    int modEpollIO(int fd,unsigned flag);
    int	setEpollIO(int fd,unsigned flag);
    int delEpollIO(int fd);
    void attachSocket(CEpollSocket* sock);
    void detachSocket(CEpollSocket* sock);
    int  loopForEvent(int timeout);
    int  waitForEvent(int timeout);
    char *getErrMsg();
protected:
    char			_err_msg[NET_ERRMSG_SIZE];
    int 			_epoll_fd;				//epoll的句柄
    epoll_event		_events[EPOLL_FD_MAX];	//epoll_wait的返回的事件
    int _maxfd;

    CObjectMap		_obj_map;
};

#define FD_RECV		EPOLLIN
#define FD_SEND		EPOLLOUT
#define FD_CLOSE	EPOLLHUP
#define FD_ERROR	EPOLLERR
class CEpollSocket
{
public:
    CEpollSocket();
    virtual ~CEpollSocket();

public:
//Add by huihai.shen at 2017-12-25 for system optimize begin
	struct STTask
	{
		unsigned int snd;
		std::string  data;
		STTask()
		{
			snd = 0;
			data.clear();
		}

		STTask(const char* msg, int msg_len, int snd_len):
		snd(snd_len),
	    data(msg, msg_len)
		{
		}

		void reset()
		{
			snd = 0;
		}
	};
//Add by huihai.shen at 2017-12-25 for system optimize end

public:
    virtual int onRecv()
    {
        return 0;
    };
    virtual int onSend()
    {
        return 0;
    };
    virtual int onClose()
    {
        return 0;
    };
    virtual int onError()
    {
        return 0;
    };

    int getSockHandle();
    int setSockHandle(int fd);
    int attachEpoller(CEPoller* epoller);
    int detachEpoller();

    int setEvent(unsigned event);
    int dropSocket();
    int sendTcpMsg(char* buff, int len);

	//Add by huihai.shen at 2017-12-25 for system optimize begin
	//非阻塞发送
	int sendTcpMsgNoBlock(char* buff, const int len)
	{
		int ret;
		int _snd =0;
		int _len = len;
		errno    = 0;

		while(_len > 0)
		{
			ret = send(_sock_fd, (void*)((char*)buff + _snd), _len, 0);
			if ( ret <= 0 )
			{
				if ( errno == EINTR || errno == EAGAIN )
				{
					break;
				}
				else
				{
					printf("ret:%d, errno:%d, desc:%s\n", ret, errno, strerror(errno));
					return -1;
				}
			}

			_len -= ret;
			_snd += ret;
		}


		return _snd;
	}

	//创建发送任务
	int createTask(const char* msg, int msg_len, int snd = 0)
	{
		STTask task(msg, msg_len, snd);

		task_queue.push(task);

		return 0;
	}
    //Add by huihai.shen at 2017-12-25 for system optimize end

protected:
    CEPoller*			_epoller;	//关联的epoller
    int					_sock_fd;	//数据处理的句柄

	//Add by huihai.shen at 2017-12-25 for system optimize begin
	//存储任务消息的队列
	std::queue<STTask> task_queue;
	//Add by huihai.shen at 2017-12-25 for system optimize end

};


#endif
