local bit = require('bit')
return
{
  O_RDONLY = 0,
  O_WRONLY = 1,
  O_RDWR = 2,
  O_NOCTTY = 256,

  EPOLLIN = 0x001,
  EPOLLOUT = 0x004,
  EPOLLET = bit.lshift(1, 31),

  EINPROGRESS = 115,
}