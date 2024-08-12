//输出对应的参数信息
probe sys_eneter_execve {
  out("%s", "hello, wrold");
}

//每次加上对应的retarg
probe sys_enter_execve/retarg/ {
  //类似于java的stream
  map[comm()] |> add(retarg) |> hist();
}

//计算每一个线程执行函数的次数, 并输出结果值
probe sys_enter_execve {
  map[comm()] |> add(1) |> hist();
}
