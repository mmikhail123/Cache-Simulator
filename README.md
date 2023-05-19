# Cache-Simulator

An implementation of a cache simulator. 
• simulates only one level cache; i.e., an L1 cache. It prints out number of memory reads (per cache block), memory writes (per
cache block), cache hits, and cache misses
• includes a simulation of a write through cache
• supports two replacement algorithms: First In First Out (FIFO) and Least Recently Used (LRU)


The input parameters include the cache size, associativity, the replacement policy, the block size, and the trace file (file with addresses, specifying if it is a read or write). The cache size and block size are specified in number of bytes.

To compile, simply enter make into terminal. 
To run, input the following into terminal:
```
./L1cache <cachesize><assoc:n><cache policy><block size><trace file>
```
where:
- The parameter cache size is the total size of the cache in bytes. This number should be a
power of 2
- The parameter assoc:n specifies the associativity. Here, n is a number of cache lines in a set
- The parameter cache policy specifies the cache replacement policy, which is either fifo or
lru
- The parameter block size is a power of 2 that specifies the size of the cache block in bytes
- The parameter trace file is the name of the trace file (a sample trace file is included)

