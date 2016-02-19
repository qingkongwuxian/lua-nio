local nio = require('nio')
local const = require('const')
local bit = require('bit')

local tty = nio.serial_open('/dev/ttyUSB0', 115200)
local epoll = nio.epoll_create()
nio.epoll_add(epoll, tty, bit.bor(const.EPOLLET, const.EPOLLIN))

while true do
  local events = nio.epoll_wait(epoll, -1)
  if events then
    for _, v in ipairs(events) do
      if v[2] == const.EPOLLIN then
        local s = nio.read(v[1])
        if s then
          for i = 1, #s do
            io.write(bit.tohex(string.byte(s, i), 2) .. ' ')
          end
        end
      end
    end
  end
end