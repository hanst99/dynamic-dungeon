#pragma once

#include <SDL.h>
#include <vector>
#include <memory>


namespace dungeon {
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
          int tile_margin_x=0, int tile_margin_y=0);

  int Rows() const;

  int Columns() const;

  int NumTiles() const;

  int TileWidth() const;

  int TileHeight() const;

  void DrawTile(SDL_Renderer* renderer, int tileNr, int x, int y, int w, int h) const;
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
public:
  static const int TILE_NONE;
  TileLayer(int width, int height, Tileset tileset);

  void Draw(SDL_Renderer* renderer, int x, int y, int w, int h) const override;

  void SetTile(int x, int y, int tileNr);

};


class Tilemap {
  std::vector<std::unique_ptr<MapLayer>> layers;
  int tileWidth, tileHeight;
public:

  Tilemap(int tileWidth, int tileHeight);

  void Draw(SDL_Renderer* renderer, int x, int y) const;

  void AddLayer(std::unique_ptr<MapLayer>&& layer);
};
}

