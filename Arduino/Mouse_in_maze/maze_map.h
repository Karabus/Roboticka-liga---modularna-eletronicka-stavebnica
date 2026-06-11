#include <sys/_stdint.h>
#ifndef MAZE_MAP_H
#define MAZE_MAP_H

#include <stdint.h>

#define MAZE_MAX_CELLS 256  // max 16×16

struct Cell {
    bool    walls[4] = {1,1,1,1};  // 0=> 1=^ 2=< 3=v
    bool    visited  = false;
    uint8_t x = 0, y = 0;

    void connect(uint8_t dir, Cell& neighbor) {
        walls[dir] = 0;
        neighbor.walls[(dir + 2) % 4] = 0;
    }
};

struct Map {
    uint8_t size_X     = 1;
    uint8_t size_Y     = 1;
    uint16_t numVisited = 0;
    Cell**  maze       = nullptr;

    bool begin(uint8_t x = 1, uint8_t y = 1) {
        size_X = x;
        size_Y = y;
        numVisited = 0;
        maze = alloc(x, y);
        if (!maze) return false;
        // Nastav súradnice
        for (uint8_t i = 0; i < x; i++)
            for (uint8_t j = 0; j < y; j++) {
                maze[i][j].x = i;
                maze[i][j].y = j;
            }
        return true;
    }

    void end() {
        free_maze(maze, size_X);
        maze   = nullptr;
        size_X = 0;
        size_Y = 0;
        numVisited = 0;
    }

    bool resize(bool dir) {
        if (!dir && size_X == 255) return false;
        if ( dir && size_Y == 255) return false;
        uint8_t new_X = size_X + (!dir ? 1 : 0);
        uint8_t new_Y = size_Y + ( dir ? 1 : 0);
        Cell** newMaze = alloc(new_X, new_Y);
        if (!newMaze) return false;
        for (uint8_t x = 0; x < size_X; x++)
            for (uint8_t y = 0; y < size_Y; y++)
                newMaze[x][y] = maze[x][y];
            
        // Nastav súradnice pre nové bunky
        for (uint8_t i = 0; i < new_X; i++)
            for (uint8_t j = 0; j < new_Y; j++) {
                newMaze[i][j].x = i;
                newMaze[i][j].y = j;
            }
        free_maze(maze, size_X);
        size_X = new_X;
        size_Y = new_Y;
        maze   = newMaze;
        return true;
    }

    // Navštív bunku — vráti true ak je nová
    bool visit(uint8_t x, uint8_t y) {
        if (maze[x][y].visited) return false;
        maze[x][y].visited = true;
        numVisited++;
        return true;
    }

    bool fullyExplored() {
        return numVisited >= (uint16_t)size_X * (uint16_t)size_Y;
    }

    Cell& at(uint8_t x, uint8_t y) { return maze[x][y]; }

    bool saveMap(uint8_t posX, uint8_t posY, uint8_t dir);
    bool loadMap(uint8_t& posX, uint8_t& posY, uint8_t& dir, uint8_t& gx, uint8_t& gy);

    bool bfs(uint8_t startX, uint8_t startY,
             uint8_t goalX,  uint8_t goalY,
             uint8_t outDirs[], uint8_t& pathLen);

private:
    static Cell** alloc(uint8_t x, uint8_t y) {
        Cell** m = new Cell*[x];
        if (!m) return nullptr;
        for (uint8_t i = 0; i < x; ++i) {
            m[i] = new Cell[y]();
            if (!m[i]) {
                for (uint8_t j = 0; j < i; ++j) delete[] m[j];
                delete[] m;
                return nullptr;
            }
        }
        return m;
    }

    static void free_maze(Cell** m, uint8_t x) {
        if (!m) return;
        for (uint8_t i = 0; i < x; ++i) delete[] m[i];
        delete[] m;
    }
};

#endif