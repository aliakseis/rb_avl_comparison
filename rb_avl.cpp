// rb_avl.cpp : Defines the entry point for the console application.
//

// Sources:
// http://www.canonware.com/rb/
// http://neil.brown.name/blog/AVL

#include <stdlib.h>
#include <time.h>

#if defined(_WIN64)
typedef __int64 ssize_t;
#else if defined(_WIN32)
typedef long ssize_t;
#endif

#include <stdint.h>
#include <stdbool.h>
//#define NDEBUG // (Optional, see assert(3).)
#include <assert.h>
#define RB_COMPACT // (Optional, embed color bits in right-child pointers.)

//#define ORIGINAL

#include <iostream>
#include <random>

#include "rb.h"

using std::cout;
using std::endl;


struct node_;

typedef struct node_ node_t;

struct node_ {
    rb_node(node_t) link;
    int key;
};


static int
node_cmp(const node_t *a, const node_t *b) {
    int ret = (a->key > b->key) - (a->key < b->key);
    if (ret == 0 && a != b) {
        cout << "Duplicates are not allowed in the tree" << endl;
        exit(1);
    }
    return (ret);
}


typedef rb_tree(node_t) tree_t;
rb_gen(static, tree_, tree_t, node_t, link, node_cmp);

//////////////////////////////////////////////////////////////////////////////

#ifdef ORIGINAL

typedef int value_t;

#define LEFT 0
#define RIGHT 1
#define NEITHER -1

typedef int direction;

typedef struct node_s {
    value_t        value;
    struct node_s *next[2];
    int            longer : 2;
} *node;
#define Balanced(n) (n->longer < 0)


node avl_find(node tree, value_t target)
{
    while (tree && target != tree->value) {
    	tree = tree->next[target > tree->value];
    }
    return tree;
}


static node avl_rotate_2(node *path_top, direction dir)
{
    node B, C, D, E;
    B = *path_top;
    D = B->next[dir];
    C = D->next[1 - dir];
    E = D->next[dir];

    *path_top = D;
    D->next[1 - dir] = B;
    B->next[dir] = C;
    B->longer = NEITHER;
    D->longer = NEITHER;
    return E;
}


static node avl_rotate_3(node *path_top, direction dir, direction third)
{
    node B, F, D, C, E;
    B = *path_top;
    F = B->next[dir];
    D = F->next[1 - dir];
    /* note: C and E can be NULL */
    C = D->next[1 - dir];
    E = D->next[dir];
    *path_top = D;
    D->next[1 - dir] = B;
    D->next[dir] = F;
    B->next[dir] = C;
    F->next[1 - dir] = E;
    D->longer = NEITHER;

    /* assume both trees are balanced */
    B->longer = F->longer = NEITHER;

    if (third == NEITHER)
        return NULL;
    else if (third == dir) {
        /* E holds the insertion so B is unbalanced */
        B->longer = 1 - dir;
        return E;
    }
    else {
        /* C holds the insertion so F is unbalanced */
        F->longer = dir;
        return C;
    }
}


/***************************************************
* INSERTION                                       *
***************************************************/
static inline void avl_rebalance_path(node path, value_t target)
{
    /* Each node in path is currently balanced.
    * Until we find target, mark each node as longer
    * in the direction of target because we know we have
    * inserted target there
    */
    while (path && target != path->value) {
        direction next_step = (target > path->value);
        path->longer = next_step;
        path = path->next[next_step];
    }
}

static inline void avl_rebalance_insert(node *path_top, value_t target)
{
    node path = *path_top;
    direction first, second, third;
    if (Balanced(path))
        ;
    else if (path->longer != (first = (target > path->value))) {
        /* took the shorter path */
        path->longer = NEITHER;
        path = path->next[first];
    }
    else if (first == (second = (target > path->next[first]->value))) {
        /* just a two-point rotate */
        path = avl_rotate_2(path_top, first);
    }
    else {
        /* fine details of the 3 point rotate depend on the third step.
        * However there may not be a third step, if the third point of the
        * rotation is the newly inserted point.  In that case we record
        * the third step as NEITHER
        */
        path = path->next[first]->next[second];
        if (target == path->value) third = NEITHER;
        else third = (target > path->value);
        path = avl_rotate_3(path_top, first, third);
    }
    avl_rebalance_path(path, target);
}

