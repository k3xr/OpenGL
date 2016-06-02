/************************  GPO_03 ************************************
ATG, 2014
******************************************************************************/
#include "GpO.h"


char* WINDOW_TITLE="ILUMINACION en OpenGL (GpO)";
int CurrentWidth = 600,   CurrentHeight = 450,  WindowHandle = 0;  // Tamaño ventana, handle a ventana
unsigned FrameCount = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////     CODIGO SHADERS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GLSL(src) "#version 330 core\n" #src



const char* vertex_prog1 = GLSL(       // GOURAD, LUZ LEJANA
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
out vec3 ilu;

uniform mat4 MVP;  
uniform mat4 M_normales;

uniform vec3 lightdir=vec3(1,0,0); 
const vec3 lightcolor=vec3(1.0f,1.0f,1.0f);


void main(){	
float difusa;
vec3 normal_T;

  gl_Position =  MVP*vec4(pos,1);
  
  normal_T=(M_normales*vec4(normal,0.0f)).xyz;
  normal_T=normalize(normal_T);
  
  difusa=clamp(dot(lightdir,normal_T),0.0f,1.0f);
  
  ilu=0.15+0.85*difusa*lightcolor;  //Ambiente + difusa
}
);

const char* fragment_prog1 = GLSL(
in vec3 ilu;     // Entrada = colores de vertices (interpolados en fragmentos)
out vec3 color_fragmento;
void main()
{
  color_fragmento = ilu;
}
);

//////////////////////////////////////////////////////////////////////////////////


const char* vertex_prog2 = GLSL(       // GOURAD, LUZ LEJANA
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
out vec3 normal_T;

uniform mat4 MVP;  
uniform mat4 M_normales;

void main(){	

  gl_Position =  MVP*vec4(pos,1);
  
  normal_T=(M_normales*vec4(normal,0.0f)).xyz;
  
}
);

