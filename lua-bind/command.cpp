#include "str2args.h"
#include "command.h"
#include "Common/StdOutStream.h"
#include "Common/UTFConvert.h"

#include "7zip/UI/Common/ArchiveCommandLine.h"
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include "LuaIntf.h"
#include "CppArg.h"

extern UString g_LastErr;

extern "C"
{
    extern int p7zip_executeCommand(const char *cmd) {
        int argc = 0;
        char temp[ARGC_MAX][ARGV_LEN_MAX];
        char *argv[ARGC_MAX];
        if (!str2args(cmd, temp, &argc)) {
            return 7;
        }
        for (int i = 0; i < argc; i++) {
            argv[i] = temp[i];
                //        printf("arg[%d]:[%s]", i, argv[i]);
        }
        int ret = p7zip_main(argc, argv);
        return ret;
    }


    int execute(lua_State *L) {
        const char *cmd = luaL_checkstring( L,1 );
        int ret = p7zip_executeCommand(cmd);
        lua_pushnumber(L, ret);
        AString dis;
        ConvertUnicodeToUTF8(g_LastErr, dis);
        lua_pushstring(L, dis.Ptr());
        return 2;
    }
 
    extern int luaopen_p7zip(lua_State *L)
    {
        LuaIntf::LuaRef mod = LuaIntf::LuaRef::createTable(L);
        LuaIntf::LuaBinding(L).beginModule("p7zip")
        .addVariableRef("g_StdOut", &g_StdOut)
        .addVariableRef("g_StdErr", &g_StdErr)
        .addFunction("execute", &p7zip_executeCommand, LUA_ARGS(const char*))
        .beginClass<CArcCmdLineOptions>("CArcCmdLineOptions")
            .addVariableRef("HelpMode", &CArcCmdLineOptions::HelpMode)
        .endClass()
        .beginClass<CArcCmdLineParser>("CArcCmdLineParser")
            .addFunction("Parse1", &CArcCmdLineParser::Parse1, LUA_ARGS(const UStringVector, CArcCmdLineOptions))
            .addFunction("Parse2", &CArcCmdLineParser::Parse2, LUA_ARGS(CArcCmdLineOptions))
            .addStaticFunction("lambda", [] {
                // you can use C++11 lambda expression here too
                return "yes";
            })
        .endClass()
        .beginClass<CStdOutStream>("CStdOutStream")
            .addConstructor(LUA_ARGS(FILE *))
            .addFunction("Open", &CStdOutStream::Open, LUA_ARGS(const char*, const char*))
            .addFunction("Close", &CStdOutStream::Close)
            .addFunction("Print", &CStdOutStream::Print, LUA_ARGS(const char*))
            .addFunction("Flush", &CStdOutStream::Flush)
        .endClass()
        .endModule();
        mod.pushToStack();

//        lua_settop(L, 0);
//        lua_newtable(L);
//        lua_pushcfunction(L, execute);
//        lua_setfield(L, -2, "execute");

        return 1;
    }

}
