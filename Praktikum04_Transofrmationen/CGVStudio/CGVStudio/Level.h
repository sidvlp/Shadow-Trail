#pragma once
#include <vector>
#include "Plattform.h"

class Level
{
    int width, height;
    std::vector<std::vector<Plattform>> grid;

    public:
    Level(int w, int h);
    void generatePathWithLights();
    const std::vector<Plattform*>& getPath() const;


    private:
    std::vector<Plattform*> path;
    Plattform* getNeighbor(Plattform* p, int dx, int dz);
    std::vector<Plattform*> getValidNeighbors(Plattform* p);
    bool hasGoodNeighborhood(Plattform* p);

    void generatePath();         
    void addLightPlatforms();
};


