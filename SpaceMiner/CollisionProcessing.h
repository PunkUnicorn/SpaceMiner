#ifndef COLLISIONPROCESSING_H
#define COLLISIONPROCESSING_H

#include <map>
#include <SDL.h>
#include "Glumer.h"
#include "TimerWrapper.h"
#include "Bubbles.h"
#include "GameGuts.h"
#include "UI.h"
#include "CrashRocks.h"
#include "CreateRock.h"
#include "Diagnostic.h"

namespace SpaceMiner
{

// doesnt need to be a class any more now the consoles, clickables and collision engines are in game guts data
class CollisionProcessing
{
private:
   static CollisionProcessing *singleton;

   std::map<cClickables::Clickables, unsigned int> &clickables;
   std::map<cCollisionEngines::CollisionEngines, unsigned int> &collisionEngines;
   std::map<cConsoles::Consoles, unsigned int> &consoles;

public:
   CollisionProcessing(std::map<cClickables::Clickables, unsigned int> &pclickables, std::map<cCollisionEngines::CollisionEngines, unsigned int> &pcollisionEngines, std::map<cConsoles::Consoles, unsigned int> &pconsoles)
      : clickables(pclickables), collisionEngines(pcollisionEngines), consoles(pconsoles)
   {
      if (singleton != NULL) throw -999;
      singleton = this;
   }

   ~CollisionProcessing(void)
   {
      singleton = NULL;
   }

   static const int ClearInterval = 6000;
   static void DoRadarProcessing(Bubbles::COLLISION_RESULT &ping, const Uint32 now, unsigned int &longPulseStart)
   {
      if (UI::GetRadarSwitchValue()) return;

      if (UI::IsTargettedRock() && UI::GetTargettedRock() == ping.mDistanceUnits[0].id)
      {
         static char info_line[180];
         sprintf_s(info_line, "TG%i \t\t" "%.1f" "\t\t%.1f" "\t\t%.1f", ping.mDistanceUnits[0].id,
            /*X*/ ping.mDistanceUnits[0].abs_dist,
            /*Y*/ ping.mDistanceUnits[1].abs_dist,
            /*Z*/ ping.mDistanceUnits[2].abs_dist);
         Glumer::AddConsoleCode(singleton->consoles[cConsoles::RadarOne], info_line, strlen(info_line));
      }

      if (now - longPulseStart > ClearInterval)
      {
         Glumer::SetConsoleText(singleton->consoles[cConsoles::RadarOne], "", 0);
         longPulseStart = now;
      }
   
      static const Uint32 RefreshInterval = 50;
      static Uint32 refreshPulseStart = SDL_GetTicks();
      if (now - refreshPulseStart > RefreshInterval)
      {
         static char info_line[180];
         sprintf_s(info_line, "P%iP \t\t" "%.1f" "\t\t%.1f" "\t\t%.1f", ping.mDistanceUnits[0].id, 
            /*X*/ ping.mDistanceUnits[0].abs_dist, 
            /*Y*/ ping.mDistanceUnits[1].abs_dist, 
            /*Z*/ ping.mDistanceUnits[2].abs_dist);

         Glumer::AddConsoleCode(singleton->consoles[cConsoles::RadarOne], info_line, strlen(info_line));
         refreshPulseStart = now;
      }
      else
      {
         static char info_line[180];
         sprintf_s(info_line, "DATAID \t\t%i", ping.mDistanceUnits[0].id);
         Glumer::AddConsoleCode(singleton->consoles[cConsoles::RadarOne], info_line, strlen(info_line));
            
         sprintf_s(info_line, "DATA \t\t" "%.1f" "\t\t%.1f" "\t\t%.1f", 
               /*X*/ ping.mDistanceUnits[0].abs_dist, 
               /*Y*/ ping.mDistanceUnits[1].abs_dist, 
               /*Z*/ ping.mDistanceUnits[2].abs_dist);
         Glumer::AddConsoleCode(singleton->consoles[cConsoles::RadarOne], info_line, strlen(info_line));
      }
   }

