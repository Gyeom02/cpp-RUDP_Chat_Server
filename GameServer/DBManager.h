#pragma once
#include <sql.h>
#include <sqlext.h>

const string DriverString = "Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=ChatServerDB;Trusted_Connection=Yes;";
class DBManager
{

public:
	static DBManager& Instance();


	bool SetUpDB();
	bool LoginUser(const string& id, const string& pw, OUT int32& outPrimId, OUT string& outNickname);
	int32 RegisterUser(const string& id, const string& pw, const string nickname, const string email); // 0 = simple failed, 1 = succceed, 2 = id same exsist, 3, id too short or too long, 4 = pw too short or too long, 5 = nickname too short or too long
	int32 FindAccount(const string email, OUT string& outid);
	bool SaveMessage(const string& from, const string& to, const string& message);



private:
	//CDatabase m_db;
	wstring ConvertToW(string str);
};


class DBHelper
{
public:
	DBHelper();
	~DBHelper();

	bool Connect(const std::string& connStr = DriverString);
	void Disconnect();
	bool ExecMultiQuery(const std::string& query, const std::vector<std::string>& params, std::vector<std::vector<std::string>>& results, int columnCount);

private:
	SQLHENV hEnv;
	SQLHDBC hDbc;
};
