#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>

#define timer_id1 0
#define timer_id2 1
#define PI 3.14
#define timer_id_spiral 2
#define timer_id_hole 3

static int x_coord = 3;
static float y_coord = 1;
static float z_coord = 0;
static int possible_x[] = {1,2,3,4,5};
static int possible_obs[] = {0,1,2,3};
static int timer_active;
static int timer_jump;
static float x_planeA = 3;
static float y_planeA = 0;
static float z_planeA = 30;
static float x_planeB = 3;
static float y_planeB = 0;
static float z_planeB = 60;
static int game_start = 0;
static float angle_leg = 0;

static float spiral_parameter = 0;
static int spiral_animation = 0;
static int hole_animation = 0;
static float hole_parameter = 0;

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

static void spiral_timer(int value);
static void set_planeA();
static void set_planeB();
static void move_planes(int value);
static void check_colision();
static float distance(Prepreka p);
static void draw_man();
static void draw_spiral(float x_obst,float y_obst,float z_obst);
static void draw_teleport(float x_obst,float y_obst,float z_obst);
static void draw_hole(float x_obst,float y_obst,float z_obst);
static void teleport();
static void hole_timer(int value);


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
        jump_positions[k++] = 1.5*sin(i);
        // printf("jump_possition: %lf\n",jump_positions[k++]+1);
    }
    k = 0;

    /*callback function init*/
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);

    /*OpenGL init*/
    glClearColor(0.7, 0.7, 0.7, 0);
    glEnable(GL_DEPTH_TEST);

    GLfloat light_position[] = { 5, 3, 5, 0};
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.1, 0.1, 0.1, 1 };
	
    /*ukljucuje se osvjetljenje i podesavaju parametri svetla*/
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

    /*enter main loop*/
    glutMainLoop();

    return 0;
}

static void on_display(void)
{
    float mat_ambient[] ={ 0.1,0.1,0.1,1};
    float mat_diffuse_gray[] ={ .8,.8,.8,1};
    float mat_diffuse_red[] ={ 0.7,0.1,0.1,1};
    float mat_diffuse_yellow[] ={ 0.8,0.8,0.1,1};
    float mat_specular[] ={0.2,0.2,0.2,1};
    float shininess = 80;
        
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_gray);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x_coord,5,-7,
                x_coord,y_coord,z_coord,
                0.0,1.0,0.0);

    /* Kreira se objekat. */
    // glPushMatrix();
    //     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_red);
    //     glTranslatef(x_coord, y_coord, z_coord);
    //     // draw_man();
    //     glutSolidSphere(.5,40,40);
    // glPopMatrix();

    glPushMatrix();
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_red);
        draw_man();
    glPopMatrix();


    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_gray);

    glPushMatrix();
        glTranslatef(x_planeA,y_planeA,z_planeA); 
        glScalef(6,1,30);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
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
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_yellow);
            Prepreka p = array_A[i];
            if(p.obst_type == 1) {
                glTranslatef(array_A[i].x,1,array_A[i].z);
                glutSolidCube(1);
            }
            else if(p.obst_type == 0) {
                draw_spiral(p.x, p.y, p.z);
            }
            else if(p.obst_type == 2){
                draw_teleport(p.x,p.y,p.z);
            }
            else if(p.obst_type == 3){
                draw_hole(p.x,p.y,p.z);
            }
            
        glPopMatrix();
    }

    for(int i = 0; i < size_B ; i++){
        glPushMatrix();
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_yellow);
            glTranslatef(array_B[i].x,1,array_B[i].z);
            glutSolidCube(1);
        glPopMatrix();
    }

    glDisable(GL_LIGHTING);
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
    glEnable(GL_LIGHTING);
        
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
    gluPerspective(60, (float) width / height, .1, 30);
    // glFrustum (-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
    // glMatrixMode (GL_MODELVIEW);
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

    angle_leg +=1;
    if(angle_leg > 360)
        angle_leg = 0;
    z_planeA -= 0.2;
    z_planeB -= 0.2;
    for(int i = 0 ; i < size_A ; i++){
        array_A[i].z -= 0.2;
        check_colision();
    }
    for(int i = 0 ; i < size_B ; i++){
        array_B[i].z -= 0.2;
        check_colision();
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
        glutTimerFunc(30,move_planes,0);
    }
}

