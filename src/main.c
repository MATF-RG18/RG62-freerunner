#include <stdlib.h>
#include <GL/glut.h>

/*callback function declarations*/
static void on_display(void);
static void on_reshape(int width, int height);
static void on_keyboard(unsigned char key, int x, int y);

int main(int argc, char **argv) {

  /*GLUT init*/
  glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	/*creating window*/
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Free Runner");
	glutSetIconTitle("Free Runner");

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
  /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
    GLfloat light_position[] = { 1, 1, 1, 0 };

  /* Ambijentalna boja svetla. */
    GLfloat light_ambient[] = { 0, 0, 0, 1 };

  /* Difuzna boja svetla. */
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };

  /* Spekularna boja svetla. */
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

  /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 1.0, 0.1, 0.1, 1 };

  /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };

  /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

  /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 50;

  /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Podesava se vidna tacka. */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(5.0, 7.0, 5.0, 5.0, 5.0, 0.0, 0.0, 1.0, 0.0);
  
  /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  /* Podesavaju se parametri materijala. */
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);

  /* Kreira se objekat. */
  glPushMatrix();
    glColor3f(1, 0, 1);
    glTranslatef(5, 0, -1);
    glutSolidSphere(.5,40,40);
  glPopMatrix();

  glLineWidth(5);
  glBegin(GL_LINES);
  glVertex3f(2, 0, 0);
  glVertex3f(2, 0, -200);
  glEnd();

  glBegin(GL_LINES);
  glColor3f(1,0,0);
  glVertex3f(8, 0, 0);
  glVertex3f(8, 0, -200);
  glEnd();
  
  glBegin(GL_LINES);
        glColor3f(1,1,0);
        glVertex3f(0,0,0);
        glVertex3f(10,0,0);
        
        glColor3f(0,1,0);
        glVertex3f(0,0,0);
        glVertex3f(0,10,0);
        
        glColor3f(0,0,1);
        glVertex3f(0,0,0);
        glVertex3f(0,0,10);
        
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
  }
}
