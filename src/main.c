#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "image.h"

#define timer_id1 0
#define timer_id2 1
#define PI 3.14
#define timer_id_spiral 2
#define timer_id_hole 3
#define FILENAME0 "game.bmp"
#define FILENAME1 "moss.bmp"

float mat_diffuse_sky[] ={ 0.5,0.8,1,1};
float mat_diffuse_yellow[] ={ 0.8,0.8,0.1,1};
float mat_diffuse_skin[] ={ 1,0.8,0.59,1};

static GLuint names[2];
static int score = 0;
static int helping_score = 0;

//koordinate desne strane od glavne staze
float right_side_up_x = -10;
float right_side_up_y = 0;
float right_side_up_z = 40;
float right_side_down_x = -10;
float right_side_down_y = 0;
float right_side_down_z = 10;

//koordinate leve strane od glavne staze
float left_side_up_x = 16;
float left_side_up_y = 0;
float left_side_up_z = 40;
float left_side_down_x = 16;
float left_side_down_y = 0;
float left_side_down_z = 10;

//koordinate glavnog objekta(coveka)
static float x_coord = 3;
static float y_coord = 1;
static float z_coord = 0;

static int possible_x[] = {1,2,3,4,5};
static int possible_obs[] = {0,1,2,3,4,5,6};

//tajmeri i parametri za timer funkcije
static int timer_active;
static int timer_jump;
static int hammer_timer = 0;
static float angle_leg = 0;
static float spiral_parameter = 0;
static int spiral_animation = 0;
static int hole_animation = 0;
static float hole_parameter = 0;

//koordinate ravni na kojima se generisu prepreke
static float x_planeA = 3;
static float y_planeA = 0;
static float z_planeA = 10;
static float x_planeB = 3;
static float y_planeB = 0;
static float z_planeB = 40;

//pomocne promenljive
static int game_end = 0;
static int game_start = 0;
static int imune = 0;
static int have_hammer = 0;

//struktura koja opisuje prepreke
typedef struct{
    float x,y,z;
    int obst_type;
    int to_remove;
    float hammer_angle;
} Prepreka;

static Prepreka array_A[200];
static int size_A = 0;
static Prepreka array_B[200];
static int size_B = 0;

//pomocni niz za opisivanje kretanja coveka
static int possible_moves[] = {0,0};


//deklaracija callback funkcija
static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);
static void on_release(unsigned char key,int x,int y);

//funkcije koje animiraju kretanje prepreka i ravni
static void set_planeA();
static void set_planeB();
static void move_planes(int value);

//funkcije za proveru kolizije
static void check_colision();
static float distance(Prepreka p);


static void draw_man();
static void draw_spiral(float x_obst,float y_obst,float z_obst);
static void spiral_timer(int value);
static void draw_teleport(float x_obst,float y_obst,float z_obst);
static void teleport();
static void draw_hole(float x_obst,float y_obst,float z_obst);
static void hole_timer(int value);
static void draw_hammer();
static void draw_hammer_obst(float x_obst,float y_obst,float z_obst);
static void jump(int value);
static float jump_positions[180];
static int jump_count = 0;


//funkcija za inicijalizaciju tekstura
static void initialize(void);


int main(int argc, char **argv) {

    srand(time(NULL));
    //inicijalizacija GLUT-a
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    //kreiranje prozora
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Free Runner");
    glutSetIconTitle("Free Runner");


    //puni se niz iz kojeg ce se citati (y) koordinate pri skoku
    timer_active = 0;
    timer_jump = 0;
    int k = 0;
    for(float i = 0;i <= 18;i+=0.1){
        jump_positions[k++] = 1.5*sin(i);
    }
    k = 0;

    //poziv callback funkcija
    glutDisplayFunc(on_display);
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    glutKeyboardUpFunc(on_release);

    glClearColor(0.7, 0.7, 0.7, 0);
    glEnable(GL_DEPTH_TEST);

    //nizovi za postavljanje osvetljenja
    GLfloat light_position[] = { 5, 5, 5, 0};
    GLfloat light_position1[] = { 5, 0,0, 0};
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_specular[] = { 0.1, 0.1, 0.1, 1 };
	
    //ukljucuje se osvjetljenje i podesavaju parametri svetla
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

     glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    // glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);

    initialize();
    //ulazak u glavnu petlju
    glutMainLoop();

    return 0;
}

