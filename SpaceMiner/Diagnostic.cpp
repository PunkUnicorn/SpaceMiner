#include "Diagnostic.h"
#include "Glumer.h"
#include "TimerWrapper.h"
#include "Bubbles.h"
#include "GameGuts.h"
#include "UI.h"
#include <cstdlib>

namespace SpaceMiner
{

namespace Diagnostic
{

// class DiagnosticData
std::map<unsigned int/*bullet/glumId*/, unsigned int/*hit itemId*/> DiagnosticData::bulletHits;
//unsigned int DiagnosticData::diagnosticConsoleId;
bool DiagnosticData::CLEARMAX;
Uint32 DiagnosticData::ClearMaxStart;
bool DiagnosticData::masterSwtichOff;

static inline void OutputDebugTestStrip(void)
{
   // test strip
   const char *instructions = "button resets MIN and MAX\n\n";
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], instructions, strlen(instructions));
   GameGuts::GameGutsData::my_clickables[cClickables::ClearMaxColumn] = Glumer::CreateButton(0.2f, 2.3f, 1.3f, -20.0f, Diagnostic::ClearMaxColumnOnClick);
   Glumer::AddLocation(GameGuts::GameGutsData::my_clickables[cClickables::ClearMaxColumn], 5.0f, 0.8f, 0.0f);

   const char *testStrip1 = "1 ................................. 1";
   const char *testStrip2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijk";
   const char *testStrip3 = "lmnopqrstuvwxyz0123456789.-----------";
   const char *testStrip4 = "\tTAB1\tTAB2\tTAB3\tTAB4\tTAB5\tTAB6\nSLASHN\vSLASHV %&SOMEUNKNOWNS";
   const char *testStrip5 = "2 ................................. 2";
   const char *testStrip6 = "\n\n";
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], testStrip1, strlen(testStrip1));
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], testStrip2, strlen(testStrip2));
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], testStrip3, strlen(testStrip3));
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], testStrip4, strlen(testStrip4));
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], testStrip5, strlen(testStrip5));
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], testStrip6, strlen(testStrip6));

   const char * TickTitle = "\t. \tone\ttwo\tthree";
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], TickTitle, strlen(TickTitle));
   const char * Tick = "ENGTCK ";
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], Tick, strlen(Tick));

   // title
   char info_line[180];
	sprintf_s(info_line, ". \n\t\tNUM\tMIN\tCUR\tAVG\tMAX");
	Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], info_line, strlen(info_line));
}

void STDCALL SwitchOffAll(unsigned int senderId, bool state)
{
   bool off = state;
   DiagnosticData::masterSwtichOff = off;
   if (off)
   {
      Glumer::SetConsoleText(GameGuts::GameGutsData::my_consoles[cConsoles::Bullets], "", 0);
      Glumer::SetConsoleText(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], "", 0);
      Glumer::SetConsoleText(GameGuts::GameGutsData::my_consoles[cConsoles::Status], "", 0);
      Glumer::SetInvisible(GameGuts::GameGutsData::my_clickables[cClickables::ClearMaxColumn], true);
   }
   else
   {
      OutputDebugTestStrip();
      Glumer::SetInvisible(GameGuts::GameGutsData::my_clickables[cClickables::ClearMaxColumn], false);
   }
}
void STDCALL ClearMaxColumnOnClick(unsigned int id)
{
   if (DiagnosticData::masterSwtichOff) return;

   DiagnosticData::CLEARMAX = true;
   DiagnosticData::ClearMaxStart = SDL_GetTicks();
}

void SetupDiagnostics(void)
{ 
   GameGuts::GameGutsData::my_consoles[cConsoles::Status] = Glumer::CreateConsole(0.0030f, -3.5f, 6.0f, -16.0f);
   GameGuts::GameGutsData::my_consoles[cConsoles::Debug] = Glumer::CreateConsole(0.01f, 1.0f, 2.0f, -20.0f);
   GameGuts::GameGutsData::my_consoles[cConsoles::Bullets] = Glumer::CreateConsole(0.08f, -3.0f, -2.0f, -15.0f);
   OutputDebugTestStrip();
}

