#include "maze_map.h"
#include <LittleFS.h>
#include <Arduino.h>

static const int dx[4] = { 1,  0, -1,  0};
static const int dy[4] = { 0,  1,  0, -1};

// ============================================================
// ULOŽENIE MAPY
// ============================================================
bool Map::saveMap(uint8_t posX, uint8_t posY, uint8_t dir) {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS: chyba!");
        return false;
    }
    File f = LittleFS.open("/maze.txt", "w");
    if (!f) {
        Serial.println("Chyba pri otvarani na zapis!");
        return false;
    }
    // Hlavička: rozmery
    f.print("S: ");
    f.print(size_X); f.print(", ");
    f.println(size_Y);

    for (uint8_t y = 0; y < size_Y; y++) {
        for (uint8_t x = 0; x < size_X; x++) {
            Cell& cur = maze[x][y];
            f.print("N: ");
            f.print(x);                         f.print(", ");
            f.print(y);                         f.print(", ");
            f.print(cur.walls[0] ? 1 : 0);     f.print(", ");
            f.print(cur.walls[1] ? 1 : 0);     f.print(", ");
            f.print(cur.walls[2] ? 1 : 0);     f.print(", ");
            f.println(cur.walls[3] ? 1 : 0);
        }
    }
    f.print("P: ");
    f.print(posX);      f.print(", ");
    f.print(posY);    f.print(", ");
    f.print(dir);

    f.close();
    Serial.println("Mapa ulozena.");
    return true;
}

// ============================================================
// NAČÍTANIE MAPY
// ============================================================
bool Map::loadMap(uint8_t& posX, uint8_t& posY, uint8_t& dir, uint8_t& gx, uint8_t& gy) {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS: chyba!");
        return false;
    }
    File f = LittleFS.open("/maze.txt", "r");
    if (!f) {
        Serial.println("maze.txt nenajdeny.");
        return false;
    }

    uint8_t new_X = 0, new_Y = 0;

    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();

        if (line.startsWith("S: ")) {
            sscanf(line.c_str() + 3, "%hhu, %hhu", &new_X, &new_Y);
            if (maze) end();
            if (!begin(new_X, new_Y)) {
                Serial.println("Chyba alokacie mapy!");
                f.close();
                return false;
            }
        } else if (line.startsWith("N: ")) {
            uint8_t x, y, w0, w1, w2, w3;
            sscanf(line.c_str() + 3, "%hhu, %hhu, %hhu, %hhu, %hhu, %hhu",
                   &x, &y, &w0, &w1, &w2, &w3);
            Cell& c  = maze[x][y];
            c.walls[0] = w0;
            c.walls[1] = w1;
            c.walls[2] = w2;
            c.walls[3] = w3;
            c.visited  = true;
            c.x = x;
            c.y = y;
        }
        else if (line.startsWith("P: ")) {
            sscanf(line.c_str() + 3, "%hhu, %hhu, %hhu", &posX, &posY, &dir);    
        }
        else if (line.startsWith("G: ")) {
            sscanf(line.c_str() + 3, "%hhu, %hhu", &gx, &gy);
        }

    }

    numVisited = size_X * size_Y;  // načítaná mapa = celá preskúmaná
    f.close();
    Serial.println("Mapa nacitana.");
    return true;
}

// ============================================================
// BFS — vráti pole smerov (0-3) od štartu po cieľ
// ============================================================
bool Map::bfs(uint8_t startX, uint8_t startY,
              uint8_t goalX,  uint8_t goalY,
              uint8_t outDirs[], uint8_t& pathLen) {

    uint8_t qx[MAZE_MAX_CELLS], qy[MAZE_MAX_CELLS];
    int8_t  pd[MAZE_MAX_CELLS];   // smer ktorým sme prišli do tohto uzla
    int16_t px[MAZE_MAX_CELLS];   // index rodiča v rade
    bool    visited[MAZE_MAX_CELLS] = {};

    auto flat = [&](uint8_t x, uint8_t y) -> uint8_t {
        return y * size_X + x;
    };

    int16_t head = 0, tail = 0;
    qx[tail] = startX;
    qy[tail] = startY;
    px[tail] = -1;
    pd[tail] = -1;
    visited[flat(startX, startY)] = true;
    tail++;

    while (head < tail) {
        uint8_t cx = qx[head];
        uint8_t cy = qy[head];

        if (cx == goalX && cy == goalY) {
            // Rekonštrukcia cesty — ideme späť cez px[]
            pathLen = 0;
            int16_t cur = head;
            while (px[cur] != -1) {       // štartovací uzol nemá smer
                outDirs[pathLen++] = pd[cur];
                cur = px[cur];
            }
            // Otočenie: rekonštrukcia ide cieľ→štart, chceme štart→cieľ
            for (uint8_t i = 0; i < pathLen / 2; i++) {
                uint8_t tmp = outDirs[i];
                outDirs[i] = outDirs[pathLen - 1 - i];
                outDirs[pathLen - 1 - i] = tmp;
            }
            return true;
        }

        for (int d = 0; d < 4; d++) {
            if (maze[cx][cy].walls[d]) continue;
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            if (nx < 0 || nx >= size_X) continue;
            if (ny < 0 || ny >= size_Y) continue;
            if (visited[flat(nx, ny)])  continue;
            visited[flat(nx, ny)] = true;
            qx[tail] = nx;
            qy[tail] = ny;
            px[tail] = head;
            pd[tail] = d;
            tail++;
        }
        head++;
    }

    pathLen = 0;
    return false;
}