int avl_insert(node *treep, node target)
{
    /* insert the target into the tree, returning 1 on success or 0 if it
    * already existed
    */
    node tree = *treep;
    node *path_top = treep;
    while (tree /* && target != tree->value*/) {
        if (target->value == tree->value) {
            cout << "Duplicates are not allowed in the tree" << endl;
            exit(1);
        }
        if (!Balanced(tree)) path_top = treep;
        treep = &tree->next[target->value > tree->value];
        tree = *treep;
    }
    if (tree) 
        return 0;
    //tree = (node)malloc(sizeof(*tree));
    tree = target;
    tree->next[0] = tree->next[1] = NULL;
    tree->longer = NEITHER;
    //tree->value = target;
    *treep = tree;
    avl_rebalance_insert(path_top, target->value);
    return 1;
}

/******************************************************
* DELETION                                           *
*****************************************************/

static inline node avl_swap_del(node *targetp, node *treep, direction dir)
{
    node targetn = *targetp;
    node tree = *treep;

    *targetp = tree;
    *treep = tree->next[1 - dir];
    tree->next[LEFT] = targetn->next[LEFT];
    tree->next[RIGHT] = targetn->next[RIGHT];
    tree->longer = targetn->longer;

    //free(targetn);
    return targetn;
}

static inline node *avl_rebalance_del(node *treep, value_t target, node *targetp)
{
    /* each node from treep down towards target, but
    * excluding the last, will have a subtree grow
    * and need rebalancing
    */
    node targetn = *targetp;

    while (1) {
        const node tree = *treep;
        direction dir = (target > tree->value);

        if (tree->next[dir] == NULL)
            break;

        if (Balanced(tree))
            tree->longer = 1 - dir;
        else if (tree->longer == dir)
            tree->longer = NEITHER;
        else {
            int second = tree->next[1 - dir]->longer;
            if (second == dir)
                avl_rotate_3(treep, 1 - dir,
                tree->next[1 - dir]->next[dir]->longer);
            else if (second == NEITHER) {
                avl_rotate_2(treep, 1 - dir);
                tree->longer = 1 - dir;
                (*treep)->longer = dir;
            }
            else
                avl_rotate_2(treep, 1 - dir);
            if (tree == targetn)
                targetp = &(*treep)->next[dir];
        }

        treep = &tree->next[dir];
    }
    return targetp;
}


node avl_delete(node *treep, value_t target)
{
    /* delete the target from the tree, returning 1 on success or 0 if
    * it wasn't found
    */
    node tree = *treep;
    node *path_top = treep;
    node *targetp = NULL;
    direction dir;

    while (tree) {
        dir = (target > tree->value);
        if (target == tree->value)
            targetp = treep;
        if (tree->next[dir] == NULL)
            break;
        if (Balanced(tree)
            || (tree->longer == (1 - dir) && Balanced(tree->next[1 - dir]))
            ) path_top = treep;
        treep = &tree->next[dir];
        tree = *treep;
    }
    if (!targetp)
        return 0;

    /* adjust balance, but don't lose 'targetp' */
    targetp = avl_rebalance_del(path_top, target, targetp);

    /* We have re-balanced everything, it remains only to
    * swap the end of the path (*treep) with the deleted item
    * (*targetp)
    */
    return avl_swap_del(targetp, treep, dir);

    //return 1;
}

int dir_check_depth(node tree)
{
    if (tree) {
        int err = 0;
        int rv;
        int b = dir_check_depth(tree->next[LEFT]);
        int f = dir_check_depth(tree->next[RIGHT]);
        if (b == f) {
            if (!Balanced(tree))
                err = 1;
            rv = b + 1;
        }
        else if (b == f - 1) {
            if (tree->longer != RIGHT)
                err = 1;
            rv = f + 1;
        }
        else if (b - 1 == f) {
            if (tree->longer != LEFT)
                err = 1;
            rv = b + 1;
        }
        else {
            err = 1;
            rv = 0;
        }
        if (err)
            printf("err at %d: b=%d f=%d longer=%d\n", tree->value,
            b, f, tree->longer);
        return rv;
    }
    return 0;
}

#else

typedef int value_t;

enum {
    LEFT = 0,
    RIGHT = 1,
    NEITHER = 3,
};

typedef int direction;


struct node_s;

class node {
public:
    node() : m_node(NULL) {}
    explicit node(node_s* n) : m_node(n) {}

