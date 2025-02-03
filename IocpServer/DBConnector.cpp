#include "pch.h"
#include "DBConnector.h"

DBConnector::DBConnector(){

}

DBConnector::~DBConnector(){

    // �ڿ� ����
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void DBConnector::PrintError(SQLHANDLE henv, SQLHANDLE hdbc, SQLHANDLE hstmt) {

    SQLWCHAR wszSqlState[6], wszErrorMsg[256];
    SQLINTEGER pfNativeError;
    SQLSMALLINT pcchErrorMsg;
    SQLRETURN ret;

    // SQLErrorW ȣ��
    ret = SQLErrorW(henv, hdbc, hstmt, wszSqlState, &pfNativeError, wszErrorMsg, sizeof(wszErrorMsg) / sizeof(SQLWCHAR), &pcchErrorMsg);

    if (SQL_SUCCEEDED(ret)) {
        std::wcerr << L"SQL Error - State: " << wszSqlState << L", Error: " << wszErrorMsg << std::endl;
    }
    else {
        std::wcerr << L"Error: Failed to retrieve diagnostic information!" << std::endl;
    }
}

void DBConnector::Init(){
    // ODBC ȯ�� �ڵ� �ʱ�ȭ
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to allocate environment handle!" << std::endl;
        return;
    }

    // ODBC ȯ�� �Ӽ� ���� (ODBC 3.8 ����)
    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to set ODBC version!" << std::endl;
        return;
    }

    // ODBC ���� �ڵ� �ʱ�ȭ
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to allocate connection handle!" << std::endl;
        return;
    }

}

void DBConnector::Connect(){

    // MySQL ���� ���ڿ� ���� (DSN ���� ���� ����)
    SQLWCHAR connStr[] = L"DRIVER={MySQL ODBC 8.0 Unicode Driver};SERVER=localhost;DATABASE=myserverdb;USER=root;PASSWORD=sungdls200o!;";

    // SQLDriverConnect ȣ��� MySQL�� ����
    ret = SQLDriverConnectW(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        PrintError(hEnv, hDbc, NULL);
        return;
    }

    cout << "Connect DB" << endl;

    // SQL ���� ������ �ڵ� �ʱ�ȭ
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to allocate statement handle!" << std::endl;
        return;
    }
}

void DBConnector::ExecuteQuary(){

    // SQL ���� ����
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)L"INSERT INTO employees (name, age, hire_date) VALUES ('John Doe', 30, '2023-01-01');", SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        PrintError(hEnv, hDbc, hStmt);
        return;
    }
}
