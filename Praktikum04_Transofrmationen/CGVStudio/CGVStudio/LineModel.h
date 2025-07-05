#include <stdio.h>
#include "../../src/basemodel.h"
#include "../../src/constantshader.h"
#include "../../src/vertexbuffer.h"
#include "../../src/IndexBuffer.h"


class LineModel : public BaseModel {
public:
    Vector start;
    Vector end;
    VertexBuffer VB;

    AABB BoundingBox;

    LineModel(const Vector& s, const Vector& e) : start(s), end(e) {
        VB.begin();

        VB.addVertex(start.X, start.Y, start.Z);
        VB.addVertex(end.X, end.Y, end.Z);

        VB.end();
    }

    void draw(const BaseCamera& Cam) override {
        if (!pShader) return;

        BaseModel::draw(Cam); // Damit Shader aktivieren + Transform setzen

        VB.activate();
        glDrawArrays(GL_LINES, 0, VB.vertexCount());
        VB.deactivate();
    }
    void calculateBoundingBox()
    {

    }

    AABB& getBoundingBox()
    {
        return BoundingBox;
    }
};
