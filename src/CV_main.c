/*CryptoVisualizer v.1.02 - (c) 2008 Omar alejandro Herrera Reyna
  This program is released under the GNU GPL License version 3.
*/
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define _STDCALL_SUPPORTED 1  /*Required by Glut in Windows*/
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#define WWIDTH 800
#define WHEIGHT 600
#define BUFSIZE 4096

double eyex,eyey,eyez,centerx,centery,centerz,upx,upy,upz;
float vertex;
unsigned char byteBuffer [BUFSIZE];
unsigned int visMode=1, angleView=1, bytesRead,endOfFile=0;
unsigned int SHEIGHT=32;
float GRIDSIZE=6.0, WAVERESOL=3.0, DISPLACEMENT=0.0;
long int fOffset=0;
FILE *fp;
char * filename;

void loadBytes (){
   DISPLACEMENT=SHEIGHT*2;
   fp = fopen(filename,"rb");
   if (!fp){
       printf ("Error: couldn't open file %s\n",filename);
       exit(1);
   }
   fseek(fp,fOffset,SEEK_SET);
   bytesRead=fread(byteBuffer,1,BUFSIZE,fp);
   if (bytesRead==BUFSIZE){
      printf ("%d bytes read from file at offset %ld\n",BUFSIZE,fOffset);
   }
   else if (feof(fp)){
      printf ("%d bytes read from file at offset %ld\n",bytesRead,fOffset);
      printf ("End Of File Reached\n");
      endOfFile=1;
      memset(byteBuffer+bytesRead,0,BUFSIZE-bytesRead);
   }
   else {
      printf ("%d bytes read from file at offset %ld\n",bytesRead,fOffset);
      printf ("Error reading file\n");
      exit(1);
   }
   fclose(fp);
}

void init(void){
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   loadBytes();
}
void graphCH (void){
   float x,y,a,b;
   int cont;
   cont=0; x=0.0; y=0.0;
   do {
      glBegin (GL_QUAD_STRIP);
      do {
         a=(float)byteBuffer[cont+SHEIGHT];
         b=(float)byteBuffer[cont];
         glColor3f (a/255,a/255,a/255);
         glVertex3f (x+GRIDSIZE,y,a/WAVERESOL);
         glColor3f (b/255,b/255,b/255);
         glVertex3f (x,y,b/WAVERESOL);
         cont++; y+=GRIDSIZE;
      } while (y<(SHEIGHT*GRIDSIZE));
      glEnd();
      y=0.0;
      x+=GRIDSIZE;
   } while (x<((BUFSIZE/SHEIGHT)*GRIDSIZE));
}

void graphCH2(void){
   float x,y;
   int cont;
   cont=0; x=0.0; y=0.0;
   do {
      glBegin (GL_QUAD_STRIP);
      do {
         glColor3f ((float)byteBuffer[cont]/255,0.0,0.0);
         glVertex3f (x+GRIDSIZE,y,(float)byteBuffer[cont]/WAVERESOL);
         glVertex3f (x,y,(float)byteBuffer[cont]/WAVERESOL);
         glVertex3f (x+GRIDSIZE,y+GRIDSIZE,(float)byteBuffer[cont]/WAVERESOL);
         glVertex3f (x,y+GRIDSIZE,(float)byteBuffer[cont]/WAVERESOL);
         cont++; y+=GRIDSIZE;
      } while (y<(SHEIGHT*GRIDSIZE));
      glEnd();
      y=0.0;
      x+=GRIDSIZE;
   } while (x<((BUFSIZE/SHEIGHT)*GRIDSIZE));
}

void graphC(void){
   float x,y,a,b;
   int cont;
   cont=0; x=0.0; y=0.0;
   do {
      glBegin (GL_TRIANGLE_STRIP);
      do {
         a=(float)byteBuffer[cont+SHEIGHT];
         b=(float)byteBuffer[cont];
         glColor3f (a/255,a/255,a/255);
         glVertex3f (x+GRIDSIZE,y,0.0);
         glColor3f (b/255,b/255,b/255);
         glVertex3f (x,y,0.0);
         cont++; y+=GRIDSIZE;
      } while (y<(SHEIGHT*GRIDSIZE));
      glEnd();
      y=0.0;
      x+=GRIDSIZE;
   } while (x<((BUFSIZE/SHEIGHT)*GRIDSIZE));
}