   static void STDCALL DoCollisionProcessing(unsigned int groupID, unsigned int engineID, Bubbles::COLLISION_RESULT *collisionResults, unsigned int size)
   {
      Uint32 now = SDL_GetTicks();
      static Uint32 longPulseStart;
      if (longPulseStart == 0) longPulseStart = now;

      for (unsigned int i=0; i<size; i++)
      {
         if (collisionResults[i].mCenterID == GameGuts::GameGutsData::cameraID)
            DoRadarProcessing(collisionResults[i], now, longPulseStart);
         else if (collisionResults[i].mDistanceUnits[0].id == GameGuts::GameGutsData::cameraID)
            throw -500; //camera can not be crashed into
         else
         {
            bool isFirstBullet = false, isSecondBullet = false;
            CrashRocks::CrashMeteors(singleton->consoles[cConsoles::RadarOne], CreateRock::GetBulletMap(), collisionResults[i], isFirstBullet, isSecondBullet);

            // diagnostics
            if (isFirstBullet && isSecondBullet) continue;
            
            if (isFirstBullet)
               Diagnostic::AddBulletHit(collisionResults[i].mCenterID, collisionResults[i].mDistanceUnits[0].id);
            else if (isSecondBullet)
               Diagnostic::AddBulletHit(collisionResults[i].mDistanceUnits[0].id, collisionResults[i].mCenterID);
         }
      }
   
      if (now - longPulseStart > ClearInterval)
      {
         Glumer::SetConsoleText(singleton->consoles[cConsoles::RadarOne], "", 0);
         longPulseStart = now;
      }

      static Uint32 shortPulseStart;
      if (shortPulseStart == 0) shortPulseStart = now;
      static const Uint32 TrimInterval = 2000;

      if (now - shortPulseStart > TrimInterval)
      {
         Glumer::ScrollConsoleText(singleton->consoles[cConsoles::RadarOne], 50);
         shortPulseStart = now;
      }
   }

   static void MakeCollisionEngine(cCollisionEngines::CollisionEngines engineEnum, unsigned int &groupId, bool &haveMadeGroup, TraceFunc *engineTimerTrace)
   {
      singleton->collisionEngines[engineEnum] = Bubbles::AddEngine();
      if (haveMadeGroup)
         Bubbles::AddEngineToGroup(groupId, singleton->collisionEngines[engineEnum]);
      else 
      {
         groupId = Bubbles::AddEngineGroup(singleton->collisionEngines[engineEnum]);
         haveMadeGroup = true;
      }
      Bubbles::SetEngineTimerTrace(singleton->collisionEngines[engineEnum], engineTimerTrace);
   }

   static void SetupCollisionEngines(unsigned int cameraId)
   {
      // Engines one and two are in the same 'group' meaning they share the same collision list but process half each

      // cCollisionEngines MeteorOne to MeteorThree share the same list but only process a third each
      unsigned int groupId = 0;
      bool haveMadeGroup = false;
      MakeCollisionEngine(cCollisionEngines::MeteorOne, groupId, haveMadeGroup, &Diagnostic::MeteorBubblesTraceFunc1);
      MakeCollisionEngine(cCollisionEngines::MeteorTwo, groupId, haveMadeGroup, &Diagnostic::MeteorBubblesTraceFunc2);
      MakeCollisionEngine(cCollisionEngines::MeteorThree, groupId, haveMadeGroup, &Diagnostic::MeteorBubblesTraceFunc3);

      // Start the engines. Get the rhythm right
      Bubbles::StartEngine(singleton->collisionEngines[cCollisionEngines::MeteorOne], &DoCollisionProcessing, 125);
      SDL_Delay(30);
      Bubbles::StartEngine(singleton->collisionEngines[cCollisionEngines::MeteorTwo], &DoCollisionProcessing, 125);
      SDL_Delay(30);
      Bubbles::StartEngine(singleton->collisionEngines[cCollisionEngines::MeteorThree], &DoCollisionProcessing, 125);
   }

   static void SetupRadar(unsigned int cameraId)
   {
      // Put a big bubble in engine three to feed the radar data. Etheralness means it is invisible to other things but it can see objects within its own radius
      const float radarRadius = WORLD_RADIUS/3.0f;
      //Bubbles::AddBubble(singleton->collisionEngines[cCollisionEngines::MeteorOne], cameraId, radarRadius, Glumer::CallToGetCoords());
      //Bubbles::SetEtheralness(singleton->collisionEngines[cCollisionEngines::MeteorOne], cameraId, true);

   }
};

}

#endif