static void on_display(void)
{
    //nizovi za postavljanje materijala
    float mat_ambient[] ={ 0.1,0.1,0.1,1};
    float mat_diffuse_gray[] ={ .8,.8,.8,1};
    float mat_diffuse_red[] ={ 0.7,0.1,0.1,1};
    float mat_specular[] ={0.2,0.2,0.2,1};
    float shininess = 80;
        
    //ukljucivanje materijala
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_gray);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    // Brise se prethodni sadrzaj prozora
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Podesava se vidna tacka
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x_coord,y_coord + 5,z_coord - 7,
                x_coord,y_coord,z_coord + 5,
                0.0,1.0,0.0);

    glutFullScreen();
    //prikaz trenutnog skora
    glRasterPos3f(x_coord+9,y_coord+5,5);
    char score_display[50] = "SCORE: ";
    char score_value[50];
    sprintf(score_value," %d ",score);
    strcat(score_display,score_value);

    int len = (int)strlen(score_display);

    for(int i = 0; i < len ; i++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,score_display[i]);
    }

    //hammer timer display
    glRasterPos3f(x_coord-7,y_coord+5,5);
    char hammer_display[50] = "HAMMER: ";
    char hammer_value[50];
    sprintf(hammer_value," %d ",hammer_timer);
    strcat(hammer_display,hammer_value);

    len = (int)strlen(hammer_display);

    for(int i = 0; i < len ; i++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,hammer_display[i]);
    }

    //imune timer display
    glRasterPos3f(x_coord-7.2,y_coord+4,5);
    char immune_display[50] = "IMMUNE:   ";
    char immune_value[50];
    sprintf(immune_value," %d ",imune);
    strcat(immune_display,immune_value);

    len = (int)strlen(immune_display);

    for(int i = 0; i < len ; i++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,immune_display[i]);
    }

    //crta se cekic koji covek drzi na odredjeno vreme, ako je doslo do kolizije sa cekicem koji je tip prepreke
    if(have_hammer){
        draw_hammer();
    }

    //sfera koja predstavlja nebo
    glPushMatrix();
        glDisable(GL_LIGHTING);
        glColor3f(.5,.8,1);
        glTranslatef(3,0,15);
        glutSolidSphere(30,50,50);
        glEnable(GL_LIGHTING);
    glPopMatrix();

    //crta se covek
    glPushMatrix();
        draw_man();
    glPopMatrix();

    //postavljane teksture i iscrtavanje prve ravni, ravni A
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D,names[0]);
        glEnable(GL_TEXTURE_2D);

        glBegin(GL_QUAD_STRIP);
        glNormal3f(0,1,0);
        int first = 0,second = 0;
        for(int i = 0 ; i <= 35 ;i+=7){
            
            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            glVertex3f(0,0.51,z_planeA-15+i);

            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            second = (second + 1) % 2;
            glVertex3f(6,0.51,z_planeA-15+i);
        }

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);

        glTranslatef(x_planeA,y_planeA,z_planeA); 
        glScalef(6,1,30);
        glutSolidCube(1);
    glPopMatrix();

    //postavljane teksture i iscrtavanje druge ravni, ravni B
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D,names[0]);
        glEnable(GL_TEXTURE_2D);

        glBegin(GL_QUAD_STRIP);
        glNormal3f(0,1,0);
        first = 0,second = 0;
        for(int i = 0 ; i <= 35 ;i+=7){
            
            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            glVertex3f(0,0.53,z_planeB-15+i);

            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            second = (second + 1) % 2;
            glVertex3f(6,0.53,z_planeB-15+i);
        }

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);
        glTranslatef(x_planeB,y_planeB,z_planeB); 
        glScalef(6,1,30);
        glutSolidCube(1);
    glPopMatrix();

    //postavljanje teksture i iscrtavanje ravni desno od glavne ravni
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,names[1]);
        glEnable(GL_TEXTURE_2D);

        glBegin(GL_QUAD_STRIP);
        glNormal3f(0,1,0);
        first = 0,second = 0;
        for(int i = 0 ; i <= 30 ;i+=5){
            
            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            glVertex3f(1,1.03,right_side_up_z-15+i);

            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            second = (second + 1) % 2;
            glVertex3f(-20,1.03,right_side_up_z-15+i);
        }

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);
        glTranslatef(right_side_up_x+1,right_side_up_y,right_side_up_z); 
        glScalef(20,2,30);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,names[1]);
        glEnable(GL_TEXTURE_2D);

        glBegin(GL_QUAD_STRIP);
        glNormal3f(0,1,0);
        first = 0,second = 0;
        for(int i = 0 ; i <= 30 ;i+=5){
            
            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            glVertex3f(1,1.03,right_side_down_z-15+i);

            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            second = (second + 1) % 2;
            glVertex3f(-20,1.03,right_side_down_z-15+i);
        }

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);
        glTranslatef(right_side_down_x+1,right_side_down_y,right_side_down_z); 
        glScalef(20,2,30);
        glutSolidCube(1);
    glPopMatrix();

    //postavljanje teksture i iscrtavanje ravni levo od glavne ravni
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,names[1]);
        glEnable(GL_TEXTURE_2D);

        glBegin(GL_QUAD_STRIP);
        glNormal3f(0,1,0);
        first = 0,second = 0;
        for(int i = 0 ; i <= 30 ;i+=5){
            
            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            glVertex3f(6,1.03,left_side_up_z-15+i);

            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            second = (second + 1) % 2;
            glVertex3f(26,1.03,left_side_up_z-15+i);
        }

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);
        glTranslatef(left_side_up_x,left_side_up_y,left_side_up_z); 
        glScalef(20,2,30);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D,names[1]);
        glEnable(GL_TEXTURE_2D);

        glBegin(GL_QUAD_STRIP);
        glNormal3f(0,1,0);
        first = 0,second = 0;
        for(int i = 0 ; i <= 30 ;i+=5){
            
            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            glVertex3f(6,1.03,left_side_down_z-15+i);

            glTexCoord2f(first,second);
            first = (first + 1) % 2;
            second = (second + 1) % 2;
            glVertex3f(26,1.03,left_side_down_z-15+i);
        }

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);
        glTranslatef(left_side_down_x,left_side_down_y,left_side_down_z); 
        glScalef(20,2,30);
        glutSolidCube(1);
    glPopMatrix();

    //na pocetku se prepreke postavljaju samo na ravan B
    if(!game_start){
        set_planeB();
    }
    
    //crtaju se prepreke u zavisnosti od njihovog tipa, koji se dodeljuje u set_planeA funkicji
    for(int i = 0; i < size_A ; i++){
        glPushMatrix();
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_red);
            Prepreka p = array_A[i];
            if(p.obst_type == 1 || p.obst_type == 5 || p.obst_type == 6)  {
                if(p.to_remove){
                    //simulacija kretanja kocke ako je pogodjena, dok covek ima cekic
                    glTranslatef(-5*sin(array_A[i].hammer_angle * PI/180) ,
                                 5 * sin(2.2*array_A[i].hammer_angle * PI/180),
                                  25 * sin(array_A[i].hammer_angle * PI/180));
                    if(array_A[i].hammer_angle <=90)
                        array_A[i].hammer_angle += 5;
                }
                if(array_A[i].hammer_angle < 180) {
                    glTranslatef(array_A[i].x,1,array_A[i].z);
                    glPushMatrix();
                        glRotatef(-2*angle_leg,1,0,0);
                        glutSolidCube(1);
                    glPopMatrix();
                }
                
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
            else if(p.obst_type == 4){
                draw_hammer_obst(p.x,p.y+.6,p.z-.5);
            }
            
        glPopMatrix();
    }

    //crtaju se prepreke u zavisnosti od njihovog tipa, koji se dodeljuje u set_planeB funkicji
    for(int i = 0; i < size_B ; i++){
        glPushMatrix();
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_red);
            Prepreka p = array_B[i];
            if(p.obst_type == 1 || p.obst_type == 5 || p.obst_type == 6) {
                if(p.to_remove){
                    //simulacija kretanja kocke ako je pogodjena, dok covek ima cekic
                    glTranslatef(5*sin(array_B[i].hammer_angle * PI/180) ,
                                 5 * sin(2.2*array_B[i].hammer_angle * PI/180),
                                  25 * sin(array_B[i].hammer_angle * PI/180));
                    if(array_B[i].hammer_angle <=90)
                        array_B[i].hammer_angle += 5;
                }
                glTranslatef(array_B[i].x,1,array_B[i].z);
                glPushMatrix();
                        glRotatef(-2*angle_leg,1,0,0);
                        glutSolidCube(1);
                    glPopMatrix();
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
            else if(p.obst_type == 4){
                draw_hammer_obst(p.x,p.y+0.6,p.z-.5);
            }
            
        glPopMatrix();
    }
        
    // Nova slika se salje na ekran
    glutSwapBuffers();
}

