#include <buffer.cpp>
inline bool buffer::isl(block *x)
{
    block *y = x->next;
    return !y || !keycomp(y, x);
}
inline bool buffer::isr(block *x) { return !isl(x); }
inline block *buffer::fa(block *x)
{
    if (!x || x->next == NULL)
        return NULL;
    return (isr(x) || x->next->prev == x) ? x->next : x->next->next;
}
inline block *buffer::lc(block *x)
{
    if (!x || !(x->prev))
        return NULL;
    block *y = x->prev;
    return (isl(y) && y != x) ? y : NULL;
}
inline block *buffer::rc(block *x)
{
    if (!x || !(x->prev))
        return NULL;
    block *y = x->prev;
    return isr(y) ? y : (y->next != x ? y->next : NULL);
}
inline void buffer::pushup(block *x)
{
    if (!x)
        return;
    if (!lc(x) && rc(x))
        x->prev->next = x;
    if (lc(x) && !rc(x))
        x->prev->next = x;
}

inline void buffer::ldel(block *x)
{
    if (!x || !lc(x))
        return;
    block *xl = lc(x), *xr = rc(x);
    xl->next = NULL;
    if (!xr)
        x->prev = NULL;
    else
    {
        x->prev = xr;
        xr->next = x;
    }
    ASSERT(!lc(x))
}
inline void buffer::rdel(block *x)
{
    if (!x || !rc(x))
        return;
    block *xl = lc(x), *xr = rc(x);
    xr->next = NULL;
    x->prev = xl;
    if (xl)
        xl->next = x;
    ASSERT(!rc(x))
}
inline void buffer::fadel(block *x)
{
    if (!x)
        return;
    block *f = fa(x);
    if (f)
        lc(f) == x ? ldel(f) : rdel(f);
    ASSERT(!fa(x))
}
inline void buffer::inl(block *x, block *t)
{
    if (!x || !t)
        return;
    block *xr = rc(x);
    x->prev = t;
    if (!xr)
        t->next = x;
    else
    {
        t->next = xr;
        xr->next = x;
    }
    ASSERT(lc(x) == t)
    ASSERT(fa(t) == x)
}
inline void buffer::inr(block *x, block *t)
{
    if (!x || !t)
        return;
    block *xl = lc(x);
    t->next = x;
    !xl ? x->prev = t : xl->next = t;
    ASSERT(rc(x) == t)
    ASSERT(fa(t) == x)
}
inline void buffer::lset(block *x, block *t)
{
    fadel(t);
    ldel(x);
    inl(x, t);
}
inline void buffer::rset(block *x, block *t)
{
    fadel(t);
    rdel(x);
    inr(x, t);
}
inline void buffer::reset(block *x)
{
    fadel(x);
    ldel(x);
    rdel(x);
}
bool buffer::checkintegrity(block *x)
{
    if (!x)
        return true;
    block *y = fa(x);
    if (y)
    {
        if (isl(x) && (size(y) < size(x) || lc(y) != x))
            return false;
        if (isr(x) && (size(y) > size(x) || rc(y) != x))
            return false;
    }
    return checkintegrity(lc(x)) && checkintegrity(rc(x));
}
bool buffer::checkintegrity() { return checkintegrity(root); }