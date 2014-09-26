#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <map>
#include <SDL.h>
#include "STDCALL.h"
#include "GameGuts.h"

namespace SpaceMiner
{

namespace Diagnostic
{

void STDCALL ClearMaxColumnOnClick(unsigned int id);

int STDCALL MeteorBubblesTraceFunc1(unsigned int engineId, int duration);
int STDCALL MeteorBubblesTraceFunc2(unsigned int engineId, int duration);
int STDCALL MeteorBubblesTraceFunc3(unsigned int engineId, int duration);
int STDCALL MeteorBubblesTraceFunc4(unsigned int engineId, int duration);
int STDCALL MeteorBubblesTraceFunc5(unsigned int engineId, int duration);
void STDCALL SwitchOffAll(unsigned int senderId, bool state);
void ShowDebugTimingMessageAndShit(char *code, Uint32 num, Uint32 &start, unsigned int &end, Uint32 &min, Uint32 &max, std::vector<int> &avgSource, unsigned int &avgIndex);
void ShowBulletHits(std::map<cConsoles::Consoles, unsigned int> &consoles);
void ShowStatusInfo(Uint32 functionStart);
void SetupDiagnostics(void);
void AddBulletHit(unsigned int bulletId, unsigned int theHitId);
void ShowTargetInfo(void);

class DiagnosticData
{
public:
   static std::map<unsigned int/*bullet/glumId*/, unsigned int/*hit itemId*/> bulletHits;
   //static unsigned int diagnosticConsoleId;

   static bool CLEARMAX;
   static Uint32 ClearMaxStart;
   static const int avgNum = 50;
   static bool masterSwtichOff;
};

}

}

#endif