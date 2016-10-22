#include "dungeon_maps.h"

#include "dungeon_util.h"

dungeon::Tileset::Tileset(SDL_Texture* tex,
                          int tile_width, int tile_height,
                          int tile_offset_x, int tile_offset_y,
                          int tile_margin_x, int tile_margin_y)
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

int dungeon::Tileset::Rows() const
{
  return rows;
}

int dungeon::Tileset::Columns() const
{
  return columns;
}

int dungeon::Tileset::NumTiles() const {
  return rows*columns;
}

int dungeon::Tileset::TileWidth() const {
  return tile_width;
}

int dungeon::Tileset::TileHeight() const {
  return tile_height;
}

void dungeon::Tileset::DrawTile(SDL_Renderer* renderer, int tileNr, int x, int y, int w, int h) const {
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

dungeon::TileLayer::TileLayer(int width, int height, Tileset tileset)
: width(width), height(height), tileset(tileset), tiles(width*height,TILE_NONE)
{}

void dungeon::TileLayer::Draw(SDL_Renderer* renderer, int x, int y, int w, int h) const {
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


void dungeon::TileLayer::SetTile(int x, int y, int tileNr) {
  tiles[y*width+x] = tileNr;
}

dungeon::Tilemap::Tilemap(int tileWidth, int tileHeight)
  : layers(), tileWidth(tileWidth), tileHeight(tileHeight)
{}

void dungeon::Tilemap::Draw(SDL_Renderer* renderer, int x, int y) const {
  for(const auto& layer : layers) {
    layer->Draw(renderer, x, y, tileWidth, tileHeight);
  }
}

void dungeon::Tilemap::AddLayer(std::unique_ptr<MapLayer>&& layer) {
  layers.emplace_back(std::forward<decltype(layer)>(layer));
}

const int dungeon::TileLayer::TILE_NONE = -1;
