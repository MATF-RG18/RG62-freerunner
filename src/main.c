#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>

#define timer_id1 0
#define timer_id2 1
#define PI 3.14


static int x_coord = 3;
static float y_coord = 1;
static int possible_x[] = {1,2,3,4,5};
static int possible_obs[] = {0,1,2};
static int timer_active;
static int timer_jump;
static float x_planeA = 3;
static float y_planeA = 0;
static float z_planeA = 30;
static float x_planeB = 3;
static float y_planeB = 0;
static float z_planeB = 60;
static int game_start = 0;

typedef struct{
    float x,y,z;
    int obst_type;
} Prepreka;

static Prepreka array_A[200];
static int size_A = 0;
static Prepreka array_B[200];
static int size_B = 0;

static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void set_planeA();
static void set_planeB();
static void move_planes(int value);
static void jump(int value);
static float jump_positions[180];
static int jump_count = 0;


int main(int argc, char **argv) {

    srand(time(NULL));
    /*GLUT init*/
    glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

        /*creating window*/
        glutInitWindowSize(800, 600);
        glutInitWindowPosition(100, 100);
        glutCreateWindow("Free Runner");
        glutSetIconTitle("Free Runner");

    timer_active = 0;
    timer_jump = 0;
    int k = 0;
    for(float i = 0;i <= 18;i+=0.1){
        jump_positions[k] = 1.5*sin(i);
        printf("jump_possition: %lf\n",jump_positions[k++]+1);
    }
    k = 0;

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
    gluLookAt(x_coord,7.0,-5.0,x_coord,5.0,0.0,0.0,1.0,0.0);


    /* Kreira se objekat. */
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, shine);
        glTranslatef(x_coord, y_coord, 0);
        glutSolidSphere(.5,40,40);
    glPopMatrix();
    
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, shine);
        glTranslatef(x_planeA,y_planeA,z_planeA); 
        glScalef(6,1,30);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
        glMaterialfv(GL_FRONT, GL_SHININESS, shine1);
        glTranslatef(x_planeB,y_planeB,z_planeB); 
        glScalef(6,1,30);
        glutSolidCube(1);
    glPopMatrix();

    if(!game_start){
        set_planeA();
        set_planeB();
    }
    
    for(int i = 0; i < size_A ; i++){
        glPushMatrix();
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
            glMaterialfv(GL_FRONT, GL_SHININESS, shine1);
            glTranslatef(array_A[i].x,1,array_A[i].z);
            switch(array_A[i].obst_type){
            case 0: glutSolidCube(1);break;
            case 1: glutSolidCone(.5,1,40,40);break;
            case 2: glutSolidSphere(.5,40,40);break;
        }
        glPopMatrix();
    }

    for(int i = 0; i < size_B ; i++){
        glPushMatrix();
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
            glMaterialfv(GL_FRONT, GL_SHININESS, shine1);
            glTranslatef(array_B[i].x,1,array_B[i].z);
            switch(array_A[i].obst_type){
            case 0: glutSolidCube(1);break;
            case 1: glutSolidCone(.5,1,40,40);break;
            case 2: glutSolidSphere(.5,40,40);break;
        }
        glPopMatrix();
    }


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
            game_start = 1;
            if(!timer_active){
                glutTimerFunc(50,move_planes,timer_id1);
                timer_active = 1;
            };break;
        
        case 'p':
            timer_active = 0;break;
        case 'a':
        case 'A':
            game_start = 1;
            if(x_coord < 6){
                x_coord += 1;
            }
                glutPostRedisplay();
            break;
            
        case 'd':
        case 'D':
            game_start = 1;
            if(x_coord > 0){
                x_coord -= 1;
            }
                glutPostRedisplay();
            break;
        case 'j':
        case 'J':
            if(!timer_jump){
                glutTimerFunc(50,jump,timer_id2);
                timer_jump = 1;
            }
            break;
    }
}

static void move_planes(int value)
{
    if(value)
        return;

    z_planeA -= 0.2;
    z_planeB -= 0.2;
    for(int i = 0 ; i < size_A ; i++){
        array_A[i].z -= 0.2;
    }
    for(int i = 0 ; i < size_B ; i++){
        array_B[i].z -= 0.2;
    }
    if(z_planeA + 15 < 0){
        z_planeA = 45;
        set_planeA();
    }

    if(z_planeB + 15 < 0){
        z_planeB = 45;
        set_planeB();
    }

    glutPostRedisplay();
    if(timer_active) {
        glutTimerFunc(50,move_planes,0);
    }
}

static void set_planeB()
{
    size_B = 0;
    int num_obst = 0;
    Prepreka p;

    for(int i = 0 ; i < 30 ; i += 5){
        num_obst = (rand() % 3 ) + 1;
        for(int j = 0 ; j < num_obst ; j++){
            p.obst_type = possible_obs[rand()%3];
            p.x = possible_x[rand() % 5];
            p.y = 0;
            p.z = z_planeB + i - 14.5;
            array_B[size_B++] = p;
        }
    }
}

static void set_planeA()
{
    size_A = 0;
    
    int num_obst = 0;
    Prepreka p;
    for(int i = 0 ; i < 30 ; i+=5){
        num_obst = (rand() % 3) + 1;
        for(int j = 0 ; j < num_obst ; j++){
            p.obst_type = possible_obs[rand()%3];
            p.x = possible_x[rand() % 5];
            p.y = 0;
            p.z = z_planeA + i - 14.5;
            array_A[size_A++] = p;
        }
    }
}

static void jump(int value)
{
    if(value != timer_id2)
        return;
    
    y_coord = 1 + jump_positions[jump_count++];
    // printf("Y coord : %lf",y_coord);
    if(jump_count < 32)
        glutTimerFunc(30,jump,timer_id2);
    else{
        timer_jump = 0;
        y_coord = 1;
        jump_count = 0;
    }
}

