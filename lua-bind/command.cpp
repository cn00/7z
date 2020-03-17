#include "str2args.h"
#include "command.h"
#include "Common/StdOutStream.h"
#include "Common/UTFConvert.h"

extern UString g_LastErr;

extern "C"
{
    extern int luaopen_p7zip( lua_State *L );

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
        lua_settop(L, 0);
        lua_newtable(L);
        lua_pushcfunction(L, execute);
        lua_setfield(L, -2, "execute");

        return 1;
    }

}
