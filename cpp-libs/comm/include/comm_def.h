/* ===========================================================================
*   Description: ���񹫹������ļ�
*   Author:  huihai.shen@inin88.com
*   Date :  
*   histroy:
* ============================================================================
*/
#ifndef  _COMM_DEF_H_
#define  _COMM_DEF_H_


//��ȫ�����ڴ�
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


//��ȫ�ͷ��ڴ���
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


