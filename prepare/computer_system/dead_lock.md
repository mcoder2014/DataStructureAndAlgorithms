# 进程同步的三个经典问题

## 进程同步

进程是并发执行的，不同的进程之间存在不同的相互制约关系。我们把异步环境下的一组并发进程**因直接制约而互相发送消息、进行互相合作、互相等待，使得各进程按一定的速度执行的过程**称为进程间的同步。

### 1. 生产者消费者问题

#### 问题描述

生产者消费者问题（英语：Producer-consumer problem），也称有限缓冲问题（英语：Bounded-buffer problem），是一个多进程同步问题的经典案例。

1. 该问题描述了共享固定大小缓冲区的两个进程——即所谓的“生产者”和“消费者”——在实际运行时会发生的问题。
2. 生产者的主要作用是生成一定量的数据放到缓冲区中，然后重复此过程。
3. 与此同时，消费者也在缓冲区消耗这些数据。
4. 该问题的关键就是要保证生产者不会在缓冲区满时加入数据，消费者也不会在缓冲区中空时消耗数据。

要解决该问题，就必须让生产者在缓冲区满时休眠（要么干脆就放弃数据），等到下次消费者消耗缓冲区中的数据的时候，生产者才能被唤醒，开始往缓冲区添加数据。同样，也可以让消费者在缓冲区空时进入休眠，等到生产者往缓冲区添加数据之后，再唤醒消费者。

通常采用进程间通信的方法解决该问题。如果解决方法不够完善，则容易出现死锁的情况。出现死锁时，两个线程都会陷入休眠，等待对方唤醒自己。该问题也能被推广到多个生产者和消费者的情形。

#### **使用信号量**的解决方法

为了同步生产者和消费者的行为，需要记录缓冲区中物品的数量。**数量可以使用信号量来进行统计**，这里需要使用两个信号量：**empty 记录空缓冲区的数量**，**full 记录满缓冲区的数量**。其中，empty 信号量是在生产者进程中使用，当 empty 不为 0 时，生产者才可以放入物品；full 信号量是在消费者进程中使用，当 full 信号量不为 0 时，消费者才可以取走物品。

**注意，不能先对缓冲区进行加锁，再测试信号量。也就是说，不能先执行 down(mutex) 再执行 down(empty)。**如果这么做了，那么可能会出现这种情况：生产者对缓冲区加锁后，执行 down(empty) 操作，发现 empty = 0，此时生产者睡眠。消费者不能进入临界区，因为生产者对缓冲区加锁了，消费者就无法执行 up(empty) 操作，empty 永远都为 0，导致生产者永远等待下，不会释放锁，消费者因此也会永远等待下去。

```c
#define N 100
typedef int semaphore;
semaphore mutex = 1;
semaphore empty = N;
semaphore full = 0;

void producer() {
    while(TRUE) {
        int item = produce_item();
        down(&empty);
        down(&mutex);
        insert_item(item);
        up(&mutex);
        up(&full);
    }
}

void consumer() {
    while(TRUE) {
        down(&full);
        down(&mutex);
        int item = remove_item();
        consume_item(item);
        up(&mutex);
        up(&empty);
    }
}
```

#### 使用条件变量解决生产者消费者问题

```cpp
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

#define CONSUMERS_COUNT 2
#define PRODUCERS_COUNT 1

pthread_mutex_t g_mutex;
pthread_cond_t g_cond;

pthread_t g_thread[CONSUMERS_COUNT + PRODUCERS_COUNT];

int nready = 0;

void *consume(void *arg)
{
    int num = (int)arg;
    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        while (nready == 0)
        {
            printf("%d begin wait a condtion ...\n", num);
            pthread_cond_wait(&g_cond, &g_mutex);
        }

        printf("%d end wait a condtion ...\n", num);
        printf("%d begin consume product ...\n", num);
        --nready;
        printf("%d end consume product ...\n", num);
        pthread_mutex_unlock(&g_mutex);
        sleep(1);
    }
    return NULL;
}

void *produce(void *arg)
{
    int num = (int)arg;
    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        printf("%d begin produce product ...\n", num);
        ++nready;
        printf("%d end produce product ...\n", num);
        pthread_cond_signal(&g_cond);
        printf("%d signal ...\n", num);
        pthread_mutex_unlock(&g_mutex);
        sleep(1);
    }
    return NULL;
}

int main(void)
{
    int i;

    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_cond, NULL);


    for (i = 0; i < CONSUMERS_COUNT; i++)
        pthread_create(&g_thread[i], NULL, consume, (void *)i);

    sleep(1);

    for (i = 0; i < PRODUCERS_COUNT; i++)
        pthread_create(&g_thread[CONSUMERS_COUNT + i], NULL, produce, (void *)i);

    for (i = 0; i < CONSUMERS_COUNT + PRODUCERS_COUNT; i++)
        pthread_join(g_thread[i], NULL);

    pthread_mutex_destroy(&g_mutex);
    pthread_cond_destroy(&g_cond);

    return 0;
}
```

### 2. 读者写者问题

允许多个进程同时对数据进行读操作，但是不允许读和写以及写和写操作同时发生。

一个整型变量 count 记录在对数据进行读操作的进程数量，一个互斥量 count_mutex 用于对 count 加锁，一个互斥量 data_mutex 用于对读写的数据加锁。

```c
typedef int semaphore;
semaphore count_mutex = 1;
semaphore data_mutex = 1;
int count = 0;

void reader() {
    while(TRUE) {
        down(&count_mutex);
        count++;

        // 第一个读者需要对数据进行加锁，防止写进程访问
        if(count == 1)
            down(&data_mutex);

        up(&count_mutex);
        read();
        down(&count_mutex);
        count--;

        // 最后一个结束阅读的读者，释放写锁
        if(count == 0)
            up(&data_mutex);
        up(&count_mutex);
    }
}

void writer() {
    while(TRUE) {
        down(&data_mutex);
        write();
        up(&data_mutex);
    }
}
```

