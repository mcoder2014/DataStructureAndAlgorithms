# 操作系统如何陷入内核态

内核态：当一个任务（进程）执行系统调用而陷入内核代码中执行时，我们就称进程处于内核运行态（或简称为内核态）。其他的属于用户态。用户程序运行在用户态,操作系统运行在内核态.（操作系统内核运行在内核态，而服务器运行在用户态）。用户态不能干扰内核态.所以CPU指令就有两种,特权指令和非特权指令.不同的状态对应不同的指令。特权指令：只能由操作系统内核部分使用，不允许用户直接使用的指令。如，I/O指令、置终端屏蔽指令、清内存、建存储保护、设置时钟指令（这几种记好，属于内核态）。 非特权指令：所有程序均可直接使用。

系统态（核心态、特态、管态）：执行全部指令。
用户态（常态、目态）：执行非特权指令。

## 用户态和内核态的转换

### 用户态切换到内核态的3种方式

1. 系统调用(直接系统调用、库函数、shell)
    > 这是用户态进程主动要求切换到内核态的一种方式，用户态进程通过系统调用申请使用操作系统提供的服务程序完成工作，比如前例中fork()实际上就是执行了一个创建新进程的系统调用。而系统调用的机制其核心还是使用了操作系统为用户特别开放的一个中断来实现，例如Linux的int 80h中断。
2. 异常(缺页、文件IO完成)
    > 当CPU在执行运行在用户态下的程序时，发生了某些事先不可知的异常，这时会触发由当前运行进程切换到处理此异常的内核相关程序中，也就转到了内核态，比如缺页异常。
3. 外围设备的中断
    > 当外围设备完成用户请求的操作后，会向CPU发出相应的中断信号，这时CPU会暂停执行下一条即将要执行的指令转而去执行与中断信号对应的处理程序，如果先前执行的指令是用户态下的程序，那么这个转换的过程自然也就发生了由用户态到内核态的切换。比如硬盘读写操作完成，系统会切换到硬盘读写的中断处理程序中执行后续操作等。