static void on_reshape(int width, int height)
{
    // Podesava se viewport
    glViewport(0, 0, width, height);

    // Podesava se projekcija
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, .1, 60);
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
     // Zavrsava se program
        exit(0);
        break;
    //pokrece se igra    
        case 'S':
        case 's':
            game_start = 1;
            if(!game_end){
                if(!timer_active){
                    glutTimerFunc(50,move_planes,timer_id1);
                    timer_active = 1;
                }
            };break;
    //pauzira se igra
        case 'p':
            timer_active = 0;break;
    //skretanje u levo
        case 'a':
        case 'A':
            game_start = 1;
            
            possible_moves[0] = 1;
            glutPostRedisplay();
            break;
    //skretanje u desno
        case 'd':
        case 'D':
            game_start = 1;
            possible_moves[1] = 1;
            glutPostRedisplay();
            break;
    //skok
        case 'j':
        case 'J':
        case 32:
            if(!timer_jump){
                glutTimerFunc(50,jump,timer_id2);
                timer_jump = 1;
            }
            break;
    }
}

//tajmer u kom se pomeraju glavne ravni (A i B), ujedno i glavni tajmer
static void move_planes(int value)
{
    if(value)
        return;

    //igrac je nakon teleporta imun neko vreme
    if(imune > 0)
        imune--;

    //kretanje u tajmeru
    if(possible_moves[0] && x_coord < 5.5){
        x_coord += 0.1;
    }
    if(possible_moves[1] && x_coord > 1.5){
        x_coord -= 0.1;
    }
    
    //igrac ima cekic odredjeno vreme
    if(hammer_timer){
        hammer_timer -= 1;
    }
    else{
        have_hammer = 0;
    }
    
    helping_score++;
    if(helping_score == 10){
        score++;
        helping_score = 0;
    }

    //ugao koji se koristi za animiranje kretanja covekovih ruku i nogu
    angle_leg +=1;
    if(angle_leg > 360)
        angle_leg = 0;

    //pomeraju se ravni
    z_planeA -= 0.25;
    z_planeB -= 0.25;
    for(int i = 0 ; i < size_A ; i++){
        array_A[i].z -= 0.25;
    }
    for(int i = 0 ; i < size_B ; i++){
        array_B[i].z -= 0.25;
    }
    //proverava se kolizija
    check_colision();

    //vraca se ravan A i opet se na njoj generisu prepreke
    if(z_planeA + 15 < 0){
        z_planeA = 45;
        set_planeA();
    }

    //vraca se ravan B i opet se na njoj generisu prepreke
    if(z_planeB + 15 < 0){
        z_planeB = 45;
        set_planeB();
    }

    //pomeranje pomocnih ravni
    right_side_down_z -= .25;
    if(right_side_down_z + 15 < 0){
        right_side_down_z = 45;
    }

    right_side_up_z -= .25;
    if(right_side_up_z + 15 < 0){
        right_side_up_z = 45;
    }

    left_side_down_z -= .25;
    if(left_side_down_z + 15 < 0){
        left_side_down_z = 45;
    }

    left_side_up_z -= .25;
    if(left_side_up_z + 15 < 0){
        left_side_up_z = 45;
    }

    glutPostRedisplay();
    if(timer_active) {
        glutTimerFunc(30,move_planes,0);
    }
}

