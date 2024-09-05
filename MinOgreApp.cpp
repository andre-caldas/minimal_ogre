// You should include this only when a directory name is needed.
#include "config_dir.h"

#include "MinOgreApp.h"
#include "GismoSurface.h"

#include <future>

#include <gsNurbs/gsBSpline.h>
#include <gsNurbs/gsNurbs.h>
#include <gsNurbs/gsTensorNurbs.h>

#include <Ogre.h>
#include <SDL3/SDL.h>

using namespace Ogre;
using namespace gismo;

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

  auto shadergen = RTShader::ShaderGenerator::getSingletonPtr();
  shadergen->addSceneManager(sceneManager);

  sceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

  auto light1 = sceneManager->createLight("MainLight1");
  auto lightNode1 = sceneManager->getRootSceneNode()->createChildSceneNode();
  lightNode1->attachObject(light1);
  lightNode1->setPosition(20, 80, 50);

  auto light2 = sceneManager->createLight("MainLight2");
  auto lightNode2 = sceneManager->getRootSceneNode()->createChildSceneNode();
  lightNode2->attachObject(light2);
  lightNode2->setPosition(200, 80, -50);

  auto camera = sceneManager->createCamera("App camera");
  camera->setNearClipDistance(5); // specific to this sample
  camera->setAutoAspectRatio(true);

  cameraNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  cameraNode->attachObject(camera);

  getRenderWindow()->addViewport(camera);

  // Construct NURBS.
  gsKnotVector<real_t> KV1 (0,1,1,3,1);
  gsKnotVector<real_t> KV2 (0,1,1,3,1);
  gsMatrix<real_t> C(16,3);
  C << 0.0, 0.0, 0.0,
       0.0, 15.0, 0.0,
       0.0, 30.0, 0.0,
       0.0, 45.0, 0.0,
       15.0, 0.0, 0.0,
       15.0, 15.0, -30.0,
       15.0, 30.0, 0.0,
       15.0, 45.0, 0.0,
       30.0, 0.0, 0.0,
       30.0, 15.0, 0.0,
       30.0, 30.0, 30.0,
       30.0, 45.0, 0.0,
       45.0, 0.0, 0.0,
       45.0, 15.0, 0.0,
       45.0, 30.0, 0.0,
       45.0, 45.0, 0.0;

    gsMatrix<real_t> W(16,1);
    W << 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
         1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0;

  auto nurbs = std::make_shared<surface_t>(KV1,KV2, give(C), give(W));

  std::shared_ptr<GismoSurface> surface = std::make_shared<GismoSurface>(nurbs);
  auto mesh = MeshManager::getSingleton().createManual("my nubrs", RGN_DEFAULT);
  surface->populateMesh(mesh.get());

  auto meshEntity = sceneManager->createEntity("my nurbs xxx", mesh);
  meshNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  meshNode->attachObject(meshEntity);

  cameraNode->setPosition(10, 10, 100);
  cameraNode->lookAt({0,0,0}, Node::TransformSpace::TS_WORLD);
}


void MinOgreApp::pollEvents()
{
  if(stopToken.stop_requested()) {
    closeApp();
  }
}
