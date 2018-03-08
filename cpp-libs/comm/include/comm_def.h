/* ===========================================================================
*   Description: 服务公共定义文件
*   Author:  huihai.shen@inin88.com
*   Date :  
*   histroy:
* ============================================================================
*/
#ifndef  _COMM_DEF_H_
#define  _COMM_DEF_H_


//安全申请内存
#define ADDMEMREC(p)

#define NEW( var, classname ) \
        { \
            try \
            { \
                var = new classname; \
            } \
            catch (...) \
            { \
                var = NULL; \
            } \
            if(NULL != var) \
            { \
                ADDMEMREC(var); \
            } \
            else \
            { \
                ERROR_LOG("Failed to allocate memory for new %s\n", #classname); \
            } \
        }


//安全释放内存在
#define DELMEMREC(p)

#define DELETE(var) \
        if(NULL != var) \
        { \
            DELMEMREC(var); \
            delete var; \
            var = NULL; \
        }
        
#define DELETE_A( var ) \
        if(NULL != var) \
        { \
            DELMEMREC(var); \
            delete [] var; \
            var = NULL; \
        }




#endif


