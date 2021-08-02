#include <malloc.cpp>
#include <visual.cpp>
#include <iostream>
#include <sstream>

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
char cur_info[MAX_INFO_SIZE];
char input_info[MAX_INFO_SIZE];
//#ifdef OPENGL

void glUnfillrect(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}
void gledgerect(float x1, float y1, float x2, float y2, i_color color = WHITE, i_color bound_color = BLACK)
{
    setcolor(color);
    glRectf(x1, y1, x2, y2);
    setcolor(bound_color);
    glUnfillrect(x1, y1, x2, y2);
}
float transform_y(float x)
{
    x /= (float)buf->len();
    x = -(x - 0.5) * 2;
    return x;
}
void visualize_splay()
{
}
void visualize_help()
{
    float x = buffer_side + 0.05;
    float y = edge_side;
    float w = 0.2;
    float h = 0.05;
    float e = 0.01;
    float d = h * 1.4;
    gledgerect(x, y, x + w, y - h, RED, WHITE);
    ptext(x + w + e, y - h + e, "Regular Allocated Block", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    gledgerect(x, y, x + w, y - h, GREEN, WHITE);
    ptext(x + w + e, y - h + e, "Regular Free Block", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    gledgerect(x, y, x + w, y - h, BLUE, WHITE);
    ptext(x + w + e, y - h + e, "Small Allocated Block", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    gledgerect(x, y, x + w, y - h, LIGHT_BLUE, WHITE);
    ptext(x + w + e, y - h + e, "Small Free Block", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    ptext(x + e, y - h + e, "ibits(in header): ", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    ptext(x + e, y - h + e, "F/A : free | allocated", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    ptext(x + e, y - h + e, "P/N : previous block, allocated or not", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d * 2;

    ptext(x + e, y - h + e, "S/R : previous block, small or regular", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d * 2;

    ptext(x + e, y - h + e, "Input Sample Usage:", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    ptext(x + e, y - h + e, "n 512 (new buffer with size 512)", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    ptext(x + e, y - h + e, "m 25 (malloc with size 25)", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;

    ptext(x + e, y - h + e, "f 1 (free pointer ID 1)", WHITE, GLUT_BITMAP_9_BY_15);
    y -= d;
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
    glRectf(-buffer_side, -1, buffer_side, 1);

    //title
    setcolor(PINK);
    glRectf(-buffer_side * 2, 1.0 - top_note, -buffer_side, 1);

    ptext(-buffer_side * 1.75, 1.0 - top_note / 1.6, "Buffer", WHITE);

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
    sin >> op >> num;
    if (op == 'n')
    {
        if (isvalid(buf))
            delete buf;
        buf = new buffer(num);
        sprintf(cur_info, "new buffer with size %d\n", (int)buf->len());
    }
    else if ((op == 'm' || op == 'f') && buf)
    {
        if (op == 'm')
        {
            void *p = buf->malloc(num);
            if(!num){
                sprintf(cur_info, "can not malloc with size 0!\n");
            }
            if (p)
            {
                malloc_set[id] = p;
                sprintf(cur_info, "malloc with size %d,id = %d,adr = %p\n", num, id, malloc_set[id]);
                id++;
            }
            else
                sprintf(cur_info, "no more space availble!\n");
        }
        else if (op == 'f')
        {
            if (malloc_set.find(num) != malloc_set.end())
            {
                buf->free(malloc_set[num]);
                sprintf(cur_info, "free pointer id = %d\n", num);
                malloc_set.erase(num);
            }
            else
                sprintf(cur_info, "free unexisted pointer!\n");
        }
        buf->print();
        buf->printblocks();
    }
    else
    {
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