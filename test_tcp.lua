local nio = require('nio')
local const = require('const')
local bit = require('bit')

local sock = nio.tcp()
local epoll = nio.epoll_create()

local connected = true
local ret, err = nio.connect(sock, 'www.baidu.com', 80)
if ret == -1 and err == const.EINPROGRESS then
  connected = false
end

local evt = bit.bor(const.EPOLLET, const.EPOLLIN)
if not connected then
  evt = bit.bor(evt, const.EPOLLOUT)
end

nio.epoll_add(epoll, sock, evt)

while true do
  local events = nio.epoll_wait(epoll, -1)
  if events then
    for _, v in ipairs(events) do
      if v[2] == const.EPOLLOUT then
        if not connected then
          print('connected')
          local evt = bit.bor(const.EPOLLET, const.EPOLLIN)
          nio.epoll_mod(epoll, v[1], evt)

          local s = 'GET / HTTP/1.1\r\n\r\n';
          local n = nio.write(v[1], s, 0)
          while n < #s do
            local n1 = nio.write(v[1], s, n)
            n = n + n1
          end
        else
          print('error connecting')
        end
      elseif v[2] == const.EPOLLIN then
        local s = nio.read(v[1])
        if s then
          print(s)
        end
      end
    end
  end
end
