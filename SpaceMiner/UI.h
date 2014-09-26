#ifndef UI_H
#define UI_H

#include "GameGuts.h"
#include "STDCALL.h"

namespace SpaceMiner
{

namespace UI
{

void SetupUI(void);

void STDCALL FireAllCannons(unsigned int);
void STDCALL RockClicked(unsigned int senderId);
void STDCALL MatchTargetDirectionOnClick(unsigned int senderId, bool state);
void STDCALL CreateNewMeteorOnClick(unsigned int id);
void STDCALL ShortRangeRadarOnOffOnClick(unsigned int senderId, bool state);
bool GetRadarSwitchValue(void);

void MasterUnSetTarget(void);
void MasterSetTarget(unsigned int targettedRock);
bool IsTargettedRock(void);
unsigned int GetTargettedRock(void);
bool IsMatchTargetDirection(void);
bool GetRadarSwitchValue(void);

}

}

#endif