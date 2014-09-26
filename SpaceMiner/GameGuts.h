#ifndef GAMEGUTS_H
#define GAMEGUTS_H

#include <vector>
#include "TimerWrapper.h"

namespace SpaceMiner
{

const float WORLD_RADIUS   = 5000.0f;
const float WORLD_DIAMITER = 10000.0f;

class cClickables { public: enum Clickables {TestOnOffPulseSwitch = 0, TestOnOffStaticSwitch, CreateNewAsteroidButton, FireAllCannons, ClearMaxColumn, TurnAllThisRubbishOff }; }; 
class cCollisionEngines { public: enum CollisionEngines {MeteorOne = 0, MeteorTwo, MeteorThree, RadarOne, RadarTwo,/* RadarThree, RadarFour,*/ }; }; 
class cConsoles { public: enum Consoles {Debug = 0, Status, RadarOne, RadarTwo, Bullets }; };

namespace GameGuts
{

float GetRandom(float min, float max);
unsigned int NewRock(float x, float y, float z, float scale, 
         float angle, float angleX, float angleY, float angleZ, float angleInc,
         float dirX, float dirY, float dirZ, bool isBullet);

class GameGutsData
{
public:
   static TimerWrapper::cMutexWrapper my_glumerIdListLock;
   static std::vector<unsigned int> my_glumerIdList;

   static unsigned int cameraID;

   static std::map<cClickables::Clickables, unsigned int> my_clickables;
   static std::map<cCollisionEngines::CollisionEngines, unsigned int> my_collisionEngines;
   static std::map<cConsoles::Consoles, unsigned int> my_consoles;
};

}

}

#endif