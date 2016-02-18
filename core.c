#include <errno.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "nio.h"

int nio_set_non_blocking(lua_State *L)
{
  int fd = luaL_checkint(L, 1);

  int flags = fcntl(fd, F_GETFL, 0);
  if(flags == -1)
    return luaL_error(L, "fcntl: %s", strerror(errno));
  flags |= O_NONBLOCK;
  int ret = fcntl(fd, F_SETFL, flags);
  if(ret == -1)
    return luaL_error(L, "fcntl: %s", strerror(errno));

  return 0;
}

int nio_open(lua_State *L)
{
  const char *file = luaL_checkstring(L, 1);
  int flag = luaL_checkint(L, 2);

  int fd = open(file, flag);
  if(fd == -1)
    return luaL_error(L, "open: %s", strerror(errno));
  lua_pushinteger(L, fd);

  return 1;
}

int nio_read(lua_State *L)
{
  int fd = luaL_checkint(L, 1);

  size_t avail;
  ioctl(fd, FIONREAD, &avail);
  if(avail > 0)
  {
    char buf[avail];
    int done;
    done = 0;
    while(done < avail)
    {
      ssize_t count = read(fd, buf + done, avail);
      if(count == -1)
        break;
      else if(count == 0)
        return luaL_error(L, "read: %s", strerror(errno));
      else
        done += count;
    }
    lua_pushlstring(L, buf, avail);
  }
  else
    lua_pushnil(L);

  return 1;
}

int nio_close(lua_State *L)
{
  int fd = luaL_checkint(L, 1);

  close(fd);

  return 0;
}