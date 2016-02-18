#include <errno.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <sys/epoll.h>
#include "nio.h"

struct epoll_event events[MAXEVENT];

int nio_epoll_create(lua_State *L)
{
  int epollfd = epoll_create1(0);

  if(epollfd == -1)
    return luaL_error(L, "epoll_create1: %s", strerror(errno));

  lua_pushinteger(L, epollfd);
  return 1;
}

int nio_epoll_add(lua_State *L)
{
  int epollfd = luaL_checkint(L, 1);
  int fd = luaL_checkint(L, 2);
  int evt = luaL_checkint(L, 3);

  uint32_t evts = EPOLLET;
  switch(evt)
  {
    case 1:
      evts |= EPOLLIN;
      break;
    case 2:
      evts |= EPOLLOUT;
      break;
    default:
      return luaL_error(L, "only 1(EPOLLIN) or 2(EPOLLOUT) allowed");
  }

  struct epoll_event event;
  event.data.fd = fd;
  event.events = evts;
  int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  if(ret == -1)
    return luaL_error(L, "epoll_ctl: %s", strerror(errno));

  return 0;
}

int nio_epoll_wait(lua_State *L)
{
  int epollfd = luaL_checkint(L, 1);
  int timeout = luaL_checkint(L, 2);

  int n = epoll_wait(epollfd, events, MAXEVENT, timeout);
  if(n > 0)
  {
    int i;

    lua_newtable(L);
    for(i = 0; i < n; i++)
      if(!(events[i].events & EPOLLERR) && !(events[i].events & EPOLLHUP) && (events[i].events & EPOLLIN))
      {
        lua_pushinteger(L, i + 1);
        lua_pushinteger(L, events[i].data.fd);
        lua_settable(L, -3);
      }
  }
  else
    lua_pushnil(L);

  return 1;
}