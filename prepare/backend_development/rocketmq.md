# 消息队列

# RocketMQ

# 幂等性

# 有三个消息队列，随机的向三个队列插入元素(概率可能不同)，如何保证插入和查找的高效

(有两种思路，一种是加一个控制器，队列通知占用率，由控制器控制插入和查找，另一种是计算阻塞次数，再根据阻塞次数按比例控制该队列的插入频率和查找频率)