void graphT (void){
   float x,y;
   int cont,grad;
   cont=0; x=0.0; y=0.0;
      do {
         glBegin (GL_TRIANGLE_STRIP);
         do {
            grad=byteBuffer[cont+SHEIGHT];
            if (grad >=170) {
               glColor3f (0.0,0.0,(float)byteBuffer[cont+SHEIGHT]/255);
            }
            if (grad<170 && grad>=85) {
               glColor3f (0.0,(float)byteBuffer[cont+SHEIGHT]/255,0.0);
            }
            if (grad<85) {
               glColor3f ((float)byteBuffer[cont+SHEIGHT]/255,0.0,0.0);
            }
            glVertex3f (x+GRIDSIZE, y,0.0);
            grad=byteBuffer[cont];
            if (grad >=170) {
               glColor3f (0.0,0.0,(float)byteBuffer[cont]/255);
            }
            if (grad<170 && grad>=85) {
               glColor3f (0.0,(float)byteBuffer[cont]/255,0.0);
            }
            if (grad <85) {
               glColor3f ((float)byteBuffer[cont]/255,0.0,0.0);
            }
            glVertex3f (x,y,0.0);
            cont++; y+=GRIDSIZE;
         } while (y<(SHEIGHT*GRIDSIZE));
         glEnd();
         y=0.0;
         x+=GRIDSIZE;
      } while (x<((BUFSIZE/SHEIGHT)*GRIDSIZE));
}

void graphTH (void){
   float x,y;
   int cont,grad;
   cont=0; x=0.0; y=0.0;
      do {
         glBegin (GL_TRIANGLE_STRIP);
         do {
            grad=byteBuffer[cont+SHEIGHT];
            if (grad >=170) {
               glColor3f (0.0,0.0,(float)byteBuffer[cont+SHEIGHT]/255);
            }
            if (grad<170 && grad>=85) {
               glColor3f (0.0,(float)byteBuffer[cont+SHEIGHT]/255,0.0);
            }
            if (grad<85) {
               glColor3f ((float)byteBuffer[cont+SHEIGHT]/255,0.0,0.0);
            }
            glVertex3f (x+GRIDSIZE, y,byteBuffer[cont+SHEIGHT]/WAVERESOL);
            grad=byteBuffer[cont];
            if (grad >=170) {
               glColor3f (0.0,0.0,(float)byteBuffer[cont]/255);
            }
            if (grad<170 && grad>=85) {
               glColor3f (0.0,(float)byteBuffer[cont]/255,0.0);
            }
            if (grad <85) {
               glColor3f ((float)byteBuffer[cont]/255,0.0,0.0);
            }
            glVertex3f (x, y, byteBuffer[cont]/WAVERESOL);
            cont++; y+=GRIDSIZE;
         } while (y<(SHEIGHT*GRIDSIZE));
         glEnd();
         y=0.0;
         x+=GRIDSIZE;
      } while (x<((BUFSIZE/SHEIGHT)*GRIDSIZE));
}

void display(void){
   glLoadIdentity ();
   glClear (GL_COLOR_BUFFER_BIT);
      switch (angleView){
         case 1:
            //glTranslatef(1.0,150.0,-5000.0);
            glTranslatef(WHEIGHT-550,(WWIDTH/2-SHEIGHT*GRIDSIZE/2)-80,-5000.0);
            glRotatef(340.0, 1.0, 0.0, 0.0);
            glRotatef(0.0, 0.0, 0.0, 1.0);
            break;
         case 2:
            //glTranslatef(1.0,450,-5000.0);
            glTranslatef(WWIDTH/2 - SHEIGHT*2 -100,
                        WHEIGHT/2 - SHEIGHT*2 +100,-5000.0);
            glRotatef(340.0, 1.0, 0.0, 0.0);
            glRotatef(315.0, 0.0, 0.0, 1.0);
            break;
         case 3:
            glTranslatef(WWIDTH/2-SHEIGHT*GRIDSIZE/2,WHEIGHT-10*3,-5000.0);
            glRotatef(340.0, 1.0, 0.0, 0.0);
            glRotatef(270.0, 0.0, 0.0, 1.0);
            break;
      }
      switch(visMode){
         case 1:
            graphC();
            break;
         case 2:
            graphCH();
            break;
         case 3:
            graphT();
            break;
         case 4:
            graphTH();
            break;
         case 5:
            graphCH2();
            break;
         default:
            graphC();
            break;
      }
   glFlush();
   glutSwapBuffers();
}

