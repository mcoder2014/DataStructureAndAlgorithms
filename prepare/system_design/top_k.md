# Top K 问题

## 10亿个IP，取出频次出现最多的TOP10个IP

问题一：怎么在海量数据中找出重复次数最多的一个

算法思想：
方案1：先做hash，然后求模映射为小文件，求出每个小文件中重复次数最多的一个，并记录重复次数。

然后找出上一步求出的数据中重复次数最多的一个就是所求（如下）。

问题二：网站日志中记录了用户的IP，找出访问次数最多的IP。

算法思想：
IP地址最多有2^32=4G种取值可能，所以不能完全加载到内存中。
可以考虑分而治之的策略；

- **map**：按照IP地址的hash(IP)%1024值，将海量日志存储到1024个小文件中，每个小文件最多包含4M个IP地址。

- **reduce**：对于每个小文件，可以构建一个IP作为key，出现次数作为value的hash_map，并记录当前出现次数最多的1个IP地址。
有了1024个小文件中的出现次数最多的IP，我们就可以轻松得到总体上出现次数最多的IP

## 千万条搜索结果里找频率最高的

搜索引擎会通过日志文件把用户每次检索使用的所有检索串都记录下来，每个查询串的长度为1-255字节。假设目前有一千万个记录（这些查询串的重复度比较高，虽然总数是1千万，但如果除去重复后，不超过3百万个。一个查询串的重复度越高，说明查询它的用户越多，也就是越热门。），请你统计最热门的10个查询串，要求使用的内存不能超过1G。

典型的Top K算法，还是在这篇文章里头有所阐述。 文中，给出的最终算法是：

1. 第一步、先对这批海量数据预处理，在O（N）的时间内用Hash表完成排序；
2. 然后，第二步、借助堆这个数据结构，找出Top K，时间复杂度为N‘logK。 即，借助堆结构，我们可以在log量级的时间内查找和调整/移动。因此，维护一个K(该题目中是10)大小的小根堆，
3. 然后遍历300万的Query，分别和根元素进行对比所以，我们最终的时间复杂度是：O（N） + N'*O（logK），（N为1000万，N’为300万）。ok，更多，详情，请参考原文。

　　或者：采用trie树，关键字域存该查询串出现的次数，没有出现为0。最后用10个元素的最小推来对出现频率进行排序

## 有一个1G大小的一个文件，里面每一行是一个词，词的大小不超过16字节，内存限制大小是1M。返回频数最高的100个词

方案：顺序读文件中，对于每个词x，取hash(x)%5000，然后按照该值存到5000个小文件（记为x0,x1,...x4999）中。这样每个文件大概是200k左右。

　　如果其中的有的文件超过了1M大小，还可以按照类似的方法继续往下分，直到分解得到的小文件的大小都不超过1M。 对每个小文件，统计每个文件中出现的词以及相应的频率（可以采用trie树/hash_map等），并取出出现频率最大的100个词（可以用含100个结点的最小堆），并把100个词及相应的频率存入文件，这样又得到了5000个文件。下一步就是把这5000个文件进行归并（类似与归并排序）的过程了。

## 给定a、b两个文件，各存放50亿个url，每个url各占64字节，内存限制是4G，让你找出a、b文件共同的url

**方案1** : 可以估计每个文件安的大小为5G×64=320G，远远大于内存限制的4G。所以不可能将其完全加载到内存中处理。考虑采取分而治之的方法。

遍历文件a，对每个url求取hash(url)%1000，然后根据所取得的值将url分别存储到1000个小文件（记为a0,a1,...,a999）中。这样每个小文件的大约为300M。

遍历文件b，采取和a相同的方式将url分别存储到1000小文件（记为b0,b1,...,b999）。这样处理后，所有可能相同的url都在对应的小文件（a0vsb0,a1vsb1,...,a999vsb999）中，不对应的小文件不可能有相同的url。然后我们只要求出1000对小文件中相同的url即可。

求每对小文件中相同的url时，可以把其中一个小文件的url存储到hash_set中。然后遍历另一个小文件的每个url，看其是否在刚才构建的hash_set中，如果是，那么就是共同的url，存到文件里面就可以了。

**方案2** ：如果允许有一定的错误率，可以使用 ，4G内存大概可以表示340亿bit。将其中一个文件中的url使用Bloom filter映射为这340亿bit，然后挨个读取另外一个文件的url，检查是否与Bloom filter，如果是，那么该url应该是共同的url（注意会有一定的错误率）。

## 100w个数中找出最大的100个数

**方案1** ：在前面的题中，我们已经提到了，用一个含100个元素的最小堆完成。复杂度为O(100w*lg100)。

**方案2** ：采用快速排序的思想，每次分割之后只考虑比轴大的一部分，知道比轴大的一部分在比100多的时候，采用传统排序算法排序，取前100个。复杂度为O(100w*100)。

**方案3** ：采用局部淘汰法。选取前100个元素，并排序，记为序列L。然后一次扫描剩余的元素x，与排好序的100个元素中最小的元素比，如果比这个最小的要大，那么把这个最小的元素删除，并把x利用插入排序的思想，插入到序列L中。依次循环，知道扫描了所有的元素。复杂度为O(100w*100)。

## 方法总结

1. 布隆过滤器；
2. 利用哈希函数对数据规模进行分割；
3. 位图法；
4. 堆

## 参考

1. [十道海量数据处理面试题与十个方法总结](https://cloud.tencent.com/developer/article/1064552)
2. [从一亿个ip找出出现次数最多的IP](https://blog.csdn.net/qq_23100787/article/details/80392135)
