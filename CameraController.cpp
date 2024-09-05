#include "CameraController.h"
#include "MinOgreApp.h"

#include <cmath>
#include <iostream>

#include <Bites/OgreInput.h>
#include <Ogre.h>

using namespace Ogre;

void CameraController::setTargetSpeed(const Vector3& speed)
{
  from_speed = interpolate(from_speed, target_speed);
  elapsed_time = 0.0;
  target_speed = speed;
}


void CameraController::moveCamera(const Vector3& displacement)
{
  auto app = app_weak.lock();
  if(!app) {
    return;
  }
  auto node = app->getCameraNode();
  auto pos = node->getPosition() + displacement;
  node->setPosition(std::move(pos));
//  node->lookAt(Vector3::ZERO, Node::TransformSpace::TS_WORLD);
}

void CameraController::update(double dt_sec)
{
  if(elapsed_time > total_time && target_speed.isZeroLength()) {
    return;
  }

  if(elapsed_time < total_time) {
    // current_factor = 2t(1-t) * overshoot + t^2
    // t = elapsed_time / total_time
    elapsed_time += dt_sec;
  }

  auto current_speed = interpolate(from_speed, target_speed);
  moveCamera(dt_sec * std::move(current_speed));
}

Vector3 CameraController::interpolate(
    const Vector3& from, const Vector3& to) const
{
  if(elapsed_time > total_time) {
    return to;
  }
  // current_speed = A_0 * from_speed + (A_1 * overshoot + A_2 * 1.0) * target_speed;
  // current_speed = (1-t)^2 from_speed + (2t(1-t) * overshoot + t^2) * target_speed;
  // t = elapsed_time / total_time
  const auto t = elapsed_time / total_time;
  const auto a = (1.-t)*(1.-t);
  const auto b = 2.*t*(1.-t) * factor_overshooting + t*t;
  return a * from + b * to;
}


bool CameraController::keyPressed(const OgreBites::KeyboardEvent& evt)
{
  if(evt.repeat) {
    return false;
  }

std::cerr << "Event key press (" << evt.keysym.sym << ")\n";
  switch(evt.keysym.sym) {
  case 'r':
    {
      auto app = app_weak.lock();
      if(!app) {
        break;
      }
      auto node = app->getMeshNode();
      node->rotate(Vector3{0,1,0}, Radian{.2}, Node::TransformSpace::TS_PARENT);
    }
    break;
  case 's':
    setTargetSpeed({0,0,0});
    break;
  case 'b':
    setTargetSpeed({0,0,10});
    break;
  case 'f':
    setTargetSpeed({0,0,-10});
    break;
  case OgreBites::SDLK_RIGHT:
    setTargetSpeed({-100,0,0});
    break;
  case OgreBites::SDLK_LEFT:
    setTargetSpeed({100,0,0});
    break;
  case OgreBites::SDLK_UP:
    setTargetSpeed({0,-100,0});
    break;
  case OgreBites::SDLK_DOWN:
    setTargetSpeed({0,100,0});
    break;
  default:
    return false;
  }
  return true;
}

bool CameraController::keyReleased(const OgreBites::KeyboardEvent& evt)
{
std::cerr << "Event key release (" << evt.keysym.sym << ")\n";
return false;
}

bool CameraController::touchMoved(const OgreBites::TouchFingerEvent& evt)
{
std::cerr << "Event touch move (" << evt.x << ")\n";
return false;
}

bool CameraController::touchPressed(const OgreBites::TouchFingerEvent& evt)
{
std::cerr << "Event touch pressed (" << evt.x << ")\n";
return false;
}

bool CameraController::touchReleased(const OgreBites::TouchFingerEvent& evt)
{
std::cerr << "Event touch release (" << evt.x << ")\n";
return false;
}

bool CameraController::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
std::cerr << "Event mouse move (" << evt.xrel << ")\n";
return false;
}

bool CameraController::mouseWheelRolled(const OgreBites::MouseWheelEvent& evt)
{
std::cerr << "Event mouse wheel (" << evt.y << ")\n";
return false;
}

bool CameraController::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
std::cerr << "Event mouse pressed (" << evt.button << ")\n";
return false;
}

bool CameraController::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
std::cerr << "Event mouse released (" << evt.button << ")\n";
return false;
}

bool CameraController::textInput(const OgreBites::TextInputEvent& evt)
{
std::cerr << "Event text input (" << evt.chars << ")\n";
return false;
}

bool CameraController::axisMoved(const OgreBites::AxisEvent& evt)
{
std::cerr << "Event axis move (" << evt.which << ")\n";
return false;
}

bool CameraController::buttonPressed(const OgreBites::ButtonEvent& evt)
{
std::cerr << "Event button pressed (" << evt.which << ")\n";
return false;
}

bool CameraController::buttonReleased(const OgreBites::ButtonEvent& evt)
{
std::cerr << "Event button released (" << evt.which << ")\n";
return false;
}
