#include <malloc.cpp>
#ifdef OPENGL
#include <visual.cpp>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
using namespace std;
#define backspace 8
#define enter 13
#define MAX_INFO_SIZE 256
std::stringstream ss_keystream;
buffer *buf;
map<int, void *> malloc_set;
int id = 0;
const float edge_side = 0.95;
const float buffer_width = 0.5;
const float buffer_side = buffer_width / 2.0;
const float top_note = 0.1;
const float tree_decay = 0.4;
const float global_x_diff = 0.2;
char cur_info[MAX_INFO_SIZE];
char input_info[MAX_INFO_SIZE];

vector<std::string> history;
void wait_sec(float sec)
{
    auto start = clock();
    while ((float)(clock() - start) / CLOCKS_PER_SEC < sec)
        ;
}

//#ifdef OPENGL

float transform_y(float x)
{
    x /= (float)buf->len();
    x = -(x - 0.5) * 2;
    return x;
}
void visualize_tree_node(block *t, float width, float x, float y, float depth, int dep, float rob)
{
    if (!t)
        return;
    block *l = buf->lc(t);
    block *r = buf->rc(t);
    float x_diff = width / 2.0;
    float y_diff = 0.125;
    float nlx = x - x_diff, nrx = x + x_diff, ny = y - y_diff;

    void *font = NULL;
    if (dep == 1)
    {
        font = GLUT_BITMAP_HELVETICA_18;
    }
    else if (dep <= 3)
    {
        font = GLUT_BITMAP_HELVETICA_12;
    }
    else
    {
        font = GLUT_BITMAP_HELVETICA_10;
    }
    setcolor(BROWN);
    if (l)
    {
        glbar(x, y, nlx, ny, rob, rob / 5, true);
    }
    if (r)
    {
        glbar(x, y, nrx, ny, rob, rob / 5, true);
    }

    ptext_with_box(x, y, std::to_string(size(t)).c_str(), WHITE, DARK_GREEN, font, true, true);
    rob *= 0.66667;
    visualize_tree_node(l, width * (tree_decay + depth), nlx, ny, depth * 0.7, dep + 1, rob);
    visualize_tree_node(r, width * (tree_decay + depth), nrx, ny, depth * 0.7, dep + 1, rob);
}
void visualize_splay()
{
    if (!buf)
        return;
    block *r = buf->getroot();
    setcolor(LIGHT_BROWN);
    glbar(-(buffer_side + 1) / 2.0 + 0.1, 0.80, -(buffer_side + 1) / 2.0 + 0.1, 0.46, 0.018, 0.0, true);
    ptext(-(buffer_side + 1) / 2.0 + 0.1, 0.70, "S", GREEN, GLUT_BITMAP_TIMES_ROMAN_24, true, true);
    ptext(-(buffer_side + 1) / 2.0 + 0.1, 0.65, "P", GREEN, GLUT_BITMAP_TIMES_ROMAN_24, true, true);
    ptext(-(buffer_side + 1) / 2.0 + 0.1, 0.60, "L", GREEN, GLUT_BITMAP_TIMES_ROMAN_24, true, true);
    ptext(-(buffer_side + 1) / 2.0 + 0.1, 0.55, "A", GREEN, GLUT_BITMAP_TIMES_ROMAN_24, true, true);
    ptext(-(buffer_side + 1) / 2.0 + 0.1, 0.50, "Y", GREEN, GLUT_BITMAP_TIMES_ROMAN_24, true, true);
    visualize_tree_node(r, (1.0 - buffer_side) / 2.0 + 0.01, -(buffer_side + 1) / 2.0 + 0.1, 0.45, 0.2, 1, 0.015);
}
void visualize_help()
{
    float x = buffer_side + 0.05;
    float y = edge_side;
    float w = 0.2;
    float h = 0.05;
    float e = 0.01;
    float d = h * 1.4;

    auto funB = [&x, &y, &w, &h, &e, &d](const char *message)
    {
        ptext(x + e, y - h + e, message, WHITE, GLUT_BITMAP_9_BY_15);
        y -= d;
    }; 
    auto funC = [&x, &y, &w, &h, &e, &d, &funB](const char *message, i_color color)
    {
        gledgerect(x, y, x + w, y - h, color, WHITE);
        ptext(x + w + e, y - h + e, message, WHITE, GLUT_BITMAP_9_BY_15);
        y -= d;
    };
    funC("Regular Allocated Block", RED);
    funC("Regular Free Block", GREEN);
    funC("Small Allocated Block", BLUE);
    funC("Small Free Block", LIGHT_BLUE);
    funB("ibits(in header): ");
    funB("F/A : free | allocated");
    funB("P/N : previous block, allocated or not");
    y -= d;
    funB("S/R : previous block, small or regular");
    y -= d;

    funB("Input Sample Usage:");
    funB("n 512 (new buffer with size 512)");
    funB("m 25 (malloc with size 25)");
    funB("f 1 (free pointer ID 1)");
    funB("s $str$ (save trace to (default),../history.txt)");
    y -= d;
    funB("l $str$ (load trace to (default),../history.txt)");
    y -= d;
    funB("u (compute and display util)");
    funB("h" "calculate throughput");
}
void visualize_block(block *x, int id = 0)
{
    float top = (float)(buf->dis_head(x));
    float bottom = top + (float)size(x);

    top = transform_y(top);
    bottom = transform_y(bottom);

    float mid_y = (top + bottom) / 2.0;

    i_color block_color;
    if (issmall(x) && isalloc(x))
        block_color = BLUE;
    if (!issmall(x) && isalloc(x))
        block_color = RED;
    if (issmall(x) && !isalloc(x))
        block_color = LIGHT_BLUE;
    if (!issmall(x) && !isalloc(x))
        block_color = GREEN;

    //block
    gledgerect(-buffer_side, bottom, buffer_side, top, block_color);

    ptext(-edge_side, edge_side, cur_info, GREEN, GLUT_BITMAP_9_BY_15);

    //cur_info
    auto info = std::to_string(size(x));
    if (isalloc(x))
        info += "    ID:   " + std::to_string(id);
    else
        info += "             ";
    info += "         ";
    char i_presmall = ispresmall(x) ? 'S' : 'R';
    char i_prealloc = isprealloc(x) ? 'P' : 'N';
    char i_alloc = isalloc(x) ? 'A' : 'F';
    info = info + i_presmall + i_prealloc + i_alloc;
    float dis;
    void *font;
    if (buf->len() <= 768)
    {
        dis = 0.015;
        font = GLUT_BITMAP_HELVETICA_18;
    }
    else if (buf->len() <= 1536)
    {
        dis = 0.010;
        font = GLUT_BITMAP_HELVETICA_12;
    }
    else
    {
        dis = 0.005;
        font = GLUT_BITMAP_HELVETICA_10;
    }
    ptext(-buffer_side / 1.2, mid_y - dis, info.c_str(), WHITE, font);
}
void displayMe()
{
    glClear(GL_COLOR_BUFFER_BIT);
    //space
    setcolor(WHITE);
    glrectf(-buffer_side, -1, buffer_side, 1);

    //title
    setcolor(PINK);
    glrectf(-buffer_side * 2, 1.0 - top_note, -buffer_side, 1);

    ptext(-buffer_side * 1.75, 1.0 - top_note / 1.6, "Buffer Layout", WHITE);

    if (buf)
    {
        vector<block *> block_set;
        buf->get_all_blocks(block_set);
        for (auto x : block_set) //free_block
            visualize_block(x);
        for (auto t : malloc_set)
        { //allocated_block
            block *x = Payload_To_Header(t.second);
            visualize_block(x, t.first);
        }
    }
    //help
    visualize_help();

    //splay
    visualize_splay();

    //input info
    std::string real_input = "In:," + std::string(input_info) + '_';
    ptext(-edge_side, 0.75, real_input.c_str(), WHITE, GLUT_BITMAP_9_BY_15);

    glFlush();
}
#ifdef OPENGL
void handle(std::stringstream &sin)
{
#else
void handle(std::istream &sin)
{
#endif
    char op;
    int num;
    std::string str, content;
    void *ptr = NULL;
    history.push_back(sin.str());
    sin >> op;
    switch (op)
    {
    case 't': //read from exist trace
    {
        sin >> str;
        if (str == "")
            str = "../history.txt";
        sprintf(cur_info, "load trace from file %s\n", str.c_str());

        std::ifstream ifs(str.c_str());
        while (!ifs.eof() && getline(ifs, content)) //&& cnt)
        {
            std::stringstream tr(content);
            handle(tr);
        }

        break;
    }
    case 'n': //new buffer
        sin >> num;
        if (isvalid(buf))
        {
            delete buf;
            malloc_set.clear();
        }
        buf = new buffer(num);
        sprintf(cur_info, "new buffer with size %d\n", (int)buf->len());
        break;
    case 'm': // malloc
        sin >> num;
        if (buf)
        {
            ptr = buf->malloc(num);
            if (!num)
                sprintf(cur_info, "can not malloc with size 0!\n");
            if (ptr)
            {
                malloc_set[id] = ptr;
                sprintf(cur_info, "malloc with size %d,id = %d,adr = %p\n", num, id, malloc_set[id]);
                id++;
            }
            else
                sprintf(cur_info, "no more space availble!\n");
#ifdef DEBUG
            buf->print();
            buf->printblocks();
#endif
        }
        else
            sprintf(cur_info, "can not malloc without a buffer, try n -size first!\n");
        break;
    case 'f': //free
        sin >> num;
        if (malloc_set.find(num) != malloc_set.end())
        {
            buf->free(malloc_set[num]);
            sprintf(cur_info, "free pointer id = %d\n", num);
            malloc_set.erase(num);
        }
        else
            sprintf(cur_info, "free unexisted pointer!\n");
#ifdef DEBUG
        buf->print();
        buf->printblocks();
#endif
        break;
    case 'p': //print record history
    {
        ofstream ofs("../history.txt");
        if (!history.empty())
            history.pop_back();
        for (auto &his : history)
            ofs << his << std::endl;
        while (!history.empty())
            history.pop_back();
        break;
    }
    default:
        history.pop_back();
        sprintf(cur_info, "illegal input!\n");
    }
    printf("%s", cur_info);
}
void keyboard(unsigned char key, int x, int y)
{
    std::string st;
    char op;
    switch (key)
    {
    case backspace:
        st = ss_keystream.str();
        if (st.length())
            st.pop_back();
        ss_keystream.clear();
        ss_keystream.str("");
        ss_keystream << st;

        break;
    case enter:
        handle(ss_keystream);
        ss_keystream.str("");
        ss_keystream.clear();
        break;
    default:
        ss_keystream << key;
    }
    sprintf(input_info, ss_keystream.str().c_str());
    glutPostRedisplay();
}
int last_T = 0;
void animate()
{
    glutPostRedisplay();
}
void idle()
{
    int now_T = clock();
}
void initglut(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Malloc Interacter");
    glutDisplayFunc(displayMe);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
}
//#endif

//#endif
void test(int argc, char **argv)
{
    buf = NULL;
#ifdef OPENGL
    initglut(argc, argv);
#else
    while (true)
        handle(cin);
#endif
}
int main(int argc, char **argv)
{
    test(argc, argv);
    return 0;
};