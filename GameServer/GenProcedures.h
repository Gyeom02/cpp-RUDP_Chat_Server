#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

namespace SP
{
	
    class MakeAccount : public DBBind<3,0>
    {
    public:
    	MakeAccount(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spMakeAccount(?,?,?)}") { }
    	template<int32 N> void In_Id(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Id(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Id(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Id(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	template<int32 N> void In_Pw(WCHAR(&v)[N]) { BindParam(1, v); };
    	template<int32 N> void In_Pw(const WCHAR(&v)[N]) { BindParam(1, v); };
    	void In_Pw(WCHAR* v, int32 count) { BindParam(1, v, count); };
    	void In_Pw(const WCHAR* v, int32 count) { BindParam(1, v, count); };
    	template<int32 N> void In_Nickname(WCHAR(&v)[N]) { BindParam(2, v); };
    	template<int32 N> void In_Nickname(const WCHAR(&v)[N]) { BindParam(2, v); };
    	void In_Nickname(WCHAR* v, int32 count) { BindParam(2, v, count); };
    	void In_Nickname(const WCHAR* v, int32 count) { BindParam(2, v, count); };

    private:
    };

    class Login : public DBBind<2,2>
    {
    public:
    	Login(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spLogin(?,?)}") { }
    	template<int32 N> void In_Id(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Id(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Id(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Id(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	template<int32 N> void In_Pw(WCHAR(&v)[N]) { BindParam(1, v); };
    	template<int32 N> void In_Pw(const WCHAR(&v)[N]) { BindParam(1, v); };
    	void In_Pw(WCHAR* v, int32 count) { BindParam(1, v, count); };
    	void In_Pw(const WCHAR* v, int32 count) { BindParam(1, v, count); };
    	void Out_Primid(OUT int32& v) { BindCol(0, v); };
    	template<int32 N> void Out_Nickname(OUT WCHAR(&v)[N]) { BindCol(1, v); };

    private:
    };


     
};