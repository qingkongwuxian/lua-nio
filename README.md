# 摘要
<pre>关于epoll，串口, TCP, UDP等操作的Lua封装</pre>

# 函数说明

### nio.open
<pre>
打开一个文件，返回文件描述符

<b>语法</b>
nio.open(file, flag)

<b>示例</b>
local nio = require('nio')
local const = require('const')

local fd = nio.open('/tmp/foo', const.O_RDWR)
</pre>

### nio.read
<pre>
从文件中读取缓冲中的全部内容，返回为lua string
缓冲区无内容则返回nil

<b>语法</b>
nio.read(fd)

<b>示例</b>
local nio = require('nio')
local const = require('const')

local fd = nio.open('/tmp/foo', const.O_RDONLY)
local s = nio.read(fd)
</pre>

### nio.write
<pre>
将字符串写入文件，返回成功写出的字节数

<b>语法</b>
nio.write(fd, str, offset)

<b>示例</b>
local nio = require('nio')
local const = require('const')

local fd = nio.open('/tmp/foo', const.O_WRONLY)
local s = 'hello world!'
local n = write(fd, s, 0)
</pre>

### nio.close
<pre>
关闭文件

<b>语法</b>
nio.close(fd)

<b>示例</b>
local nio = require('nio')
local const = require('const')

local fd = nio.open('/tmp/foo', const.O_RDWR)
nio.close(fd)
</pre>

### nio.serial_open
<pre>
以O_RDWR模式打开串口，返回串口文件描述符

<b>语法</b>
nio.serial_open(file, baudrate)

<b>示例</b>
local nio = require('nio')

local tty = nio.serial_open('/dev/ttyUSB0', 115200)
</pre>

### nio.epoll_create
<pre>
创建并返回epoll描述符

<b>语法</b>
nio.epoll_create()

<b>示例</b>
local nio = require('nio')

local epoll = nio.epoll_create()
</pre>

### nio.epoll_add
<pre>
将文件描述符加入到epoll中，并设置事件类型

<b>语法</b>
nio.epoll_add(epoll, fd, event)

<b>示例</b>
local nio = require('nio')
local const = require('const')
local bit = require('bit')

local tty = nio.serial_open('/dev/ttyUSB0', 115200)
local epoll = nio.epoll_create()
nio.epoll_add(epoll, tty, bit.bor(const.EPOLLET, const.EPOLLIN))
</pre>

### nio.epoll_mod
<pre>
更改事件类型

<b>语法</b>
nio.epoll_add(epoll, fd, event)

<b>示例</b>
local nio = require('nio')
local const = require('const')
local bit = require('bit')

local tty = nio.serial_open('/dev/ttyUSB0', 115200)
local epoll = nio.epoll_create()
nio.epoll_add(epoll, tty, bit.bor(const.EPOLLET, const.EPOLLIN))
nio.epoll_mod(epoll, tty, bit.bor(const.EPOLLET, const.EPOLLIN, const.EPOLLOUT))
</pre>

### nio.epoll_del
<pre>
将文件描述符从epoll事件中移除

<b>语法</b>
nio.epoll_del(epoll, fd)

<b>示例</b>
local nio = require('nio')
local const = require('const')
local bit = require('bit')

local tty = nio.serial_open('/dev/ttyUSB0', 115200)
local epoll = nio.epoll_create()
nio.epoll_add(epoll, tty, bit.bor(const.EPOLLET, const.EPOLLIN))
nio.epoll_del(epoll, tty)
</pre>

### nio.epoll_wait
<pre>
等待epoll事件，返回事件所对应的事件数组，最大事件个数为64, 超时则返回nil

<b>语法</b>
nio.epoll_wait(epoll, timeout)

<b>timeout</b>
毫秒
-1: 永久

<b>返回值成员格式</b>
{fd, event}

<b>示例</b>
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
</pre>

### nio.tcp
<pre>
创建一个TCP socket文件描述符

<b>语法</b>
nio.tcp()

<b>示例</b>
local nio = require('nio')

local sock = nio.tcp()
</pre>

### nio.connect
<pre>
建立一个TCP连接，并返回连接结果

<b>语法</b>
nio.connect(host, port)

<b>示例</b>
local nio = require('nio')

local sock = nio.tcp()
local ret = nio.connect('www.google.cn', 80)
</pre>