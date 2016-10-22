#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <memory>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>

#include "dungeon_util.h"
#include "dungeon_maps.h"

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

const int TILE_WIDTH=32;
const int TILE_HEIGHT=32;

class Sprite {
  SDL_Texture* tex;
  SDL_Rect srcRect;
public:
  Sprite(SDL_Texture* tex, int offX, int offY, int spriteWidth, int spriteHeight)
    : tex(tex), srcRect{offX,offY,spriteWidth,spriteHeight}
  {}
  void Draw(SDL_Renderer* renderer, int x, int y, int w, int h) {
    SDL_Rect destRect {x,y,w,h};
    SDL_RenderCopy(renderer, tex, &srcRect, &destRect);
  }
};

SDL_Texture* LoadTexture(SDL_Renderer* renderer, const std::string& texPath) {
  SDL_Surface* surf = IMG_Load(texPath.c_str());
  util::at_exit free_surf([=]() {
      SDL_FreeSurface(surf);
    });
  return SDL_CreateTextureFromSurface(renderer, surf);
}

struct Vec2 {
  int x,y;
  Vec2& operator+=(const Vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  Vec2 operator+(const Vec2& other) const {
    return Vec2(*this)+=other;
  }
  Vec2& operator-=(const Vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;;
  }
  Vec2 operator-(const Vec2& other) const {
    return Vec2(*this)-=other;
  }
};

int main(int argc, char** argv) {
  std::ofstream log_out("dungeons.log");
  SDL_LogSetOutputFunction(&LogToStream, &log_out);
  if(int errcode = SDL_Init(SDL_INIT_EVERYTHING)) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to initialize SDL [%d]: %s", errcode, SDL_GetError());
    return -errcode;
  }
  SDL_Log("Successfully initialized SDL");
  IMG_Init(IMG_INIT_PNG);
  SDL_Window* window;
  SDL_Renderer* renderer;
  if(SDL_CreateWindowAndRenderer(1024,768,0,&window,&renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to create window/renderer: %s", SDL_GetError());
    return 1;
  }
  util::at_exit destroy_window_and_renderer([=]() {
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
  });

  auto tilesetTex = LoadTexture(renderer, "assets/texture/roguelikeSheet_transparent.png");
  util::at_exit destroy_tileset_tex([=]() {
      SDL_DestroyTexture(tilesetTex);
  });

  dungeon::Tileset roguelikeSheet(tilesetTex,
                         16,16,
                         0,0,
                         1,1);
  dungeon::Tilemap testMap(TILE_WIDTH, TILE_HEIGHT);
  auto layer1 = std::make_unique<dungeon::TileLayer>(20,20,roguelikeSheet);
  for(int row=0; row<20; ++row) {
    for(int column=0; column<20; ++column) {
      layer1->SetTile(row,column,0);
    }
  }
  testMap.AddLayer(std::move(layer1));

  auto characterTex = LoadTexture(renderer, "assets/texture/roguelikeChar_transparent.png");
  util::at_exit destroy_character_tex([=]() {
      SDL_DestroyTexture(characterTex);
  });
  Sprite charSprite(characterTex, 17,17*6,16,16);
  Vec2 charPos{0,0};
  const Vec2
    left{-TILE_WIDTH,0},
    right{TILE_WIDTH,0},
    up{0, -TILE_HEIGHT},
    down{0, TILE_HEIGHT};
  //main game loop
  bool has_quit = false;
  Uint32 last_frame_time = 0;
  while(!has_quit) {
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
      switch(ev.type) {
      case SDL_QUIT:
        has_quit = true;
        break;
      case SDL_KEYDOWN:
        switch(ev.key.keysym.sym) {
        case SDLK_UP: charPos+=up; break;
        case SDLK_DOWN: charPos+=down; break;
        case SDLK_LEFT: charPos+=left; break;
        case SDLK_RIGHT: charPos+=right; break;
        default: break;
        }
        break;
      default: break;
      }
    }
    Uint32 current_time = SDL_GetTicks();
    if(current_time-last_frame_time >= util::FRAME_INTERVAL) {
      last_frame_time = current_time;
      SDL_SetRenderDrawColor(renderer, 220, 180, 100, 255);
      SDL_RenderClear(renderer);
      testMap.Draw(renderer, 0, 0);
      charSprite.Draw(renderer, charPos.x, charPos.y, TILE_WIDTH, TILE_HEIGHT);
      SDL_RenderPresent(renderer);
    }
  }
}
