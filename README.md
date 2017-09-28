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
