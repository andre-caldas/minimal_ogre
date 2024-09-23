#pragma once

#include <cmath>
#include <memory>
#include <unordered_map>

#include <OgreCamera.h>
#include <Bites/OgreInput.h>

class MinOgreApp;

class CameraController
    : public OgreBites::InputListener
{
public:
  CameraController(std::weak_ptr<MinOgreApp> app = {}) : app_weak(app) {}

  void update(double dt_sec);
  void setTargetSpeed(const std::shared_ptr<MinOgreApp>& app);
  void setRotation(const std::shared_ptr<MinOgreApp>& app);


  Ogre::Vector3 interpolate(const Ogre::Vector3& from, const Ogre::Vector3& to) const;

  bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
  bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
  bool keyPressedOrReleased(const OgreBites::KeyboardEvent& evt, bool pressed);
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

  // Rotation
  Ogre::Vector3 rotation_normal{0,0,0};
  float rotation_rad_second = 0;

  // Translation
  Ogre::Vector3 target_speed{0,0,0};
  Ogre::Vector3 from_speed{0,0,0};

  bool move_right = false;
  bool move_left  = false;
  bool move_up    = false;
  bool move_down  = false;

  bool rotate_right = false;
  bool rotate_left  = false;
  bool rotate_up    = false;
  bool rotate_down  = false;

  std::unordered_map<int, bool*> char_to_bool{
    {'a', &rotate_left},
    {'d', &rotate_right},
    {'w', &rotate_up},
    {'s', &rotate_down},
    {OgreBites::SDLK_LEFT,  &move_left},
    {OgreBites::SDLK_RIGHT, &move_right},
    {OgreBites::SDLK_UP,    &move_up},
    {OgreBites::SDLK_DOWN,  &move_down}
  };

  // (a + b)^2 = a^2 + 2ab + b^2
  // vvvv a = 1-t, b = t vvvv
  // 1 = (1-t)^2 + 2t(1-t) + t^2
  // 1 = A_0     + A_1     + A_2
  // current_speed = A_0 * from_speed + (A_1 * overshoot + A_2 * 1.0) * target_speed;
  // current_speed = (1-t)^2 from_speed + (2t(1-t) * overshoot + t^2) * target_speed;
  // t = elapsed_time / total_time
  const double total_time = 1.0;
  double elapsed_time = 0.0;
  double factor_overshooting = 1.8;

  void moveObject(const std::shared_ptr<MinOgreApp>& app, const Ogre::Vector3& displacement);
  void rotateObject(const std::shared_ptr<MinOgreApp>& app, float rad);
};
