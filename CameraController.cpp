#include "CameraController.h"
#include "MinOgreApp.h"

#include <cmath>
#include <iostream>

#include <Bites/OgreInput.h>
#include <Ogre.h>

using namespace Ogre;

void CameraController::setTargetSpeed(const std::shared_ptr<MinOgreApp>& app)
{
  from_speed = interpolate(from_speed, target_speed);
  elapsed_time = 0.0;

  auto node = app->getCameraNode();
  target_speed = Vector3::ZERO;
  if(move_right) target_speed += 10.0 * node->getOrientation().xAxis();
  if(move_left)  target_speed -= 10.0 * node->getOrientation().xAxis();
  if(move_up)    target_speed += 10.0 * node->getOrientation().yAxis();
  if(move_down)  target_speed -= 10.0 * node->getOrientation().yAxis();
}


void CameraController::setRotation(const std::shared_ptr<MinOgreApp>& app)
{
  auto node = app->getCameraNode();
  Vector3 normal{0,0,0};
  if(rotate_right) normal -= node->getOrientation().zAxis();
  if(rotate_left)  normal += node->getOrientation().zAxis();
  if(rotate_up)    normal -= node->getOrientation().xAxis();
  if(rotate_down)  normal += node->getOrientation().xAxis();
  if(normal.isZeroLength()) {
    rotation_rad_second = 0;
    return;
  }
  normal.normalise();
  rotation_normal = std::move(normal); // Thread unsafe.
  rotation_rad_second = .5;
}


void CameraController::moveObject(const std::shared_ptr<MinOgreApp>& app,
                                  const Vector3& displacement)
{
  auto node = app->getMeshNode();
  auto pos = node->getPosition() + displacement;
  node->setPosition(std::move(pos));
//  node->lookAt(Vector3::ZERO, Node::TransformSpace::TS_WORLD);
}

void CameraController::rotateObject(const std::shared_ptr<MinOgreApp>& app, float rad)
{
  if(!rotation_rad_second) {
    return;
  }
  auto node = app->getMeshNode();
  node->rotate(rotation_normal, Radian{rad}, Node::TransformSpace::TS_PARENT);
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

  auto app = app_weak.lock();
  if(!app) {
    return;
  }
  setTargetSpeed(app);
  setRotation(app);

  auto current_speed = interpolate(from_speed, target_speed);
  moveObject(app, dt_sec * std::move(current_speed));
  rotateObject(app, dt_sec * rotation_rad_second);
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

  return keyPressedOrReleased(evt, true);
}

bool CameraController::keyReleased(const OgreBites::KeyboardEvent& evt)
{
  assert(!evt.repeat); // Released is never repeated. If it was, we should do as in keyPressed.
  return keyPressedOrReleased(evt, false);
}

bool CameraController::keyPressedOrReleased(const OgreBites::KeyboardEvent& evt, bool pressed)
{
  auto it = char_to_bool.find(evt.keysym.sym);
  if(it == char_to_bool.end()) {
    return false;
  }

  *(it->second) = pressed;
  return true;
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
