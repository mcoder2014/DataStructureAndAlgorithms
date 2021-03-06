# MySQL（更多面试问题，欢迎关注微信公众号「帅地玩编程」）

## 不同引擎对比

### InnodDB

- 支持行锁
- 支持外键
- 支持在线热备份
- 支持事物
- 5.6之前不支持全文搜索

### MyISAM

- 支持全文索引：使用 match against来查找
- 支持空间数据索引

### memory

- 支持哈希索引

## 索引

### 前缀索引

- 例如对于一些字段值很大的，如TEXT，BLOB就可以使用前缀索引了

### 最左前缀原则

### 聚族索引：其实就是主键索引

### 二级索引：其实就是非主键索引

## 事物

### 四大特性

- 原子性
- 隔离性：隔离会导致出现脏读、幻读等问题

  - 读未提交
  - 读提交
  - 可重复读
  - 串行化

- 一致性
- 持久性

### 幻读

- 会出现数据不一致的情况
- 幻读是指一个事务前后两次当前读查询，后一次查询到了前一次没有的行，并且这一行是新插入的行
- 行锁只能锁已经有的行，却不能锁新插入的行，所以需要间隙锁的解决幻读问题

## 常用配置命令

### explain

### slow log：慢日记

- set long_query_time = 数值
- 子主题 2

### analyze table t：重新统计索引信息

### show index from table：重看索引基数

### show processlist：查看线程的情况

## 锁

### 全局锁

- 一般用于备份
- flush tables with read lock(FTWRL)，只读状态

### 表级锁

- 表锁： lock tables ... read/write
- 元数据锁（MDL）

### 行级锁

- 两阶段协议
- 解决死锁的方法

  - 超时释放
  - 检测-回滚

## 日记系统

### redo log

- InnodDB 特有的
- 记录的是这个数据页做了什么修改
- 更新操作时，先把记录写到redolog上，再更新内存。系统再空闲的时候把redoLog持久化到磁盘
- 循环写

### binlog

- server层，所有引擎都共用
- 记录的是一个完整的更新命令
- binlog可以追加

### 两阶段提交

### 为什么需要两个日记系统

## 性能相关

## 一些核心语句

## 常规面试题

### 一条sql语句是如何执行的

- 连接器->查询缓存->分析器（语法-词法分析）->优化器->执行器