static void set_planeB()
{
    size_B = 0;
    int num_obst = 0;
    Prepreka p;

    for(int i = 5 ; i < 30 ; i += 5){
        num_obst = (rand() % 3 ) + 1;
        for(int j = 0 ; j < num_obst ; j++){
            p.obst_type = possible_obs[rand()%4];
            // printf("%d",p.obst_type);
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
    for(int i = 5 ; i < 30 ; i+=5){
        num_obst = (rand() % 3) + 1;
        for(int j = 0 ; j < num_obst ; j++){
            p.obst_type = possible_obs[(int)rand()%4];
            // printf("%d",p.obst_type);
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
        glutTimerFunc(20,jump,timer_id2);
    else{
        timer_jump = 0;
        y_coord = 1;
        jump_count = 0;
    }
}
int k = 0;
static void check_colision()
{
    if (z_planeA < z_planeB){
        for(int i = 0 ; i < size_A ; i++){
            // printf("udaljenost: %f",distance(array_A[i]));
            //hardkodovano zbog greske u racunu
            if (distance(array_A[i]) <= 1.02){
                switch(array_A[i].obst_type){
                    case 0:
                        // printf("SPIRALA\n");
                        if(!spiral_animation){
                            spiral_animation = 1;
                            glutTimerFunc(20,spiral_timer,timer_id_spiral);
                        }
                    break;
                    case 1:
                        timer_active = 0;
                        break;
                    case 2:
                        teleport();
                    break;
                    case 3:
                        if(!hole_animation){
                            hole_animation = 1;
                            glutTimerFunc(20,hole_timer,timer_id_hole);
                        }
                    break;

                }
                // printf("Udario sam %d\n", k);
                // timer_active = 0;
                k++;
            }
        }
    }
    else{
        for(int i = 0 ; i < size_B ; i++){
            //hardkodovano zbog greske u racunu
            if (distance(array_B[i]) <= 1.02){
                // printf("Udario sam %d\n", k);
                // timer_active = 0;
                switch(array_A[i].obst_type){
                    case 0:
                        // printf("SPIRALA\n");
                        if(!spiral_animation){
                            spiral_animation = 1;
                            glutTimerFunc(20,spiral_timer,timer_id_spiral);
                        }
                    break;
                    case 1:
                        timer_active = 0;
                        break;
                    case 2:
                        teleport();
                    break;
                    case 3:
                        if(!hole_animation){
                            hole_animation = 1;
                            glutTimerFunc(20,hole_timer,timer_id_hole);
                        }
                    break;

                }
                k++;
            }
        }

    }
}

static float distance(Prepreka p)
{
    float x = powf((p.x - x_coord),2);
    float y = powf((p.y - y_coord),2);
    float z = powf((p.z - z_coord),2);

    return sqrtf(x + y + z);
}

static void draw_man()
{
    glPushMatrix();
        glTranslatef(x_coord,y_coord+1.2,z_coord);
        glutSolidSphere(0.2,10,10);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(x_coord,y_coord+.6,z_coord);
        glScalef(.5,.7,.3);
        glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(x_coord+.15,y_coord+.1,z_coord);
        glScalef(.3,1.5,.3);
        glPushMatrix();
            glRotatef(20*sin(30*angle_leg*PI/180),1,0,0);
            glutSolidCube(.5);
        glPopMatrix();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(x_coord-.15,y_coord+.1,z_coord);
        //TODO stampaj MODELVIEW
        glScalef(.3,1.5,.1);
        glPushMatrix();
            glRotatef(20*cos(30*angle_leg*PI/180),1,0,0);
            glutSolidCube(.5);
        glPopMatrix();
    glPopMatrix();
    // glPushMatrix();
    //     glTranslatef(x_coord+.5,y_coord-1.3,5);
    //     glRotatef(40,0,0,1);
    //     glScalef(.1,.6,.05);
    //     glPushMatrix();
    //         glRotatef(20*cos(angle_leg*PI/15),1,0,0);
    //         glutSolidCube(1);
    //     glPopMatrix();
    // glPopMatrix();
    // glPushMatrix();
    //     glTranslatef(x_coord-.5,y_coord-1.3,5);
    //     glRotatef(-40,0,0,1);
    //     glScalef(.1,.6,.05);
    //     glPushMatrix();
    //         glRotatef(20*sin(angle_leg*PI/15),1,0,0);
    //         glutSolidCube(1);
    //     glPopMatrix();
    // glPopMatrix();
    glPushMatrix();
        glTranslatef(x_coord -.3,y_coord+.7,z_coord);
        glRotatef(-20,0,0,1);
        glScalef(.1,.4,.1);
        glPushMatrix();
            glRotatef(20*sin(15*angle_leg*PI/180),1,0,0);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(x_coord +.3,y_coord+.7,z_coord);
        glRotatef(20,0,0,1);
        glScalef(.1,.4,.1);
        glPushMatrix();
            glRotatef(20*cos(15*angle_leg*PI/180),1,0,0);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}

static void draw_spiral(float x_obst,float y_obst,float z_obst)
{
    glPushMatrix();
        glTranslatef(x_obst,y_obst,z_obst);
        GLfloat x = 0,y=0.8,z=0,angle, r  = .0001;
        glBegin(GL_LINE_STRIP);
        glLineWidth(7);
        for(angle = 0; angle < 1400; angle += 1)
        {   
            x = r * cos(angle * PI/180);
            z = r * sin(angle * PI/180);
            glVertex3f(x,y,z);
            y+=0.0006;
            r+= .0005;
        }
        glEnd();
    glPopMatrix();
}

static void spiral_timer(int value)
{
    if(value != timer_id_spiral)
        return;
    
    spiral_parameter += 5;

    if(spiral_parameter >=180){
        spiral_parameter = 0;
        spiral_animation = 0;
    }

    y_coord = 15*sin(spiral_parameter * PI/180)+1;

    glutPostRedisplay();

    if(spiral_animation)
        glutTimerFunc(20,spiral_timer,timer_id_spiral);
    
}

static void draw_teleport(float x_obst,float y_obst,float z_obst)
{
    glPushMatrix();
        glTranslatef(x_obst,y_obst+1.5,z_obst);
        glScalef(.6,1.2,0);
        GLfloat x = 0,y=0.8,z=0,angle, r  = .0001;
        glBegin(GL_LINE_STRIP);
        glLineWidth(7);
        for(angle = 0; angle < 1800; angle += 1)
        {   
            x = r * cos(angle * PI/180);
            y = r * sin(angle * PI/180);
            z=z_obst -.5;
            glVertex3f(x,y,z);
            
            r+= .0005;
        }
        glEnd();
    glPopMatrix();

}
static void teleport()
{
    x_coord = possible_x[(int)rand()%5+1];
}

static void hole_timer(int value)
{
    if(value != timer_id_hole)
        return;

    hole_parameter += 1;
    if(hole_parameter > 90){
        timer_active = 0;
        hole_animation = 0;
    }
    y_coord = -15*sin(hole_parameter * PI/180);

    glutPostRedisplay();
    if(hole_animation)
        glutTimerFunc(20,hole_timer,timer_id_hole);

}
static void draw_hole(float x_obst,float y_obst,float z_obst)
{
    glDisable(GL_LIGHTING);
    glColor3f(0,0,0);
    glPushMatrix();
        glTranslatef(x_obst,y_obst+.1,z_obst);
        glutSolidCube(1);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}