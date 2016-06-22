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

const char* vertex_prog = GLSL(
layout(location = 0) in vec3 pos;     
layout(location = 1) in vec3 normal;

// Output data ; will be interpolated for each fragment.
out vec3 normal_t;
out vec3 pos_escena;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 M_normales;
uniform mat4 M;

void main(){
	vec4 pos2 = vec4(pos.x, pos.y, pos.z, 1.0f);
	pos_escena = (pos2*M).xyz;
	gl_Position =  MVP*vec4(pos,1);
    normal_t=normalize((M_normales*vec4(normal,0.0f)).xyz);  
}
);


const char* fragment_prog = GLSL(
in vec3 normal_t;
in vec3 pos_escena;
out vec3 color;

const vec3 lightcolor=vec3(1.0f,1.0f,1.0f);

uniform vec3 pos_luz;

void main(){
	vec3 L = (pos_luz - pos_escena);
	vec3 l=normalize(L);
	vec3 n = normalize(normal_t);
	float d = length(L);

	
	float at = 1.0f / (1.0f + 0.2f*pow(d,2));
	float I=clamp(dot(l,n),0 , 1 );
	color = 0.1+ at * I*lightcolor;
	
}
);


using namespace glm;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint prog;
mat4 Projection,View;
vec3 dir_luz;
vec3 pos_luz;
objeto modelo;


void clean_memory(void)
{
 glDeleteProgram(prog);
}

// Dibuja objeto indexado
void dibujar_indexado(objeto obj)
{
  glBindVertexArray(obj.VAO);              // Activamos VAO asociado al objeto
  glDrawElements(GL_TRIANGLES,obj.Ni,obj.tipo_indice,(void*)0);  // Dibujar (indexado)
  glBindVertexArray(0);
}

// Inicializacion escena
void init_scene()
{
	prog=LinkShaders(vertex_prog,fragment_prog); // Compile shaders

	Projection = perspective(50.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	View       = lookAt(vec3(4.0,0,1.0),vec3(-3.0,0.0,0.0),vec3(0,0,1));
	//dir_luz    = vec3(0.0f,0.7f,0.7f);
	pos_luz    = vec3(0.0f,0.0f,0.5f);

	modelo=cargar_modelo("pared_n.bix");// modelo.Ni=6;
	
	glUseProgram(prog);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}


void render_scene()
{
	FrameCount++;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	float tt = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	
	mat4 M          =  mat4(1.0f);
	mat4 MVP        =  Projection * View * M; 
	mat4 M_normales=transpose(inverse(M));
	transfer_mat4("M", M);
	transfer_mat4("MVP",MVP);
	transfer_mat4("M_normales",M_normales);
	transfer_vec3("pos_luz", pos_luz);
	//transfer_vec3("pos_luz", pos_luz);

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
	glutMouseFunc(mouse);           // Eventos del ratón
	glutSpecialFunc(key_special);  // Teclas de función, cursores, etc
//	glutPassiveMotionFunc(mouse_mov); // Mov del raón
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 27:
		  glutLeaveMainLoop();
		  return;

	}
}

void key_special(int key, int x, int y)
{
  switch (key)
	{
	  case GLUT_KEY_F1:  // F1
		   printf("F1\n");
	    break;
	  case GLUT_KEY_UP:
		
         break;
	  case GLUT_KEY_DOWN:
		
         break;
	  case GLUT_KEY_LEFT:
		pos_luz = pos_luz + vec3(0.0f, -0.05f, 0.0f);
		fprintf(stdout, "new position is (%.2f, %.2f , %.2f) \n", pos_luz.x, pos_luz.y, pos_luz.z);
	     break;
      case GLUT_KEY_RIGHT:
		  pos_luz = pos_luz + vec3(0.0f, 0.05f, 0.0f);
		  fprintf(stdout, "new position is (%.2f, %.2f , %.2f) \n ", pos_luz.x, pos_luz.y, pos_luz.z);
		break;
	}

}


int ini_x,ini_y;

void mouse(int but,int state, int x, int y)
{
	if( 4 == but && 0 == state )
	{
		pos_luz = pos_luz + vec3(0.0f, 0.0f, 0.05f);
		fprintf(stdout, "new position is (%.2f, %.2f , %.2f) \n", pos_luz.x, pos_luz.y, pos_luz.z);
	}
	if (3 == but && 0 == state)
	{
		pos_luz = pos_luz + vec3(0.0f, 0.0f, -0.05f);
		fprintf(stdout, "new position is (%.2f, %.2f , %.2f) \n", pos_luz.x, pos_luz.y, pos_luz.z);
	}
	int k = 0;
}

void mouse_mov(int x, int y)
{

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GLUT:  CREACION VENTANAS E INTERACCION
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void reshape_window(int, int);
void TimerFunction(int);
void IdleFunction(void);



int main(int argc, char* argv[])
{
 Initialize(argc, argv);
 init_scene();
 glutMainLoop();
 clean_memory();
 exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
   InitWindow(argc, argv);
   
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


void InitWindow(int argc, char* argv[])
{
    glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitContextVersion(3,3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutInitWindowSize(CurrentWidth, CurrentHeight);

	fprintf(stdout,"Pantalla  %d x %d pixels\n",glutGet(GLUT_SCREEN_WIDTH),glutGet(GLUT_SCREEN_HEIGHT));
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-CurrentWidth)/2,(glutGet(GLUT_SCREEN_HEIGHT)-CurrentHeight)/2);
	// Creamos ventana centrada en pantalla
   

    WindowHandle = glutCreateWindow("PRIMERA VENTANA");
    if(WindowHandle < 1) 
	  { 
	    fprintf(stderr,"ERROR: Could not create a new rendering window.\n"); exit(EXIT_FAILURE);
      }
	else   fprintf(stderr,"Ventana creada (%d)\n",WindowHandle);


	// Asociar funciones de eventos
	glutDisplayFunc(render_scene);   //render, llamada de forma continua en el bucle
	glutReshapeFunc(reshape_window); // Caso de cambiar tamaño de ventana
    glutIdleFunc(IdleFunction);
    glutTimerFunc(0, TimerFunction, 0);


	eventos_teclado_mouse();

}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape_window(int Width, int Height)
{
    CurrentWidth = Width;
    CurrentHeight = Height;
    glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void IdleFunction(void)
{
    glutPostRedisplay();
}
 







