#ifndef _OBJECT_MAP_H_
#define _OBJECT_MAP_H_

#include "list.h"


struct COBJECTMAP
{
    unsigned int  id;
    void* 	      objaddr;
    struct list_head list_item;
};

class CCObjectMap
{
private:
    struct COBJECTMAP* hash_table;
    int hash_table_size;


public:
    CCObjectMap();
    ~CCObjectMap();

    int createMap(int map_size);
    int addMapObj(unsigned int id, void* obj);
    int delMapObj(unsigned int id);
    void* getMapObj(unsigned int id);
    int dropMap();
};

#endif
