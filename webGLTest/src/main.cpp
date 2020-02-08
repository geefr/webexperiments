#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

#include <GLES3/gl3.h>


#ifdef __EMSCRIPTEN__
# include <emscripten.h>
# include <emscripten/bind.h>
#endif

#include <iostream>
#include <string>

#include "engine/renderer.h"
#include "engine/shaders/shader_phong.h"

#include "entities/laser.h"
#include "entities/player.h"

std::unique_ptr<Renderer> renderer;
std::vector<std::shared_ptr<Laser>> lasers;
auto maxLasers = 8;
std::shared_ptr<Player> player;
// First 2 are directional scene lights
// The rest are used by lasers
std::vector<Light> lights;

#ifndef __EMSCRIPTEN
void endApplication() {
  SDL_DestroyWindow(renderer->window());
  SDL_Quit();
  std::exit(0);
}
#endif

void initResources() {
	// Setup the shaders we need and register on the renderer
  std::shared_ptr<Shader> phong(new Shader_phong());
  // In this case just use phong for everything, we can change it later
  renderer->shaders()["laser"] = phong;
  renderer->shaders()["player"] = phong;
  
  renderer->clearColour() = {1.0f,1.0f,1.0f,1.0f};
}

void initEntities() {
	// Setup entities
	player.reset(new Player(*renderer.get()));
	for( auto i = 0; i < maxLasers; ++i ) {
		lasers.emplace_back(new Laser(*renderer.get()));
	}
}

void initPlaySpace() {
	// Put things in the right place to start with
	player->position() = {0.0f, 0.0f, 0.0f};
	
	// TODO: These shouldn't all be spawned at once :P
	for( auto i = 0; i < maxLasers; ++i ) {
		lasers[i]->position() = {i, i, i};
	}
	
	// The 2 overall scene lights
	Light ambient;
	ambient.position = {0.0f,-1.0f,0.0f,0.0f}; // Directional, from above
	ambient.colour = {1.0f, 1.0f, 1.0f};
	ambient.intensity = {0.5f, 0.2f, 0.1f};
	
	lights.emplace_back(ambient);
}

void updatePlayer() {
	
}

void updateLasers() {
	
}

void updateRenderParams() {
	renderer->camera().position = player->position() + glm::vec3(0.0f,1.5f,1.5f);
	renderer->camera().lookat = player->position() + glm::vec3(0.0f,0.0f,-5.0f);
	
	// TODO: This should be inside the engine somewhere really
	for( auto& s : renderer->shaders() ) {
		if( auto ls = dynamic_cast<Shader_lightinterface*>(s.second.get()) ) {
			ls->lights() = lights;
		}
	}
}

void render() {
#ifndef __EMSCRIPTEN__
  // Get the next event
  SDL_Event event;
  if(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      endApplication();
    }
  }
#endif

  updatePlayer();
  updateLasers();
  updateRenderParams();

	renderer->render();
}

int main(int argc, char** argv) {
  if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		std::cerr << "Failed to initialise SDL Video" << std::endl;
		return EXIT_FAILURE;
	}

  if( !(IMG_Init(IMG_INIT_PNG) &
        (IMG_INIT_PNG) ) ) {
		std::cerr << "Failed to initialise SDL Image" << std::endl;
		return EXIT_FAILURE;
	}

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
#ifdef __EMSCRIPTEN__  
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
#else  
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  // TODO: Only in debug builds
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

  renderer.reset(new Renderer(800, 600));
  initResources();
  initEntities();
  
  initPlaySpace();

  // Upload data to gpu, init etc
  // - compile/link shaders
  // - upload textures in renderer->textures
  // - upload geometry in renderer->geometry
  renderer->initialiseGLData();
  
// Fire the main loop. Won't quit this until SDL does in some form
#ifdef __EMSCRIPTEN__
  // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
  emscripten_set_main_loop(render, 60, 1);
#else
  while( true ) {
	  render();
	  // TODO: Framerate throttling/vsync
  }
#endif  

  // TODO: All the cleanup and stuff
  // SDL_Quit();
  
  return 0;
}

