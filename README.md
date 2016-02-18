# 摘要
<pre>关于epoll，串口等操作的Lua封装</pre>

# 函数说明
<pre>
open(file, flag)
打开一个文件，返回文件描述符

<b>flag</b>
0: O_RDONLY 
1: O_WRONLY
2: O_RDWR

<b>示例</b>
local f = open('/tmp/foo', 1)
</pre>
