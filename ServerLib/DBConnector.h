#pragma once

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

class DBConnector {
public:
	DBConnector();
	~DBConnector();

	void PrintError(SQLHANDLE henv, SQLHANDLE hdbc, SQLHANDLE hstmt);
	void Init();
	void Connect();
	bool ExecuteQuary(wstring query);
	vector<vector<wstring>> ExecuteSelectQuery(wstring query);

private:
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;
	SQLRETURN ret;

};