    node_s* operator ->() const { return as_node(); }
    explicit operator bool() const { return m_node != NULL; }
    int longer() const { return uintptr_t(m_node) & 3; }
    void set_longer(int longer)
    {
        assert(m_node != NULL);
        m_node = (node_s*)((intptr_t(m_node) & ssize_t(-4)) | longer);
    }
    bool balanced() const { return (uintptr_t(m_node) & 2) != 0; }

    bool operator == (const node& other) const { return as_node() == other.as_node(); }
    bool operator != (const node& other) const { return as_node() != other.as_node(); }

private:
    node_s* as_node() const { return (node_s*)(intptr_t(m_node) & ssize_t(-4)); }

    node_s* m_node;
};


struct node_s {
    value_t        value;
    node next[2];
};


node avl_find(node tree, value_t target)
{
    while (tree && target != tree->value) {
        tree = tree->next[target > tree->value];
    }
    return tree;
}

static node* avl_rotate_2(node *path_top, direction dir, int longer_ = NEITHER)
{
    node B, C, D;
    B = *path_top;
    D = B->next[dir];
    C = D->next[1 - dir];

    *path_top = D;

    path_top->set_longer(NEITHER);

    node* B_ = &(*path_top)->next[1 - dir];
    *B_ = B;
    B_->set_longer(longer_);//NEITHER;
    (*B_)->next[dir] = C;

    return &D->next[dir];
}


static node* avl_rotate_3(node *path_top, direction dir, direction third)
{
    node B, F, D, C, E;
    B = *path_top;
    F = B->next[dir];
    D = F->next[1 - dir];
    /* note: C and E can be NULL */
    C = D->next[1 - dir];
    E = D->next[dir];
    *path_top = D;

    node* B_ = &D->next[1 - dir];
    *B_ = B;
    node* F_ = &D->next[dir];
    *F_ = F;

    node* C_ = &B->next[dir];
    *C_ = C;
    node* E_ = &F->next[1 - dir];
    *E_ = E;

    path_top->set_longer(NEITHER);

    /* assume both trees are balanced */
    B_->set_longer(NEITHER);
    F_->set_longer(NEITHER);

    if (third == NEITHER) {
        static node null(NULL);
        return &null;
    }
    else if (third == dir) {
        /* E holds the insertion so B is unbalanced */
        B_->set_longer(1 - dir);
        return E_;
    }
    else {
        /* C holds the insertion so F is unbalanced */
        F_->set_longer(dir);
        return C_;
    }
}


/***************************************************
* INSERTION                                       *
***************************************************/

static inline void avl_rebalance_path(node* path, value_t target)
{
    /* Each node in path is currently balanced.
    * Until we find target, mark each node as longer
    * in the direction of target because we know we have
    * inserted target there
    */
    while ((*path) && target != (*path)->value) {
        direction next_step = (target > (*path)->value);
        path->set_longer(next_step);
        path = &(*path)->next[next_step];
    }
}

static inline void avl_rebalance_insert(node *path_top, value_t target)
{
    node* path = path_top;
    direction first, second, third;
    if (path->balanced())
        ;
    else if (path->longer() != (first = (target > (*path)->value))) {
        /* took the shorter path */
        path->set_longer(NEITHER);
        path = &(*path)->next[first];
    }
    else if (first == (second = (target > (*path)->next[first]->value))) {
        /* just a two-point rotate */
        path = avl_rotate_2(path_top, first);
    }
    else {
        /* fine details of the 3 point rotate depend on the third step.
        * However there may not be a third step, if the third point of the
        * rotation is the newly inserted point.  In that case we record
        * the third step as NEITHER
        */
        path = &(*path)->next[first]->next[second];
        if (target == (*path)->value) 
            third = NEITHER;
        else 
            third = (target > (*path)->value);
        path = avl_rotate_3(path_top, first, third);
    }
    avl_rebalance_path(path, target);
}



int avl_insert(node *treep, node target)
{
    /* insert the target into the tree, returning 1 on success or 0 if it
    * already existed
    */
    node *path_top = treep;
    while ((*treep) /* && target != tree->value*/) {
        if (target->value == (*treep)->value) {
            cout << "Duplicates are not allowed in the tree" << endl;
            exit(1);
        }
        if (!treep->balanced()) path_top = treep;
        treep = &(*treep)->next[target->value > (*treep)->value];
    }
    if (*treep)
        return 0;
    //tree = (node)malloc(sizeof(*tree));
    (*treep) = target;
    (*treep)->next[0] = node(NULL);
    (*treep)->next[1] = node(NULL);
    treep->set_longer(NEITHER);
    //tree->value = target;

    avl_rebalance_insert(path_top, target->value);
    return 1;
}