//odredjuju se broj prepreka u redu i tipovi prepreka 
static void set_planeB()
{
    size_B = 0;
    
    int num_obst = 0;
    
    for(int i = 5 ; i < 30 ; i+=5){
        num_obst = ((int)rand() % 3) + 1;
        int hits = 0;
        int taken_possitions[] = {0,0,0,0,0};
        while(hits < num_obst){
            Prepreka p;
            int position = 100;
            while(position == 100 || taken_possitions[position]){
                position = possible_x[(int)rand()%5];
            }

                taken_possitions[position] = 1;
                p.hammer_angle = 0;
                p.x = possible_x[position];
                p.y = 0;
                p.z = z_planeB + i - 14.5;
                p.obst_type = possible_obs[(int)rand()%7];
                p.to_remove = 0;
                array_B[size_B++] = p;
                hits++;
        }
    }
}

//odredjuju se broj prepreka u redu i tipovi prepreka 
static void set_planeA()
{
    size_A = 0;
    
    int num_obst = 0;
    
    for(int i = 5 ; i < 30 ; i+=5){
        num_obst = ((int)rand() % 3) + 1;
        //for(int j = 0 ; j < num_obst ; j++){
        int hits = 0;
        int taken_possitions[] = {0,0,0,0,0};
        
        while(hits < num_obst){
            Prepreka p;
            int position = 100;
            while(position == 100 || taken_possitions[position]){
                position = possible_x[(int)rand()%5];
            }

                taken_possitions[position] = 1;
                p.hammer_angle = 0;
                p.x = possible_x[position];
                p.y = 0;
                p.z = z_planeA + i - 14.5;
                p.obst_type = possible_obs[(int)rand()%7];
                p.to_remove = 0;
                array_A[size_A++] = p;
                hits++;
        }
    }
}

