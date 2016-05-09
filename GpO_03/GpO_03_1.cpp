/************************  GPO_03 ************************************
ATG, 2014
******************************************************************************/
#include "GpO.h"


char* WINDOW_TITLE="TEXTURAS en OpenGL (GpO)";
int CurrentWidth = 600,   CurrentHeight = 450,  WindowHandle = 0;  // Tamaño ventana, handle a ventana
unsigned FrameCount = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////     CODIGO SHADERS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GLSL(src) "#version 330 core\n" #src

const char* vertex_prog = GLSL(
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
out vec2 UV;  
uniform mat4 MVP;
void main(){	
	gl_Position =  MVP*vec4(pos,1);
	UV=uv;
}
);


const char* fragment_prog = GLSL(
in vec2 UV;
out vec3 color;
uniform sampler2D tex;
void main()
{
 color=texture(tex,UV).rgb;

}
);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

GLuint prog;
objeto obj;

objeto crear_cuadrado(void)
{
  objeto obj;
  GLuint VAO;
  GLuint buffer,i_buffer;

  // Just one square = 2 triangles = 4 vertex
  
static const GLfloat vertex_data[] = {
     0.0f,-1.0f,1.0f,    0.0f,1.0f, //0
	 0.0f,1.0f,1.0f,     1.0f,1.0f, //1
	 0.0f,1.0f,-1.0f,    1.0f,0.0f, //2
	 0.0f,-1.0f,-1.0f,   0.0f,0.0f}; //3

    GLbyte indices[]={ 0,1,2, 0,2,3}; 
	obj.Ni=2*3;
	obj.tipo_indice=GL_UNSIGNED_BYTE; 

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	
		// Defino 1er argumento (atributo 0) del vertex shader
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	// Defino 2º argumento (atributo 1) del vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);  // Asignados atributos, podemos desconectar BUFFER

	glGenBuffers(1, &i_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),indices, GL_STATIC_DRAW);
	
	// Cerramos Vertex Array con todo lidto para ser pintado. Dejamos enlazado
	// el buffer de indices (i_buffer) dentro del VAO para que cuando enlacemos 
	// el VAO para pintar este todo listo sin tener que enlaza indices
	glBindVertexArray(0);  
	
	obj.VAO=VAO; 
	
	return obj;

}



void dibujar_indexado(objeto obj)
{
  glBindVertexArray(obj.VAO);              // Activamos VAO asociado al objeto
  glDrawElements(GL_TRIANGLES,obj.Ni,obj.tipo_indice,(void*)0);  // Dibujar (indexado)
  glBindVertexArray(0);
}


// Compilación programas a ejecutar en la tarjeta gráfica:  vertex shader, fragment shaders
// Preparación de los datos de los objetos a dibujar, envialarlos a la GPU
// Opciones generales de render de OpenGL
void init_scene()  
{
  GLuint tex0,tex1;
 
  prog=LinkShaders(vertex_prog,fragment_prog); // Compile shaders, crear programa a usar, Mandar a GPU
   glUseProgram(prog);    // Indicamos que programa vamos a usar 

  obj=crear_cuadrado();  // Datos del objeto, mandar a GPU
  // Carga imagen de bmp, crea objeto textura tex0, 
  // ajusta propiedades y lo asocia a texture slot GL_TEXTURE0
  tex0=cargar_textura_from_bmp("img0.bmp",GL_TEXTURE0);  
  tex1=cargar_textura_from_bmp("img1.bmp",GL_TEXTURE1);  
}





vec3 pos_obs=vec3(4.0f,0.0f,0.0f);
vec3 target = vec3(0.0f,0.0f,0.0f);


// Dibujar objetos 
// Actualizar escena: cambiar posición objetos, nuevos objetros, posición cámara, luces, etc.
void render_scene()
{
	FrameCount++;
	float tt = glutGet(GLUT_ELAPSED_TIME)/1000.0f;  // Tiempo en segundos

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Especifica color (RGB+alfa)
	glClear(GL_COLOR_BUFFER_BIT);                   // Aplica color asignado 


	///////// Aqui vendría nuestr código para actualizar escena  /////////

	mat4 P = perspective(40.0f, 4.0f / 3.0f, 0.1f, 20.0f);  //40º Y-FOV,  4:3 ,  Znear=0.1, Zfar=20
	mat4 V = lookAt(pos_obs, target,  vec3(0,0,1) );  // Pos camara, Lookat, head up
	mat4 M,T,R,S;

	M = mat4(1.0f);

	R = rotate(60*tt,vec3(0.0f,0.0f,1.0f));

	transfer_mat4("MVP",P*V*M*R);

	if(sin(tt)>0){
		transfer_int("tex",1);
	}
	else{
		transfer_int("tex",0);
	}
	
    dibujar_indexado(obj);

	////////////////////////////////////////////////////////

	glutSwapBuffers();  // Intercambiamos buffers de refresco y display
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
//	glutSpecialFunc(key_special);  // Teclas de función, cursores, etc

//	glutMouseFunc(mouse);           // Eventos del ratón
//	glutMotionFunc(mouse_mov); // Mov del ratón con boton presionado
//	glutPassiveMotionFunc(mouse_mov); // Mov del ratón
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

void key_special(int key, int x, int y)
{
  switch (key)
	{
	  case GLUT_KEY_F1:  // Teclas de Funcion
	    break;
	  case GLUT_KEY_UP:  //Teclas cursor;  
       
		break;         
	  case GLUT_KEY_DOWN:

         break;
	  case GLUT_KEY_LEFT:
	     break;
      case GLUT_KEY_RIGHT:
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
 

void clean_up(void) // Borrar los objetos creados (programas, objetos graficos)
{
 glDeleteProgram(prog);
}