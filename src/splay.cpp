#include <two_pointer_tree.cpp>
inline void buffer::zig(block *x)
{
    if (!x)
        return;
    block *y = fa(x);
    block *z = y ? fa(y) : NULL;
    //block *t = y ? rc(y) : NULL;
    if (z)
        lc(z) == y ? lset(z, x) : rset(z, x);
    lset(y, rc(x));
    rset(x, y);
    //rset(y, t);pushup(y);pushup(x);
}
inline void buffer::zag(block *x)
{
    if (!x)
        return;
    block *y = fa(x);
    block *z = y ? fa(y) : NULL;
    if (z)
        lc(z) == y ? lset(z, x) : rset(z, x);
    rset(y, lc(x));
    lset(x, y);
    //pushup(y);pushup(x);rset(x, xr);lset(y, t);
}
void buffer::splay(block *x)
{
    if (!x)
        return;
    while (fa(x))
    {
        block *y = fa(x);
        block *z = y ? fa(y) : NULL;
        if (!fa(y))
        {
            lc(y) == x ? zig(x) : zag(x);
            break;
        }
        if (lc(z) == y)
            lc(y) == x ? (zig(y), zig(x)) : (zag(x), zig(x));
        else
            rc(y) == x ? (zag(y), zag(x)) : (zig(x), zag(x));
    }
    root = x;
}

void buffer::insert(block *x)
{
    if (!root)
    {
        root = x;
        return;
    }
    block *t = root;
    while (true)
    {
        if (keycomp(x, t))
        {
            if (!lc(t))
            {
                lset(t, x);
                break;
            }
            t = lc(t);
        }
        else
        {
            if (!rc(t))
            {
                rset(t, x);
                break;
            }
            t = rc(t);
        }
    }
    splay(x);
    if (x)
        Count++;
    ASSERT(checkintegrity())
}
bool buffer::find(block *x)
{
    if (!x)
        return false;
    block *t = root;
    while (t)
    {
        if (t == x)
            return true;
        t = keycomp(x, t) ? lc(t) : rc(t);
    }
    splay(x);
    return false;
}
void buffer::remove(block *x)
{
    if (!find(x))
        return;
    splay(x);
    block *y = maxi(lc(x));
    if (!y)
        root = rc(x);
    else
        splay(y);
    rset(y, rc(x));
    if (x)
        Count--;
    ASSERT(checkintegrity())
}
block *buffer::mini(block *x)
{
    if (!x)
        return NULL;
    while (lc(x))
        x = lc(x);
    splay(x);
    return x;
}
block *buffer::maxi(block *x)
{
    if (!x)
        return NULL;
    while (rc(x))
        x = rc(x);
    splay(x);
    return x;
}
block *buffer::mini() { return mini(root); }
block *buffer::maxi() { return maxi(root); }
block *buffer::Begin() { return mini(root); }
block *buffer::rBegin() { return maxi(root); }
block *buffer::End() { return NULL; }
block *buffer::Prev(block *x)
{
    if (!x)
        return NULL;
    block *y = maxi(lc(x)), *f = fa(x);
    if (y)
        return y;
    while (f)
    {
        if (keycomp(f, x))
            return f;
        f = fa(f);
    }
    return NULL;
}
block *buffer::Next(block *x)
{
    if (!x)
        return NULL;
    block *y = mini(rc(x)), *f = fa(x);
    if (y)
        return y;
    while (f)
    {
        if (keycomp(x, f))
            return f;
        f = fa(f);
    }
    return NULL;
}
block *buffer::lower_bound(size_t key) // find the smallest block that size(block) >= key
{
    block *x = root, *pa = NULL;
    while (x)
    {
        pa = x;
        x = key < size(x) ? lc(x) : rc(x);
    }
    splay(pa);
    return (pa && size(pa) < key) ? Next(pa) : pa;
}
void buffer::print(block *x, size_t sz = 0, int depth = 0, bool p = 0) // print lc(x), rc(x) recursion tree
{
    if (!x)
        return;
    if (sz > 0 && size(x) != sz)
        return;
    for (int i = 0; i < depth; i++)
        std::cout << "     ";
    std::cout << p << (isl(x) ? " L " : " R ") << x << ':' << size(x) << std::endl;
    print(lc(x), 0, depth + 1, 0);
    print(rc(x), 0, depth + 1, 1);
}
void buffer::printh(block *x, size_t sz = 0, int depth = 0, bool p = 0) // print x->prev, x->next recursion tree
{
    if (!x)
        return;
    if (sz > 0 && size(x) != sz)
        return;
    if (depth > 10)
        return;
    for (int i = 0; i < depth; i++)
        std::cout << "     ";
    std::cout << p << (isl(x) ? " l " : " r ") << x << ':' << size(x) << std::endl;
    printh(x->prev, 0, depth + 1, 0);
    printh(x->next, 0, depth + 1, 1);
}
void buffer::print() { print(root); }
void buffer::printh() { printh(root); }
size_t buffer::get_depth() { return get_depth(root); }
size_t buffer::get_depth(block *x) { return x ? std::max(get_depth(lc(x)), get_depth(rc(x))) + 1 : 0; }