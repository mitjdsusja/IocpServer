#pragma once

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

class DBConnector{
public:
	DBConnector();
	~DBConnector();

	void PrintError(SQLHANDLE henv, SQLHANDLE hdbc, SQLHANDLE hstmt);
	void Init();
	void Connect();
	void ExecuteQuary();

private:
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret;

};

