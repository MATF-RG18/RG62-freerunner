#include <stdlib.h>
#include <GL/glut.h>

static int x_coord = 5;
static int timer_active;

/*callback function declarations*/
static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_timer(int value);

int main(int argc, char **argv) {

    /*GLUT init*/
    glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

            /*creating window*/
            glutInitWindowSize(800, 600);
            glutInitWindowPosition(100, 100);
            glutCreateWindow("Free Runner");
            glutSetIconTitle("Free Runner");

    timer_active = 0;
    /*callback function init*/
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);

    /*OpenGL init*/
    glClearColor(0.7, 0.7, 0.7, 0);
            glEnable(GL_DEPTH_TEST);

    /*enter main loop*/
    glutMainLoop();

    return 0;
}

static void on_display(void)
{
    
    GLfloat light_position[] = { 0, 5, 0, 0};
    
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 };
    
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    
    GLfloat light_specular[] = { 0.1, 0.1, 0.1, 1 };
	
    /*ukljucuje se osvjetljenje i podesavaju parametri svetla*/
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    

    /* Brise se prethodni sadrzaj prozora. */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*gluLookAt(5.0, 7.0, 5.0, 5.0, 5.0, 0.0, 0.0, 1.0, 0.0);*/
    gluLookAt(5.0,7.0,-5.0,5.0,5.0,0.0,0.0,1.0,0.0);


    /* Kreira se objekat. */
    glPushMatrix();
        glColor3f(1, 0, 0);
        glTranslatef(x_coord, 0, -1);
        glutSolidSphere(.5,40,40);
    glPopMatrix();

    /*iscrtavaje staze*/
    glLineWidth(3);
    glBegin(GL_POLYGON);
    glVertex3f(1, 0, -2);
    glVertex3f(9, 0, -2);
    glVertex3f(9, 0, 50);
    glVertex3f(1, 0, 50);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(0, 0, 1);
    glVertex3f(0, 0, 9);
    glVertex3f(-60, 0, 1);
    glVertex3f(-60, 0, 9);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex3f(10, 0, 1);
    glVertex3f(10, 0, 9);
    glVertex3f(60, 0, 1);
    glVertex3f(60, 0, 9);
    glEnd();
    
    glBegin(GL_LINES);
            glColor3f(1,0,0);
            glVertex3f(0,0,0);
            glVertex3f(100,0,0);
            
            glColor3f(0,1,0);
            glVertex3f(0,0,0);
            glVertex3f(0,100,0);
            
            glColor3f(0,0,1);
            glVertex3f(0,0,0);
            glVertex3f(0,0,100);
            
    glEnd();
        
    /* Nova slika se salje na ekran. */
    glutSwapBuffers();
}

static void on_reshape(int width, int height)
{
    /* Podesava se viewport. */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /*gluPerspective(45, (float) width / height, 1, 40);*/
    glFrustum (-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
    glMatrixMode (GL_MODELVIEW);
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
    /* Zavrsava se program. */
        exit(0);
        break;
        
        case ' ':
            if(!timer_active){
                glutTimerFunc(50,on_timer,0);
                timer_active = 1;
            };break;
        
        case 'a':
        case 'A':
            if(x_coord != 7)
                x_coord += 2;
                glutPostRedisplay();
            break;
            
        case 'd':
        case 'D':
            if(x_coord != 3)
                x_coord -= 2;
                glutPostRedisplay();
            break;
        
    }
}

static void on_timer(int value)
{
    if(value)
        return;
    
    /*TODO simulacija kretanja prepreka prema objektu koji predstavlja igraca*/
}