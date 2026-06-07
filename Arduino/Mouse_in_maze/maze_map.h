#ifndef MAZE_MAP_H
#define MAZE_MAP_H
#include <sys/types.h>
struct Cell {
  bool walls[4] {};
  bool visited = false;
};

struct Map{
  uint8_t size_X = 1;
  uint8_t size_Y = 1;
  Cell** maze = nullptr;

  bool begin(uint8_t x = 1,uint8_t y = 1){
    size_X = x;
    size_Y = y;
    maze = alloc(x, y);
    return maze != nullptr;
  }

  void end(){
    free_maze(maze, int size_X);
    maze = nullptr;
    size_X = 0;
    size_Y = 0;
  }

  bool resize(bool dir){
    if(!dir && size_X == 255) return false;
    if(dir && size_Y == 255) return false;

    uint8_t new_X = size_X + !dir;
    uint8_t new_X = size_X + dir;

    Cell** newMaze = alloc(new_X, new_Y);
    for (int x = 0; x < size_X; x++){
      for (int y = 0; y < size_Y; y++){
        newMaze[x][y] = maze[x][y];
      } 
    }
    free_maze(maze, size_X);
    size_X = new_X;
    size_Y = new_Y;
    maze = newMaze;
    return true;
  }

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