// GPO_00.cpp (2016)
// Solo prueba la correcta compilaci�n del proyecto
// Crea una ventana y cambia su color
// Vuelca un par de matrices de giro y escalado usando glm

#include "./GpO.h"


char* WINDOW_TITLE = "OPENGL (GpO)";
int CurrentWidth = 600,   CurrentHeight = 450,  WindowHandle = 0;  // Tama�o ventana, handle a ventana
unsigned FrameCount = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   RENDER CODE AND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Compilaci�n programas a ejecutar en la tarjeta gr�fica:  vertex shader, fragment shaders
// Preparaci�n de los datos de los objetos a dibujar, envialarlos a la GPU
// Opciones generales de render de OpenGL
void init_scene()  
{	

 glm::mat4 M;
 M = glm::scale(2.0f,1.5f,0.5f); 
 printf("---------------------------------------------\n");
 printf("Matriz de escalado usando libreria GLM \n");
 vuelca_mat4(M);

 M = glm::rotate(30.0f,vec3(0.0f,0.0f,1.0f));
 printf("---------------------------------------------\n");
 printf("Matriz de giro usando libreria GLM \n");
 vuelca_mat4(M);



}


// Dibujar objetos 
// Actualizar escena: cambiar posici�n objetos, nuevos objetros, posici�n c�mara, luces, etc.
void render_scene()
{
	FrameCount++;
	float tt = glutGet(GLUT_ELAPSED_TIME)/1000.0f;  // Tiempo en segundos

	glClearColor((1 + cos(tt)) / 2.0f, (1 + sin(tt)) / 2.0f, 0.0f, 0.0f);  // Especifica color (RGB+alfa)
	glClear(GL_COLOR_BUFFER_BIT);                   // Aplica color asignado 

	///////// Aqui vendr�a nuestr c�digo para actualizar escena  /////////
	
	
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

void asignacion_eventos_teclado_mouse()
{
	glutKeyboardFunc(keyboard);      // Caso de pulsar alguna tecla
	//	glutSpecialFunc(key_special);  // Teclas de funci�n, cursores, etc
	//	glutMouseFunc(mouse);           // Eventos del rat�n
	//	glutPassiveMotionFunc(mouse_mov); // Mov del ra�n
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 27:  // Escape key
		  glutLeaveMainLoop(); // Salimos del bucle  
		  return;
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
 Init_Opengl();            // Inicializa OpenGL, comprueba versi�n.
 init_scene();             // Preppara escena
 glutMainLoop();           // Entra en bucle (render_scene se ejecuta constantemente)
 exit(EXIT_SUCCESS);
}

void Init_Opengl(void)
{   
   LoadFunctions();
   fprintf(stdout,"OpenGL Version:  %s ",glGetString(GL_VERSION));
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
	
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
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
	glutReshapeFunc(cambia_window); // Caso de cambiar tama�o de ventana
    glutIdleFunc(IdleFunction);
    glutTimerFunc(200, TimerFunction, 0);
	glutCloseFunc(clean_up);

	// Eventos de teclado

	asignacion_eventos_teclado_mouse();


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
 
// Borrar los objetos creados (programas, objetos graficos)
void clean_up(void)
{
}

////////////////////// AUXILIARES ///////////////////////////////////////////////

