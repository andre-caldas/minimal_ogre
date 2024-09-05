#pragma once

#include <string>
#include <thread>

#include <Bites/OgreApplicationContextSDL3.h>
#include <Bites/OgreInput.h>

class MinOgreApp
    : public OgreBites::ApplicationContextSDL3
, public OgreBites::InputListener
{
public:
  using OgreBites::ApplicationContextSDL3::ApplicationContextSDL3;
  ~MinOgreApp();

  std::string title;

  void init();
  void terminate();
  bool hasTerminated() const {return terminated;}

  Ogre::SceneNode* getCameraNode() const {return cameraNode;}
  Ogre::SceneNode* getMeshNode() const {return meshNode;}

protected:
  /**
   * This will be called by the base class.
   */
  void setup() override;

  /**
   * This will be called from the rendering thread.
   * We shall use it only to veryfy the state of the stopToken
   * and stop rendering.
   */
  void pollEvents() override;

private:
  bool terminated = false;

  std::jthread renderThread;
  // This variable shall be accessed ONLY by the rendering thread.
  std::stop_token stopToken;

  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::SceneNode* cameraNode = nullptr;
  Ogre::SceneNode* meshNode = nullptr;

  // Exit on ESC key pressed.
  bool keyPressed(const OgreBites::KeyboardEvent& evt);
};
