-c 表示catch，表示获取对应的包
-e 表示equal, 判断当前的字节时候和后面传递的值一样

pass 表示通过
drop 表示丢弃
| 表示管道操作符号, 将上面一个结果值传递到下面一个位置

xdp -c u16@12 -e 0x800 | u8@57302 -e (0x1337 pass) (0xdead drop)
