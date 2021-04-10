#include <GL/glut.h>
#include <iostream>
#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

GLuint pinwu;

#define SERVER "localhost" 
#define USER   "usuario"                      //
#define PASS   "contraseña"                   // REEMPLAZAR POR TU USUARIO Y CONTRASEÑA
#define DBCP   "nombre de la base de datos"   // 
// CREATE TABLE pinwinos(Pinwi_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, Username varchar(50) NOT NULL, posX INT NOT NULL, posY INT NOT NULL, rotP INT NOT NULL); -- THE DATABASE SCRIPT FOR MULTIPLAYER
#include "Res/pinwino.c"
int width = 640, height = 480;
int x1 = 320,yi = 240,x2 = 320,y2 = 240,px = 320,py = 240; int l,pa; int tmp_r;

std::string username;

std::string info_data;

int player_c;

float positionsX[50];
float positionsY[50];
int   rotationsP[50];
std::string uname[50];
                 /*  0   |  1  |  2  |  3   | 4  |  5   |  6  |  7  |*/
                 /*  LU  |  L  | LD  |  F   | RD |  R   | RU  |  B */
float indexes[8] = {0.0f,0.125f,0.25f,0.375f,0.5f,0.625f,0.75f,0.875f};

MYSQL *connect;

void screen();
void m_move(int x, int y);
void create_tex(int bpp, int w, int h, const void *data);
void RenderString(float x, float y,const char *str,float r, float g, float b);
void m_button(int button, int state, int x, int y);
void k_detect(unsigned char, int, int);
void standf();
void mysql_upd(int val);
void mysql_asm(int val);
void glIni();
void resetPrj();
void define_id();
void resz(int,int);
void setOrtho();

int main(int argc, char **argv){
 std::cout<<"Introduzca su nombre de usuario:"<<std::endl;
 std::cin>>username;
 connect = mysql_init(NULL);
 if(!connect){
  std::cout<<"could not start mysql client...\nERROR: "<<mysql_errno(connect)<<std::endl;
  exit(1);
 }
 if(mysql_real_connect(connect, SERVER, USER, PASS, DBCP, 3306, NULL, 0)){
  std::cout<<"MySQL connected correctly to server :D"<<std::endl;
  std::string req = "INSERT INTO pinwinos (Username, posX, posY, rotP) VALUES ('"+username+"',320,240,0);";
  mysql_query(connect, req.c_str());
 }else{
  std::cout<<"Could not connect to MySQL server D:"<<std::endl<<"ERROR: "<<mysql_errno(connect)<<std::endl;
 }
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(640, 480);
  glutInitWindowPosition(300,64);
  glutCreateWindow(" _ QUICK PENGUIN _ ");
 glIni();
  glutDisplayFunc(screen);
  glutPassiveMotionFunc(m_move);
  glutMouseFunc(m_button);
  glutKeyboardFunc(k_detect);
  glutReshapeFunc(resz);
  glutTimerFunc(25, mysql_upd, 0);
  glutIdleFunc(standf);
 glutMainLoop(); 
 return 0;
}

void standf(){
 usleep(33);
 glutPostRedisplay();
}

void setOrtho(){ // INICIAR EL MODO EN 2D DE OPENGL
    glMatrixMode(GL_PROJECTION); // MODO DE PROYECCION DE OPENGL
    glPushMatrix(); // INICIAR MATRIZ (PARA LOS POLIGONOS Y LOS OBJETOS QUE SE DIBUJAN EN GENERAL)
    glLoadIdentity(); // CARGAR TODOS LOS OBJETOS UNA VEZ MAS 
    gluOrtho2D(0, width, 0, height); // DEFINIR EL TIPO DE PROYECCION A ORTHOGONAL PARA 2D SIN PERSPECTIVA
    glScalef(1, -1, 1); // DEFINIR EL TAMAÑO DE LA PROYECCION
    glTranslatef(0, -height, 0); // DEFINIR EL LUGAR PARA LA PROYECCION
    glMatrixMode(GL_MODELVIEW); // VOLVER AL MODO DE MEMORIA EN OPENGL
}

void resz(int xs, int ys){ // FUNCION DE REESCALAMIENTO
    const float ar = (float) xs / (float) ys; // TAMAÑO DEL PIXEL ( ASPECT RATIO )
    width = xs; // DEFINIR EL TAMAÑO NUEVO DE LA PANTALLA
    height = ys; // DEFINIR EL TAMAÑO NUEVO DE LA PANTALLA
    glViewport(0, 0, xs, ys); // GENERAR UN VIEWPORT DE DIBUJO
    glMatrixMode(GL_PROJECTION); // CAMBIAR EL MODO A PROYECCION
    glLoadIdentity(); // CARGAR LOS OBJETOS
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0); // ARREGLAR LA RESOLUCION
    glMatrixMode(GL_MODELVIEW); // CAMBIAR EL MODO OTRA VEZ A MEMORIA
    glLoadIdentity() ; // MOSTRAR LOS OBJETOS
}

void k_detect(unsigned char key, int x, int y){
 std::string tmp;
 switch(key){
  case '.':
   tmp = "DELETE FROM pinwinos WHERE Username = '"+username+"';";
   mysql_query(connect, tmp.c_str());
   exit(1);
  break;
 }
 std::cout<<"alive\n";
}

