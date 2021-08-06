#include <block.hpp>
#define diff_head(x) ((size_t)((char *)x - (char *)payload)) //relative distance from buffer head to the block head
#define diff_tail(x) (Len - diff_head(x) - size(x))          //relative distance from buffer tail to the block tail

class buffer //real heap implemented using 2 pointer splay(child sibiling)
{
public:
    //constructor and deconstructor
    buffer();
    ~buffer();
    buffer(size_t sz);

#define This ((buffer *)((char *)this - offsetof(buffer, base)))
    struct iterator
    {
        iterator() : m_ptr(This->root) {}
        iterator(block *ptr) : m_ptr(ptr) {}
        block &operator*() const { return *m_ptr; }
        block *operator->() { return m_ptr; }
        friend bool operator==(const iterator &a, const iterator &b) { return a.m_ptr == b.m_ptr; }
        friend bool operator!=(const iterator &a, const iterator &b) { return a.m_ptr != b.m_ptr; }
        iterator &operator--()
        {
            m_ptr = This->Prev(m_ptr);
            return *this;
        }
        iterator &operator++()
        {
            m_ptr = This->Next(m_ptr);
            return *this;
        }

    private:
        block *m_ptr;
    } base;

    //splay functions
    inline block *fa(block *x);           //return the father of node x
    inline block *lc(block *x);           //return the left child of node x
    inline block *rc(block *x);           //return the right child of node x
    inline void zig(block *x); //right rotate
    inline void zag(block *x); //left rotate
    void splay(block *x);      //splay x to the root
    bool find(block *x);       //find block x
    void insert(block *x);     //insert block x
    void remove(block *x);     //remove block x
    block *mini(block *x);     //minimum of subtree x
    block *maxi(block *x);     //maximum of subtree x
    block *mini();             //minimum of root x
    block *maxi();             //maximum of root x

    block *Begin();
    block *rBegin();
    block *End();
    block *Prev(block *x);
    block *Next(block *x);

    iterator begin() { return iterator(Begin()); }
    iterator rbegin() { return iterator(rBegin()); }
    iterator end() { return iterator(End()); }

    //can not implement(no augmenting space)
    size_t rank(size_t key);
    block *getkth(size_t k);

    //yet to be implemented
    std::pair<block *, block *> split(size_t key);
    void join(block *x, block *y);

    block *lower_bound(size_t key); // find the smallest block that size(block) >= key

    inline block *getroot() { return root; }
    inline block *setroot(block *x) { root = x; }

    //helper or debugging functions
    bool checksize(size_t sz);
    bool checkintegrity(block *x);                       //checkif tree x is valid binary search tree;
    bool checkintegrity();                               //checkif root is valid binary search tree;
    void print(block *x, size_t sz, int depth, bool p);  // print lc(x), rc(x) recursion tree
    void printh(block *x, size_t sz, int depth, bool p); // print x->prev, x->next recursion tree
    void print();
    void printh();
    void printblocks();
    void get_all_blocks(block *, std::vector<block *> &);
    void get_regular_blocks(block *, std::vector<block *> &);
    void get_small_blocks(block *, std::vector<block *> &);
    void get_all_blocks(std::vector<block *> &);
    void get_regular_blocks(std::vector<block *> &);
    void get_small_blocks(std::vector<block *> &);
    size_t get_depth();                                 //return depth of the splay
    size_t get_depth(block* x);                         //return depth of the splay

    //attribute functions
    size_t count() { return Count; }
    size_t len() { return Len; }
    size_t dis_head(block* x) {return diff_head(x);}
    size_t dis_tail(block* x) {return diff_tail(x);}

    //dynamic memory functions
    void *malloc(size_t sz);
    void free(void *p);
    void *realloc(void *p, size_t sz);
    void *calloc(size_t unit, size_t length);

    //clean up functions
    void destroy(bool clean);
    void destroy(block *x, bool clean);

private:
    block *root;       //free block tree root
    block *small_head; //head of small block
    size_t Len;        //length of buffer
    size_t Count;
    char *payload; //payload of buffer, base pointer

    //internal tree functions
    inline bool isl(block *x);            //whether x is a left node
    inline bool isr(block *x);            //whether x is a right node
    inline void ldel(block *x);           //delete the left child of node x
    inline void rdel(block *x);           //delete the right child of node x
    inline void fadel(block *x);          //delete the father of node x
    inline void inl(block *x, block *t);  // make t the left child of x(x must not have a left child)
    inline void inr(block *x, block *t);  // make t the right child of x(x must not have a right child)
    inline void lset(block *x, block *t); // make t the left child of x(cut their old connections)
    inline void rset(block *x, block *t); // make t the right child of x(cut their old connections)
    inline void reset(block *x);          //remove all connections of x

    //splay tree maintenance functions
    inline void pushup(block *x);   //update the information of node x
    inline void pushdown(block *x); //update the information of node x

    //malloc functions
    void new_free_block(void *p, size_t sz); //generate a new free block, update_info as well
    void split_block(block *x, size_t sz);   // split block x with size sz && allocate
    void insert_block(block *x);
    void delete_block(block *x);
    block *find_block(size_t sz);
    block *prev_block(block *x); //must ensure knowing the previous block is free(use isalloc to check)
    block *next_block(block *x);
    void update_info(block *x); //update presmall and prealloc bit of the next block of x
    void coalesce(block *x);    //coalesce adjacent blocks

    //probably unable to do
    void extend_heap();
    void shrink_heap();
};
