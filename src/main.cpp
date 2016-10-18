#include <iostream>
#include <fstream>
#include <ctime>
#include <SDL.h>
#include <SDL_image.h>

#include "dungeon_util.h"



void LogToStream(void *stream,
               int category,
               SDL_LogPriority priority,
               const char* message) {
  std::ofstream* out = static_cast<std::ofstream*>(stream);
  auto now = std::time(nullptr);
  std::string now_text = std::ctime(&now);
  // remove newline if any
  now_text = now_text.substr(0, now_text.find('\n'));
  *out << "[" << now_text << "] " << message << std::endl;
}

int main(int argc, char** argv) {
  using util::at_exit;
  std::ofstream log_out("dungeons.log");
  SDL_LogSetOutputFunction(&LogToStream, &log_out);
  if(int errcode = SDL_Init(SDL_INIT_EVERYTHING)) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to initialize SDL [%d]: %s", errcode, SDL_GetError());
    return -errcode;
  }
  SDL_Log("Successfully initialized SDL");
  SDL_Window* window;
  SDL_Renderer* renderer;
  if(SDL_CreateWindowAndRenderer(1024,768,0,&window,&renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to create window/renderer: %s", SDL_GetError());
    return 1;
  }
  at_exit destroy_window_and_renderer([=]() {
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
    });

  //main game loop
  bool has_quit = false;
  Uint32 last_frame_time = 0;
  while(!has_quit) {
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
      if(ev.type == SDL_QUIT) {
        has_quit = true;
      }
    }
    Uint32 current_time = SDL_GetTicks();
    if(current_time-last_frame_time >= util::FRAME_INTERVAL) {
      last_frame_time = current_time;
      SDL_SetRenderDrawColor(renderer, 220, 180, 100, 255);
      SDL_RenderClear(renderer);
      SDL_RenderPresent(renderer);
    }
  }
}