/******************************************************
* DELETION                                           *
*****************************************************/

static inline node avl_swap_del(node *targetp, node *treep, direction dir)
{
    node targetn = *targetp;
    node tree = *treep;

    *targetp = tree;
    *treep = tree->next[1 - dir];
    if (targetn != tree) {
        (*targetp)->next[LEFT] = targetn->next[LEFT];
        (*targetp)->next[RIGHT] = targetn->next[RIGHT];
        targetp->set_longer(targetn.longer());
    }

    //free(targetn);
    return targetn;
}


static inline node *avl_rebalance_del(node *treep, value_t target, node *targetp)
{
    /* each node from treep down towards target, but
    * excluding the last, will have a subtree grow
    * and need rebalancing
    */
    node targetn = *targetp;

    while (1) {
        const node tree = *treep;
        direction dir = (target > tree->value);

        if (!tree->next[dir])
            break;

        const int longer = tree.longer();
        if (longer == NEITHER)
            treep->set_longer(1 - dir);
        else if (longer == dir)
            treep->set_longer(NEITHER);
        else {
            const int second = tree->next[longer].longer();
            if (second == dir)
                avl_rotate_3(treep, longer,
                    tree->next[longer]->next[dir].longer());
            else if (second == NEITHER) {
                avl_rotate_2(treep, longer, longer);
                treep->set_longer(dir);
            }
            else
                avl_rotate_2(treep, longer);
            if (tree == targetn)
                targetp = &(*treep)->next[dir];
        }

        treep = &tree->next[dir];
    }
    return targetp;
}


node avl_delete(node *treep, value_t target)
{
    /* delete the target from the tree, returning 1 on success or 0 if
    * it wasn't found
    */
    node *path_top = treep;
    node *targetp = NULL;
    direction dir;

    while (*treep) {
        dir = (target > (*treep)->value);
        if (target == (*treep)->value)
            targetp = treep;
        if (!(*treep)->next[dir])
            break;
        const int longer = treep->longer();
        if (longer == NEITHER
            || (longer != dir && (*treep)->next[longer].balanced())
            ) path_top = treep;
        treep = &(*treep)->next[dir];
    }
    if (!targetp)
        return node(NULL);

    /* adjust balance, but don't lose 'targetp' */
    targetp = avl_rebalance_del(path_top, target, targetp);

    /* We have re-balanced everything, it remains only to
    * swap the end of the path (*treep) with the deleted item
    * (*targetp)
    */
    return avl_swap_del(targetp, treep, dir);

    //return 1;
}

int dir_check_depth(node tree)
{
    if (tree) {
        int err = 0;
        int rv;
        int b = dir_check_depth(tree->next[LEFT]);
        int f = dir_check_depth(tree->next[RIGHT]);
        if (b == f) {
            if (!tree.balanced())
                err = 1;
            rv = b + 1;
        }
        else if (b == f - 1) {
            if (tree.longer() != RIGHT)
                err = 1;
            rv = f + 1;
        }
        else if (b - 1 == f) {
            if (tree.longer() != LEFT)
                err = 1;
            rv = b + 1;
        }
        else {
            err = 1;
            rv = 0;
        }
        if (err)
            printf("err at %d: b=%d f=%d longer=%d\n", tree->value,
            b, f, tree.longer());
        return rv;
    }
    return 0;
}

#endif //ORIGINAL

//////////////////////////////////////////////////////////////////////////////

std::pair<unsigned int, unsigned long long> doGetGepth(node_t* tree, node_t* null, unsigned int level)
{
    if (tree == null)
        return std::make_pair(0u, 0ull);
    auto left(doGetGepth(rbtn_left_get(node_t, link, tree), null, level + 1)), 
        right(doGetGepth(rbtn_right_get(node_t, link, tree), null, level + 1));
    return{ 1u + left.first + right.first, left.second + right.second + level };
}

