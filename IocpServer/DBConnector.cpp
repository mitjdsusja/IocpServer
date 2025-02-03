#include "pch.h"
#include "DBConnector.h"

DBConnector::DBConnector(){

}

DBConnector::~DBConnector(){

    // 자원 해제
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

    // SQLErrorW 호출
    ret = SQLErrorW(henv, hdbc, hstmt, wszSqlState, &pfNativeError, wszErrorMsg, sizeof(wszErrorMsg) / sizeof(SQLWCHAR), &pcchErrorMsg);

    if (SQL_SUCCEEDED(ret)) {
        std::wcerr << L"SQL Error - State: " << wszSqlState << L", Error: " << wszErrorMsg << std::endl;
    }
    else {
        std::wcerr << L"Error: Failed to retrieve diagnostic information!" << std::endl;
    }
}

void DBConnector::Init(){
    // ODBC 환경 핸들 초기화
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to allocate environment handle!" << std::endl;
        return;
    }

    // ODBC 환경 속성 설정 (ODBC 3.8 지원)
    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to set ODBC version!" << std::endl;
        return;
    }

    // ODBC 연결 핸들 초기화
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to allocate connection handle!" << std::endl;
        return;
    }

}

void DBConnector::Connect(){

    // MySQL 연결 문자열 설정 (DSN 없이 직접 연결)
    SQLWCHAR connStr[] = L"DRIVER={MySQL ODBC 8.0 Unicode Driver};SERVER=localhost;DATABASE=myserverdb;USER=root;PASSWORD=sungdls200o!;";

    // SQLDriverConnect 호출로 MySQL에 연결
    ret = SQLDriverConnectW(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        PrintError(hEnv, hDbc, NULL);
        return;
    }

    cout << "Connect DB" << endl;

    // SQL 문을 실행할 핸들 초기화
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Failed to allocate statement handle!" << std::endl;
        return;
    }
}

void DBConnector::ExecuteQuary(){

    // SQL 쿼리 실행
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)L"INSERT INTO employees (name, age, hire_date) VALUES ('John Doe', 30, '2023-01-01');", SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        PrintError(hEnv, hDbc, hStmt);
        return;
    }
}