void ShowBulletHits(std::map<cConsoles::Consoles, unsigned int> &consoles)
{
   if (DiagnosticData::masterSwtichOff) return;

   static const unsigned int ClearEvery = 5000; //ms
   static Uint32 refreshAt = SDL_GetTicks() + ClearEvery;
   Uint32 now = SDL_GetTicks();

   std::map<unsigned int, unsigned int>::iterator bulletHiterator = DiagnosticData::bulletHits.begin();
   for (; bulletHiterator != DiagnosticData::bulletHits.end(); bulletHiterator++)
   {
      static char info_line[80];
      static int count;
   	sprintf_s(info_line, "BULLET%i \t\t\t" "%i", bulletHiterator->first, bulletHiterator->second);
	   Glumer::AddConsoleCode(consoles[cConsoles::Bullets], info_line, strlen(info_line));
      if (++count > 15)
      {
         Glumer::ScrollConsoleText(consoles[cConsoles::Bullets], 15);
         count = 0;
      }
   }

   if (refreshAt < now)
   {
      DiagnosticData::bulletHits.clear();
      refreshAt = now + ClearEvery;
      Glumer::SetConsoleText(consoles[cConsoles::Bullets], "", 0);
   }
}

void ShowDebugTimingMessageAndShit(char *code, Uint32 num, Uint32 &start, unsigned int &end, Uint32 &min, Uint32 &max, std::vector<int> &avgSource, unsigned int &avgIndex)
{
   if (DiagnosticData::masterSwtichOff) return;

   // calculate min, max and average
   char info_line[180];
   Uint32 duration = end-start;
   if (duration > max) max = duration;
   if (duration < min || min == 0) min = duration;
   if (avgIndex == DiagnosticData::avgNum) avgIndex = 0;
   int avg = 0;
   avgSource[avgIndex++] = (int) duration;

   for (std::vector<int>::iterator it = avgSource.begin(); it != avgSource.end(); it++) 
      avg += *it;
   avg /= DiagnosticData::avgNum;

   sprintf_s(info_line, "%s \t\t%i\t%i\t%i\t%i\t%i", code, num, min, duration, avg, max);
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], info_line, strlen(info_line));
   
   if (DiagnosticData::CLEARMAX)
   {
      max = min = 0;
      static const int JustOverASecond = 1111;
      Uint32 now = SDL_GetTicks();
      if (now - DiagnosticData::ClearMaxStart > JustOverASecond)
      {
         DiagnosticData::CLEARMAX = false;
      }
   }
}

int STDCALL MeteorBubblesTraceFunc1(unsigned int engineId, int duration)
{
   if (DiagnosticData::masterSwtichOff) return 0;

   char codeTok[180];
	sprintf_s(codeTok, "ENG%i", engineId);

   unsigned int start = 0;
   unsigned int end = duration;

   static unsigned int min;
   static unsigned int max;
   static std::vector<int> avgSource(DiagnosticData::avgNum);
   static unsigned int avgIndex;

   ShowDebugTimingMessageAndShit(codeTok, Bubbles::GetBubbleCount(engineId), start, end, min, max, avgSource, avgIndex);
   sprintf_s(codeTok, "ENGTCK \t\t\t\tX");
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], codeTok, strlen(codeTok));
   return 0;
}
int STDCALL MeteorBubblesTraceFunc2(unsigned int engineId, int duration)
{
   if (DiagnosticData::masterSwtichOff) return 0;

   char codeTok[180];
	sprintf_s(codeTok, "ENG%i", engineId);

   unsigned int start = 0;
   unsigned int end = duration;

   static unsigned int min;
   static unsigned int max;
   static std::vector<int> avgSource(DiagnosticData::avgNum);
   static unsigned int avgIndex;

   ShowDebugTimingMessageAndShit(codeTok, Bubbles::GetBubbleCount(engineId), start, end, min, max, avgSource, avgIndex);
   sprintf_s(codeTok, "ENGTCK \t\t\tX\t\t\t");
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], codeTok, strlen(codeTok));
   return 0;
}
int STDCALL MeteorBubblesTraceFunc3(unsigned int engineId, int duration)
{
   if (DiagnosticData::masterSwtichOff) return 0;

   char codeTok[180];
	sprintf_s(codeTok, "ENG%i", engineId);

   unsigned int start = 0;
   unsigned int end = duration;

   static unsigned int min;
   static unsigned int max;
   static std::vector<int> avgSource(DiagnosticData::avgNum);
   static unsigned int avgIndex;

   ShowDebugTimingMessageAndShit(codeTok, Bubbles::GetBubbleCount(engineId), start, end, min, max, avgSource, avgIndex);
   sprintf_s(codeTok, "ENGTCK \t\tX\t\t\t");
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], codeTok, strlen(codeTok));
   return 0;
}
int STDCALL MeteorBubblesTraceFunc4(unsigned int engineId, int duration)
{
   if (DiagnosticData::masterSwtichOff) return 0;

   char codeTok[180];
	sprintf_s(codeTok, "RAD%i", engineId);

   unsigned int start = 0;
   unsigned int end = duration;

   static unsigned int min;
   static unsigned int max;
   static std::vector<int> avgSource(DiagnosticData::avgNum);
   static unsigned int avgIndex;

   ShowDebugTimingMessageAndShit(codeTok, Bubbles::GetBubbleCount(engineId), start, end, min, max, avgSource, avgIndex);
   sprintf_s(codeTok, "RADTCK \t\tX");
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], codeTok, strlen(codeTok));
   return 0;
}
int STDCALL MeteorBubblesTraceFunc5(unsigned int engineId, int duration)
{
   if (DiagnosticData::masterSwtichOff) return 0;

   char codeTok[180];
	sprintf_s(codeTok, "RAD%i", engineId);

   unsigned int start = 0;
   unsigned int end = duration;

   static unsigned int min;
   static unsigned int max;
   static std::vector<int> avgSource(DiagnosticData::avgNum);
   static unsigned int avgIndex;

   ShowDebugTimingMessageAndShit(codeTok, Bubbles::GetBubbleCount(engineId), start, end, min, max, avgSource, avgIndex);
   sprintf_s(codeTok, "RADTCK \t\t\tX");
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Debug], codeTok, strlen(codeTok));
   return 0;
}

