
/* [SDL] Dev-C++ with MinGW and MSYS 
Alfredo Solano Martinez sdl@libsdl.org 
http://www.libsdl.org/pipermail/sdl/2002-June/046382.html
(old link with old version number)

Setting up Dev-C++ with SDL and OpenGL support
DavW's Programming Page
http://davw.nfshost.com/ogltut1.php

Neon Helium productions
http://nehe.gamedev.net/


http://www.opengl.org/resources/faq/technical/polygonoffset.htm */

#include <windows.h>
#include <SDL.h>  
#include <gl/gl.h>   
#include <gl/glu.h> 

#define DllDeclspec
namespace SpaceMiner 
{

static const float WORLD_RADIUS   = 5000.0f;
static const float WORLD_DIAMITER = 10000.0f;

static const float WORLD_RADIUS_DISPLAY_CLIPPING = 500.0f;

namespace GameGuts
{
   DllDeclspec void DrawScene_Init(void);
   DllDeclspec void DrawScene_Close(void);
   DllDeclspec void DrawScene(int r, int g, int b);
   DllDeclspec void GameGuts(void);
   DllDeclspec void PointerHitTest(unsigned int mouse_x, unsigned int mouse_y, int mouse_z);
   //DllDeclspec void CollisionProcessing(void);
   //DllDeclspec void GameWorldBoundsProcessing(void);
}

namespace CameraMovement
{
   DllDeclspec void PointerMotionChange(unsigned int mouse_x, unsigned int mouse_y, int mouse_z);
   DllDeclspec void FirstPersonForwardMove(float step);
   DllDeclspec void FirstPersonBackMove(float step);
   DllDeclspec void FirstPersonUpMove(float step);
   DllDeclspec void FirstPersonDownMove(float step);
   DllDeclspec void FirstPersonStrafeLeft(float step);
   DllDeclspec void FirstPersonStrafeRight(float step);
   DllDeclspec void FirstPersonLeftTurn(float step);
   DllDeclspec void FirstPersonRightTurn(float step);
   DllDeclspec void FirstPersonStop(void);
   DllDeclspec void FirstPersonAngleChange(float world_move_x, float world_move_y, float forward_unit = 0.0f);
}

}