void mysql_upd(int val){
 /*
  AQUI VA LA ACTUALIZACION DE LOS DATOS DEL USUARIO Y LA DESCARGA DE LOS DEMAS
 */
 if(x1 < x2){
  x1+=10;
 }
 if(x1 > x2){
  x1-=10;
 }
 if(yi < y2){
  yi+=10;
 }
 if(yi > y2){
  yi-=10;
 }
 
 px = x1;
 py = yi;
 std::string tmp1 = "UPDATE pinwinos SET posX = " + std::to_string(px) + " WHERE Username = '" + username + "';";
 mysql_query(connect, tmp1.c_str());
 std::string tmp2 = "UPDATE pinwinos SET posY = " + std::to_string(py) + " WHERE Username = '" + username + "';";
 mysql_query(connect, tmp2.c_str());
 MYSQL_RES *res_set;
 MYSQL_ROW row;
 mysql_query(connect, "SELECT * FROM pinwinos;");
 res_set = mysql_store_result(connect);
 player_c = mysql_num_rows(res_set);
 int tmp = 0;
 while( ( row = mysql_fetch_row(res_set) ) != NULL ){
  uname[tmp] = row[1];
  positionsX[tmp] = atof(row[2]);
  positionsY[tmp] = atof(row[3]);
  rotationsP[tmp] = atoi(row[4]);
  tmp++;
 }
 glutTimerFunc(25, mysql_upd , 0);
}


void define_id(){
 int ta = pa;
 l = 2;
 if(ta >= -22 && ta <= 22){
  l = 3;
 }
 if(ta >= 22 && ta <= 66){
  l = 4;
 }
 if(ta >= 66 && ta <= 115){
  l = 5;
 }
 if(ta >= 115 && ta <= 160){
  l = 6;
 }
 if(ta >= 160 || ta <= -160){
  l = 7;
 }
 if(ta >= -160 && ta <= -115){
  l = 0;
 }
 if(ta >= -115 && ta <= -71){
  l = 1;
 }
 std::string tmp1 = "UPDATE pinwinos SET rotP = "+std::to_string(l)+" WHERE Username = '" + username + "';";
 mysql_query(connect, tmp1.c_str());
}

void resetPrj(){ // REFRESCAR LA MATRIZ PARA DIBUJAR 
    glMatrixMode(GL_PROJECTION); // ENTRAR AL MODO PROYECCION
    glPopMatrix(); // EMPUJAR AL STACK UNA MATRIZ
    glMatrixMode(GL_MODELVIEW); // VOLVER AL MODO NORMAL
}

void m_move(int x, int y){
 /*
  la rotacion del objeto
 */
 pa = (atan2((x-px),(y-py))*180)/3.14159;
 define_id();
 //std::cout<<"move: "<<pa<<"\n";
}

void m_button(int button, int state, int x, int y){
 if(button == GLUT_LEFT_BUTTON){
  x2 = x;
  y2 = y;
 }
}

void draw_player(GLuint tex,float id){
 glPushMatrix();
 glBindTexture(GL_TEXTURE_2D,tex);
  glBegin(GL_QUADS);
   glTexCoord2f(0+id,1);
   glVertex2f(-0.1,-0.15);
   glTexCoord2f(0.125+id,1);
   glVertex2f( 0.1,-0.15);
   glTexCoord2f(0.125+id,0);
   glVertex2f( 0.1, 0.15);
   glTexCoord2f(0+id,0);
   glVertex2f(-0.1, 0.15);
  glEnd();
 glPopMatrix();
}

void create_tex(int bpp, int w, int h, const void *data){
 glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
 gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, w, h, GL_RGBA,  GL_UNSIGNED_BYTE, data);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void glIni(){
 glEnable(GL_TEXTURE_2D);
 glEnable(GL_BLEND);
 glClearColor(1.0f,1.0f,1.0f,0.0f);
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glGenTextures(1,&pinwu);
 glBindTexture(GL_TEXTURE_2D,pinwu);
 create_tex(pingu_spr.bytes_per_pixel,pingu_spr.width,pingu_spr.height, pingu_spr.pixel_data);
}

void RenderString(float x, float y,const char *str,float r, float g, float b){  
 const char *c; // VARIABLE AUXILIAR CON EL CONTENIDO
 glDisable(GL_TEXTURE_2D); // PERMITIR EL USO DE TEXTURAS EN OPENGL
 glColor3f(r, g, b); 
 glRasterPos2f(x, y); // MOVER EL TEXTO A LA LOCALIZACION QUE BUSCAMOS 
 for (c=str; *c != '\0'; c++) { // REPETIR PARA CADA LETRA DEL TEXTO
  glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c); // MOSTRAR UN CARACTER EN PANTALLA
 }
}

void screen(){
 glClear(GL_COLOR_BUFFER_BIT);
 setOrtho();
 
 for(int i = 0; i < player_c; i++){
  //std::cout<<"x: "<<positionsX[i]<<" y:"<<positionsY[i]<<" r:"<<rotationsP[i]<<std::endl;
  glPushMatrix();
   glEnable(GL_TEXTURE_2D);
    glTranslatef(positionsX[i],positionsY[i],0);
    glScalef(200,-200,0);
    tmp_r = rotationsP[i];
    draw_player(pinwu,indexes[tmp_r]);
  glPopMatrix();
 }
 for(int i = 0; i < player_c; i++){
  glPushMatrix();
   RenderString(positionsX[i]-30,positionsY[i]+30, uname[i].c_str(), 0.0f,0.0f,0.0f);
  glPopMatrix();
 }
  glPushMatrix();
   info_data = "Quick penguin server v 0.0.0.0 BETA players: " + std::to_string(player_c) + "/ 50  online. ";
   RenderString(1,15, info_data.c_str(), 0.0f,0.0f,0.0f);
   RenderString(1,30, "by: boniikawaiidesu", 0.0f,0.0f,0.0f);
  glPopMatrix();
 glEnable(GL_TEXTURE_2D);
 glFlush();
 glutSwapBuffers();
}
