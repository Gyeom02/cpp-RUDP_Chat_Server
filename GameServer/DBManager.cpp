#include "pch.h"
#include "DBManager.h"
#include "SQL_query.h"
#include "MakeFriendCode.h"

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
bool DBManager::LoginUser(const string& id, const string& pw, OUT int32& outPrimId, OUT  string& outNickname, OUT string & getfriendcode)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return false;
	}
	string query = R"(DECLARE @GetPrimid INT;
					SELECT @GetPrimid = primid FROM [dbo].[Account] WHERE id = ? AND pw = ?;
					SELECT primid, nickname, friendcode FROM [dbo].[User] WHERE primid = @GetPrimid;)";
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { id, pw }
		, result, 3))
	{
		if(result.empty())
			return false;
		outPrimId = stoi(result[0][0]);
		outNickname = result[0][1];
		getfriendcode = result[0][2];
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
	string friendcode;
	while (true)
	{
		friendcode = generateRandomCode(id);
		std::string friendcode_query = std::format(R"(
		SELECT primid FROM [dbo].[User] WHERE friendcode = '{0}';
)", friendcode);

		std::vector<std::vector<std::string>> result;
		if (db.ExecMultiQuery(friendcode_query, { friendcode }
			, result, 1))
		{
			if (result.empty())
				break;
			if (result[0][0] == "")
			{
				break;
			}
			else
				continue;
			
		}
		else
		{
			std::cerr << "Make FriendCode Query failed" << std::endl;
			continue;
		}
	}
	std::string query = std::format(R"(
	DECLARE @GetPrimId INT;
	SELECT id, email
	FROM [dbo].[Account]
	WHERE id = '{0}' OR email = '{3}';

	IF NOT EXISTS (SELECT 1 FROM [dbo].[Account] WHERE id = '{0}' OR email = '{3}')
	BEGIN
	    INSERT INTO [dbo].[Account] (id, pw, email)
	    VALUES ('{0}', '{1}', '{3}');
	
	    SET @GetPrimId = SCOPE_IDENTITY();
	
	    INSERT INTO [dbo].[User] (primid, nickname, friendcode)
	    VALUES (@GetPrimId, '{2}', '{4}');
	END

)", id, pw, nickname, email, friendcode);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { id, pw, nickname, email,friendcode }
		, result, 2))
	{
		if (result.empty())
			return 1;
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

int32 DBManager::RequestFriend(const string friendcode, const string primid)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return 0;
	}
	std::string query = std::format(R"(
	DECLARE @getPrimid INT;
	DECLARE @isExist INT;
	DECLARE @isExist2 INT;
    SELECT @getPrimid = primid FROM [dbo].[User] WHERE friendcode = '{0}';
	SELECT @getPrimid AS primid;
	IF @getPrimid IS NOT NULL
	BEGIN
		IF @getPrimid != '{1}'
		BEGIN
			SELECT @isExist = request_id FROM [dbo].[FriendRequest] WHERE from_id = '{1}' AND to_id = @getPrimid;
			IF @isExist IS NULL
			BEGIN
				SELECT @isExist2 = user_id FROM [dbo].[Friend] WHERE user_id = @getPrimid AND friend_id = '{1}';
				IF @isExist2 IS NULL
				BEGIN
					INSERT INTO [dbo].[FriendRequest] ( from_id, to_id ) VALUES ( '{1}', @getPrimid);
				END
			END
		END
	END
)", friendcode, primid);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { friendcode, primid }
		, result, 1))
	{
		if (result.empty())
		{
			cout << "RequestFriend result empty" << endl;
			return 0;
		}
			
		else if (result[0][0] == "")
		{
			cout << "RequestFriend NULL" << endl;
			return 0;
		}
		if (result[0][0] == primid)
		{
			cout << "RequestFriend Requested himself" << endl;
			return 0;
		}
		return 1;
	}
	else
	{
		std::cerr << "RequestFriend Query failed" << std::endl;
		return 0;
	}


}

bool DBManager::GetRequests(string primid, vector<int32>& _ids, vector<string>& _names)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return 0;
	}
	std::string query = std::format(R"(
	SELECT 
		fr.from_id,
		u.nickname
	FROM [dbo].[FriendRequest] AS fr
	JOIN [dbo].[User] AS u
		ON fr.from_id = u.primid
	WHERE fr.to_id = '{0}'; 
)", primid);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { primid }
		, result, 2))
	{
		if (result.empty())
		{
			cout << "RequestFriend result empty" << endl;
			return false;
		}

		else if (result[0][0] == "")
		{
			cout << "RequestFriend NULL" << endl;
			return false;
		}
		for (int32 i = 0; i < result.size(); i++)
		{
			_ids.push_back(atoi(result[i][0].c_str()));
			_names.push_back(result[i][1]);
		}
		return true;
	}
	else
	{
		std::cerr << "RequestFriend Query failed" << std::endl;
		return false;
	}

}

