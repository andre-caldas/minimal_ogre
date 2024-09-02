#include "MinOgreApp.h"
#include "CameraController.h"

#include <chrono>
#include <iostream>
#include <memory>

#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

namespace chrono = std::chrono;

typedef chrono::time_point<chrono::steady_clock>          time_point_t;
typedef chrono::duration<double, chrono::seconds::period> seconds_diff_t;

namespace {
  time_point_t last_time;
  std::shared_ptr<MinOgreApp> app;
  std::unique_ptr<CameraController> camera_controller;
}

SDL_AppResult SDL_AppInit(void** /*appstate*/, int /*argc*/, char** /*argv*/)
{
  app = std::make_shared<MinOgreApp>(
    "(Not so) minimal (yet complete) Ogre application");
  app->init();
  camera_controller = std::make_unique<CameraController>(app);
  app->addInputListener(camera_controller.get());

  SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");
  last_time = chrono::steady_clock::now();
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* /*appstate*/)
{
  // SDL_HINT_MAIN_CALLBACK_RATE is not properly working yet.
  // So, we check by hand. Remove this when fixed.
  // Begin.
  static const auto htz = chrono::duration_cast<time_point_t::duration>(chrono::duration<long int, std::ratio<1,60>>{1});
  static auto last_iteration = chrono::steady_clock::now();
  auto xxx_now = chrono::steady_clock::now();
  if(xxx_now - last_iteration < htz) {
    return SDL_APP_CONTINUE;
  }
  last_iteration = xxx_now;
  // End.

  if(app->hasTerminated()) {
    // TODO: report errors.
    return SDL_APP_SUCCESS;
  }

  auto next_now = chrono::steady_clock::now();
  seconds_diff_t delta_seconds = next_now - last_time;
  last_time = next_now;

  camera_controller->update(delta_seconds.count());

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* /*appstate*/, const SDL_Event* event)
{
  app->processOneEvent(*event);
  if(app->hasTerminated()) {
    // TODO: report errors.
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* /*appstate*/)
{
  // Uneeded, I suppose.
  // Just for the peace of the heart! :-)
  app = nullptr;
}
