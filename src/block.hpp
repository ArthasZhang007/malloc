/*
Authored by Arthas Zhang
*/
#include <bits/stdc++.h>
#ifdef DEBUG
#define ASSERT(x) assert(x);
#else
#define ASSERT(x)
#endif
const size_t size_mask = ~(0x07); // XXXXXXXX000

//const
#define BUFFERSIZE 1024
#define word 8 //sizeof(size_t)
#define dword (word<<1)

//basic info macro
#define size(x) ((x)->header & size_mask) //包括header　block容量
#define sizeh(h) ((h)&size_mask)
#define rsize(x) (((x)->header & size_mask) - word) //不包括header　block真实容量
#define rsizeh(h) (((h)&size_mask) - word)

//internal jump
#define Footer(x) ((size_t *)((char *)(x) + rsize((block *)(x))))
#define Footer_To_Header(x) ((block *)((char *)(x) - rsizeh(*x)))
#define Header_To_Payload(x) ((void *)((char *)(x) + word))
#define Payload_To_Header(x) ((block *)((char *)(x) - word))

//information bits get macro
#define isalloc(x) ((x)->header & 1) //是否非空闲块
#define isfree(x) (!isalloc(x))　 //是否是空闲块
#define issmall(x) (size(x) == dword)
#define isprealloc(x) ((x)->header & 2) //前面的block是否alloc
#define ispresmall(x) ((x)->header & 4) //前面的block是否是小block

//information bits set macro
#define set_f(x,y)        (y ? (x)->header |= 1 : (x)->header &= (~1))
#define set_prealloc(x,y) (y ? (x)->header |= 2 : (x)->header &= (~2))
#define set_presmall(x,y) (y ? (x)->header |= 4 : (x)->header &= (~4))

#define keycomp(x,y) (size(x) == size(y) ? x < y : size(x) < size(y)) 
#define write_header(x,y) ((x)->header = y + ((x)->header & 0x07))
#define write_footer(x,y) (*(Footer(x)) = y + ((x)->header & 0x07))


/* header layout: address 1 byte = 8 bit -> 8 bytes = 64 bits
         a b c  d    
0x.......0 0 0  1

d : free 0 allocated 1
c : regular 0 small block 1
b : preregular 0 pre small 1
*/
struct block
{
    size_t header;
    union
    {
        struct
        {
            block *next;
            block *prev;
        };
        char payload[0];
    };
};
