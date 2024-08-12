//输出对应的参数信息
probe sys_eneter_execve {
  out("%s", "hello, wrold");
}

//直接赋值
probe sys_exit_read /retarg/{
  //每次加上对应的返回值参数
  map[comm()] += retarg;
  map[comm()] |> hist();
}

//使用对应的方法
probe sys_enter_execve{
  map[comm()] |> count() |> hist();
}