const char* fragment_prog2 = GLSL(
in vec3 normal_T;     // Entrada = colores de vertices (interpolados en fragmentos)
out vec3 color_fragmento;

uniform vec3 lightdir; 
const vec3 lightcolor=vec3(1.0f,1.0f,0.8f);
void main()
{
	float difusa;
	vec3 nn;
  difusa=clamp(dot(lightdir,normalize(normal_T)),0,1);
  color_fragmento=0.15+0.85*difusa*lightcolor;  //Ambiente + difusa
}
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint prog1,prog2;


void clean_up(void) // Borrar los objetos creados (programas, objetos graficos)
{
 glDeleteProgram(prog2);
 glDeleteProgram(prog1);
}


// Dibuja objeto indexado
void dibujar_indexado(objeto obj)
{
  glBindVertexArray(obj.VAO);              // Activamos VAO asociado al objeto
  glDrawElements(GL_TRIANGLES,obj.Ni,obj.tipo_indice,(void*)0);  // Dibujar (indexado)
  glBindVertexArray(0);
}


// Variables globales
glm::mat4 Proy,View,M,MVP;
vec3 campos=vec3(0.0f,0.0f,4.0f);
vec3 target=vec3(0.0f,0.0f,0.0f);
float az=0.0f, elev=0.0f;
vec3 lightdir = vec3(1/sqrt(2.0f),1/sqrt(2.0f),0.0f);  //vec3(1.0f, 0.0f, 0.0f);


objeto modelo;

// Compilación programas a ejecutar en la tarjeta gráfica:  vertex shader, fragment shaders
// Preparación de los datos de los objetos a dibujar, envialarlos a la GPU
// Opciones generales de render de OpenGL
void init_scene()
{
	prog1=LinkShaders(vertex_prog1,fragment_prog1); // Compile shaders
	prog2=LinkShaders(vertex_prog2,fragment_prog2); // Compile shaders

	modelo=cargar_modelo("esfera_106_n.bix");

	Proy = glm::perspective(55.0f, 4.0f / 3.0f, 0.1f, 100.0f); 
	View = glm::lookAt(campos,target,glm::vec3(0,1,0));
	M      = mat4(1.0f);
	MVP     =  Proy * View * M; 

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(prog2);
	transfer_vec3("lightdir",lightdir);

	glUseProgram(prog1);
	transfer_vec3("lightdir",lightdir);
}



 
// Actualizar escena: cambiar posición objetos, nuevos objetros, posición cámara, luces, etc.
void render_scene()
{
	FrameCount++;
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float tt = glutGet(GLUT_ELAPSED_TIME)/1000.0f;


	glm::mat4 adjunta_M;

	glm::vec3 t=vec3(cos(tt), 0.0f, sin(tt));
	M = translate(t)*rotate(50 * tt, vec3(0.0f, 1.0f, 0.0f));   //Rotate model
	MVP        =  Proy * View * M; 
	transfer_mat4("MVP",MVP);


	adjunta_M=glm::transpose(glm::inverse(M));
	transfer_mat4("M_normales",adjunta_M);
	dibujar_indexado(modelo);



	glutSwapBuffers();
	
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////  INTERACCION  TECLADO RATON
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void keyboard(unsigned char, int, int);
void key_special(int, int, int);
void mouse(int,int, int, int);
void mouse_mov(int,int);


void eventos_teclado_mouse()
{
	glutKeyboardFunc(keyboard);      // Caso de pulsar alguna tecla
	glutSpecialFunc(key_special);  // Teclas de función, cursores, etc

//	glutMouseFunc(mouse);           // Eventos del ratón
//	glutMotionFunc(mouse_mov); // Mov del raón
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 27:  // Escape key
		  glutLeaveMainLoop(); // Salimos del bucle  
		  return;
		  break;
	}
}

int modo = 0;

void key_special(int key, int x, int y)
{
  switch (key)
	{
	  case GLUT_KEY_F1:  // Teclas de Funcion
		  if(modo == 0){
			glUseProgram(prog1);
			modo = 1;
		  }
		  else{
			  glUseProgram(prog2);
			  modo = 0;
		  }
	    break;
	  case GLUT_KEY_UP:  //Teclas cursor;  
		  elev = elev  + 0.02f;
		  lightdir =  vec3(cos(elev) * cos(az), sin(elev), cos(elev)*sin(az));
		  transfer_vec3("lightdir", lightdir);
		break;         
	  case GLUT_KEY_DOWN:
		  elev = elev - 0.02f;
		  lightdir = vec3(cos(elev) * cos(az), sin(elev), cos(elev)*sin(az));
		  transfer_vec3("lightdir", lightdir);
         break;
	  case GLUT_KEY_LEFT:
		  az = az + 0.02f;
		  lightdir = vec3(cos(elev) * cos(az), sin(elev), cos(elev)*sin(az));
		  transfer_vec3("lightdir", lightdir);
	     break;
      case GLUT_KEY_RIGHT:
		  az = az - 0.02f;
		  lightdir = vec3(cos(elev) * cos(az), sin(elev), cos(elev)*sin(az));
		  transfer_vec3("lightdir", lightdir);
		break;
	}
}

void mouse(int but,int state, int x, int y)
{
}

void mouse_mov(int x, int y)
{
	fprintf(stdout,"Pos (%3d,%3d)\n",x-CurrentWidth/2,y-CurrentHeight/2);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GLUT:  CREACION VENTANAS E INTERACCION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void Init_Opengl(void);
void Init_Window(int, char*[]);
void cambia_window(int, int);
void TimerFunction(int);
void IdleFunction(void);
void clean_up(void);


int main(int argc, char* argv[])
{
 Init_Window(argc, argv);  // Prepara y abre ventana
 Init_Opengl();            // Inicializa OpenGL, comprueba versión.
 init_scene();             // Preppara escena
 glutMainLoop();           // Entra en bucle (render_scene se ejecuta constantemente)
 exit(EXIT_SUCCESS);
}

void Init_Opengl(void)
{   
   LoadFunctions();
   fprintf(stdout,"OpenGL Version:  ",glGetString(GL_VERSION));
   if(IsVersionGEQ(3,3)) printf(">=3.3 => OK.\n");
   else
	{
	 printf("Se necesita OpenGL >= 3.3 para estos ejemplos.\n");
	 glutDestroyWindow(WindowHandle);
	 exit(EXIT_FAILURE);
	}
	
}


void Init_Window(int argc, char* argv[])
{
    glutInit(&argc, argv);
	
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA);
    glutInitContextVersion(3,3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitWindowSize(CurrentWidth, CurrentHeight);

	fprintf(stdout,"Pantalla  %d x %d pixels\n",glutGet(GLUT_SCREEN_WIDTH),glutGet(GLUT_SCREEN_HEIGHT));
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-CurrentWidth)/2,(glutGet(GLUT_SCREEN_HEIGHT)-CurrentHeight)/2);
	// Creamos ventana centrada en pantalla
   

    WindowHandle = glutCreateWindow("TITULO");
    if(WindowHandle < 1) 
	  { 
	    fprintf(stderr,"ERROR: Could not create a new rendering window.\n"); exit(EXIT_FAILURE);
      }
	else   fprintf(stderr,"Ventana creada (%d)\n",WindowHandle);


	// Asociar funciones de eventos
	glutDisplayFunc(render_scene);   //render, llamada de forma continua en el bucle
	glutReshapeFunc(cambia_window); // Caso de cambiar tamaño de ventana
    glutIdleFunc(IdleFunction);
    glutTimerFunc(200, TimerFunction, 0);
	glutCloseFunc(clean_up);


	eventos_teclado_mouse();

}

//Called whenever the window is resized. 
void cambia_window(int Width, int Height)
{
    CurrentWidth = Width; CurrentHeight = Height;
	
    glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void IdleFunction(void)
{
    glutPostRedisplay();
}
 






