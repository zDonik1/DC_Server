#include "map.h"

Map::Map(ServerEntityManager *l_entityMgr)
    : m_maxMapSize(32, 32)
    , m_entityManager(l_entityMgr)
{
    loadTiles("tiles.cfg");
}

Map::~Map()
{
    purgeMap();
    purgeTileSet();
}

Tile *Map::getTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer)
{
    if (l_x >= m_maxMapSize.x || l_y >= m_maxMapSize.y || l_layer >= Sheet::Num_Layers) {
        return nullptr;
    }

    auto itr = m_tileMap.find(convertCoords(l_x, l_y, l_layer));
    if (itr == m_tileMap.end()) {
        return nullptr;
    }

    return itr->second;
}

TileInfo *Map::getDefaultTile()
{
    return &m_defaultTile;
}

unsigned int Map::getTileSize() const
{
    return Sheet::Tile_Size;
}

const sf::Vector2u& Map::getMapSize() const
{
    return m_maxMapSize;
}

const sf::Vector2f& Map::getPlayerStart() const
{
    return m_playerStart;
}

unsigned int Map::convertCoords(const unsigned int& l_x, const unsigned int& l_y, unsigned int l_layer) const
{
    return ((l_layer * m_maxMapSize.y + l_y) * m_maxMapSize.x + l_x);
}

void Map::purgeMap()
{
    while (m_tileMap.begin() != m_tileMap.end()) {
        delete m_tileMap.begin()->second;
        m_tileMap.erase(m_tileMap.begin());
    }
    m_tileCount = 0;
    m_entityManager->purge();
}

void Map::purgeTileSet()
{
    for (auto& itr : m_tileSet)
        delete itr.second;

    m_tileSet.clear();
    m_tileSetCount = 0;
}

void Map::loadTiles(const std::string& l_path)
{
    std::ifstream tileSetFile;
    tileSetFile.open(Utils::getWorkingDirectory() + l_path);
    if (!tileSetFile.is_open()) {
        std::cout << "! Failed loading tile set file: " << l_path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(tileSetFile, line)) {
        if (line[0] == '|')
            continue;

        std::stringstream keystream(line);

        int tileId;
        keystream >> tileId;
        if (tileId < 0)
            continue;

        TileInfo* tile = new TileInfo(static_cast<unsigned int>(tileId));
        keystream >> tile->m_name >> tile->m_friction.x >> tile->m_friction.y >> tile->m_deadly;
        if (!m_tileSet.emplace(tileId, tile).second) {
            std::cout << "! Duplicate tile type: " << tile->m_name << std::endl;
            delete tile;
        }
    }

    tileSetFile.close();
}

void Map::loadMap(const std::string& l_path)
{
    std::ifstream mapFile;
    mapFile.open(Utils::getWorkingDirectory() + l_path);
    if (!mapFile.is_open()) {
        std::cout << "! Failed loading map file: " << l_path << std::endl;
        return;
    }

    std::string line;
    std::cout << "--- Loading a map: " << l_path << std::endl;

    while (std::getline(mapFile, line)) {
        if (line[0] == '|')
            continue;

        std::stringstream keystream(line);

        std::string type;
        keystream >> type;
        if (type == "TILE") { // loads tile information
            int tileId = 0;
            keystream >> tileId;
            if (tileId < 0) {
                std::cout << "! Bad tile id: " << tileId << std::endl;
                continue;
            }
            auto itr = m_tileSet.find(static_cast<unsigned int>(tileId));
            if (itr == m_tileSet.end()) {
                std::cout << "! Tile id(" << tileId << ") was not found in tilesheet." << std::endl;
                continue;
            }
            sf::Vector2i tileCoords;
            unsigned int tileLayer = 0;
            unsigned int tileSolidity = 0;
            keystream >> tileCoords.x >> tileCoords.y >> tileLayer >> tileSolidity;
            if (static_cast<unsigned int>(tileCoords.x) > m_maxMapSize.x
                    || static_cast<unsigned int>(tileCoords.y) > m_maxMapSize.y
                    || tileLayer >= Sheet::Num_Layers)
            {
                std::cout << "! Tile is out of range: " << tileCoords.x << " " << tileCoords.y << std::endl;
                continue;
            }
            Tile* tile = new Tile();
            tile->m_properties = itr->second;
            tile->m_solid = static_cast<bool>(tileSolidity);
            if (!m_tileMap.emplace(convertCoords(static_cast<unsigned int>(tileCoords.x),
                                                 static_cast<unsigned int>(tileCoords.y),
                                                 tileLayer),
                                   tile).second) {
                std::cout << "! Duplicate tile! : " << tileCoords.x << " " << tileCoords.y << std::endl;
                delete tile;
                tile = nullptr;
                continue;
            }
            std::string warp;
            keystream >> warp;
            tile->m_warp = false;
            if (warp == "WARP") {
                tile->m_warp = true;
            }
        }
        else if (type == "SIZE") {
            keystream >> m_maxMapSize.x >> m_maxMapSize.y;
        }
        else if (type == "DEFAULT_FRICTION") {
            keystream >> m_defaultTile.m_friction.x >> m_defaultTile.m_friction.y;
        }
        else if (type == "ENTITY") {
            std::string name;
            keystream >> name;
            int entityID = m_entityManager->addEntity(name);
            if (entityID < 0) {
                continue;
            }

            C_Base *position = nullptr;
            if ((position = m_entityManager->getComponent<C_Position>(static_cast<unsigned int>(entityID), Component::Position))) {
                keystream >> *position;
            }
        }
        else {
            std::cout << "!Unknown type \"" << type << "\"." << std::endl;
        }
    }

    mapFile.close();
    std::cout << "--- Map loaded! ---" << std::endl;
}
