#ifndef P7ZIP_COMMAND_H
#define P7ZIP_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <MyTypes.h>
#include <lua.hpp>
    
int MY_CDECL
p7zip_main(
#ifndef _WIN32
        int numArgs, char *args[]
#endif
);

extern int p7zip_executeCommand(const char *cmd);
extern int luaopen_p7zip(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif
