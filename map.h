#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include "utilities.h"
#include "serverentitymanager.h"

enum Sheet
{
    Tile_Size = 32,
    Sheet_Width = 256,
    Sheet_Height = 256,
    Num_Layers = 4
};

using TileID = unsigned int;

struct TileInfo
{
    TileInfo(TileID l_id = 0)
    {
        m_deadly = false;
        m_id = l_id;
    }

    ~TileInfo()
    {
    }

    TileID m_id;
    std::string m_name;
    sf::Vector2f m_friction;
    bool m_deadly;
};

struct Tile
{
    TileInfo* m_properties;
    bool m_warp;
    bool m_solid;
};

using TileMap = std::unordered_map<TileID, Tile*>;
using TileSet = std::unordered_map<TileID, TileInfo*>;

class Map
{
public:
    Map(ServerEntityManager* l_entityMgr);
    ~Map();

    Tile *getTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
    TileInfo *getDefaultTile();
    unsigned int getTileSize() const;
    const sf::Vector2u& getMapSize() const;
    const sf::Vector2f& getPlayerStart() const;

    void loadMap(const std::string& l_path);

private:
    unsigned int convertCoords(const unsigned int& l_x, const unsigned int& l_y, unsigned int l_layer) const;
    void loadTiles(const std::string& l_path);
    void purgeMap();
    void purgeTileSet();

    TileSet m_tileSet; // types of tiles
    TileMap m_tileMap; // tile pointers and positions in the map
    TileInfo m_defaultTile; // desribes the default tile information
    sf::Vector2u m_maxMapSize;
    sf::Vector2f m_playerStart; // player starting position
    unsigned int m_tileCount; // number of tiles on the map
    unsigned int m_tileSetCount; // number of types of tiles
    ServerEntityManager *m_entityManager;
};
