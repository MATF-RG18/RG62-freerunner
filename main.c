#include <stdlib.h>
#include <GL/glut.h>
#include <time.h>

static int x_coord = 5;
static int moguce_x_prepreke[] = {3,5,7};
static float x_prepreke = 5;
static float z_prepreke = 20;
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

    /*zemljani*/
    float mat_ambient[] ={ 0.19125f, 0.0735f, 0.0225f, 1.0f };
    float mat_diffuse[] ={0.7038f, 0.27048f, 0.0828f, 1.0f };
    float mat_specular[] ={0.256777f, 0.137622f, 0.086014f, 1.0f };
    float shine[] = {12.8};
    
    /*zeleni*/
    float mat_ambient1[] ={ 0.0f,0.05f,0.0f,1.0f };
    float mat_diffuse1[] ={ 0.4f,0.5f,0.4f,1.0f};
    float mat_specular1[] ={0.04f,0.7f,0.04f,1.0f };
    float shine1[] =  {10.0f};
        

    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*gluLookAt(5.0, 7.0, 5.0, 5.0, 5.0, 0.0, 0.0, 1.0, 0.0);*/
    gluLookAt(5.0,7.0,-5.0,5.0,5.0,0.0,0.0,1.0,0.0);


    /* Kreira se objekat. */
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, shine);
        glTranslatef(x_coord, 1, -1);
        glutSolidSphere(.5,40,40);
    glPopMatrix();

    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
        glMaterialfv(GL_FRONT, GL_SHININESS, shine1);
        glTranslatef(5,0,14); 
        glScalef(5,1,30);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, shine1);

        glTranslatef(x_prepreke,2,z_prepreke);
        glScalef(1,1,1);
        glutSolidCube(1);
    glPopMatrix();

    /*iscrtavaje staze*/
    glPushMatrix();
        glLineWidth(3);

        /*
        glBegin(GL_POLYGON);
            glVertex3f(1, 0, -2);
            glVertex3f(9, 0, -2);
            glVertex3f(9, 0, 50);
            glVertex3f(1, 0, 50);
        glEnd();
        */
        
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
    
    glPopMatrix();

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
        
        case 'S':
        case 's':
            if(!timer_active){
                glutTimerFunc(50,on_timer,0);
                timer_active = 1;
            };break;
        
        case 'p':
            timer_active = 0;break;
        case 'a':
        case 'A':
            if(x_coord != 7){
                x_coord += 2;
            }
                glutPostRedisplay();
            break;
            
        case 'd':
        case 'D':
            if(x_coord != 3){
                x_coord -= 2;
            }
                glutPostRedisplay();
            break;
        
    }
}

static void on_timer(int value)
{
    if(value)
        return;
    
    /*TODO simulacija kretanja prepreka prema objektu koji predstavlja igraca*/

    z_prepreke -= 1;
    if(z_prepreke < 0){
        z_prepreke = 20;
        x_prepreke = moguce_x_prepreke[time(NULL) % 3];
    }
    glutPostRedisplay();
    if(timer_active) {
        glutTimerFunc(50,on_timer,0);
    }
}

