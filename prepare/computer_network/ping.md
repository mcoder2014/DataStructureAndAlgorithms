# Ping 的过程

ping命令的工作原理是：向网络上的另一个主机系统发送ICMP报文，如果指定系统得到了报文，它将把报文一模一样地传回给发送者，这有点象潜水艇声纳系统中使用的发声装置。

ping 使用的 IP 层的 ICMP 协议。

## 通常情况

1. ping test.com 先向DNS查IP；
2. DNS返回IP；
3. 已知IP，在同一网段的话，使用ARP协议查找MAC物理地址，发送ARP请求；
4. 获得ARP答应；首先，交换机会收到这个报文后，交换机有学习MAC地址的功能，所以他会检索自己有没有保存主机B的MAC地址，如果有，就返回给主机A，如果没有，就会向所有端口发送ARP广播，其它主机收到后，发现不是在找自己，就纷纷丢弃了该报文，不去理会。直到主机B收到了报文后，就立即响应，我的MAC地址是多少，同时学到主机A的MAC地址，并按同样的ARP报文格式返回给主机A。
5. 再发送 ICMP 报文；
6. 接受报文；

## 不在同一网段的情况

1. 如果主机A要ping主机C，那么主机A发现主机C的IP和自己不是同一网段，他就去找网关转发，但是他也不知道网关的MAC地址情况下呢？他就会向之前那个步骤一样**先发送一个ARP广播，找到网关的MAC地址** ，再发封装ICMP报文给网关路由器。
2. 当路由器收到主机A发过来的ICMP报文，发现自己的目的地址是其本身MAC地址，**根据目的的IP2.1.1.1，查路由表，发现2.1.1.1/24的路由表项，得到一个出口指针，去掉原来的MAC头部，加上自己的MAC地址向主机C转发** 。
3. 最后，在主机C已找到路由器2端口MAC地址，路由器2端口转发给路由器1端口，路由1端口学到主机A的MAC地址的情况下，**他们就不需要再做ARP解析** ，就将ICMP的回显请求回复过来

## ICMP 协议

它是TCP/IP协议簇的一个子协议，用于在IP主机、路由器之间传递控制消息。控制消息是指网络通不通、主机是否可达、路由是否可用等网络本身的消息。
