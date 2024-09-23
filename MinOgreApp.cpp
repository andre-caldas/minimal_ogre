// You should include this only when a directory name is needed.
#include "config_dir.h"

#include "MinOgreApp.h"
#include "GismoSurface.h"

#include <future>

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
  lightNode1->setPosition(600, 800, 500);

  auto light2 = sceneManager->createLight("MainLight2");
  auto lightNode2 = sceneManager->getRootSceneNode()->createChildSceneNode();
  lightNode2->attachObject(light2);
  lightNode2->setPosition(-700, 800, -500);

  auto camera = sceneManager->createCamera("App camera");
  camera->setNearClipDistance(5); // specific to this sample
  camera->setAutoAspectRatio(true);

  cameraNode = sceneManager->getRootSceneNode()->createChildSceneNode();
  cameraNode->attachObject(camera);

  getRenderWindow()->addViewport(camera);

  // Construct THB-Spline.
  gsKnotVector<real_t> KV1 (0,1,1,3);
  gsKnotVector<real_t> KV2 (0,1,1,3);
  surface_basis_t basis(gsTensorBSplineBasis<2, real_t>{KV1, KV2});
  gsMatrix<real_t> C(16,3);
  C << -15.0, -15.0, 0.0,
       -15.0, -5.0, 0.0,
       -15.0, 5.0, 0.0,
       -15.0, 15.0, 0.0,
       -5.0, -15.0, 0.0,
       -5.0, -5.0, -30.0,
       -5.0, 5.0, 0.0,
       -5.0, 15.0, 0.0,
       5.0, -15.0, 0.0,
       5.0, -5.0, 0.0,
       5.0, 5.0, 30.0,
       5.0, 15.0, 0.0,
       15.0, -15.0, 0.0,
       15.0, -5.0, 0.0,
       15.0, 5.0, 0.0,
       15.0, 15.0, 0.0;

  auto thb = std::make_shared<surface_t>(basis, give(C));

  std::shared_ptr<GismoSurface> surface = std::make_shared<GismoSurface>(thb);
  auto mesh = MeshManager::getSingleton().createManual("my surface", RGN_DEFAULT);
//  auto anchors = MeshManager::getSingleton().createManual("my surface anchors", RGN_DEFAULT);
  surface->populateMesh(mesh.get());
//  surface->populateAnchors(anchors.get());

  auto meshEntity = sceneManager->createEntity("my surface xxx", mesh);
  meshEntity->setMaterialName("WoodPallet");
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
