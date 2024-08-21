//输出对应的参数信息
probe sys_enter_execve {
  out("%s", "hello, world");
}

//输出pid, cpu, comm
probe sys_exit_read {
  out("comm: %s pid: %d cpu: %d", comm(), pid(), cpu());
}

//输出跟踪函数的参数, arg(1, "128s");
probe sys_enter_execve {
  out("arg:%s", arg(1, "128s"));
}

//输出对应的栈信息, stack是存储在map中的
probe sys_enter_execve {
  out("%d", stack());
}

//条件表达式过滤, 指定特定的进程进行执行
probe sys_enter_execve /pid() == 134/ {
  out("%d", pid());
}

probe sys_enter_execve /comm() == "sh"/ {
  map[pid()] = arg(1, "123s");
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

//获取cpu上面的线程分配情况
probe sys_enter_execve {
  map[cpu()] |> count();
}

//基本的控制结构
probe sys_enter_execve {
  //变量
  a = arg(1);
  if (a > 1) {
     out("%d", a);
  }
}

//如果需要挂在到多个跟踪点上
probe(sys_enter_execve, sys_exit_execve) {
  out("%d\n", 1);
}

//BEGIN表达式, 用于输出对应的header信息
BEGIN {
    out("%-18s %-16s %-6s\n", "PID", "COMM", "FILE");
}

probe sys_enter_execve {
    out("%-18d %-16s %-6s\n", pid(), comm(), arg());
}
