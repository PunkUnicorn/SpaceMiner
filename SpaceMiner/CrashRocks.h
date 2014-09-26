#include <map>
#include <Sdl.h>
#include <Bubbles.h>
namespace SpaceMiner
{

namespace CrashRocks
{
// CrashRocks.cpp
void UnSetTarget(void);
void SetTarget(unsigned int targettedRock);
void CrashMeteors(unsigned int reportToConsoleId, std::map<unsigned int, Uint32> &bulletMap, Bubbles::COLLISION_RESULT &crash, bool &isFirstBullet, bool &isSecondBullet);
}

}