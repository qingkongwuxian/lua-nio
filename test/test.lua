local nio = require('nio')
local bit = require('bit')

local tty = nio.serial_open('/dev/ttyUSB0', 115200)
nio.set_non_blocking(tty)
local epoll = nio.epoll_create()
nio.epoll_add(epoll, tty, 1) -- 1: EPOLLIN 2: EPOLLOUT

while true do
  local fs = nio.epoll_wait(epoll, -1)
  if fs then
    for _, v in ipairs(fs) do
      local s = nio.read(v)
      if s then
        for i = 1, #s do
          io.write(bit.tohex(string.byte(s, i), 2) .. ' ')
        end
      end
    end
  end
end

nio.serial_close(tty)