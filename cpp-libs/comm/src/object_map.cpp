#include "object_map.h"


CCObjectMap::CCObjectMap()
{
    hash_table_size = 0;
    hash_table = NULL;
}

CCObjectMap::~CCObjectMap()
{
    if ( hash_table )
    {
        dropMap();
    }
    hash_table = NULL ;
}

int CCObjectMap::createMap(int map_size)
{
    hash_table = new struct COBJECTMAP[map_size];
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

int CCObjectMap::addMapObj(unsigned int id, void* obj)
{
    int idx = (int)(id % hash_table_size);

    struct COBJECTMAP* item;
    item = new struct COBJECTMAP();
    if ( !item )
        return -1;

    item->id = id;
    item->objaddr = obj;
    list_add_tail(&item->list_item,&hash_table[idx].list_item);

    return 0;
}

int CCObjectMap::delMapObj(unsigned int id)
{
    int idx = (int)(id % hash_table_size);

    struct list_head* _head;
    struct list_head* _tmp1;
    struct list_head* _tmp2;
    struct COBJECTMAP*  item;

    _head = &hash_table[idx].list_item;
    list_for_each_safe(_tmp1,_tmp2,_head)
    {
        item = list_entry(_tmp1,struct COBJECTMAP,list_item);
        if ( item->id == id )
        {
            list_del(_tmp1);
            delete item;
            return 0;
        }
    }

    return -1;
}

void* CCObjectMap::getMapObj(unsigned int id)
{
    int idx = (int)(id % hash_table_size);

    struct list_head* _head;
    struct list_head* _tmp;
    struct COBJECTMAP*  item;

    _head = &hash_table[idx].list_item;
    list_for_each(_tmp,_head)
    {
        item = list_entry(_tmp,struct COBJECTMAP,list_item);
        if ( item->id == id )
        {
            return item->objaddr;
        }
    }

    return NULL;
}

int CCObjectMap::dropMap()
{
    struct list_head* _head;
    struct list_head* _tmp1;
    struct list_head* _tmp2;
    struct COBJECTMAP*  item;

    for( int i=0; i<hash_table_size; i++ )
    {
        _head = &hash_table[i].list_item;
        list_for_each_safe(_tmp1,_tmp2,_head)
        {
            item = list_entry(_tmp1,struct COBJECTMAP,list_item);
            list_del(_tmp1);
            delete item;
        }
    }

    delete[] hash_table;
    hash_table = NULL;
    hash_table_size = 0;

    return 0;
}