#### 3. 哲学家就餐

五个哲学家围着一张圆桌，每个哲学家面前放着食物。哲学家的生活有两种交替活动：吃饭以及思考。当一个哲学家吃饭时，需要先拿起自己左右两边的两根筷子，并且一次只能拿起一根筷子。

下面是一种错误的解法，如果所有哲学家同时拿起左手边的筷子，那么所有哲学家都在等待其它哲学家吃完并释放自己手中的筷子，导致死锁。

为了防止死锁的发生，可以设置两个条件：

- 必须同时拿起左右两根筷子；
- 只有在两个邻居都没有进餐的情况下才允许进餐。

```c
#define N 5
#define LEFT (i + N - 1) % N // 左邻居
#define RIGHT (i + 1) % N    // 右邻居
#define THINKING 0
#define HUNGRY   1
#define EATING   2
typedef int semaphore;
int state[N];                // 跟踪每个哲学家的状态
semaphore mutex = 1;         // 临界区的互斥，临界区是 state 数组，对其修改需要互斥
semaphore s[N];              // 每个哲学家一个信号量

void philosopher(int i) {
    while(TRUE) {
        think(i);
        take_two(i);
        eat(i);
        put_two(i);
    }
}

void take_two(int i) {
    down(&mutex);
    state[i] = HUNGRY;
    check(i);
    up(&mutex);
    down(&s[i]); // 只有收到通知之后才可以开始吃，否则会一直等下去
}

void put_two(i) {
    down(&mutex);
    state[i] = THINKING;
    check(LEFT); // 尝试通知左右邻居，自己吃完了，你们可以开始吃了
    check(RIGHT);
    up(&mutex);
}

void eat(int i) {
    down(&mutex);
    state[i] = EATING;
    up(&mutex);
}

// 检查两个邻居是否都没有用餐，
// 如果是的话，就 up(&s[i])，使得 down(&s[i]) 能够得到通知并继续执行
void check(i) {
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] !=EATING) {
        state[i] = EATING;
        up(&s[i]);
    }
}
```

## 死锁

### 死锁的必要条件

1. **互斥条件** 进程要求的资源具有排他性；
2. **不可剥夺条件** 进程在使用完成前，资源不能被强行夺走；
3. **请求和保持条件** 进程占有自己当前拥有的资源，并请求其他的资源；
4. **循环等待条件** 存在一种进程资源的循环等待链。相互等待其他进程释放资源，同时自己又包含了其他进程请求的资源。

### 安全状态

定义了各个进程占有的资源数, 要请求的资源数, 系统剩余资源数. 从安全状态出发, 存在一个调用顺序,
使得所有进程都可以得到请求的资源数, 即所有进程都可以执行完.

### 银行家算法

银行家算法是一种避免死锁的著名算法。
基于安全状态的定义, 保证每一次资源分配之后, 系统都处于安全状态.

**背景**：在银行中，客户申请贷款的数量是有限的，每个客户在第一次申请贷款时要声明完成该项目所需的最大资金量，在满足所有贷款要求时，客户应及时归还。银行家在客户申请的贷款数量不超过自己拥有的最大值时，都应尽量满足客户的需要。在这样的描述中，银行家就好比操作系统，资金就是资源，客户就相当于要申请资源的进程。

算法的整体思路是：

1. 先判断手中的资源是否满足请求资源的进程的需求；
2. 尝试分配，然后检查当前状态是否是安全状态；
3. 如果不安全，则不分配资源（确保每一次分配资源都是安全的）；

```sh
P - 进程的集合
Mp - 进程p的最大的请求数目
Cp - 进程p当前被分配的资源
A - 当前可用的资源

while (P != ∅) {
    found = FALSE;
    foreach (p ∈ P) {
        if (Mp − Cp ≤ A) {
             /* p可以獲得他所需的資源。假設他得到資源後執行；執行終止，並釋放所擁有的資源。*/
             A = A + Cp ;
             P = P − {p};
             found = TRUE;
        }
    }
    if (! found) return FAIL;
}
return OK;
```

### 死锁的解决方案

死锁的处理方法有 预防死锁、避免死锁 以及 死锁检测和解除三类。

#### 预防死锁

设置限制**破坏死锁的四个产生条件**：

- 破坏互斥条件：有些资源的特性无法破坏；
- 破坏不可剥夺条件：当一个已经保持了不可剥夺资源的进程请求新资源而得不到满足时，他必须释放已经保持的所有资源，等待之后需要时再次申请；
- 破坏请求和保持条件：采用预先静态分配方式，在进程开始运行前一次性申请完所有需要的资源，在他的资源未满足前，不把它投入运行。
- 破坏循环等待条件：采用顺序资源分配法，首先给出系统的资源编号，规定每个进程必须按照资源递增的顺序请求资源，同一资源一次性申请完。

#### 避免死锁

通过一些算法防止系统进入不安全状态，从而避免死锁。允许进程动态申请资源，但系统在分配资源前，应先计算这次分配资源的安全性。银行家算法。

#### 死锁的检测和解除

1. 检测死锁：利用资源分配图来检测死锁的存在；
2. 解除死锁：
    - 资源剥夺法：挂起某些死锁进程，并抢夺它的资源，使其他进程继续执行；
    - 撤销进程法：强制撤销部分、甚至全部死锁进程并剥夺他们的资源；
    - 进程回退法：让进程回退到足以避免死锁的地方。
