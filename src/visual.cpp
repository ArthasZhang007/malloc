#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#define window_width 1024.0
#define window_height 768.0
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
#define setcolor(x) glColor3f(x.r, x.g, x.b)
void ptext(float x, float y, const char *string, i_color color = BLACK, void *font = GLUT_BITMAP_HELVETICA_18)
{
    setcolor(color);
    const char *c;
    glRasterPos2f(x, y);
    size_t cnt = 0;
    for (c = string; *c != '\0'; c++)
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
