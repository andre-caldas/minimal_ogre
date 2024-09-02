// You should include this only when a directory name is needed.
#include "config_dir.h"

#include "MinOgreApp.h"
#include "GismoSurface.h"

#include <Ogre.h>
#include <SDL3/SDL.h>

MinOgreApp::~MinOgreApp()
{
  terminate();
  renderThread.join();
}


void MinOgreApp::init()
{
  std::promise<bool> promise;
  auto future = promise.get_future();
  renderThread = std::jthread(
    [this](auto promise)
    {
      initApp();
      promise.set_value(true);
      getRoot()->startRendering();
      terminated = true;
    }, std::move(promise));
  future.wait();
}


void MinOgreApp::terminate()
{
  getRoot()->queueEndRendering();
  terminated = true;
}


bool MinOgreApp::keyPressed(const OgreBites::KeyboardEvent& evt)
{
  if(evt.keysym.sym == SDLK_ESCAPE) {
    terminate();
  }
  return true;
}


void MinOgreApp::setup()
{
  ApplicationContextSDL3::setup();
  addInputListener(this);

  auto root = getRoot();
  sceneManager = root->createSceneManager();

  auto shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shadergen->addSceneManager(sceneManager);

  sceneManager->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

  auto light = sceneManager->createLight("MainLight");
  auto lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  lightNode->attachObject(light);
  lightNode->setPosition(20, 80, 50);

  auto camera = sceneManager->createCamera("App camera");
  camera->setNearClipDistance(5); // specific to this sample
  camera->setAutoAspectRatio(true);

  cameraNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  cameraNode->attachObject(camera);
  cameraNode->setPosition(0, 0, 140);

  getRenderWindow()->addViewport(camera);

  auto ogreEntity = sceneManager->createEntity("ogrehead.mesh");
  auto ogreNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  ogreNode->attachObject(ogreEntity);
}


void MinOgreApp::pollEvents()
{
  if(stopToken.stop_requested()) {
    closeApp();
  }
}
