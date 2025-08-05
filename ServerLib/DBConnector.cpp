#include "pch.h"
#include "DBConnector.h"

DBConnector::DBConnector() {
    Init();
    Connect();
}

DBConnector::~DBConnector() {

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

        std::wstring ws_state(wszSqlState);
        std::wstring ws_msg(wszErrorMsg);

        spdlog::info("SQL Error - State: {}, Error: ", boost::locale::conv::utf_to_utf<char>(wszSqlState), boost::locale::conv::utf_to_utf<char>(wszErrorMsg));
        //std::wcerr << L"SQL Error - State: " << wszSqlState << L", Error: " << wszErrorMsg << std::endl;
    }
    else {

        spdlog::info("Error: Failed to retrieve diagnostic information!");
        //std::wcerr << L"Error: Failed to retrieve diagnostic information!" << std::endl;
    }
}

void DBConnector::Init() {
    // ODBC 환경 핸들 초기화
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {

        spdlog::info("Failed to allocate environment handle!");
        //std::cerr << "Failed to allocate environment handle!" << std::endl;
        return;
    }

    // ODBC 환경 속성 설정 (ODBC 3.8 지원)
    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {

        spdlog::info("Failed to set ODBC version!");
        //std::cerr << "Failed to set ODBC version!" << std::endl;
        return;
    }

    // ODBC 연결 핸들 초기화
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {

        spdlog::info("Failed to allocate connection handle!");
        //std::cerr << "Failed to allocate connection handle!" << std::endl;
        return;
    }

}

void DBConnector::Connect() {

    // MySQL 연결 문자열 설정 (DSN 없이 직접 연결)
    SQLWCHAR connStr[] = L"DRIVER={MySQL ODBC 8.0 Unicode Driver};SERVER=localhost;DATABASE=myserverdb;USER=root;PASSWORD=sungdls200o!;";

    // SQLDriverConnect 호출로 MySQL에 연결
    ret = SQLDriverConnectW(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        PrintError(hEnv, hDbc, NULL);
        return;
    }

    spdlog::info("Connect DB");
    //cout << "Connect DB" << endl;

    // SQL 문을 실행할 핸들 초기화
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {

        spdlog::info("Failed to allocate statement handle!");
        //std::cerr << "Failed to allocate statement handle!" << std::endl;
        return;
    }
}

bool DBConnector::ExecuteQuery(wstring query) {

    // SQL 쿼리 실행
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        PrintError(hEnv, hDbc, hStmt);
        return false;
    }

    return true;
}

vector<vector<wstring>> DBConnector::ExecuteSelectQuery(wstring query) {
    vector<vector<wstring>> results;

    // SQL 실행
    ret = SQLExecDirect(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        PrintError(hEnv, hDbc, hStmt);
        return results; // 빈 결과 반환
    }

    SQLSMALLINT colCount = 0;
    SQLNumResultCols(hStmt, &colCount);

    if (colCount <= 0) {

        spdlog::info("No columns found in result set!");
        //std::cerr << "No columns found in result set!" << std::endl;
        return results;
    }

    // 결과를 저장할 변수
    vector<SQLWCHAR> buffer(256);
    SQLLEN indicator;

    // 데이터 가져오기
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        vector<wstring> row;
        for (SQLSMALLINT i = 1; i <= colCount; ++i) {
            ret = SQLGetData(hStmt, i, SQL_C_WCHAR, buffer.data(), buffer.size() * sizeof(SQLWCHAR), &indicator);
            if (SQL_SUCCEEDED(ret)) {
                row.push_back(wstring(buffer.data()));
            }
            else {
                row.push_back(L"NULL");
            }
        }
        results.push_back(row);
    }

    SQLCloseCursor(hStmt);
    return results;
}

