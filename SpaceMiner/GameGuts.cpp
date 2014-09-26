#include "Glumer.h"
#include "TimerWrapper.h"
#include "Bubbles.h"
#include <random>
#include <algorithm>
#include <map>
#include <cstdlib>

#include "CameraMovement.h"
#include "CrashRocks.h"
#include "CreateRock.h"
#include "UI.h"
#include "STDCALL.h"
#include "Diagnostic.h"
#include "CollisionProcessing.h"

namespace SpaceMiner
{

namespace GameGuts
{

static void CreateTestRocks(void)
{
   CreateRock::NewRock(0.0f, 0.0f, 0.0f, 30.0f,
            0.0f, 1.0f, 0.9f, 0.8f, 0.75f,
            0.0f, 0.0f, 0.001f, false);

   CreateRock::NewRock(16.0f, 15.0f, -19.0f, 10.0f, 
           0.0f, 0.25f, 0.75f, 1.0f, 0.25f,
           0.002f, 0.0005f, 0.001f, false);

   CreateRock::NewRock(-4.0f, -3.0f, -30.0f, 10.0f,
            0.0f, 0.05f, 1.0f, 4.0f, 0.25f,
            0.0f, 0.0f, 0.005f, false);
}

TimerWrapper::cMutexWrapper GameGutsData::my_glumerIdListLock;
std::vector<unsigned int> GameGutsData::my_glumerIdList;
unsigned int GameGutsData::cameraID;
std::map<cClickables::Clickables, unsigned int> GameGutsData::my_clickables;
std::map<cCollisionEngines::CollisionEngines, unsigned int> GameGutsData::my_collisionEngines;
std::map<cConsoles::Consoles, unsigned int> GameGutsData::my_consoles;


static CollisionProcessing *collisionProcessing;

/*
 * 
 * Main entry point
 *
 */
void DrawScene_Init(void)
{
   Glumer::InitGlumer();
   Bubbles::InitBubbles(false);

   GameGutsData::cameraID = Glumer::CreateCamera();
   Glumer::AddLocation(GameGutsData::cameraID, 0.0f, 0.0f, 2000.0f);
   CameraMovement::SetCameraId(GameGutsData::cameraID);

   UI::SetupUI();
   Diagnostic::SetupDiagnostics();

   collisionProcessing = new CollisionProcessing(GameGutsData::my_clickables, GameGutsData::my_collisionEngines, GameGutsData::my_consoles);   
   CollisionProcessing::SetupCollisionEngines(GameGutsData::cameraID);

   CreateTestRocks();

   Glumer::Start(GameGutsData::cameraID);
   SDL_Delay(200);
}

void DrawScene_Close(void)
{
   Bubbles::UnInitBubbles();
   Glumer::UnInitGlumer();
   delete collisionProcessing;
   collisionProcessing = NULL;
}

static void FPSDebugMessage(unsigned int &frameCount)
{
   static unsigned int frameStart;

   unsigned int now = SDL_GetTicks();
   static const int oneSecond = 1000;
   if (frameStart == 0 || now-frameStart > oneSecond)
   {
      static unsigned int frameMin;
      static unsigned int frameMax;
      static std::vector<int> frameAvgSource(Diagnostic::DiagnosticData::avgNum);
      static unsigned int frameAvgIndex;
      unsigned int start = 0;
      unsigned int end = frameCount;
      Diagnostic::ShowDebugTimingMessageAndShit("FPS", frameCount, start, end, frameMin, frameMax, frameAvgSource, frameAvgIndex);

      frameCount = 0;
      frameStart = SDL_GetTicks();
   }
}


void DrawScene(int r, int g, int b)
{
   Uint32 functionStart = SDL_GetTicks();

   static unsigned int frameCount;
   FPSDebugMessage(frameCount);
   frameCount++;

   static unsigned int min;
   static unsigned int max;
   static std::vector<int> avgSource(Diagnostic::DiagnosticData::avgNum);
   static unsigned int avgIndex;

   unsigned int start = SDL_GetTicks();
   Glumer::DrawScene(r, g, b);
   unsigned int end = SDL_GetTicks();

   Diagnostic::ShowDebugTimingMessageAndShit("RENDR", Glumer::GetRenderCount(), start, end, min, max, avgSource, avgIndex);
   Diagnostic::ShowStatusInfo(functionStart);
}


static inline void CheckGameShapeBounds(unsigned int id)
{
   float worldX, worldY, worldZ;
   if (Glumer::GetLocation(GameGutsData::cameraID, worldX, worldY, worldZ) == false) return;
   
   float x, y, z;
   if (Glumer::GetLocation(id, x, y, z) == false) return;

   bool add = false;
   float addX, addY, addZ;
   addX = addY = addZ = 0.0f;


   // X
   if (x - worldX > WORLD_RADIUS)
   {
      addX = (-WORLD_DIAMITER);
      add = true;
   }
   else if ( x - worldX < -WORLD_RADIUS)
   {
      addX = (WORLD_DIAMITER);
      add = true;
   }

   // Y
   if (y - worldY > WORLD_RADIUS)
   {
      addY = (-WORLD_DIAMITER);
      add = true;
   }
   else if ( y - worldY < -WORLD_RADIUS)
   {
      addY = (WORLD_DIAMITER);
      add = true;
   }

   // Z
   if (z - worldZ > WORLD_RADIUS)
   {
      addZ = (-WORLD_DIAMITER);
      add = true;
   }
   else if ( z - worldZ < -WORLD_RADIUS)
   {
      addZ = (WORLD_DIAMITER);
      add = true;
   }

   if (add)
      Glumer::AddLocation(id, addX, addY, addZ);
}

static inline void CheckMeteorBound(unsigned int id)
{   
	CheckGameShapeBounds(id);
}

static void GameWorldBoundsProcessing(void)
{
   TimerWrapper::cMutexWrapper::Lock lock(&GameGutsData::my_glumerIdListLock);
   std::for_each(GameGutsData::my_glumerIdList.begin(), GameGutsData::my_glumerIdList.end(), CheckMeteorBound);
}

void GameGuts(void)
{
	GameWorldBoundsProcessing();
   SDL_Delay(0);

   CreateRock::ExpireBullets();
  
   // react to 'match target speed' button
   if (UI::IsTargettedRock() && UI::IsMatchTargetDirection())
   {
      float x, y, z;
      Glumer::GetDirection(UI::GetTargettedRock(), x, y, z);
      Glumer::SetDirection(GameGutsData::cameraID, x, y, z);
   }

   Diagnostic::ShowTargetInfo();
}

void PointerHitTest(unsigned int mouse_x, unsigned int mouse_y, int mouse_z)
{
   static char info_line[180];
   static unsigned int min;
   static unsigned int max;
   static std::vector<int> avgSource(Diagnostic::DiagnosticData::avgNum);
   static unsigned int avgIndex;
   
   SDL_Delay(0);

   unsigned int start = SDL_GetTicks();
   Glumer::HitTest(mouse_x, mouse_y, mouse_z);
   unsigned int end = SDL_GetTicks();
   Diagnostic::ShowDebugTimingMessageAndShit("CLICK", 0, start, end, min, max, avgSource, avgIndex);

   SDL_Delay(0);
}

static void MakeRadars(void)
{
	////radars
	////cGameShape_HUDShortRadar::PTR my_radar1;
	//my_radar1 = shape_factory.NewHUDShortRadar(unversal_hud_colour, 500.0f, 0.005f);
	//my_obj.ptr = my_radar1.ptr;
	//ShowistAdd(my_obj);

	//my_radar1.ptr->GetCenterWrite().FactorySetX(0.0f);
	//my_radar1.ptr->GetCenterWrite().FactorySetY(0.5f);
	//my_radar1.ptr->GetCenterWrite().FactorySetZ(-8.4f);

	//my_radar1.ptr->mOrientation.FactorySetAngleX(1.0f);
	//my_radar1.ptr->mOrientation.FactorySetAngleY(0.0f);
	//my_radar1.ptr->mOrientation.FactorySetAngleZ(0.0f);
	//my_radar1.ptr->mOrientation.FactorySetAngle(25.0f);
	//my_radar1.ptr->mOrientation.FactorySetAngleIncrement(0.0f);

	//my_radar1.ptr->GetDirectionWrite().FactorySetX(0.0f);
	//my_radar1.ptr->GetDirectionWrite().FactorySetY(0.0f);
	//my_radar1.ptr->GetDirectionWrite().FactorySetZ(0.0f);


	////cGameShape_HUDLongRadar::PTR my_radar2;
	//my_radar2 = shape_factory.NewHUDLongRadar(unversal_hud_colour, 1000.0f, 0.005f);
	//my_obj.ptr = my_radar2.ptr;
	////ShowistAdd(my_obj);

	//my_radar2.ptr->GetCenterWrite().FactorySetX(0.0f);
	//my_radar2.ptr->GetCenterWrite().FactorySetY(0.5f);
	//my_radar2.ptr->GetCenterWrite().FactorySetZ(-8.4f);

	//my_radar2.ptr->mOrientation.FactorySetAngleX(1.0f);
	//my_radar2.ptr->mOrientation.FactorySetAngleY(0.0f);
	//my_radar2.ptr->mOrientation.FactorySetAngleZ(0.0f);
	//my_radar2.ptr->mOrientation.FactorySetAngle(25.0f);
	//my_radar2.ptr->mOrientation.FactorySetAngleIncrement(0.0f);

	//my_radar2.ptr->GetDirectionWrite().FactorySetX(0.0f);
	//my_radar2.ptr->GetDirectionWrite().FactorySetY(0.0f);
	//my_radar2.ptr->GetDirectionWrite().FactorySetZ(0.0f);
}
}

}

