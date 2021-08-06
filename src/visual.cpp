#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define window_width 1920.0
#define window_height 1080.0
struct i_color
{
    float r, g, b;
    i_color() { r = g = b = 0.0; }
    i_color(float R, float G, float B) : r(R), g(G), b(B) {}
};
#define BLACK i_color(0, 0, 0)
#define RED i_color(1, 0, 0)
#define GREEN i_color(0, 1, 0)
#define BLUE i_color(0, 0, 1)
#define WHITE i_color(1, 1, 1)
#define PINK i_color(1, 0, 1)
#define LIGHT_BLUE i_color(0, 1, 1)
#define DARK_GREEN i_color(0, 0.4, 0)
#define BROWN i_color(165.0 / 255, 42.0 / 255, 0)
#define LIGHT_BROWN i_color(181.0 / 255, 101.0 / 255, 29.0 / 255)
#define setcolor(x) glColor3f(x.r, x.g, x.b)

const float g_diff = 0.2;
void glrectf(float x1, float y1, float x2, float y2, bool global_diff = false)
{
    if (!global_diff)
    {
        x1 += g_diff;
        x2 += g_diff;
    }
    glRectf(x1, y1, x2, y2);
}

void glUnfillrect(float x1, float y1, float x2, float y2, bool global_diff = false)
{
    if (!global_diff)
    {
        x1 += g_diff;
        x2 += g_diff;
    }
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}
void gledgerect(float x1, float y1, float x2, float y2, i_color color = WHITE, i_color bound_color = BLACK, bool global_diff = false)
{
    if (!global_diff)
    {
        x1 += g_diff;
        x2 += g_diff;
    }
    setcolor(color);
    glRectf(x1, y1, x2, y2);
    setcolor(bound_color);
    glUnfillrect(x1, y1, x2, y2, true);
}

void ptext(float x, float y, const char *str, i_color color = BLACK, void *font = GLUT_BITMAP_HELVETICA_18, bool center = false, bool global_diff = false)
{
    if (!global_diff)
    {
        x += g_diff;
    }
    if (center) //居中
    {
        int sz = glutBitmapLength(font, (const unsigned char *)str);
        float diff = (float)sz / window_width;
        x -= diff;
    }

    setcolor(color);
    const char *c;
    glRasterPos2f(x, y);
    size_t cnt = 0;
    for (c = str; *c != '\0'; c++)
    {
        if (cnt % 40 == 39 || *c == ',')
        {
            cnt = 0;
            y -= 0.05;
            glRasterPos2f(x, y);
        }
        if (*c != '\n' && *c != ',')
            glutBitmapCharacter(font, *c);
        cnt++;
    }
}
void ptext_with_box(float x, float y, const char *str, i_color color = BLACK, i_color box_color = WHITE,
                    void *font = GLUT_BITMAP_HELVETICA_18, bool center = false, bool global_diff = false)
{
    if (!global_diff)
        x += g_diff;
    int h = glutBitmapLength(font, (const unsigned char *)str);
    int w = 0;
    for (const char *c = str; *c != '\0'; c++)
        w = std::max(w, glutBitmapWidth(font, *c));
    w = w * 3 / 2;
    if (center) //居中
    {

        float diff = (float)h / window_width;
        x -= diff;
    }
    //setcolor(box_color);
    gledgerect(x, y, x + h / window_width * 2, y + w / window_height * 2, box_color, DARK_GREEN, true);
    //glUnfillrect(x, y, x + h / window_width * 2, y + w / window_height * 2);

    setcolor(color);
    glRasterPos2f(x, y);
    size_t cnt = 0;
    for (const char *c = str; *c != '\0'; c++)
    {
        if (cnt % 40 == 39 || *c == ',')
        {
            cnt = 0;
            y -= 0.05;
            glRasterPos2f(x, y);
        }
        if (*c != '\n' && *c != ',')
            glutBitmapCharacter(font, *c);
        cnt++;
    }
}
void glbar(float x1, float y1, float x2, float y2, float r1, float r2 = 0, bool global_diff = false)
{
    if (!global_diff)
    {
        x1 += g_diff;
        x2 += g_diff;
    }
    float dx = x2 - x1;
    float dy = y2 - y1;

    float l = sqrt(dx * dx + dy * dy);
    float r = r2 / l;
    float rn = r1 / l;
    float nx = rn * dy;
    float ny = rn * (-dx);
    x1 += r * dx;
    y1 += r * dy;
    x2 -= r * dx;
    y2 -= r * dy;

    glBegin(GL_POLYGON);
    glVertex2f(x1 - nx, y1 - ny);
    glVertex2f(x1 + nx, y1 + ny);
    glVertex2f(x2 + nx, y2 + ny);
    glVertex2f(x2 - nx, y2 - ny);
    glEnd();
}
