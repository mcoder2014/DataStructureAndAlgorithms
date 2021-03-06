# Linux 操作系统相关

## 一些 ID的关系

- **实际用户 ID**： 也称真实用户 ID，是登录 Unix shell 所使用的用户 ID；
- **有效用户 ID EUID**： 定义了操作者的权限，有效用户 ID 是进程的属性，决定了该进程对文件的访问权限；一般情况下，进程的有效用户ID就被设成执行该进程的实际用户ID；
- **设置用户ID位 SUID**： 用于对外的权限的开发，作用是如何去修改有效用户ID。有效用户ID和有效用户组ID：进程用来决定我们对资源的访问权限。**一般情况下，有效用户ID等于实际用户ID，有效用户组ID等于实际用户组ID。当设置-用户-ID（SUID）位设置，则有效用户ID等于文件的所有者的uid，而不是实际用户ID**；同样，如果设置了设置-用户组-ID（SGID）位，则有效用户组ID等于文件所有者的gid，而不是实际用户组ID
- **文件所有者 ID**： 进程创建新文件时，使用的文件所有者ID是进程的有效用户ID；

### EXAMPLE

- Linux系统中某个可执行文件属于root并且有setid，当一个普通用户mike运行这个程序时，产生的进程的有效用户和实际用户分别是？
    > 有效用户是root，实际用户是mike。
- 一个进程如果没有SUID或SGID位，则euid=uid egid=gid，分别是运行这个程序的用户的uid和gid。例如kevin用户的uid和gid分别为204和202，foo用户的uid和gid为 200，201，kevin运行myfile程序形成的进程的euid=uid=204，egid=gid=202，内核根据这些值来判断进程对资源访问的限制，其实就是kevin用户对资源访问的权限，和foo没关系。
- 如果一个程序设置了SUID，则euid和egid变成被运行的程序的所有者的uid和gid，例如kevin用户运行myfile，euid=200，egid=201，uid=204，gid=202，则这个进程具有它的属主foo的资源访问权限。
- SUID的作用就是这样：让本来没有相应权限的用户运行这个程序时，可以访问他没有权限访问的资源。passwd就是一个很鲜明的例子。

## TCB PCB 线程控制块 进程控制块

为了描述控制进程的运行，系统中存放进程的管理和控制信息的数据结构称为进程控制块（PCB Process Control Block），它是进程实体的一部分，是操作系统中最重要的记录性数据结构。它是进程管理和控制的最重要的数据结构，每一个进程均有一个PCB，在创建进程时，建立PCB，伴随进程运行的全过程，直到进程撤消而撤消。

在Unix或类Unix系统中，**进程是由进程控制块，进程执行的程序，进程执行时所用数据，进程运行使用的工作区组成** 。其中进程控制块是最重要的一部分。
PCB一般包括：

1. 程序ID（PID、进程句柄）：它是唯一的，一个进程都必须对应一个PID。PID一般是整形数字
2. 特征信息：一般分系统进程、用户进程、或者内核进程等
3. 进程状态：运行、就绪、阻塞，表示进程现的运行情况
4. 优先级：表示获得CPU控制权的优先级大小
5. 通信信息：进程之间的通信关系的反映，由于操作系统会提供通信信道
6. 现场保护区：保护阻塞的进程用
7. 资源需求、分配控制信息
8. 进程实体信息，指明程序路径和名称，进程数据在物理内存还是在交换分区（分页）中
9. 其他信息：工作单位，工作区，文件信息等

线程控制块（Thread Control Block，TCB）是与进程的控制块（PCB）相似的子控制块，只是TCB中所保存的线程状态比PCB中保存少而已（当然可以说这个定义是浅显或错误的）。

## 守护进程、僵尸进程、孤儿进程，守护进程的作用是什么

- **僵尸进程** : **子进程退出后，父进程没有调用 `wait,waitpid` 回收子进程的状态信息**，那么子进程的进程描述符仍然保存在操作系统内。
- **孤儿进程** : **父进程退出后，子进程依然在运行，那么子进程会变成孤儿进程。**孤儿进程会被init进程收养（进程号 1）。
- **守护进程** : 是一种在后台执行的电脑程序。此类程序会被以进程的形式初始化。通常，守护进程没有任何存在的父进程（即PPID=1），且在UNIX系统进程层级中直接位于init之下。

系统通常在启动时开启很多**服务** ，一同引导守护进程。守护进程为对网络请求，硬件活动等进行响应，或其他通过某些任务对其他应用程序的请求进行回应提供支持。**也能够完成很多系统服务，能够对硬件进行配置（如在某些Linux系统上的devfsd），运行计划任务（例如cron），以及运行其他任务。**

## 一个文件被删除了，没有被释放是什么原因呢

可能是 硬链接，inode 的引用没有释放完毕。

## 弱类型、强类型、动态类型、静态类型语言的区别是什么

基础概念

1. Program Errors:`trapped errors`导致程序终止执行;`untrapped errors`出错后继续执行，但可能出现任意行为
2. Forbidden Behaviours 语言设计时，可以定义一组forbidden behaviors. 它必须包括所有untrapped errors, 但可能包含trapped errors.
3. Well behaved 如果程序执行不可能出现forbidden behaviors, 则为well behaved。
4. Ill behaved 否则为ill behaved...

- **强类型** ：如果一种语言的所有程序都是well behaved——即不可能出现forbidden behaviors，则该语言为strongly typed。偏向于不容忍隐式类型转换。譬如说haskell的int就不能变成double
- **弱类型** ：否则为weakly typed。比如C语言的缓冲区溢出，属于trapped errors，即属于forbidden behaviors..故C是弱类型。偏向于容忍隐式类型转换。譬如说C语言的int可以变成double
- **静态类型 statically** : 如果在编译时拒绝ill behaved程序，则是statically typed;编译的时候就知道每一个变量的类型，因为类型错误而不能做的事情是语法错误。
- **动态类型dynamiclly** : 如果在运行时拒绝ill behaviors, 则是dynamiclly typed。编译的时候不知道每一个变量的类型，因为类型错误而不能做的事情是运行时错误。譬如说你不能对一个数字a写a[10]当数组用。

### Example

- 无类型： 汇编
- 弱类型、静态类型 ： **C/C++**
- 弱类型、动态类型检查： Perl/PHP
- 强类型、静态类型检查 ：Java/C#
- 强类型、动态类型检查 ：Python, Scheme
- 静态显式类型 ：Java/C
- 静态隐式类型 ：Ocaml, Haskell

## Reference

1. [APUE](https://item.jd.com/12720738.html)
2. [鸟哥的 Linux 私房菜 第三版](https://item.jd.com/12443890.html)
