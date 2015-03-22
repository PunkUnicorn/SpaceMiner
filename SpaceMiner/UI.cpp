#include <algorithm>
#include "Glumer.h"
#include "CrashRocks.h"
#include "TimerWrapper.h"
#include "GameGuts.h"
#include "STDCALL.h"
#include "CreateRock.h"
#include "Diagnostic.h"
#include "UI.h"

namespace SpaceMiner
{

namespace UI
{

void SetupUI(void)
{   
   GameGuts::GameGutsData::my_consoles[cConsoles::RadarOne] = Glumer::CreateConsole(0.004f, -16.f, 12.0f, -30.0f); // Medium scale. Back, far left. Top
   GameGuts::GameGutsData::my_consoles[cConsoles::RadarTwo] = Glumer::CreateConsole(0.004f, -8.f, 12.0f, -30.0f); // Medium scale. Back, left. Top

   GameGuts::GameGutsData::my_clickables[cClickables::TurnAllThisRubbishOff] = Glumer::CreateSwitch(0.05f, -1.42f, 1.15f, -3.0f, false, &Diagnostic::SwitchOffAll);

   GameGuts::GameGutsData::my_clickables[cClickables::TestOnOffPulseSwitch] = Glumer::CreateSwitch(0.25f, -1.0f, 1.0f, -4.5f, true, &UI::MatchTargetDirectionOnClick); // Medium scale. Left
   GameGuts::GameGutsData::my_clickables[cClickables::TestOnOffStaticSwitch] = Glumer::CreateSwitch(0.25f, -1.0f, 0.5f, -4.2f, false, &UI::ShortRangeRadarOnOffOnClick);
   GameGuts::GameGutsData::my_clickables[cClickables::CreateNewAsteroidButton] = Glumer::CreateButton(0.25f, -1.0f, 0.0f, -3.9f, &UI::CreateNewMeteorOnClick);
   GameGuts::GameGutsData::my_clickables[cClickables::FireAllCannons] = Glumer::CreateButton(0.25f, -1.0f, -0.5f, -3.6f, &UI::FireAllCannons);
}

void STDCALL FireAllCannons(unsigned int)
{
   float cameraX, cameraY, cameraZ;
   Glumer::GetLocation(GameGuts::GameGutsData::cameraID, cameraX, cameraY, cameraZ);

   float angle, camOrX, camOrY, camOrZ, camOrInc;
   Glumer::GetOrientation(GameGuts::GameGutsData::cameraID, angle, camOrX, camOrY, camOrZ, camOrInc);

   float camDirX, camDirY, camDirZ;
   Glumer::GetDirection(GameGuts::GameGutsData::cameraID, camDirX, camDirY, camDirZ);

   static const float bulletSpeed = 10.0f;
   static const float bulletRenderRadius = 1.0f;

   //front cannons only, they don't rotate with the ship yet :/
   CreateRock::NewRock(cameraX-4.0f, cameraY-4.0f, cameraZ, bulletRenderRadius, 
         -45.0f, 0.5f, 0.5f, 0.5f, -2.0f, 
         camDirX+0.0f, camDirY+0.0f, camDirZ-bulletSpeed, true);

   CreateRock::NewRock(cameraX+4.0f, cameraY-4.0f, cameraZ, bulletRenderRadius, 
         45.0f, 0.5f, 0.5f, 0.5f, 2.0f, 
         camDirX+0.0f, camDirY+0.0f, camDirZ-bulletSpeed, true);
}

static inline float GetRandomRockScale(void)
{
   return CreateRock::GetRandom(150.0f, 1000.0f)/10.0f;
}

void STDCALL CreateNewMeteorOnClick(unsigned int id)
{
	float scale = GetRandomRockScale();
	for (int yy=0; yy<10; yy++)
	{
      static const float MaxScale = 220.0f;
      if (scale > MaxScale) 
         scale = GetRandomRockScale();
		else 
         scale *= 1.5f;

      float x = CreateRock::GetRandom(-WORLD_RADIUS, WORLD_RADIUS);
      float y = CreateRock::GetRandom(-WORLD_RADIUS, WORLD_RADIUS);
      float z = CreateRock::GetRandom(-WORLD_RADIUS, WORLD_RADIUS);

      CreateRock::NewRock(x, y, z, scale, 
               /* rotation */
               /*rot angle*/CreateRock::GetRandom(0.0f, 360.0f), 
               /*rotX*/CreateRock::GetRandom(1.0f, 100.0f)/100.0f, 
               /*rotY*/CreateRock::GetRandom(1.0f, 100.0f)/100.0f, 
               /*rotZ*/CreateRock::GetRandom(1.0f, 100.0f)/100.0f, 
               /*increment*/CreateRock::GetRandom(1.0f, 100.0f)/100.0f,
               /* direction */
               /*X*/CreateRock::GetRandom(-300.0f, 300.0f)/100.0f, 
               /*Y*/CreateRock::GetRandom(-300.0f, 300.0f)/100.0f, 
               /*Z*/CreateRock::GetRandom(-300.0f, 300.0f)/100.0f, 
               false);
	}
}

static unsigned int targettedRock;
static bool isTargettedRock;
bool IsTargettedRock(void)
{
   return isTargettedRock;
}
unsigned int GetTargettedRock(void)
{
   return targettedRock;
}
void MasterUnSetTarget(void)
{
   isTargettedRock = false;
   CrashRocks::UnSetTarget();
}
void MasterSetTarget(unsigned int ptargettedRock)
{
   targettedRock = ptargettedRock;
   isTargettedRock = true;
   CrashRocks::SetTarget(targettedRock);
}
void STDCALL RockClicked(unsigned int senderId)
{
   if (isTargettedRock && senderId == targettedRock) 
   {
      TimerWrapper::cMutexWrapper::Lock lock(&GameGuts::GameGutsData::my_glumerIdListLock);
      MasterSetTarget(GameGuts::GameGutsData::cameraID);
      Bubbles::RemoveBubble(GameGuts::GameGutsData::my_collisionEngines[cCollisionEngines::MeteorOne], senderId);

      std::vector<unsigned int>::iterator intIt 
		  = std::find(GameGuts::GameGutsData::my_glumerIdList.begin(), GameGuts::GameGutsData::my_glumerIdList.end(), senderId);

      if (intIt != GameGuts::GameGutsData::my_glumerIdList.end())
      {
         std::swap(*intIt, GameGuts::GameGutsData::my_glumerIdList.back());
	      GameGuts::GameGutsData::my_glumerIdList.pop_back();
         Glumer::Delete(senderId);
      }
   }
   else 
   {
      MasterSetTarget(senderId);
   }
}
static bool matchTargetDirection;
bool IsMatchTargetDirection(void)
{
   return matchTargetDirection;
}
void STDCALL MatchTargetDirectionOnClick(unsigned int senderId, bool state)
{
	matchTargetDirection = state;
}

static bool radarSwitchValue = true;
bool GetRadarSwitchValue(void)
{
   return radarSwitchValue;
}
void STDCALL ShortRangeRadarOnOffOnClick(unsigned int senderId, bool state)
{
	radarSwitchValue = !radarSwitchValue;
}

}

}