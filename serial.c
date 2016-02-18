#include <errno.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "nio.h"

struct termios oldtio;

int nio_serial_open(lua_State *L)
{
  const char *tty = luaL_checkstring(L, 1);
  int baudrate = luaL_checkint(L, 2);

  int fd = open(tty, O_RDWR | O_NOCTTY);
  if(fd == -1)
    return luaL_error(L, "open: %s", strerror(errno));
  struct termios newtio;
  if(tcgetattr(fd, &oldtio) != 0)
    return luaL_error(L, "tcgetattr: %s", strerror(errno));
  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = baudrate | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | ICRNL;
  newtio.c_oflag = 0;
  newtio.c_lflag = ICANON;
  /*newtio.c_cc[VINTR] = 0;
  newtio.c_cc[VQUIT] = 0;
  newtio.c_cc[VERASE] = 0;
  newtio.c_cc[VKILL] = 0;
  newtio.c_cc[VEOF] = 4;
  newtio.c_cc[VTIME] = 0;
  newtio.c_cc[VMIN] = 1;
  newtio.c_cc[VSWTC] = 0;
  newtio.c_cc[VSTART] = 0;
  newtio.c_cc[VSTOP] = 0;
  newtio.c_cc[VSUSP] = 0;
  newtio.c_cc[VEOL] = 0;
  newtio.c_cc[VREPRINT] = 0;
  newtio.c_cc[VDISCARD] = 0;
  newtio.c_cc[VWERASE] = 0;
  newtio.c_cc[VLNEXT] = 0;
  newtio.c_cc[VEOL2] = 0;*/
  tcflush(fd, TCIOFLUSH);
  if((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    return luaL_error(L, "tcsetattr: %s", strerror(errno));
  lua_pushinteger(L, fd);

  return 1;
}

int nio_serial_close(lua_State *L)
{
  int fd = luaL_checkint(L, 1);

  tcflush(fd, TCIOFLUSH);
  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);

  return 0;
}