#include <stdio.h>
#include <string.h>
#include "datasvr_freetds.h"


CDataSvrFreeTDS::CDataSvrFreeTDS():m_dbprocess(FAIL)
{
    is_free = true;
}


CDataSvrFreeTDS::~CDataSvrFreeTDS()
{ 
  if(!is_free)
  {
      dbfreebuf(m_dbprocess);
      dbclose(m_dbprocess); 
      dbexit();
  }
}

bool CDataSvrFreeTDS::init(const std::string & db_host, 
                           const unsigned short db_port, 
                           const std::string & db_name, 
                           const std::string & db_user, 
                           const std::string & db_pwd)
{
    host          = db_host;
    port          = db_port;
    database_name = db_name;
    user_name     = db_user;
    user_passwd   = db_pwd;

    char buf[16];
    memset(buf, 0x00, sizeof(buf));
    sprintf(buf, "%u", port);

    conn_server = host + std::string(":") + std::string(buf);

    return true;
}

bool CDataSvrFreeTDS::connect()
{
    if(dbinit() == FAIL)
    {
        throw TdsException(TdsException::TDS_EXCEPT_INIT_ERR, 
            "CDataSvrFreeTDS::connect(): dbinit => FAIL");
    }

    LOGINREC *loginrec = dblogin(); 
    if(loginrec == NULL)
    {
        //dbexit();
        throw TdsException(TdsException::TDS_EXCEPT_LOGIN_ERR, 
            "CDataSvrFreeTDS::connect(): dblogin => FAIL");
    }

    DBSETLUSER(loginrec, user_name.c_str());         
    DBSETLPWD(loginrec,  user_passwd.c_str());  
    m_dbprocess = dbopen(loginrec, conn_server.c_str());
    if(m_dbprocess == FAIL)  
    {  
        dbloginfree(loginrec);
        //dbexit();
        std::string err_infor = std::string("CDataSvrFreeTDS::connect(): dbuse => FAIL, ")  
            + conn_server + ", user:" + user_name + ", pass:" + user_passwd;
        throw TdsException(TdsException::TDS_EXCEPT_OPEN_ERR, err_infor.c_str());
    }  
         
    if(dbuse(m_dbprocess, database_name.c_str()) == FAIL)  
    {
        dbloginfree(loginrec);
        dbfreebuf(m_dbprocess);
        //dbexit();
        std::string err_infor = std::string("CDataSvrFreeTDS::connect(): dbuse => FAIL, ") 
            + conn_server + ", user:" + user_name 
            + ", pass:" + user_passwd + ", db:" + database_name;
        throw TdsException(TdsException::TDS_EXCEPT_DBUSE_ERR, err_infor.c_str());
    }

    dbloginfree(loginrec);
    is_free = false;

    /*WRITELOG("Connect msserver database success, conn_server:%s, user:%s, pwd:%s\n", 
        conn_server.c_str(), user_name.c_str(), user_passwd.c_str());*/

    return true;  
}


void CDataSvrFreeTDS::disconnect()
{
    dbfreebuf(m_dbprocess);
    dbclose(m_dbprocess); 
    //dbexit();
    is_free = true;

    return;  
}


/*bool FTDS::ExecSQL(const char *strSQL)
{
    dbcmd(m_dbprocess, strSQL);  
    if(dbsqlexec(m_dbprocess) == SUCCEED)  
    {  
        return true;   
    }
    else
    {
    	dbcancel(m_dbprocess);
	    throw TdsException("nextRow(): dbnextrow => FAIL");
    	
    }
}*/

bool CDataSvrFreeTDS::execSQL(const char *strSQL){
	if (dbcmd(m_dbprocess, strSQL) != SUCCEED) {
		dbcancel(m_dbprocess);
        //throw TdsException("execDML(): dbcmd => FAIL");
		throw TdsException(TdsException::TDS_EXCEPT_DBCMD_ERR, 
            "execSQL(): dbcmd => FAIL");
	}

	if (dbsqlexec(m_dbprocess) != SUCCEED) {
		dbcancel(m_dbprocess);
        //throw TdsException("execDML(): dbsqlexec => FAIL");
		throw TdsException(TdsException::TDS_EXCEPT_DBEXEC_ERR, 
            "execSQL(): dbsqlexec => FAIL");
	}
	return true;
}


/*int FTDS::GetResult()
{
    DBINT result_code = dbresults(m_dbprocess);
    if(result_code == SUCCEED)
    {
       return 1;
    }
    else if(result_code == NO_MORE_RESULTS)
    {
       return 0;
    }
    else //if(result_code == FAIL)
    {
       dbcancel(m_dbprocess);
	   throw TdsException("nextRow(): dbnextrow => FAIL");
     
    }
}*/

int CDataSvrFreeTDS::getResult(){
	int ret;
	while ((ret = dbresults(m_dbprocess)) != NO_MORE_RESULTS) {
		if (ret == FAIL) {
			dbcancel(m_dbprocess);
			//throw TdsException("execDML(): dbcancel => FAIL");
            throw TdsException(TdsException::TDS_EXCEPT_GETRESULT_ERR, 
                "getResult(): dbcancel => FAIL");
		}

		return 1;
	}
	return 0;
}



bool CDataSvrFreeTDS::bindColumn(int column, int varLen, char *varAddr)
{
    if(dbbind(m_dbprocess, column, NTBSTRINGBIND, (DBINT)varLen, (BYTE*)varAddr) == SUCCEED)
    {
    	return true;   
    }
    else
    {
       //return false;
       throw TdsException(TdsException::TDS_EXCEPT_BIND_ERR, 
           "BindColumn(): dbbind => FAIL");
    }
}


bool CDataSvrFreeTDS::bindColumn(int column, int varLen, int *varAddr)
{
    if(dbbind(m_dbprocess, column, INTBIND, (DBINT)varLen, (BYTE*)varAddr) == SUCCEED)
    {
    	return true;   
    }
    else
    {
       //return false;
       throw TdsException(TdsException::TDS_EXCEPT_BIND_ERR, 
           "BindColumn(): dbbind => FAIL");
    }
}


/*bool FTDS::MoveNextRow()
{
    if(dbnextrow(m_dbprocess) != NO_MORE_ROWS)
    {
    	return true;   
    }
    else
    {
       return false;
    }
}*/

bool CDataSvrFreeTDS::moveNextRow()
{
	int row_code = dbnextrow(m_dbprocess);
	if (row_code == NO_MORE_ROWS) 
    {
		return false;
	}
    else if (row_code == REG_ROW) 
    {
		return true;
	}else if (row_code == BUF_FULL) 
    {
		//throw TdsException("nextRow(): dbnextrow => BUF_FULL");
        throw TdsException(TdsException::TDS_EXCEPT_BUFFULL_ERR, 
            "nextRow(): dbnextrow => BUF_FULL");
	}else 
    {	
		dbcancel(m_dbprocess);
		//throw TdsException("nextRow(): dbnextrow => FAIL");
        throw TdsException(TdsException::TDS_EXCEPT_NEXTROW_ERR, 
            "nextRow(): dbnextrow => FAIL");
	}
}


int CDataSvrFreeTDS::countRow()
{
    return int(dbcount(m_dbprocess));
}


bool CDataSvrFreeTDS::isCount()
{
    if(dbiscount(m_dbprocess) == TRUE)
    {
        return true;
    }
    else
    {
        return false;
    } 
}

bool CDataSvrFreeTDS::cancel()
{
	dbcancel(m_dbprocess);
	return true;
}









 
