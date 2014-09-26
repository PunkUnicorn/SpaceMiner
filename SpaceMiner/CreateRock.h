#ifndef CREATEROCK_H
#define CREATEROCK_H

namespace SpaceMiner
{

namespace CreateRock
{

float GetRandom(float min, float max);
unsigned int NewRock(float x, float y, float z, float scale, 
   float angle, float angleX, float angleY, float angleZ, float angleInc,
   float dirX, float dirY, float dirZ, bool isBullet);
std::map<unsigned int, Uint32> &GetBulletMap(void);
void ExpireBullets(void);
}

}

#endif