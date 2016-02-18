#ifndef LUA_NIO_H
#define LUA_NIO_H

#include <lua.h>

#define MAXEVENT 64

int nio_set_non_blocking(lua_State *L);

int nio_open(lua_State *L);

int nio_read(lua_State *L);

int nio_close(lua_State *L);

int nio_serial_open(lua_State *L);

int nio_serial_close(lua_State *L);

int nio_epoll_create(lua_State *L);

int nio_epoll_wait(lua_State *L);

int nio_epoll_add(lua_State *L);

#endif