void AddBulletHit(unsigned int bulletId, unsigned int theHitId)
{
   if (DiagnosticData::masterSwtichOff) return;

   Diagnostic::DiagnosticData::bulletHits[bulletId] = theHitId;
}

void ShowStatusInfo(Uint32 functionStart)
{
   if (DiagnosticData::masterSwtichOff) return;

   static char info_line[180];
   float cameraX, cameraY, cameraZ;
   Glumer::GetLocation(GameGuts::GameGutsData::cameraID, cameraX, cameraY, cameraZ);
   sprintf_s(info_line, "\t\tXYZ " "\t" "%.1f" "\t\t" "%.1f" "\t\t" "%.1f", cameraX, cameraY, cameraZ);
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Status], info_line, strlen(info_line));

   float cameraDirX, cameraDirY, cameraDirZ;
   Glumer::GetDirection(GameGuts::GameGutsData::cameraID, cameraDirX, cameraDirY, cameraDirZ);
   sprintf_s(info_line, "\t\tDIR " "\t" "%.1f" "\t\t" "%.1f" "\t\t" "%.1f", cameraDirX, cameraDirY, cameraDirZ);
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Status], info_line, strlen(info_line));

   Diagnostic::ShowBulletHits(GameGuts::GameGutsData::my_consoles);

   Uint32 functionEnd = SDL_GetTicks();
   sprintf_s(info_line, "DrawScene " "\t\t\t" "%i", functionEnd - functionStart);
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Status], info_line, strlen(info_line));
}

void ShowTargetInfo(void)
{
   if (DiagnosticData::masterSwtichOff) return;

   // show target info
   float x, y, z; // location
   float orX, orY, orZ; // we three kings of orientation
   float angle, inc; // orientation angle and increment
   if (UI::IsTargettedRock())
   {
      Glumer::GetLocation(UI::GetTargettedRock(), x, y, z);
      Glumer::GetOrientation(UI::GetTargettedRock(), angle, orX, orY, orZ, inc);
   }
   else
   {
      x=y=z
         =orX=orY=orZ
            =angle=inc=0.0f;
   }

   static char info_line[180];
	sprintf_s(info_line, "TRGETLOC \t\t\t" "%.1f" "\t\t" "%.1f" "\t\t" "%.1f", x, y, z);
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Status], info_line, strlen(info_line));

	sprintf_s(info_line, "TRGETID \t\t\t" "%i", UI::GetTargettedRock());
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Status], info_line, strlen(info_line));

	sprintf_s(info_line, "TRGORAX \t\t\t" "%.1f" "\t\t" "%.1f" "\t\t" "%.1f", orX, orY, orZ);
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Status], info_line, strlen(info_line));

   sprintf_s(info_line, "TRGOANG \t\t\t" "%.1f" "\t\t" "%.1f", angle, inc);
   Glumer::AddConsoleCode(GameGuts::GameGutsData::my_consoles[cConsoles::Status], info_line, strlen(info_line));
   
}

}

}