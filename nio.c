#include <lua.h>
#include <lauxlib.h>
#include "nio.h"

struct luaL_Reg nio[] = {
  {"open", nio_open},
  {"close", nio_close},
  {"read", nio_read},
  {"set_non_blocking", nio_set_non_blocking},

  {"serial_open", nio_serial_open},
  {"serial_close", nio_serial_close},

  {"epoll_create", nio_epoll_create},
  {"epoll_add", nio_epoll_add},
  {"epoll_wait", nio_epoll_wait},

  {NULL, NULL}
};

int luaopen_nio(lua_State *L)
{
  luaL_register(L, "nio", nio);
  return 1;
}