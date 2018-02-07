# rb_avl_comparison
### Comparing RB and AVL implementations

#### RB tree:

"We developed a new red-black tree implementation that has the same low memory overhead (two pointer fields per node), but is approximately 30% faster for insertion/removal."

https://www.facebook.com/notes/facebook-engineering/scalable-memory-allocation-using-jemalloc/480222803919/

#### AVL tree:

http://neil.brown.name/blog/AVL - after a small modification to reduce memory hits

### 10000000 nodes test

CPU  E2180  @ 2.00GHz

Target: x86_64-redhat-linux
gcc version 5.3.1 20160406 (Red Hat 5.3.1-6) (GCC)

sudo nice -n -20 ./main

  tree_insert time: 19.4895 seconds  
    RB depth: 22.1559  
  tree_search time: 17.764 seconds  
  alternative tree_search time: 19.4725 seconds  
  tree_remove time: 23.3087 seconds  

  avl_insert time: 18.5318 seconds  
    AVL depth: 21.7497  
  avl_find time: 17.5718 seconds  
  alternative avl_find time: 19.446 seconds  
  avl_delete time: 22.9048 seconds  

CPU i5-6600 @ 3.30GHz

Target: x86_64-linux-gnu
gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.6)

sudo nice -n -20 ./main

  tree_insert time: 10.375 seconds  
    RB depth: 22.1559  
  tree_search time: 9.85938 seconds  
  alternative tree_search time: 10.9844 seconds  
  tree_remove time: 12.0156 seconds  

  avl_insert time: 9.89062 seconds  
    AVL depth: 21.7497  
  avl_find time: 7.84375 seconds  
  alternative avl_find time: 7.67188 seconds  
  avl_delete time: 11.5781 seconds  
