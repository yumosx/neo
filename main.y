//输出对应的参数信息
probe sys_enter_execve {
  out("%s", "hello, world");
}

//输出pid, cpu, comm
probe sys_exit_read {
  out("comm: %s pid: %d cpu: %d", comm(), pid(), cpu());
}

//输出跟踪函数参数, 第一个参数
probe sys_enter_execve {
  out("arg:%s", arg(1));
}

//输出stack信息
probe sys_enter_execve {
  out("%d", stack());
}

//条件表达式过滤
probe sys_enter_execve /pid() == 134/ {
  out("%d", pid());
}

//每次加上对应的retarg
probe sys_exit_read/retarg/ {
  //类似于java的方法调用
  map[comm()] |> add(retarg) |> hist();
}

//计算每一个线程执行函数的次数, 并输出结果值
probe sys_enter_execve {
  map[comm()] |> add(1) |> hist();
}

