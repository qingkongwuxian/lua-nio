# 摘要
<pre>关于epoll，串口等操作的Lua封装</pre>

# 函数说明

### nio.open
<pre>
打开一个文件，返回文件描述符

<b>语法</b>
nio.open(file, flag)

<b>flag</b>
0: O_RDONLY 
1: O_WRONLY
2: O_RDWR

<b>示例</b>
local nio = require('nio')
local fd = nio.open('/tmp/foo', 1)
</pre>

### nio.set_non_blocking
<pre>
设置文件为非阻塞模式

<b>语法</b>
nio.set_non_blocking(fd)

<b>示例</b>
local nio = require('nio')
local fd = nio.open('/tmp/foo', 1)
nio.set_non_blocking(fd)
</pre>

### nio.read
<pre>
从文件中读取缓冲中的全部内容，返回为lua string
缓冲区无内容则返回nil

<b>语法</b>
nio.read(fd)

<b>示例</b>
local nio = require('nio')
local fd = nio.open('/tmp/foo', 1)
nio.set_non_blocking(fd)
local s = nio.read(fd)
</pre>

### nio.close
<pre>
关闭文件

<b>语法</b>
nio.close(fd)

<b>示例</b>
local nio = require('nio')
local fd = nio.open('/tmp/foo', 1)
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

### nio.serial_close
<pre>
关闭重置串口

<b>语法</b>
nio.serial_close(fd)

<b>示例</b>
local nio = require('nio')
local tty = nio.serial_open('/dev/ttyUSB0', 115200)
nio.serial_close(tty)
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
将文件描述符加入到epoll中，并设置事件类型，Edge Trigger模式

<b>语法</b>
nio.epoll_add(epoll, fd, event)

<b>event</b>
1: EPOLLIN
2: EPOLLOUT

<b>示例</b>
local nio = require('nio')
local tty = nio.serial_open('/dev/ttyUSB0', 115200)
nio.serial_close(tty)
local epoll = nio.epoll_create()
nio.epoll_add(epoll, tty, 1)
</pre>

### nio.epoll_wait
<pre>
等待epoll事件，返回事件所对应的文件描述符数组，最大事件个数为64
超时则返回nil

<b>语法</b>
nio.epoll_wait(epoll, timeout)

<b>timeout</b>
毫秒
-1: 永久

<b>示例</b>
local nio = require('nio')
local tty = nio.serial_open('/dev/ttyUSB0', 115200)
nio.serial_close(tty)
local epoll = nio.epoll_create()
nio.epoll_add(epoll, tty, 1)

while true do
  local fds = nio.epoll_wait(epoll, -1)
  if fds then
    for _, v in ipairs(fds) do
      local s = nio.read(v)
      if s then
        for i = 1, #s do
          io.write(bit.tohex(string.byte(s, i), 2) .. ' ')
        end
      end
    end
  end
end
</pre>