//imprelentacija skoka
static void jump(int value)
{
    if(value != timer_id2)
        return;
    
    y_coord = 1 + jump_positions[jump_count++];
    if(jump_count < 32)
        glutTimerFunc(20,jump,timer_id2);
    else{
        timer_jump = 0;
        y_coord = 1;
        jump_count = 0;
    }
}
int k = 0;

//funkcija koja proverava koliziju, i poziva druge funkcije ako dodje do kolizije za odredjenim tipom prepreka
static void check_colision()
{
    if(imune)
        return;

    if (z_planeA < z_planeB){
        for(int i = 0 ; i < size_A ; i++){
            //hardkodovano zbog greske u racunu
            if (distance(array_A[i]) <= 1.22){
                switch(array_A[i].obst_type){
                    case 0:
                        if(!spiral_animation){
                            score += 10;
                            spiral_animation = 1;
                            glutTimerFunc(20,spiral_timer,timer_id_spiral);
                        }
                    break;
                    case 1:
                    case 5:
                    case 6:
                        if(have_hammer){
                            array_A[i].to_remove = 1;
                            return;
                        }
                        game_end = 1;
                        timer_active = 0;
                        break;
                    case 2:
                        score += 10;
                        teleport();
                    break;
                    case 3:
                        if(!hole_animation){
                            game_end = 1;
                            hole_animation = 1;
                            glutTimerFunc(20,hole_timer,timer_id_hole);
                        }
                    break;
                    case 4:
                            have_hammer = 1;
                            hammer_timer = 100;
                    break;

                }
                k++;
            }
        }
    }
    else{
        for(int i = 0 ; i < size_B ; i++){
            //hardkodovano zbog greske u racunu
            if (distance(array_B[i]) <= 1.22){
                switch(array_B[i].obst_type){
                    case 0:
                        if(!spiral_animation){
                            spiral_animation = 1;
                            score += 10;
                            glutTimerFunc(20,spiral_timer,timer_id_spiral);
                        }
                    break;
                    case 1:
                    case 5:
                    case 6:
                        if(have_hammer){
                            array_B[i].to_remove = 1;
                            return;
                        }
                        game_end = 1;
                        timer_active = 0;
                        break;
                    case 2:
                        score += 10;
                        teleport();
                    break;
                    case 3:
                        if(!hole_animation){
                            game_end = 1;
                            hole_animation = 1;
                            glutTimerFunc(20,hole_timer,timer_id_hole);
                        }
                    break;
                    case 4:
                            have_hammer = 1;
                            hammer_timer = 100;
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

//funkcija koja iscrtava coveka
static void draw_man()
{
    //glava
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_skin);
    glPushMatrix();
        glTranslatef(x_coord,y_coord+1.2,z_coord);
        glutSolidSphere(0.2,10,10);
    glPopMatrix();
    //telo
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_yellow);
    glPushMatrix();
        glTranslatef(x_coord,y_coord+.6,z_coord);
        glScalef(.5,.7,.3);
        glutSolidCube(1);
    glPopMatrix();
    //noge
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_sky);
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
            glRotatef(20*sin(30*angle_leg*PI/180 + PI),1,0,0);
            glutSolidCube(.5);
        glPopMatrix();
    glPopMatrix();
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse_yellow);
    //ruke
    glPushMatrix();
        glTranslatef(x_coord -.3,y_coord+.7,z_coord);
        glRotatef(-20,0,0,1);
        glScalef(.1,.4,.1);
        glPushMatrix();
            glRotatef(20*sin(30*angle_leg*PI/180),1,0,0);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(x_coord +.3,y_coord+.7,z_coord);
        glRotatef(20,0,0,1);
        glScalef(.1,.4,.1);
        glPushMatrix();
            glRotatef(20*sin(30*angle_leg*PI/180 + PI),1,0,0);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}

//funkcija koja crta spiralu
static void draw_spiral(float x_obst,float y_obst,float z_obst)
{
    glPushMatrix();
        glTranslatef(x_obst,y_obst,z_obst);
        GLfloat x = 0,y=0.8,z=0,angle, r  = .0001;
        glBegin(GL_LINE_STRIP);
        glLineWidth(10);
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

//funkcija koja nam kaze sta se desi kada dodje do kolizije sa spiralom
static void spiral_timer(int value)
{
    if(value != timer_id_spiral)
        return;
    
    spiral_parameter += 5;

    if(spiral_parameter >=180){
        spiral_parameter = 0;
        spiral_animation = 0;
    }

    y_coord = 13*sin(spiral_parameter * PI/180)+1;

    glutPostRedisplay();

    if(spiral_animation)
        glutTimerFunc(20,spiral_timer,timer_id_spiral);   
}


//funkcija koja iscrtava teleport
static void draw_teleport(float x_obst,float y_obst,float z_obst)
{
    glPushMatrix();
        glTranslatef(x_obst,y_obst+1.5,z_obst);
        glScalef(.6,1.2,0);
        GLfloat x = 0,y=0.8,z=0,angle, r  = .0001;
        glBegin(GL_LINE_STRIP);
        // glColor3f(.5,.8,1);
        glLineWidth(50);
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

//funkcija koja nam kaze sta se desi kada dodje do kolizije sa teleportom
static void teleport()
{
    imune = 100;
    x_coord = (int)rand()%6+1;
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

//funkcija koja iscrtava rupu
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

static void initialize(void)
{
    // Objekat koji predstavlja teskturu ucitanu iz fajla
    Image * image;

    // Ukljucuje se testiranje z-koordinate piksela
    glEnable(GL_DEPTH_TEST);

    // Ukljucuju se teksture
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    image = image_init(0, 0);

    image_read(image, FILENAME0);

    glGenTextures(2, names);

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    // Iskljucujemo aktivnu teksturu
    glBindTexture(GL_TEXTURE_2D, 0);

    // Unistava se objekat za citanje tekstura iz fajla
    image_done(image);

    //Inicijalizujemo matricu rotacije
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//funkcija koje detektuje otpustanje tastera
static void on_release(unsigned char key,int x,int y)
{
    switch(key){
        case 'a':
        case 'A':
            possible_moves[0] -= 1;break;
        case 'd':
        case 'D': possible_moves[1] -= 1;break;
    }
}

//funkcija koja crta cekic u ruci igraca
static void draw_hammer()
{
    glPushMatrix();
        glTranslatef(x_coord -.35,y_coord+.5,z_coord);
        glRotatef(-45,1,0,0);
        glRotatef(-30,0,1,0);
        glPushMatrix();
            glRotatef(20*sin(30*angle_leg*PI/180),1,0,0);
            gluCylinder(gluNewQuadric(),.05,.05,.5,40,40);
        glPopMatrix();


        glTranslatef(0,.05,.5);
        glScalef(1,.8,.3);
        glPushMatrix();
            glRotatef(20*sin(30*angle_leg*PI/180+PI),1,0,0);
            glutSolidCube(.5);
        glPopMatrix();
    glPopMatrix();
}

//funkcija koja crta cekic kao prepreku na mapi
static void draw_hammer_obst(float x_obst,float y_obst,float z_obst)
{
    glPushMatrix(); 
        glTranslatef(x_obst,y_obst,z_obst);
        glPushMatrix();
            glRotatef(-90,1,0,0);
            glRotatef(-30,0,1,0);
            gluCylinder(gluNewQuadric(),.1,.1,1,40,40);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-0.5,1,0);
            glRotatef(-60,0,0,1);
            glScalef(.5,.7,.2);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}