void reshape (int w, int h){
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   if (w >= h)
      glOrtho (0.0, 800.0, 0.0, 800.0 * (GLfloat) h/(GLfloat) w,0.0,100000.0);
   else
      glOrtho (0.0, 800.0 * (GLfloat) w/(GLfloat) h, 0.0, 800.0,0.0,100000.0);
   glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
         exit(0);
         break;
      case '?':
         printf("CryptoVisualizer v1.0 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)\n");
         printf("Syntax: CryptoVisualizer.exe <fname>\n");
         printf("  Interactive command keys:\n");
         printf("   's'/'S' - Decrease/increase width of graph\n");
         printf("   'w'/'W' - Decrease/increase wave height of graph\n");
         printf("   '<'/'>' - Rotate graph graph\n");
         printf("   '+'/'-' - Move forward/bakwards file pointer\n");
         printf("   '1'-'5' - Toggle visual mode\n");
         printf("   'ESC'   - Exit program\n");
         printf("   '?'     - Print this help page\n\n");
         break;
      case '1':
         visMode=1;
         break;
      case '2':
         visMode=2;
         break;
      case '3':
         visMode=3;
         break;
      case '4':
         visMode=4;
         break;
      case '5':
         visMode=5;
         break;
      case '>':
         if (angleView<3){angleView+=1;}
         break;
      case '<':
         if (angleView>1){angleView-=1;}
         break;
      case 'S':                  //Increments Side Length of graph
         if (SHEIGHT<128){
            SHEIGHT*=2;
            DISPLACEMENT=SHEIGHT*2;
            printf ("Graph width incremented. GrWidth = %u\n",SHEIGHT);
         }
         break;
      case 's':                  //Decrements Side Length of graph
         if (SHEIGHT>2){
            SHEIGHT/=2;
            DISPLACEMENT=SHEIGHT*2;
            printf ("Graph width decremented. GrWidth = %u\n",SHEIGHT);
         }
         break;
      case 'W':                  //Increments Wave amplitude
         if (WAVERESOL>1.0){
            WAVERESOL-=1.0;
            printf ("Wave Amplitude incremented. Waveresol = %f\n",WAVERESOL);
         }
         break;
      case 'w':
         if (WAVERESOL<20.0){      //Decrements Wave amplitude
            WAVERESOL+=1.0;
            printf ("Wave Amplitude decremented. Waveresol = %f\n",WAVERESOL);
         }
         break;
      case '+':
         if (!endOfFile){
            fOffset+=DISPLACEMENT;
            loadBytes();
         }
         break;
      case '-':
         if ((fOffset-DISPLACEMENT)>=0){
            fOffset-=DISPLACEMENT;
            loadBytes();
         }
         else{
            fOffset=0;
         }
         endOfFile=0;
         break;
   }
   glutPostRedisplay();
}

int main(int argc, char** argv){
   printf ("CryptoVisualizer v1.0 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)\n");
   if (argc==2){
      filename=argv[1];
   }else{
      printf("Syntax: %s <fname>\n",argv[0]);
      printf("  Interactive command keys:\n");
      printf("   's'/'S' - Decrease/increase width of graph\n");
      printf("   'w'/'W' - Decrease/increase wave height of graph\n");
      printf("   '<'/'>' - Rotate graph graph\n");
      printf("   '+'/'-' - Move forward/bakwards file pointer\n");
      printf("   '1'-'5' - Toggle visual mode\n");
      printf("   'ESC'   - Exit program\n");
      printf("   '?'     - Print this help page\n\n");
      exit (1);
   }
   argc=1;
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (1024, 768);
   glutInitWindowPosition (0,0);
   glutCreateWindow ("CryptoVisualizer v1.0 - (c) 2008 Omar A. Herrera Reyna (oherrera@prodigy.net.mx)");
   init ();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return (0);
}
