#include "str2args.h"
#include "command.h"
#include "Common/StdOutStream.h"
#include "Common/UTFConvert.h"

#include "7zip/UI/Common/ArchiveCommandLine.h"
#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include "LuaIntf.h"
#include "CppArg.h"

extern UString g_LastErr;


 class Web
 {
    static std::string _base_url;
 public:
     // base_url is optional
     Web(const std::string& base_url){_base_url = base_url;};
     ~Web(){};

     static void go_home(){};

     static std::string home_url(){return _base_url;}
     static void set_home_url(const std::string& url){_base_url = url;}

     std::string url() const{return _base_url;}
     void set_url(const std::string& url){};
     std::string resolve_url(const std::string& uri){return _base_url + uri;}

     // doing reload if uri is empty
     std::string load(const std::string& uri){return _base_url + uri;};
 };
std::string Web::_base_url;
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
        LuaIntf::LuaBinding(L).beginClass<Web>("Web")
            .addConstructor(LUA_ARGS(const std::string))
            .addStaticProperty("home_url", &Web::home_url, &Web::set_home_url)
            .addStaticFunction("go_home", &Web::go_home)
            .addProperty("url", &Web::url, &Web::set_url)
            .addFunction("resolve_url", &Web::resolve_url)
            .addFunction("load", &Web::load, LUA_ARGS(const std::string))
            .addStaticFunction("lambda", [] {
                // you can use C++11 lambda expression here too
                return "yes";
            })
        .endClass();
        
        LuaIntf::LuaBinding(L).beginModule("p7zip")
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
        .endModule();
        
        lua_settop(L, 0);
        lua_newtable(L);
        lua_pushcfunction(L, execute);
        lua_setfield(L, -2, "execute");

        return 1;
    }

}
