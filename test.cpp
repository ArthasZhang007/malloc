#include <malloc.cpp>
using namespace std;
int rand(int l, int r)
{
    return l + rand() % (r - l + 1);
}
void test() // for splay
{
    const int n = 9;
    size_t num[n] = {800, 640, 960, 520, 1080, 180, 2080, 2400, 720};
    buffer *bf[n];
    for (size_t i = 0; i < 1; i++)
    {
        bf[i] = new buffer(num[i]);
        if (i)
            bf[0]->insert(bf[i]->getroot());
        bf[0]->print();
    }
    std::cout << sizeof(buffer) << std::endl;
    auto b = *bf[0];
    for (auto it = b.begin(); it != b.end(); ++it)
    {
        cout << size(it) << endl;
    }
    b.printblocks();
    auto p = b.malloc(25);
    /*bf1->remove(bf4->root);
    bf1->print(bf1->root);
    bf1->remove(bf1->root);
    bf1->print(bf1->root);
    bf1->remove(bf7->root);
    bf1->print(bf1->root);
    bf1->remove(bf3->root);
    bf1->print(bf1->root);
    bf1->remove(bf5->root);
    bf1->print(bf1->root);
    bf1->remove(bf2->root);
    bf1->print(bf1->root);
    bf1->remove(bf6->root);
    bf1->print(bf1->root);
    bf1->remove(bf8->root);
    bf1->print(bf1->root);
    bf1->remove(bf9->root);
    bf1->print(bf1->root);
    bf1->remove(bf1->root);
    bf1->print(bf1->root);*/
}
template <typename T>
void test2() // for type
{
    for (int i = 0; i < 10; i++)
    {
        T *a = new T;
        std::cout << sizeof(T) << ' ' << (size_t)a << std::endl;
    }
}
void test3()
{ // basic malloc
    buffer B(1024);
    auto x = B.malloc(32);
    auto y = B.malloc(64);
    auto z = B.malloc(96);
    B.print();
    B.printblocks();
    //std::cout<<sizeof(buffer)<<std::endl;
}
using namespace std;
void test4(int argc, char **argv)
{ //self defined
    int n;
    map<int, void *> F;
    cin >> n;
    buffer B(n);
    int id = 0;
    B.printblocks();
#ifdef OPENGL
    initglut(argc, argv);
#endif
    while (true)
    {
        char a;
        int b;
        cin >> a;
        if (a != 'm' && a != 'f')
        {
            cout << "illegal input!" << endl;
            continue;
        }
        cin >> b;
        if (a == 'm')
        {
            F[id] = B.malloc(b);
            printf("malloc with size %d: id = %d, adr = %p\n", b, id, F[id]);
            id++;
        }
        else if (a == 'f')
        {
            B.free(F[b]);
            printf("free pointer id = %d", id);
        }
        B.printblocks();
    }
}
int main(int argc, char **argv)
{
    //test();
    //test3();
    test4(argc, argv);
    return 0;
};