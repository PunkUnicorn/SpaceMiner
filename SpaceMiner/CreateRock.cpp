#include <map>
#include <SDL.h>
#include <algorithm>
#include "Glumer.h"
#include "TimerWrapper.h"
#include "Bubbles.h"
#include "GameGuts.h"
#include "UI.h"

namespace SpaceMiner
{

namespace CreateRock
{

static void SeedRandom(void)
{
   static bool seeded;
   if (seeded == false) 
   {  
      seeded = true;
      std::srand(SDL_GetTicks());
   }
}

float GetRandom(float min, float max)
{
   SeedRandom();
   float difference = max - min;

   int r = std::rand() % ( (int) difference + 1);
   float value = (float) r + min;

   return value;
}

static std::map<unsigned int/*bullet/glumId*/, Uint32 /*active untill*/> bulletMap;
std::map<unsigned int, Uint32> &GetBulletMap(void)
{
   return bulletMap;
}

unsigned int NewRock(float x, float y, float z, float scale, 
                                   float angle, float angleX, float angleY, float angleZ, float angleInc,
                                   float dirX, float dirY, float dirZ, 
                                   bool isBullet)
{
   // Add the rock to the renderer
   unsigned int id;
   bool setIt = true;
   if (isBullet)
   {
      //id = Glumer::CreateBullet(scale, x, y, z, dirZ, dirY, dirZ);
	   id = Glumer::CreatePolyhedron(scale, Glumer::cPolyhedronType::Octahedron, x, y, z, NULL);
   }
   else
   {
      //id = Glumer::CreateRock(scale, x, y, z, &UI::RockClicked);
	  id = Glumer::CreatePolyhedron(scale, Glumer::cPolyhedronType::Cube, x, y, z, &UI::RockClicked);
   }

   setIt = setIt && Glumer::SetOrientation(id, angle, angleX, angleY, angleZ, angleInc);
   setIt = setIt && Glumer::SetDirection(id, dirX, dirY, dirZ);

   if (setIt)
   {
      TimerWrapper::cMutexWrapper::Lock lock(&GameGuts::GameGutsData::my_glumerIdListLock);
      GameGuts::GameGutsData::my_glumerIdList.push_back(id);
   }
   else
      return 0;

   static const int BulletExpireIn = 5000; //ms
   if (isBullet)
      bulletMap[id] = SDL_GetTicks() + BulletExpireIn;

   // Add the rock to the collision engine
   static int engineIndex;
   unsigned int listIndex = cCollisionEngines::MeteorOne + engineIndex;
   Bubbles::AddBubble(GameGuts::GameGutsData::my_collisionEngines[(cCollisionEngines::CollisionEngines)listIndex], id, scale, Glumer::CallToGetCoords());
   engineIndex++;
   if (engineIndex == 3) engineIndex = 0;

   return id;
}

void ExpireBullets(void)
{
   // expire bullets
   Uint32 now = SDL_GetTicks();
   std::map<unsigned int, Uint32>::iterator it = CreateRock::GetBulletMap().begin();
   std::vector<unsigned int> removeList;
   for (; it != CreateRock::GetBulletMap().end(); it++)
   {
      if (it->second < now) // delete if expired
      {
         Bubbles::RemoveBubble(GameGuts::GameGutsData::my_collisionEngines[cCollisionEngines::MeteorOne], it->first);

         TimerWrapper::cMutexWrapper::Lock lock(&GameGuts::GameGutsData::my_glumerIdListLock);
         std::vector<unsigned int>::iterator intIt = std::find(GameGuts::GameGutsData::my_glumerIdList.begin(), GameGuts::GameGutsData::my_glumerIdList.end(), it->first);
         if (intIt != GameGuts::GameGutsData::my_glumerIdList.end())
         {
            std::swap(*intIt, GameGuts::GameGutsData::my_glumerIdList.back());
            GameGuts::GameGutsData::my_glumerIdList.pop_back();
            Glumer::Delete(it->first);
            removeList.push_back(it->first);
         }
      }
   }

   std::vector<unsigned int>::iterator removeIt = removeList.begin();
   for (; removeIt != removeList.end(); removeIt++)
   {
      std::map<unsigned int, Uint32>::iterator findIt = CreateRock::GetBulletMap().find(*removeIt);
      if (findIt == CreateRock::GetBulletMap().end()) return;
      CreateRock::GetBulletMap().erase(findIt);
   }
}

}

}