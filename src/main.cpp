#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <memory>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>

#include "dungeon_util.h"

class Tileset {
  SDL_Texture* tex;
  int rows;
  int columns;
  int tile_width;
  int tile_height;
  int tile_offset_x;
  int tile_offset_y;
  int tile_margin_x;
  int tile_margin_y;
public:
  Tileset(SDL_Texture* tex,
          int tile_width, int tile_height,
          int tile_offset_x=0, int tile_offset_y=0,
          int tile_margin_x=0, int tile_margin_y=0)
    : tex(tex),
      rows(0), columns(0),
      tile_width(tile_width), tile_height(tile_height),
      tile_offset_x(tile_offset_x), tile_offset_y(tile_offset_y),
      tile_margin_x(tile_margin_x), tile_margin_y(tile_margin_y)
  {
    int width, height;
    SDL_QueryTexture(tex, nullptr, nullptr, &width, &height);
    columns = (width-tile_offset_x)/tile_width;
    rows = (height-tile_offset_y)/tile_height;
  }

  int Rows() const {
    return rows;
  }

  int Columns() const {
    return columns;
  }

  int NumTiles() const {
    return rows*columns;
  }

  int TileWidth() const {
    return tile_width;
  }

  int TileHeight() const {
    return tile_height;
  }

  void DrawTile(SDL_Renderer* renderer, int tileNr, int x, int y, int w, int h) const {
    int row = tileNr/columns;
    int column = tileNr - row*columns;
    SDL_Rect srcRect {
      tile_offset_x+column*(tile_width+tile_margin_x),
      tile_offset_y+row*(tile_height+tile_margin_y),
      tile_width,
      tile_height
    };
    SDL_Rect destRect {
      x,y,w,h
    };
    SDL_RenderCopy(renderer, tex, &srcRect, &destRect);
  }
};

class MapLayer {
public:
  virtual void Draw(SDL_Renderer* renderer, int x, int y, int w, int h) const = 0;
  virtual ~MapLayer() = default;
};

class TileLayer : public MapLayer {
  int width;
  int height;
  Tileset tileset;
  std::vector<int> tiles;
  static const int TILE_NONE;
public:
  TileLayer(int width, int height, Tileset tileset)
    : width(width), height(height), tileset(tileset), tiles(width*height,TILE_NONE)
  {}
  void Draw(SDL_Renderer* renderer, int x, int y, int w, int h) const override {
    for(int row=0; row<height; ++row) {
      for(int column=0; column<width; ++column) {
        int tileNr = tiles[row*width+column];
        if(tileNr != TILE_NONE) {
          tileset.DrawTile(renderer,
                           tileNr,
                           x+column*w, y+row*h,
                           w, h);
        }
      }
    }
  }
  void SetTile(int x, int y, int tileNr) {
    tiles[y*width+x] = tileNr;
  }
};

const int TileLayer::TILE_NONE = -1;

class Tilemap {
  std::vector<std::unique_ptr<MapLayer>> layers;
  int tileWidth, tileHeight;
public:
  Tilemap(int tileWidth, int tileHeight)
    : layers(), tileWidth(tileWidth), tileHeight(tileHeight)
  {}
  void Draw(SDL_Renderer* renderer, int x, int y) const {
    for(const auto& layer : layers) {
      layer->Draw(renderer, x, y, tileWidth, tileHeight);
    }
  }
  void AddLayer(std::unique_ptr<MapLayer>&& layer) {
    layers.emplace_back(std::forward<decltype(layer)>(layer));
  }
};


SDL_Texture* LoadTexture(SDL_Renderer* renderer, const std::string& texPath) {
  SDL_Surface* surf = IMG_Load(texPath.c_str());
  util::at_exit free_surf([=]() {
      SDL_FreeSurface(surf);
  });
  return SDL_CreateTextureFromSurface(renderer, surf);
}


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

  Tileset roguelikeSheet(tilesetTex,
                         16,16,
                         0,0,
                         1,1);
  Tilemap testMap(TILE_WIDTH, TILE_HEIGHT);
  auto layer1 = std::make_unique<TileLayer>(20,20,roguelikeSheet);
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