bool DBManager::ReplyRequest(string primid, string friendid, string  reply)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return 0;
	}
	

	std::string query = std::format(R"(
	DECLARE @reply INT;
	DECLARE @isExist INT;
	SELECT @isExist = request_id FROM [dbo].[FriendRequest] WHERE from_id = '{2}' AND to_id = '{1}';
	
	IF @isExist IS NOT NULL
	BEGIN
		IF NOT EXISTS (SELECT 1 FROM [dbo].[Friend] WHERE friend_id = '{1}' AND user_id = '{2}')
		BEGIN
			SET @reply = '{0}';
			
			IF @reply = 1
			BEGIN
				INSERT INTO [dbo].[Friend] ( user_id, friend_id ) VALUES ( '{1}', '{2}');
				INSERT INTO [dbo].[Friend] ( user_id, friend_id ) VALUES ( '{2}', '{1}');
			END
		END
	END
	DELETE [dbo].[FriendRequest] WHERE from_id = '{2}' AND to_id = '{1}';
	
    
)", reply, primid, friendid);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { reply , primid, friendid }
		, result, 0))
	{
		
		return true;
	}
	else
	{
		std::cerr << "ReplyRequest Query failed" << std::endl;
		return false;
	}
}

bool DBManager::GetFriends(string primid, vector<int32>& _ids, vector<string>& _names)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return 0;
	}
	std::string query = std::format(R"(
	SELECT 
		fr.friend_id,
		u.nickname
	FROM [dbo].[Friend] AS fr
	JOIN [dbo].[User] AS u
		ON fr.friend_id = u.primid
	WHERE fr.user_id = '{0}'; 
)", primid);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { primid }
		, result, 2))
	{
		if (result.empty())
		{
			cout << "GetFriends result empty" << endl;
			return false;
		}

		else if (result[0][0] == "")
		{
			cout << "GetFriends NULL" << endl;
			return false;
		}
		for (int32 i = 0; i < result.size(); i++)
		{
			_ids.push_back(atoi(result[i][0].c_str()));
			_names.push_back(result[i][1]);
		}
		return true;
	}
	else
	{
		std::cerr << "GetFriends Query failed" << std::endl;
		return false;
	}
}

bool DBManager::SetMsg(string fromid, string toid, string msg)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return 0;
	}
	std::string query = std::format(R"(
	DECLARE @isFriend INT;
	SELECT @isFriend = user_id FROM [dbo].[Friend] WHERE user_id = '{0}' AND friend_id = '{1}';
	SELECT @isFriend AS user_id;
	IF @isFriend IS NOT NULL
	BEGIN
		INSERT INTO [dbo].[ChatLog] (sender_id, receiver_id, message) VALUES( '{0}', '{1}', '{2}');
	END
		 
)", fromid, toid, msg);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { fromid, toid, msg }
		, result, 1))
	{
		if (result.empty())
		{
			std::cerr << "SetMsg result.empty()" << std::endl;
			return false;
		}
		else if (result[0][0] == "")
		{
			std::cerr << "SetMsg Not Friend" << std::endl;
			return false;
		}
		return true;
	}
	else
	{
		std::cerr << "SetMsg Query failed" << std::endl;
		return false;
	}
}

bool DBManager::GetChats(string primid, string to_id, vector<int32>& _ids, vector<string>& _chats)
{
	DBHelper db;
	if (!db.Connect())
	{
		cout << "DB 연결 실패" << endl;
		return 0;
	}
	std::string query = std::format(R"(
	SELECT sender_id, message
	FROM [dbo].[ChatLog]
	WHERE (sender_id = '{0}' AND receiver_id = '{1}') OR (sender_id = '{1}' AND receiver_id = '{0}')
	ORDER BY sent_at ASC;
)", primid, to_id);
	std::vector<std::vector<std::string>> result;
	if (db.ExecMultiQuery(query, { primid , to_id }
		, result, 2))
	{
		if (result.empty())
		{
			cout << "GetChats result empty" << endl;
			return false;
		}

		else if (result[0][0] == "")
		{
			cout << "GetChats NULL" << endl;
			return false;
		}
		for (int32 i = 0; i < result.size(); i++)
		{
			_ids.push_back(atoi(result[i][0].c_str()));
			_chats.push_back(result[i][1]);
		}
		return true;
	}
	else
	{
		std::cerr << "GetFriends Query failed" << std::endl;
		return false;
	}
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
			while (true) {
				fetchRet = SQLFetch(hStmt);
				if (fetchRet == SQL_SUCCESS || fetchRet == SQL_SUCCESS_WITH_INFO) {
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
				}
				else if (fetchRet == SQL_NO_DATA) {
					// 결과 끝
					break;
				}
				else if (fetchRet == SQL_ERROR) {
					// 에러 처리
					std::cerr << "SQLFetch ERROR!" << std::endl;
					break;
				}
				else {
					// 기타 예상치 못한 리턴
					std::cerr << "SQLFetch returned: " << fetchRet << std::endl;
					break;
				}
				//loopCount++;
				//if (loopCount > 10) break;
			}
		} while (SQLMoreResults(hStmt) == SQL_SUCCESS);
	}
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return true;
}