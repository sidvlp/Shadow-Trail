//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "basemodel.h"
#include "tank.h"
#include "../CGVStudio/CGVStudio/Player.h"
#include "TriangleBoxModel.h";

class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
   
    Application(GLFWwindow* pWin);
    void start();
    void update(float);
    void draw();
    void end();
    void keyPress(float&, float&);
    float toRadian(float degrees);
protected:
    Vector calc3DRay( float x, float y, Vector& Pos);
    Camera Cam;
    ModelList Models;
   
    GLFWwindow* pWindow;
    Tank* pTank;
    Model* pTankTop;
    Model* pTankBot;
    Player* player;
    float fb, lr;
    double mx, my;
};

#endif /* Application_hpp */
