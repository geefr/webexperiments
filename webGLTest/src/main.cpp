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
#include "engine/textures/texture_sdl2image.h"

#include "entities/laser.h"
#include "entities/player.h"

std::unique_ptr<Renderer> renderer;
std::vector<std::shared_ptr<Laser>> lasers;
int maxLasers = 8;
std::shared_ptr<Player> player;
// First 2 are directional scene lights
// The rest are used by lasers
std::vector<Light> lights;
std::map<int, bool> keystate;

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
  
  // TODO: This really needs to be in the Player class - Need a way for entities to initialise gl resources
  renderer->textures()["player.diffuse"].reset(new Texture_SDL2Image("data/models/space/ships/fighter_blocky_01/Diffuse.png"));
  
  renderer->clearColour() = {0.0f,0.0f,0.0f,1.0f};
  
  // Populate the key map
  // TODO: Some kind of input manager in the engine I guess
  keystate[SDLK_w] = false;
  keystate[SDLK_s] = false;
  keystate[SDLK_a] = false;
  keystate[SDLK_d] = false;
  keystate[SDLK_q] = false;
  keystate[SDLK_e] = false;
  keystate[SDLK_LEFT] = false;
  keystate[SDLK_RIGHT] = false;
  keystate[SDLK_UP] = false;
  keystate[SDLK_DOWN] = false;
  keystate[SDLK_j] = false;
  keystate[SDLK_k] = false;
}

void initEntities() {
	// Setup entities
	player.reset(new Player(*renderer.get()));
	// TODO: We could of course share the geometry between them all, would be more efficient
	for( auto i = 0; i < maxLasers; ++i ) {
		lasers.emplace_back(new Laser(*renderer.get()));
	}
}

void initPlaySpace() {
	// Put things in the right place to start with
	player->position() = {0.0f, 0.0f, 0.0f};
	
	// TODO: These shouldn't all be spawned at once :P
	for( auto i = 0; i < maxLasers; ++i ) {
		lasers[i]->position() = {-(maxLasers / 2) + (float)i, 0.0f, -4.0f};
	}
	
	// The 2 overall scene lights
	Light ambient;
	ambient.position = {0.0f,-1.0f,-4.0f,0.0f}; // Directional, above and behind
	ambient.colour = {1.0f, 1.0f, 1.0f};
	ambient.intensity = {0.5f, 1.0f, 0.1f};
	
	lights.emplace_back(ambient);
	
	for( auto i = 0; i < maxLasers; ++i ) {
		Light light;
		light.position = {0.0f, 0.0f, 0.0f, 1.0f};
		light.colour = {1.0f, 0.0f, 0.0f};
		light.intensity = {0.0f, 0.5f, 0.8f};
		lights.emplace_back(light);
	}
	
}

void updatePlayer(float delta) {
	player->update(delta);
}

void updateLasers(float delta) {
	for( auto i = 0; i < maxLasers; ++i ) {
		auto& l = lasers[i];
		l->update(delta);
		
		Light light;
		light.position = glm::vec4(l->position(), 1.0f);
		light.colour = {1.0f, 0.0f, 0.0f};
		light.intensity = {0.5f, 0.5f, 0.8f};
		
		lights[i + 2] = light;
	}	
}

void updateRenderParams() {
	renderer->camera().position = player->position() + glm::vec3(0.0f,1.5f,2.0f);
	renderer->camera().lookat = player->position() + glm::vec3(0.0f,0.0f,-5.0f);
	
	// TODO: This should be inside the engine somewhere really
	for( auto& s : renderer->shaders() ) {
		if( auto ls = dynamic_cast<Shader_lightinterface*>(s.second.get()) ) {
			ls->lights() = lights;
		}
	}
}

void pollEvents() {
	SDL_Event event;
	/* get the input, and act upon it */
	while (SDL_PollEvent(&event))   
	{
#ifndef __EMSCRIPTEN__
		if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
		{
			endApplication();
		}
#endif
		
		if(event.type==SDL_KEYDOWN) {
      switch(event.key.keysym.sym)
			{
				case SDLK_w:
					player->thrusterOff(Player::Thruster::Back); break;
				case SDLK_s:
					player->thrusterOff(Player::Thruster::Front); break;
				case SDLK_a:
					player->thrusterOff(Player::Thruster::Right); break;
				case SDLK_d:
					player->thrusterOff(Player::Thruster::Left); break;
				case SDLK_q:
					player->thrusterOff(Player::Thruster::Top); break;
				case SDLK_e:
					player->thrusterOff(Player::Thruster::Bottom); break;
        case SDLK_LEFT:
          if(!keystate[SDLK_LEFT]) player->addRotation({0.0f,0.1f,0.0f}); break;
				case SDLK_RIGHT:
          if(!keystate[SDLK_RIGHT]) player->addRotation({0.0f,-0.1f,0.0f}); break;
				case SDLK_UP:
          if(!keystate[SDLK_UP]) player->addRotation({-0.1f,0.0f,0.0f}); break;
				case SDLK_DOWN:
          if(!keystate[SDLK_DOWN]) player->addRotation({0.1f,0.0f,0.0f}); break;
        case SDLK_j:
          if(!keystate[SDLK_j]) player->addRotation({0.0f,0.0f,0.1f}); break;
        case SDLK_k:
          if(!keystate[SDLK_k]) player->addRotation({0.0f,0.0f,-0.1f}); break;
			}
      
      keystate[event.key.keysym.sym] = true;
    } else if(event.type == SDL_KEYUP) {
      switch(event.key.keysym.sym) {
				case SDLK_w:
					player->thrusterOn(Player::Thruster::Back); break;
				case SDLK_s:
					player->thrusterOn(Player::Thruster::Front); break;
				case SDLK_a:
					player->thrusterOn(Player::Thruster::Right); break;
				case SDLK_d:
					player->thrusterOn(Player::Thruster::Left); break;
				case SDLK_q:
					player->thrusterOn(Player::Thruster::Top); break;
				case SDLK_e:
					player->thrusterOn(Player::Thruster::Bottom); break;
				case SDLK_LEFT:
          if(keystate[SDLK_LEFT]) player->addRotation({0.0f,-0.1f,0.0f}); break;
				case SDLK_RIGHT:
          if(keystate[SDLK_RIGHT]) player->addRotation({0.0f,0.1f,0.0f}); break;
				case SDLK_UP:
          if(keystate[SDLK_UP]) player->addRotation({0.1f,0.0f,0.0f}); break;
				case SDLK_DOWN:
          if(keystate[SDLK_DOWN]) player->addRotation({-0.1f,0.0f,0.0f}); break;
        case SDLK_j:
          if(keystate[SDLK_j]) player->addRotation({0.0f,0.0f,-0.1f}); break;
        case SDLK_k:
          if(keystate[SDLK_k]) player->addRotation({0.0f,0.0f,0.1f}); break;
			}
      
      keystate[event.key.keysym.sym] = false;
		}
	}
}

void render() {
  pollEvents();

  renderer->update();
  
  updatePlayer(renderer->updateDelta());
  updateLasers(renderer->updateDelta());
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

