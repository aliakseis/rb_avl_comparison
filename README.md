# rb_avl_comparison
### Comparing RB and AVL implementations

"I think I have discovered something that no one else has any idea about, and I’m not sure I can do it justice. Its scope is so broad that I can see only parts of it clearly at one time, and it is exceedingly difficult to set down comprehensibly in writing…" Jordan Peterson 

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

enum { NNODES = 20 * 10000000 };

sudo nice -n -20 ./main

  tree_insert time: 388.344 seconds  
    RB depth: 26.6402  
  tree_search time: 324.953 seconds  
  alternative tree_search time: 330.125 seconds  
  tree_remove time: 397.578 seconds  

  avl_insert time: 298.062 seconds  
    AVL depth: 26.0732  
  avl_find time: 264.625 seconds  
  alternative avl_find time: 267 seconds  
  avl_delete time: 399.719 seconds  

clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)

clang++ -Wall -O3 -std=c++11 rb_avl.cpp -o rb_avl  
sudo nice -n -20 ./rb_avl

  tree_insert time: 10.7969 seconds  
    RB depth: 22.1559  
  tree_search time: 10.125 seconds  
  alternative tree_search time: 11.0312 seconds  
  tree_remove time: 12.2969 seconds  

  avl_insert time: 10.6875 seconds  
    AVL depth: 21.7497  
  avl_find time: 5.64062 seconds  
  alternative avl_find time: 6.1875 seconds  
  avl_delete time: 12.0938 seconds  