std::pair<unsigned int, unsigned long long> doGetGepth(node tree, node null, unsigned int level)
{
    if (tree == null)
        return std::make_pair(0u, 0ull);
    auto left(doGetGepth(tree->next[0], null, level + 1)), right(doGetGepth(tree->next[1], null, level + 1));
    return{ 1u + left.first + right.first, left.second + right.second + level };
}

template<typename T>
double getDepth(T tree, T null = static_cast<T>(0))
{
    auto depth(doGetGepth(tree, null, 0u));
    return depth.first? double(depth.second) / depth.first : 0;
}


enum { NNODES = 10000000 };


int main(int argc, char* argv[])
{
    // RB
    {
        node_t* nodes = new node_t[NNODES];

        std::default_random_engine dre;
        for (int i = 0; i < NNODES; ++i)
        {
            std::uniform_int_distribution<int> di(0, i);
            const int j = di(dre);
            if (i != j)
                nodes[i].key = nodes[j].key;
            nodes[j].key = i;
        }

        tree_t tree;
        tree_new(&tree);

        clock_t start = clock();

        for (int i = 0; i < NNODES; ++i)
        {
            tree_insert(&tree, nodes + i);
        }

        cout << "  tree_insert time: " <<
            (double)(clock() - start) / CLOCKS_PER_SEC <<
            " seconds" << endl;

        cout << "    RB depth: " << getDepth(tree.rbt_root) << endl;

        start = clock();

        for (int i = 0; i < NNODES; ++i)
        {
            if (!tree_search(&tree, nodes + i))
                cout << "Strange failure to tree_search " << nodes[i].key << '\n';
        }

        cout << "  tree_search time: " <<
            (double)(clock() - start) / CLOCKS_PER_SEC <<
            " seconds" << endl;

        start = clock();

        for (int i = 0; i < NNODES; ++i)
        {
            if (!tree_search(&tree, nodes + nodes[i].key))
                cout << "Strange failure to tree_search " << nodes[i].key << '\n';
        }

        cout << "  alternative tree_search time: " <<
            (double)(clock() - start) / CLOCKS_PER_SEC <<
            " seconds" << endl;

        start = clock();

        for (int i = 0; i < NNODES; ++i)
        {
            tree_remove(&tree, nodes + i);
        }

        cout << "  tree_remove time: " <<
            (double)(clock() - start) / CLOCKS_PER_SEC <<
            " seconds" << endl;

        delete[] nodes;
    }

    cout << endl;

    // AVL
    {
        node_s* nodes = new node_s[NNODES];

        std::default_random_engine dre;
        for (int i = 0; i < NNODES; ++i)
        {
            std::uniform_int_distribution<int> di(0, i);
            const int j = di(dre);
            if (i != j)
                nodes[i].value = nodes[j].value;
            nodes[j].value = i;
        }

        node tree(NULL);

        clock_t start = clock();

        for (int i = 0; i < NNODES; ++i)
        {
            avl_insert(&tree, node(nodes + i));
        }

        cout << "  avl_insert time: " <<
            (double)(clock() - start) / CLOCKS_PER_SEC <<
            " seconds" << endl;

        cout << "    AVL depth: " << getDepth(tree) << endl;

        start = clock();

        for (int i = 0; i < NNODES; ++i)
        {
            if (!avl_find(tree, nodes[i].value))
                cout << "Strange failure to avl_find " << nodes[i].value << '\n';
        }

        cout << "  avl_find time: " <<
            (double)(clock() - start) / CLOCKS_PER_SEC <<
            " seconds" << endl;

        start = clock();

        for (int i = 0; i < NNODES; ++i)
        {
            if (!avl_find(tree, nodes[nodes[i].value].value))
                cout << "Strange failure to avl_find " << nodes[i].value << '\n';
        }

        cout << "  alternative avl_find time: " <<
            (double)(clock() - start) / CLOCKS_PER_SEC <<
            " seconds" << endl;

        dir_check_depth(tree);

        start = clock();

        for (int i = 0; i < NNODES; ++i)
        {
            if (!avl_delete(&tree, nodes[i].value))
                cout << "Strange failure to avl_delete " << nodes[i].value << '\n';
        }

        cout << "  avl_delete time: " <<
            (double)(clock() - start) / CLOCKS_PER_SEC <<
            " seconds" << endl;

        //if (tree != NULL)
        if (tree)
            cout << "AVL tree is not empty!" << endl;

        delete[] nodes;
    }

    return 0;
}
