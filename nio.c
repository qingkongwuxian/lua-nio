#include <lua.h>
#include <lauxlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAXEVENT 64

static int nio_open(lua_State *L)
{
  const char *file = luaL_checkstring(L, 1);
  int flag = luaL_checkint(L, 2);
  int fd = open(file, flag | O_NONBLOCK);
  if(fd == -1)
    return luaL_error(L, "open: %s", strerror(errno));
  lua_pushinteger(L, fd);
  return 1;
}

static int nio_read(lua_State *L)
{
  int fd = luaL_checkint(L, 1);
  size_t avail = 0;
  ioctl(fd, FIONREAD, &avail);
  if(avail > 0)
  {
    char buf[avail];
    int done = 0;
    while(done < avail)
    {
      ssize_t n = read(fd, buf + done, avail);
      if(n == -1)
        break;
      else if(n == 0)
        return luaL_error(L, "read: %s", strerror(errno));
      else
        done += n;
    }
    lua_pushlstring(L, buf, avail);
    return 1;
  }
  else
    return 0;
}

static int nio_write(lua_State *L)
{
  size_t len = 0;
  int fd = luaL_checkint(L, 1);
  const char *s = luaL_checklstring(L, 2, &len);
  int offset = luaL_checkint(L, 3);
  ssize_t n = write(fd, s + offset, len - offset);
  lua_pushinteger(L, n);
  return 1;
}

static int nio_close(lua_State *L)
{
  int fd = luaL_checkint(L, 1);
  close(fd);
  return 0;
}

static inline speed_t baudrate(int speed)
{
  switch(speed)
  {
    case 0:
      return B0;
    case 50:
      return B50;
    case 75:
      return B75;
    case 110:
      return B110;
    case 134:
      return B134;
    case 150:
      return B150;
    case 200:
      return B200;
    case 300:
      return B300;
    case 600:
      return B600;
    case 1200:
      return B1200;
    case 1800:
      return B1800;
    case 2400:
      return B2400;
    case 4800:
      return B4800;
    case 9600:
      return B9600;
    case 19200:
      return B19200;
    case 38400:
      return B38400;
    case 57600:
      return B57600;
    case 115200:
      return B115200;
    case 230400:
      return B230400;
    case 460800:
      return B460800;
    case 500000:
      return B500000;
    case 576000:
      return B576000;
    case 921600:
      return B921600;
    case 1000000:
      return B1000000;
    case 1152000:
      return B1152000;
    case 1500000:
      return B1500000;
    case 2000000:
      return B2000000;
    case 2500000:
      return B2500000;
    case 3000000:
      return B3000000;
    case 3500000:
      return B3500000;
    case 4000000:
      return B4000000;
    default:
      return __MAX_BAUD;
  }
}

static int nio_serial_open(lua_State *L)
{
  const char *tty = luaL_checkstring(L, 1);
  int speed = luaL_checkint(L, 2);
  speed_t rate = baudrate(speed);
  int fd = open(tty, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if(fd == -1)
    return luaL_error(L, "open: %s", strerror(errno));
  int flag = fcntl(fd, F_GETFL, 0);
  flag |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flag);
  struct termios newtio;
  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = rate | CLOCAL | CREAD | CS8;
  cfsetispeed(&newtio, rate);
  cfsetospeed(&newtio, rate);
  tcflush(fd, TCIOFLUSH);
  if((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    return luaL_error(L, "tcsetattr: %s", strerror(errno));
  lua_pushinteger(L, fd);
  return 1;
}

static struct epoll_event events[MAXEVENT];

static int nio_epoll_create(lua_State *L)
{
  int epollfd = epoll_create1(0);
  if(epollfd == -1)
    return luaL_error(L, "epoll_create1: %s", strerror(errno));
  lua_pushinteger(L, epollfd);
  return 1;
}

static int nio_epoll_add(lua_State *L)
{
  int epollfd = luaL_checkint(L, 1);
  int fd = luaL_checkint(L, 2);
  long evts = luaL_checklong(L, 3);
  struct epoll_event event;
  event.data.fd = fd;
  event.events = (uint32_t)evts;
  int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  if(ret == -1)
    return luaL_error(L, "epoll_ctl: %s", strerror(errno));
  return 0;
}

static int nio_epoll_mod(lua_State *L)
{
  int epollfd = luaL_checkint(L, 1);
  int fd = luaL_checkint(L, 2);
  long evts = luaL_checklong(L, 3);
  struct epoll_event event;
  event.data.fd = fd;
  event.events = (uint32_t)evts;
  int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
  if(ret == -1)
    return luaL_error(L, "epoll_ctl: %s", strerror(errno));
  return 0;
}

static int nio_epoll_del(lua_State *L)
{
  int epollfd = luaL_checkint(L, 1);
  int fd = luaL_checkint(L, 2);
  struct epoll_event event; //for bug before linux 2.6.9
  int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
  if(ret == -1)
    return luaL_error(L, "epoll_ctl: %s", strerror(errno));
  return 0;
}

static int nio_epoll_wait(lua_State *L)
{
  int epollfd = luaL_checkint(L, 1);
  int timeout = luaL_checkint(L, 2);
  int n = epoll_wait(epollfd, events, MAXEVENT, timeout);
  if(n > 0)
  {
    lua_newtable(L);
    int i;
    for(i = 0; i < n; i++)
    {
      lua_pushinteger(L, i + 1);

      lua_newtable(L);
      lua_pushinteger(L, 1);
      lua_pushinteger(L, events[i].data.fd);
      lua_settable(L, -3);
      lua_pushinteger(L, 2);
      lua_pushinteger(L, events[i].events);
      lua_settable(L, -3);

      lua_settable(L, -3);
    }
    return 1;
  }
  else
    return 0;
}

static int nio_tcp(lua_State *L)
{
  int fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
  if(fd == -1)
    return luaL_error(L, "socket: %s", strerror(errno));
  int yes = 1;
  if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    return luaL_error(L, "setsockopt: %s", strerror(errno));
  lua_pushinteger(L, fd);
  return 1;
}

static int nio_connect(lua_State *L)
{
  int fd = luaL_checkint(L, 1);
  const char *host = luaL_checkstring(L, 2);
  int port = luaL_checkint(L, 3);
  struct hostent *ent;
  if((ent = gethostbyname(host)) == NULL)
    return luaL_error(L, "gethostbyname: %s", strerror(errno));
  struct in_addr **addrs = (struct in_addr **)ent->h_addr_list;
  if(addrs[0] == NULL)
    return luaL_error(L, "hostname not found");
  struct sockaddr_in s;
  bzero(&s, sizeof(s));
  s.sin_family = AF_INET;
  s.sin_addr = *addrs[0];
  s.sin_port = htons((uint16_t)port);
  int ret = connect(fd, (struct sockaddr *)&s, sizeof(s));
  lua_pushinteger(L, ret);
  if(ret == -1)
    lua_pushinteger(L, errno);
  return ret == -1 ? 2 : 1;
}

static const struct luaL_Reg nio[] = {{"open", nio_open},
  {"close", nio_close},
  {"read", nio_read},
  {"write", nio_write},
  {"serial_open", nio_serial_open},
  {"epoll_create", nio_epoll_create},
  {"epoll_add", nio_epoll_add},
  {"epoll_mod", nio_epoll_mod},
  {"epoll_del", nio_epoll_del},
  {"epoll_wait", nio_epoll_wait},
  {"tcp", nio_tcp},
  {"connect", nio_connect},
  {NULL, NULL}};

int luaopen_nio(lua_State *L)
{
  luaL_register(L, "nio", nio);
  return 1;
}