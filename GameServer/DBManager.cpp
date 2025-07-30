#include "pch.h"
#include "DBManager.h"
#include "SQL_query.h"
DBManager& DBManager::Instance()
{
	// TODO: 여기에 return 문을 삽입합니다.
	static DBManager Instance;
	return Instance;
}


bool DBManager::SetUpDB()
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return false;
	}
	string query = SQLQuery::DeleteTableAndReCreate;
	
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, {}
		, result, 0))
	{ 
		return true;
	}
	else
	{
		std::cerr << "SetUpDB Query failed" << std::endl;
		return false;
	}

}
bool DBManager::LoginUser(const string& id, const string& pw, OUT int32& outPrimId, OUT  string& outNickname)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return false;
	}
	string query = R"(DECLARE @GetPrimid INT;
					SELECT @GetPrimid = primid FROM [dbo].[Account] WHERE id = ? AND pw = ?;
					SELECT primid, nickname FROM [dbo].[User] WHERE primid = @GetPrimid;)";
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { id, pw }
		, result, 2))
	{
		if(result.empty())
			return false;
		outPrimId = stoi(result[0][0]);
		outNickname = result[0][1];
		return true;
	}
	else
	{
		std::cerr << "LoginUser Query failed" << std::endl;
		return false;
	}
	
}

int32 DBManager::RegisterUser(const string& id, const string& pw, const string nickname, const string email)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return 0;
	}
	std::string query = std::format(R"(
    DECLARE @IsExist INT;
	DECLARE @GetPrimId INT;
	DECLARE @IsEmailExist INT;

    SELECT @IsExist = primid FROM [dbo].[Account] WHERE id = '{0}';
	SELECT id, email FROM [dbo].[Account]  WHERE id = '{0}';
		
    IF @IsExist IS NULL
    BEGIN
		SELECT id, email FROM [dbo].[Account] WHERE email = '{3}';
		SELECT @IsEmailExist = primid FROM [dbo].[Account] WHERE email = '{3}';
		
		IF @IsEmailExist IS NULL
		BEGIN
			INSERT INTO [dbo].[Account] (id, pw, email) VALUES ('{0}', '{1}', '{3}');

			SET @GetPrimId = SCOPE_IDENTITY();

			INSERT INTO [dbo].[User] (primid, nickname) VALUES (@GetPrimId, '{2}');
		END
    END

)", id, pw, nickname, email);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { id, pw, nickname, email}
		, result, 2))
	{
		if (result.empty())
			return 0;
		if (result[0][0] != "" || result[0][1] != "")
		{
			if (result[0][0] == id)
			{
				cout << result[0][0] << endl;
				return 2;
			}
			else if (result[0][1] == email)
			{
				cout << result[0][1] << endl;
				return 6;
			}
			cout << "DBManager::RegisterUser Break Error" << endl;
			return 0;
		}
		
		return 1;
	}
	else
	{
		std::cerr << "RegisterUser Query failed" << std::endl;
		return 0;
	}

}

int32 DBManager::FindAccount(const string email, string& outid)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return 0;
	}
	std::string query = std::format(R"(
    SELECT id FROM [dbo].[Account] WHERE email = '{0}';
)", email);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { email }
		, result, 1))
	{
		if (result.empty() || result[0][0] == "")
			return 0;
		
		outid = result[0][0];
		return 1;
	}
	else
	{
		std::cerr << "FindAccount Query failed" << std::endl;
		return -1;
	}
}

bool DBManager::SaveMessage(const string& from, const string& to, const string& message)
{
	return false;
}

wstring DBManager::ConvertToW(string str)
{
	return wstring();
}




DBHelper::DBHelper()
	:hEnv(NULL), hDbc(NULL)
{
}

DBHelper::~DBHelper()
{
	Disconnect();
}

bool DBHelper::Connect(const std::string& connStr)
{
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS) return false;
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS) return false;

	SQLCHAR outConnStr[1024];
	SQLSMALLINT outConnStrLen;
	SQLRETURN ret = SQLDriverConnectA(
		hDbc, NULL,
		(SQLCHAR*)connStr.c_str(), SQL_NTS,
		outConnStr, sizeof(outConnStr), &outConnStrLen,
		SQL_DRIVER_NOPROMPT
	);

	return SQL_SUCCEEDED(ret);
}

void DBHelper::Disconnect()
{
	if (hDbc) {
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
		hDbc = NULL;
	}
	if (hEnv) {
		SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
		hEnv = NULL;
	}
}

bool DBHelper::ExecMultiQuery(
	const std::string& query,
	const std::vector<std::string>& params,
	std::vector<std::vector<std::string>>& results,
	int columnCount
) {
	SQLHSTMT hStmt;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS) {
		std::cerr << "[ERROR] SQLAllocHandle 실패" << std::endl;
		return false;
	}

	if (SQLPrepareA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
		std::cerr << "[ERROR] SQLPrepare 실패" << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	// 파라미터 바인딩
	for (size_t i = 0; i < params.size(); ++i) {
		if (SQLBindParameter(
			hStmt, i + 1, SQL_PARAM_INPUT,
			SQL_C_CHAR, SQL_VARCHAR,
			255, 0, (SQLPOINTER)params[i].c_str(), 0, NULL
		) != SQL_SUCCESS) {
			std::cerr << "[ERROR] 파라미터 바인딩 실패: " << i + 1 << std::endl;
			SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			return false;
		}
	}

	if (SQLExecute(hStmt) != SQL_SUCCESS) {
		SQLCHAR state[6], msg[256];
		SQLINTEGER err;
		SQLSMALLINT len;
		SQLGetDiagRecA(SQL_HANDLE_STMT, hStmt, 1, state, &err, msg, sizeof(msg), &len);
		std::cerr << "[ERROR] SQL 실행 실패: " << msg << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	// 여러 결과 집합 처리
	if (columnCount > 0) {
		do {
			// 결과가 있다면 수집
			SQLRETURN fetchRet;
			int loopCount = 0;
			while (fetchRet = SQLFetch(hStmt) != SQL_NO_DATA) {
				std::cout << "SQLFetch: " << fetchRet << std::endl;
				std::vector<std::string> row;
				char buffer[256] = {};
				for (int col = 1; col <= columnCount; ++col) {
					memset(buffer, 0, sizeof(buffer));
					if (SQLGetData(hStmt, col, SQL_C_CHAR, buffer, sizeof(buffer), NULL) == SQL_SUCCESS) {
						row.push_back(buffer);
					}
					else {
						row.push_back(""); // 에러가 있거나 NULL일 때
					}
				}
				results.push_back(row);

				loopCount++;
				if (loopCount > 10) break;
			}
		} while (SQLMoreResults(hStmt) == SQL_SUCCESS);
	}
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return true;
}