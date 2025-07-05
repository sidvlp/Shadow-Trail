//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif
#ifdef WIN32


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
#include "../CGVStudio/CGVStudio/ParticelSystem.h"
#include "../CGVStudio/CGVStudio/MenuManager.h"
#include "../CGVStudio/CGVStudio/ShaderLightMapper.h"


class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
   
    Application(GLFWwindow* pWin);
    void initialize(Difficulty difficulty);
    void start();
    void update(float);
    void draw();
    void drawScene(Camera& cam);
    void end();
    void keyPressPlayer1(float&, float&);
    void keyPressPlayer2(float&, float&);
    float toRadian(float degrees);
    bool isGameOver() const;
    void restartGame();
    void Application::addWall(float width, float height, const Vector& position, const Vector& rotationAxis, float angleDegrees, const Vector& surfaceNormal);
protected:
    Vector calc3DRay( float x, float y, Vector& Pos);
    Camera Cam1, Cam2;
    ModelList Models;
    int windowWidth;
    int windowHeight;
    GLFWwindow* pWindow;
    Player* player1;
    Player* player2;
    float fb, lr;
    double mx, my;
    bool gameEnded = false;
    std::vector<ParticleSystem*> fireSystems;
    ParticleShader* particleShader;
    float elapsedTime = 0.0f;
    Vector endPosition;
    Difficulty difficulty;
    MenuManager& gameManager = MenuManager::instance();
    void addFireSphereAndLight(const Vector& pos);

};

#endif 
