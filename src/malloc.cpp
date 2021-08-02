/*
Authored by Arthas Zhang
*/
#include <splay.cpp>
using namespace std;


void buffer::new_free_block(void *p, size_t sz)
{
    block *x = (block *)p;
    set_f(x, 0);
    x->next = NULL;
    write_header(x, sz);
    if (sz != dword)
    {
        x->prev = NULL;
        write_footer(x, sz);
    }
    update_info(x);
}
void buffer::insert_block(block *x)
{
    if (issmall(x))
    {
        x->next = small_head;
        small_head = x;
    }
    else
        insert(x);
}
void buffer::delete_block(block *x)
{
    if (issmall(x)) //worst O(n)
    {
        block *pre = NULL;
        for (block *cur = small_head; cur; cur = cur->next)
        {
            if (cur == x)
            {
                if (pre)
                    pre->next = cur->next;
                else
                    small_head = cur->next;
                break;
            }
            pre = cur;
        }
    }
    else //worst O(1)
        remove(x);
}
block *buffer::find_block(size_t sz)
{
    return sz == dword ? small_head : lower_bound(sz);
}
block *buffer::prev_block(block *x)
{
    if (!x || !diff_head(x))
        return NULL;
    if (ispresmall(x)) //regular block
        return (block *)((char *)x - dword);
    else
        return Footer_To_Header((size_t *)((char *)x - word));
}
block *buffer::next_block(block *x)
{
    if (!x || !diff_tail(x))
        return NULL;
    return (block *)((char *)x + size(x));
}
void buffer::split_block(block *x, size_t sz) // add allocated
{
    if (!x)
        return;
    ASSERT(size(x) >= sz)
    size_t y = size(x);
    if (y - sz >= dword)
    {
        char *new_block = (char *)x + sz;
        delete_block(x);
        write_header(x, sz);
        new_free_block((void *)new_block, y - sz);
        insert_block((block *)new_block);
    }
    else
        delete_block(x);
    update_info(x);
}
void *buffer::malloc(size_t sz)
{
    if (!sz)
    {
        printf("malloc %zu too small!\n", sz);
        return NULL;
    }
    block *ans = NULL;
    if (sz <= word)
    {
        sz = dword;
        ans = find_block(sz);
        if (!ans)
        {
            ans = mini();
            split_block(ans, sz);
        }
        else delete_block(ans);
    }
    else
    {
        sz = (sz + dword + word - 1) / dword * dword; //regular block
        ans = find_block(sz);
        split_block(ans, sz);
    }

    if (ans)
    {
        set_f(ans, 1); //allocated
        update_info(ans);
    }

    return ans ? Header_To_Payload(ans) : NULL;
}
void buffer::update_info(block *x)
{
    if (!x)
        return;
    block *r = next_block(x);
    if (!r)
        return;
    set_presmall(r, (size_t)issmall(x));
    set_prealloc(r, (size_t)isalloc(x));
}
void buffer::coalesce(block *x)
{
    if (!x)
        return;
    block *l = isprealloc(x) ? NULL : prev_block(x);
    block *r = next_block(x);
    block *fi = NULL;
    if (!r || isalloc(r))
        r = NULL;
    if (!l && !r)
    {
        fi = x;
        update_info(fi);
    }
    else if (l && r)
    {
        delete_block(l);
        delete_block(r);
        new_free_block(l, size(l) + size(x) + size(r));
        fi = l;
    }
    else if (l)
    {
        delete_block(l);
        new_free_block(l, size(l) + size(x));
        fi = l;
    }
    else if (r)
    {
        delete_block(r);
        new_free_block(x, size(x) + size(r));
        fi = x;
    }
    insert_block(fi);
}
void buffer::free(void *p)
{
    block *x = Payload_To_Header(p);
    set_f(x, 0);
    coalesce(x);
}

//helper function
void buffer::printblocks()
{
    vector<block *> regular_blockset;
    vector<block *> small_blockset;
    get_regular_blocks(root, regular_blockset);
    get_small_blocks(root, small_blockset);
    sort(regular_blockset.begin(), regular_blockset.end(),
         [](const block *block1, const block *block2)
         {
             return block1 < block2;
         });
    block *base = (block *)payload;
    printf("    In splay : .......\n");
    for (auto bl : regular_blockset)
        printf("        Address : %d %d, Size: %d \n", (int)diff_head(bl), (int)diff_tail(bl), (int)size(bl));
    printf("    In small block list : .......\n");
    for (auto bl : small_blockset)
        printf("        Address : %d %d, Size: %d \n", (int)diff_head(bl), (int)diff_tail(bl), (int)size(bl));
}
void buffer::get_regular_blocks(block *x, vector<block *> &blockset)
{
    if (!x)
        return;
    get_regular_blocks(lc(x), blockset);
    blockset.push_back(x);
    get_regular_blocks(rc(x), blockset);
}
void buffer::get_small_blocks(block *x, vector<block *> &blockset)
{
    if (!x)
        return;
    for (block *bl = small_head; bl; bl = bl->next)
        blockset.push_back(bl);
}
void buffer::get_all_blocks(block *x, vector<block *> &blockset)
{
    get_small_blocks(x, blockset);
    get_regular_blocks(x, blockset);
}
void buffer::get_regular_blocks(vector<block *> &blockset)
{
    get_regular_blocks(root, blockset);
}
void buffer::get_small_blocks(vector<block *> &blockset)
{
    get_small_blocks(root, blockset);
}
void buffer::get_all_blocks(vector<block *> &blockset)
{
    get_all_blocks(root, blockset);
}