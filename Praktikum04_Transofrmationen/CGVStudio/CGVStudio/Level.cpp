#include "Level.h"
#include "Plattform.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Level::Level(int w, int h) : width(w), height(h) {
    grid.resize(height, std::vector<Plattform>(width));
    for (int z = 0; z < height; ++z)
        for (int x = 0; x < width; ++x)
            grid[z][x] = Plattform(x, z);
}

std::vector<Plattform*> Level::generatePath() {
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
            if (!current) return {};
            continue;
        }

        Plattform* chosen = neighbors[std::rand() % neighbors.size()];
        chosen->visited = true;
        chosen->predecessor = current;
        current = chosen;
    }

    std::vector<Plattform*> path;
    Plattform* temp = current;
    while (temp) {
        path.push_back(temp);
        temp = temp->predecessor;
    }

    std::reverse(path.begin(), path.end());
    return path;
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
