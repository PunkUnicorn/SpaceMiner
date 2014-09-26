#include "Glumer.h"
#include "Bubbles.h"
#include "CameraMovement.h"

namespace SpaceMiner
{

namespace CameraMovement
{

static const float piover180 = 0.0175f;

class cMovementDirection 
{
public:
   enum MovementDirection
   {
      Stop = 0,
      Forward,
      Backwards,
      Up,
      Down,
      Left,
      Right
   };
};

static cMovementDirection::MovementDirection previousDirection = cMovementDirection::Forward;
static float movementStep;
static const float movementFirst = 0.05f;
static float lookAngle;

void PointerMotionChange(unsigned int mouse_x, unsigned int mouse_y, int mouse_z)
{
	int x = mouse_x - 512;
	int y = mouse_y - 384;
}

static unsigned int movement_cameraID;
void SetCameraId(unsigned int newCameraId)
{
   movement_cameraID = newCameraId;
}

void FirstPersonStrafeRight(float step)
{
   if (previousDirection != cMovementDirection::Right)
      movementStep = movementFirst;

   Glumer::AddDirection(movement_cameraID, movementStep, 0.0f, 0.0f);
   previousDirection = cMovementDirection::Right;
   movementStep += 1.0f;
}
void FirstPersonStrafeLeft(float step)
{
   if (previousDirection != cMovementDirection::Left)
      movementStep = movementFirst;

   Glumer::AddDirection(movement_cameraID, -movementStep, 0.0f, 0.0f);
   previousDirection = cMovementDirection::Left;
   movementStep += 1.0f;
}
void FirstPersonForwardMove(float step)
{	
   if (previousDirection != cMovementDirection::Forward)
      movementStep = movementFirst;

   Glumer::AddDirection(movement_cameraID, 0.0f, 0.0f, -movementStep);
   previousDirection = cMovementDirection::Forward;
   movementStep += 1.0f;
}

void FirstPersonBackMove(float step)
{	
   if (previousDirection != cMovementDirection::Backwards)
      movementStep = movementFirst;

   Glumer::AddDirection(movement_cameraID, 0.0f, 0.0f, movementStep);
   previousDirection = cMovementDirection::Backwards;
   movementStep += 1.0f;
}
void FirstPersonDownMove(float step)
{
   if (previousDirection != cMovementDirection::Down)
      movementStep = movementFirst;

   Glumer::AddDirection(movement_cameraID, 0.0f, -movementStep, 0.0f);
   previousDirection = cMovementDirection::Down;
   movementStep += 1.0f;
}
void FirstPersonUpMove(float step)
{	
   if (previousDirection != cMovementDirection::Up)
      movementStep = movementFirst;

   Glumer::AddDirection(movement_cameraID, 0.0f, movementStep, 0.0f);
   previousDirection = cMovementDirection::Up;
   movementStep += 1.0f;
}
void FirstPersonStop(void)
{
   Glumer::SetDirection(movement_cameraID, 0.0f, 0.0f, 0.0f);
   Glumer::SetOrientation(movement_cameraID, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   previousDirection = cMovementDirection::Stop;
   movementStep = movementFirst;
   lookAngle = 0.0f;
}
void FirstPersonAngleChange(float heading_angle_x, float heading_angle_y, float forward_unit = 0.0f)
{
   Glumer::AddDirection(movement_cameraID, heading_angle_x, 0.0f, 0.0f);
}
void FirstPersonLeftTurn(float step)
{
   if (lookAngle < 0.0f)
      lookAngle = 0.0f;
   else
      lookAngle += 0.02f;

   Glumer::AddOrientation(movement_cameraID, 0.0f, 0.0f, 1.0f, 0.0f, lookAngle);
}

void FirstPersonRightTurn(float step)
{
   if (lookAngle > 0.0f)
      lookAngle = 0.0f;
   else
      lookAngle -= 0.02f;

   Glumer::AddOrientation(movement_cameraID, 0.0f, 0.0f, 1.0f, 0.0f, lookAngle);
}

}

}