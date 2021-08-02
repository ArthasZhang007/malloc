#include <buffer.hpp>
bool isvalid(buffer *x){
    return (x && x->len() && x->getroot());
}
void buffer::destroy(bool clean = false) { destroy(root, clean); }
void buffer::destroy(block *x, bool clean = false)
{
    if (!x)
        return;
    if (clean)
        memset(x, 0, size(x));
    destroy(lc(x));
    destroy(rc(x));
    delete x;
}

buffer::buffer()
{
    Len = 0;
    root = small_head = NULL;
    payload = NULL;
    Count = 0;
}
buffer::~buffer()
{
    delete[] payload;
    //destroy();
}
buffer::buffer(size_t sz)
{
    if(sz < dword * 2){
        std::cout<<"buffer size too small!"<<std::endl;
        buffer();
        return ;
    }
    Len = sz;
    payload = new char[sz];
    new_free_block((void *)payload, sz);
    root = (block *)payload;
    small_head = NULL;
    Count = 1;
}

