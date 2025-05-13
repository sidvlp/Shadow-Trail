#pragma once

class Plattform {
    public:
        int x, z;
        bool visited;
        Plattform* predecessor;

        Plattform(int x = 0, int z = 0)
            : x(x), z(z), visited(false), predecessor(nullptr) {
        }
};

