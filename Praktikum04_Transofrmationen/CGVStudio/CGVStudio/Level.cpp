#include "Level.h"
#include "Plattform.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

Level::Level(int w, int h) : width(w), height(h) {
    grid.resize(height, std::vector<Plattform>(width));
    for (int z = 0; z < height; ++z)
        for (int x = 0; x < width; ++x)
            grid[z][x] = Plattform(x, z);
}

void Level::generatePathWithLights() {
    generatePath(); 
    addLightPlatforms();
    addWalls();
}

const std::vector<Plattform*>& Level::getPath() const {
    return path;
}

void Level::addWalls() {
    //linke Wand
   
}

void Level::generatePath() {
    path.clear();

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int startX = std::rand() % width;
    Plattform* current = &grid[0][startX];
    current->visited = true;

    Plattform* next = getNeighbor(current, 0, 1);
    if (next) {
        next->visited = true;
        next->predecessor = current;
        current = next;
    }

    while (current->z < height - 1) {
        std::vector<Plattform*> neighbors = getValidNeighbors(current);

        if (neighbors.empty()) {
            current = current->predecessor;
            if (!current) return;
            continue;
        }

        Plattform* chosen = neighbors[std::rand() % neighbors.size()];
        chosen->visited = true;
        chosen->predecessor = current;
        current = chosen;
    }

    Plattform* temp = current;
    while (temp) {
        path.push_back(temp);
        temp = temp->predecessor;
    }

    std::reverse(path.begin(), path.end());

}

Plattform* Level::getNeighbor(Plattform* p, int dx, int dz) {
    int nx = p->x + dx;
    int nz = p->z + dz;
    if (nx >= 0 && nx < width && nz >= 0 && nz < height)
        return &grid[nz][nx];
    return nullptr;
}

std::vector<Plattform*> Level::getValidNeighbors(Plattform* p) {
    std::vector<Plattform*> options;
    int dirs[3][2] = { {-1, 0}, {1, 0}, {0, 1} };

    for (auto& d : dirs) {
        Plattform* n = getNeighbor(p, d[0], d[1]);
        if (n && !n->visited && hasGoodNeighborhood(n))
            options.push_back(n);
    }

    std::random_shuffle(options.begin(), options.end());
    return options;
}

bool Level::hasGoodNeighborhood(Plattform* p) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            if (dx == 0 && dz == 0) continue;
            Plattform* n = getNeighbor(p, dx, dz);
            if (n && n->visited) count++;
        }
    }
    return count < 3;
}

void Level::addLightPlatforms() {
    std::vector<Plattform*> lightPlatforms;

    size_t i = 4;// Erst ab dem 5. Pfadblock starten
    while (i < path.size()) {
        Plattform* current = path[i];

        Plattform* upNeighbor = getNeighbor(current, 0, -1);
        Plattform* leftNeighbor = getNeighbor(current, -1, 0);
        Plattform* downNeighbor = getNeighbor(current, 0, 1);
        Plattform* rightNeighbor = getNeighbor(current, 1, 0);

        Plattform* platform = nullptr;

        if (upNeighbor && !upNeighbor->visited) {
            platform = upNeighbor;
        }
        else if (leftNeighbor && !leftNeighbor->visited) {
            platform = leftNeighbor;
        }
        else if (downNeighbor && !downNeighbor->visited) {
            platform = downNeighbor;
        }
        else if (rightNeighbor && !rightNeighbor->visited) {
            platform = rightNeighbor;
        }

        if (platform) {
            bool tooClose = false;
            for (Plattform* existing : lightPlatforms) {
                int dx = std::abs(platform->x - existing->x);
                int dz = std::abs(platform->z - existing->z);

                if (dx <= 5 && dz <= 5) {
                    tooClose = true;
                    break;
                }
            }

            if (!tooClose) {
                platform->isLight = true;
                lightPlatforms.push_back(platform);
                path.push_back(platform);
                std::cout << "Licht hinzugefügttt bei (" << platform->x << ", " << platform->z << ")\n";

                // Hier ein paar Pfadblöcke überspringen, z.B. 4
                i += 4;
                continue;
            }
        }

        i++; // Normal weiter
    }

    std::cout << "\n=== Lichtplattformen ===\n";
    for (Plattform* p : lightPlatforms) {
        std::cout << "Licht bei (" << p->x << ", " << p->z << ")\n";
    }
}



