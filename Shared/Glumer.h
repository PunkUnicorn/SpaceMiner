// Glumer.h
// Copyright (c) 2014
// Author: Matthew Cocks
// License: Attribution 4.0 International (CC BY 4.0)
#ifndef GLUMER_H
#define GLUMER_H

#include "STDCALL.h"
#include "DLL_PUBLIC.h"
#include "CoordCallback.h"

namespace Glumer 
{

typedef void STDCALL GlumerOnClickedBool(unsigned int/*raiser*/, bool state);
typedef void STDCALL GlumerOnClicked(unsigned int/*raiser*/);

extern "C" DLL_PUBLIC bool STDCALL InitGlumer(void);
extern "C" DLL_PUBLIC void STDCALL UnInitGlumer(void);

extern "C" DLL_PUBLIC bool STDCALL GetLocation(unsigned int id, float &x, float &y, float &z);
extern "C" DLL_PUBLIC bool STDCALL SetLocation(unsigned int id, float x, float y, float z);
extern "C" DLL_PUBLIC bool STDCALL AddLocation(unsigned int id, float x, float y, float z);

extern "C" DLL_PUBLIC bool STDCALL GetDirection(unsigned int id, float &x, float &y, float &z);
extern "C" DLL_PUBLIC bool STDCALL SetDirection(unsigned int id, float x, float y, float z);
extern "C" DLL_PUBLIC bool STDCALL AddDirection(unsigned int id, float x, float y, float z);

extern "C" DLL_PUBLIC bool STDCALL GetOrientation(unsigned int id, float &angle, float &x, float &y, float &z, float &angleIncrement);
extern "C" DLL_PUBLIC bool STDCALL SetOrientation(unsigned int id, float angle, float x, float y, float z, float angleIncrement);
extern "C" DLL_PUBLIC bool STDCALL AddOrientation(unsigned int id, float angle, float x, float y, float z, float angleIncrement);

extern "C" DLL_PUBLIC bool STDCALL SetInvisible(unsigned glumId, bool visible);

// CreateRock
/*depreciated*/ //extern "C" DLL_PUBLIC unsigned int STDCALL CreateRock(float scale, float x, float y, float z, GlumerOnClicked *onClicked);
/*depreciated*/ //extern "C" DLL_PUBLIC bool STDCALL SetRockOnClicked(unsigned int id, GlumerOnClicked *onClicked);
/*depreciated*/ //extern "C" DLL_PUBLIC bool STDCALL GetRockRadius(unsigned int id, float &radius);

typedef int PolyhedronType;
struct cPolyhedronType 
{ 
	static PolyhedronType const Unknown = 0; 
	static PolyhedronType const Cube = 400; 
	static PolyhedronType const Octahedron = 800; 
};

// CreatePolyhedron
extern "C" DLL_PUBLIC unsigned int STDCALL CreatePolyhedron(float scale, PolyhedronType type, float x, float y, float z, GlumerOnClicked *onClicked);
extern "C" DLL_PUBLIC bool STDCALL SetPolyhedronOnClicked(unsigned int id, GlumerOnClicked *onClicked);
extern "C" DLL_PUBLIC bool STDCALL GetPolyhedronRadius(unsigned int id, float &radius);

// CreateBullet
/*depreciated*/ //extern "C" DLL_PUBLIC unsigned int STDCALL CreateBullet(float scale, float x, float y, float z, float dirX, float dirY, float dirZ);

// CreateSwitch
extern "C" DLL_PUBLIC unsigned int STDCALL CreateSwitch(float scale, float whereX, float whereY, float whereZ, bool pulseWhenOn, GlumerOnClickedBool *onClicked);

// CreateButton
extern "C" DLL_PUBLIC unsigned int STDCALL CreateButton(float scale, float whereX, float whereY, float whereZ, GlumerOnClicked *onClicked);

// CreateConsole
extern "C" DLL_PUBLIC unsigned int STDCALL CreateConsole(float scale, float whereX, float whereY, float whereZ);
// Replaces all text in console with text
extern "C" DLL_PUBLIC bool STDCALL SetConsoleText(unsigned int id, const char *text, unsigned int text_size);
// 'code' is the first word in the string (up to a space). Replaces equivalent code line in console text e.g. "XYZ %i" would replace existing line in console starting "XYZ "
extern "C" DLL_PUBLIC bool STDCALL AddConsoleCode(unsigned int id, const char *text, unsigned int text_size);
// Adds the line to the bottom of console text
extern "C" DLL_PUBLIC bool STDCALL AddConsoleText(unsigned int id, const char *text, unsigned int text_size);
// Crops the oldest (top) lines so only 'lines' amount remains
extern "C" DLL_PUBLIC bool STDCALL ScrollConsoleText(unsigned int id, unsigned int lines);

// CreateCamera
extern "C" DLL_PUBLIC unsigned int STDCALL CreateCamera(void);

// Callback for the collision engine Bubbles to get items coordinates
extern "C" DLL_PUBLIC GetCoordsFunc* STDCALL CallToGetCoords(void); 

// Delete a glumer item created with Create... functions
extern "C" DLL_PUBLIC bool STDCALL Delete(unsigned int deleteId);

extern "C" DLL_PUBLIC void STDCALL Start(unsigned int cameraId);
extern "C" DLL_PUBLIC void STDCALL DrawScene(int r, int g, int b);
extern "C" DLL_PUBLIC void STDCALL HitTest(unsigned int mouse_x, unsigned int mouse_y, int mouse_z);
extern "C" DLL_PUBLIC unsigned int STDCALL GetRenderCount(void);

}

#endif