static void SetupWindow(void)
{
   ///* Turn on double buffering with a 24bit Z buffer.
   //* You may need to change this to 16 or 32 for your system */
   //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
 
   ///* Create our window centered at 512x512 resolution */
   //*window = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
   //   512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
   //if (!*window) /* Die if creation failed */
   //sdldie("Unable to create window");
 
   ///* Create our opengl context and attach it to our window */
   //*context = SDL_GL_CreateContext(*window);
 
   ///* This makes our buffer swap syncronized with the monitor's vertical refresh */
   //SDL_GL_SetSwapInterval(1);
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   const int xres=1024; //1152;
   const int yres=768;//864;
   SDL_Event sdl_event_data;
   bool done = false;

   SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
   SDL_WM_SetCaption("Space Miner", NULL);
   SDL_SetVideoMode(xres, yres, 0, SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF);//| SDL_FULLSCREEN);
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

   glViewport(0,0,xres,yres);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   static const float farDistance = SpaceMiner::WORLD_RADIUS - SpaceMiner::WORLD_RADIUS_DISPLAY_CLIPPING;
   gluPerspective(45.0f,(GLfloat)xres/(GLfloat)yres, 2.0f, farDistance);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
	
   //glClearColor(0.0f,0.0f,0.0f,0.0f); 
   glClearDepth(1.0f);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_DEPTH_TEST);

   // fog
   //bool   gp;						// G Pressed? ( New )
   //GLuint filter;						// Which Filter To Use
   GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };	// Storage For Three Types Of Fog
   GLuint fogfilter= 1;					// Which Fog To Use
   GLfloat fogColor[4]= {0.01f, 0.01f, 0.01f, 1.0f};		// Fog Color

   glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);			// We'll Clear To The Color Of The Fog ( Modified )

   glFogi(GL_FOG_MODE, fogMode[fogfilter]);		// Fog Mode
   glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
   glFogf(GL_FOG_DENSITY, (1.0f/(farDistance/2.0f)));				// How Dense Will The Fog Be
   glHint(GL_FOG_HINT, GL_DONT_CARE);			// Fog Hint Value
   glFogf(GL_FOG_START, 20.0f);				// Fog Start Depth
   glFogf(GL_FOG_END, farDistance*2.0f);				// Fog End Depth
   glEnable(GL_FOG);					// Enables GL_FOG

   SpaceMiner::GameGuts::DrawScene_Init();

   while (!done)
   {
      SDL_Delay(15);
      SpaceMiner::GameGuts::GameGuts();

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDepthRange (0.1f, 0.9f); 
      glPushMatrix();
         //gluLookAt ( -0.05f, 0.0f, 0.0f, -0.05f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f); 
         glMatrixMode(GL_MODELVIEW);
         gluLookAt ( -0.05f, 0.0f, 0.0f, -0.05f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f); 

         SpaceMiner::GameGuts::DrawScene(6, 207, 201);//80, 167, 80);  

         glMatrixMode(GL_PROJECTION);                           
      glPopMatrix();
      SDL_Delay(0);
      glFlush();
      SDL_GL_SwapBuffers();

      while(SDL_PollEvent(&sdl_event_data)) 
      {
         static unsigned int mouse_x;
         static unsigned int mouse_y;
         static int mouse_z=0;
         static bool is_right_down=false;
         static bool is_left_down=false;
         static bool is_middle_down=false;

         switch(sdl_event_data.type) 
         {      
            case SDL_KEYDOWN:
               switch (sdl_event_data.key.keysym.sym)
			      {
				      case SDLK_ESCAPE:
					      done=true;
					      break;

				      case SDLK_INSERT:
					      mouse_z--;
					      SpaceMiner::CameraMovement::PointerMotionChange(mouse_x, mouse_y, mouse_z);
					      break;

				      case SDLK_DELETE:
					      mouse_z++;
					      if (mouse_z > -1)
						      mouse_z = -1;

					      SpaceMiner::CameraMovement::PointerMotionChange(mouse_x, mouse_y, mouse_z);
					      break;

				      case SDLK_UP:
					      SpaceMiner::CameraMovement::FirstPersonUpMove(0.0f);
					      break;

				      case SDLK_DOWN:
					      SpaceMiner::CameraMovement::FirstPersonDownMove(0.0f);
					      break;

				      case SDLK_LEFT:
					      SpaceMiner::CameraMovement::FirstPersonStrafeLeft(0.0f);
					      break;

				      case SDLK_RIGHT:
					      SpaceMiner::CameraMovement::FirstPersonStrafeRight(0.0f);
					      break;
			
				      case SDLK_SPACE:
					      SpaceMiner::CameraMovement::FirstPersonStop();
					      break;

                  case SDLK_w:
					      SpaceMiner::CameraMovement::FirstPersonForwardMove(0.0f);
					      break;

                  case SDLK_s:
					      SpaceMiner::CameraMovement::FirstPersonBackMove(0.0f);
					      break;

                  case SDLK_a:
                     SpaceMiner::CameraMovement::FirstPersonLeftTurn(0.0f);
                     break;

                  case SDLK_d:
                     SpaceMiner::CameraMovement::FirstPersonRightTurn(0.0f);
                     break;

				      case SDLK_c:
				      case SDLK_v:
				      case SDLK_b:
				      case SDLK_n:
				      case SDLK_m:
					      break;

				      case SDLK_RALT:
					      break;

				      case SDLK_LALT:
					      break;

			      }
			      break;
                                               
            case SDL_QUIT:
               done=true;
               break;

            case SDL_MOUSEBUTTONDOWN:
			      switch (sdl_event_data.button.button)
               {
                  case SDL_BUTTON_LEFT:
                     is_left_down = true;
                     SpaceMiner::GameGuts::PointerHitTest(mouse_x, mouse_y, mouse_z);
                     break;
                                          
                  case SDL_BUTTON_RIGHT:
                     is_right_down = true;
                     SpaceMiner::GameGuts::PointerHitTest(mouse_x, mouse_y, mouse_z);
                     break;

				      case SDL_BUTTON_WHEELUP:
					      mouse_z--;
					      SpaceMiner::CameraMovement::PointerMotionChange(mouse_x, mouse_y, mouse_z);
					      break;

				      case SDL_BUTTON_WHEELDOWN:
					      mouse_z++;
					      if (mouse_z > -1)
						      mouse_z = -1;
					      SpaceMiner::CameraMovement::PointerMotionChange(mouse_x, mouse_y, mouse_z);
					      break;
                  }      
                  break;
            
            case SDL_MOUSEBUTTONUP:
               switch (sdl_event_data.button.button)
               {
                  case SDL_BUTTON_LEFT:
                        is_left_down = false;
                        break;
                                          
                  case SDL_BUTTON_RIGHT:
                        is_right_down = false;
                        break;
               }      
               break;
                  
            case SDL_MOUSEMOTION:
               mouse_x = sdl_event_data.motion.x;
               mouse_y = sdl_event_data.motion.y;

   			   SpaceMiner::CameraMovement::PointerMotionChange(mouse_x, mouse_y, mouse_z);
               break;
                                    
            default:
               break;
         }
         SDL_Delay(1);
      }
   }

    
   SpaceMiner::GameGuts::DrawScene_Close();
   SDL_Quit();
   return(0);
}
