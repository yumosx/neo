//输出对应的参数信息
probe sys_enter_execve {
  out("%s", "hello, wrold");
}

//输出pid, cpu, comm
probe sys_enter_execv {
  out("comm: %s pid: %d cpu: %d", comm(), pid(), cpu());
}

//每次加上对应的retarg
probe sys_enter_execve/retarg/ {
  //类似于java的方法调用
  map[comm()] |> add(retarg) |> hist();
}

//计算每一个线程执行函数的次数, 并输出结果值
probe sys_enter_execve {
  map[comm()] |> add(1) |> hist();
}
