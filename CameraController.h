#pragma once

#include <cmath>
#include <memory>

#include <OgreCamera.h>
#include <OgreInput.h>

class MinOgreApp;

class CameraController
    : public OgreBites::InputListener
{
public:
  CameraController(std::weak_ptr<MinOgreApp> app = {}) : app_weak(app) {}

  void update(double dt_sec);
  void setTargetSpeed(const Ogre::Vector3& speed);


  Ogre::Vector3 interpolate(const Ogre::Vector3& from, const Ogre::Vector3& to) const;

  bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
  bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
  bool touchMoved(const OgreBites::TouchFingerEvent& evt) override;
  bool touchPressed(const OgreBites::TouchFingerEvent& evt) override;
  bool touchReleased(const OgreBites::TouchFingerEvent& evt) override;
  bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
  bool mouseWheelRolled(const OgreBites::MouseWheelEvent& evt) override;
  bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
  bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
  bool textInput(const OgreBites::TextInputEvent& evt) override;
  bool axisMoved(const OgreBites::AxisEvent& evt) override;
  bool buttonPressed(const OgreBites::ButtonEvent& evt) override;
  bool buttonReleased(const OgreBites::ButtonEvent& evt) override;

private:
  std::weak_ptr<MinOgreApp> app_weak;

  Ogre::Vector3 target_speed{0,0,0};
  Ogre::Vector3 from_speed{0,0,0};

  // (a + b)^2 = a^2 + 2ab + b^2
  // vvvv a = 1-t, b = t vvvv
  // 1 = (1-t)^2 + 2t(1-t) + t^2
  // 1 = A_0     + A_1     + A_2
  // current_speed = A_0 * from_speed + (A_1 * overshoot + A_2 * 1.0) * target_speed;
  // current_speed = (1-t)^2 from_speed + (2t(1-t) * overshoot + t^2) * target_speed;
  // t = elapsed_time / total_time
  const double total_time = 5.0;
  double elapsed_time = 0.0;
  double factor_overshooting = 1.8;

  void moveCamera(const Ogre::Vector3& displacement);
};
