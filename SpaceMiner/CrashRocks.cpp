#include "Glumer.h"
#include "Bubbles.h"
#include <algorithm>
#include <map>
#include <SDL.h>
#include "CrashRocks.h"

namespace SpaceMiner
{

namespace CrashRocks
{

typedef struct CRASH_PAIRtag
{
   unsigned int one; //lower of the two ids
   unsigned int other; //higher of the two ids
} CRASH_PAIR;
static bool operator <(const CRASH_PAIR &lhs, const CRASH_PAIR&rhs)
{
   return lhs.one == rhs.one ? lhs.other < rhs.other : lhs.one < rhs.one;
}
static bool operator ==(const CRASH_PAIR &lhs, const CRASH_PAIR&rhs)
{
   return lhs.one == rhs.one && lhs.other == rhs.other;
}
static std::map<CRASH_PAIR, Uint32> haveCrashed;

static inline void cGameShape_ApplyCollisionVelocity(float &velocity1, float mass1, float &velocity2, float mass2)
{
	float result1;
	result1 = velocity1 * (mass1 - mass2);
	result1 += 2.0f * mass2 * velocity2;
	result1 /= mass1+mass2;

	float result2;
	result2 = velocity2 * (mass2 - mass1);
	result2 += 2.0f * mass1 * velocity1;
	result2 /= mass1+mass2;

	velocity1 = result1;
	velocity2 = result2;
	return;
}

static inline void cGameShape_ReCalcVelocity(float x, float y, float z, float &velocity)
{
	velocity = std::max(x, y);
	velocity = std::max(velocity, z);
}

static inline void ApplyMeteorCollision(unsigned int centerId, float &centerX, float &centerY, float &centerZ, float &centerV, float centerR,
                                        unsigned int otherId, float &otherX, float &otherY, float &otherZ, float &otherV, float otherR)
{
   cGameShape_ReCalcVelocity(centerX, centerY, centerZ, centerV);
   cGameShape_ReCalcVelocity(otherX, otherY, otherZ, otherV);

   cGameShape_ReCalcVelocity(centerX, centerY, centerZ, centerV);
	cGameShape_ReCalcVelocity(otherX, otherY, otherZ, otherV);

	cGameShape_ApplyCollisionVelocity(centerX, centerR, otherX, otherR);
	cGameShape_ApplyCollisionVelocity(centerY, centerR, otherY, otherR);
	cGameShape_ApplyCollisionVelocity(centerZ, centerR, otherZ, otherR);
}

static unsigned int GetTargettedRock;
static bool IsTargettedRock;
void UnSetTarget(void)
{
   IsTargettedRock = false;
}
void SetTarget(unsigned int targettedRock)
{
   GetTargettedRock = targettedRock;
   IsTargettedRock = true;
}
void CrashMeteors(unsigned int reportToConsoleId, std::map<unsigned int, Uint32> &bulletMap, Bubbles::COLLISION_RESULT &crash, bool &isFirstBullet, bool &isSecondBullet)
{
   isFirstBullet = bulletMap.size() > 0 && bulletMap.find(crash.mCenterID) != bulletMap.end();
   isSecondBullet = bulletMap.size() > 0 && bulletMap.find(crash.mDistanceUnits[0].id) != bulletMap.end();
   if (isFirstBullet && isSecondBullet)
      return;

   SDL_Delay(0);

   Uint32 now = SDL_GetTicks();
   unsigned int cid = crash.mCenterID;
  
   if (IsTargettedRock)
      if (crash.mCenterID == GetTargettedRock || crash.mDistanceUnits[0].id == GetTargettedRock)
         //this if statement is purely a debug break point to break on collisions with targetted rocks
         if (isFirstBullet || isSecondBullet)
            Glumer::AddConsoleText(reportToConsoleId, "BOOM!", strlen("BOOM!"));

   // stop crash messages in quick succession by processing the first crash and returning/aborting quickly on subsiquent crashes
   CRASH_PAIR crashPairValue = { std::min(crash.mCenterID, crash.mDistanceUnits[0].id), std::max(crash.mCenterID, crash.mDistanceUnits[0].id) };
   if (haveCrashed.find(crashPairValue) != haveCrashed.end())
   {
      if (haveCrashed[crashPairValue] > now) // if previous crash still remembered
         return;
   }

   SDL_Delay(0);

   float cx, cy, cz, cradius; // center direction
   if (Glumer::GetDirection(cid, cx, cy, cz) == false) return;
   if (Glumer::GetRockRadius(cid, cradius) == false) return;

   unsigned int oid = crash.mDistanceUnits[0].id;
   float ox, oy, oz, oradius; // other rock direction
   if (Glumer::GetDirection(oid, ox, oy, oz) == false) return;
   if (Glumer::GetRockRadius(oid, oradius) == false) return;

   // make bullets more awesome than just rocks
   if (isFirstBullet) cradius *= 40.0f; // give them bang juice
   if (isSecondBullet) oradius *= 40.0f; 

   float cvelocity=0.0f, ovelocity=0.0f;
	ApplyMeteorCollision(cid, cx, cy, cz, cvelocity, cradius,
                        oid, ox, oy, oz, ovelocity, oradius);

   float xdist, ydist, zdist;
   xdist = crash.mDistanceUnits[0].abs_dist;
   ydist = crash.mDistanceUnits[1].abs_dist;
   zdist = crash.mDistanceUnits[2].abs_dist;
   float maxDist = std::max(std::max(xdist, ydist), zdist);
   float maxRadius = std::max(cradius, oradius);

   static const int IgnoreNextCrashUntil = 200;
   haveCrashed[crashPairValue] = now + IgnoreNextCrashUntil;

   if (Glumer::SetDirection(cid, cy, cy, cy) == false) return;
   if (Glumer::SetDirection(oid, ox, oy, oz) == false) return;
}

}

}