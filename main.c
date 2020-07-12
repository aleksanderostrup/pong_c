#include <GL/glut.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
 
void displayMe(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
        glVertex3f(0.8, 0.0, 0.5);
        glVertex3f(0.5, 0.0, 0.0);
        glVertex3f(0.0, 0.5, 0.0);
        glVertex3f(0.0, 0.0, 0.5);
    glEnd();
    glFlush();
}

void displayMe2(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
        glVertex3f(2.8, 0.0, 0.5);
        glVertex3f(0.5, 0.0, 0.0);
        glVertex3f(0.9, 0.5, 0.0);
        glVertex3f(0.0, 0.0, 0.5);
    glEnd();
    glFlush();
}


void dummyMouseClick(int button, int state, int x, int y)
{
    static bool switchDraw = true;
    if (GLUT_DOWN != state)
    {
        return;
    }
    char *butTxt;
    switch (button) 
    {
        case GLUT_LEFT_BUTTON:      butTxt = "Left";            break;
        case GLUT_MIDDLE_BUTTON:    butTxt = "Middle";          break;
        case GLUT_RIGHT_BUTTON:     butTxt = "Right";           break;
        case 3:                     butTxt = "Scroll up";       break;
        case 4:                     butTxt = "Scroll down";     break;
        case 5:                     butTxt = "Nudge left";      break;
        case 6:                     butTxt = "Nudge right";     break;
        default:                    butTxt = "Unknown";         break;
    }
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    printf("%s mouse button clicked (%u)\n", butTxt, button);
    if (switchDraw) 
    {
        displayMe2();
    }
    else 
    {
        displayMe();
    }
    switchDraw = !switchDraw;
}
 
int main(int argc, char** argv)
{
    printf("\nBinding mouse click\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(600, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hello world!");
    glutMouseFunc(dummyMouseClick);
    glutDisplayFunc(displayMe);
    glutMainLoop();
    return 0;
}
