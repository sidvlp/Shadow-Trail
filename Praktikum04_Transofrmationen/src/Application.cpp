//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "scene.h"


#define _USE_MATH_DEFINES
#include "math.h"
#include "../CGVStudio/CGVStudio/Level.h"
#include "../CGVStudio/CGVStudio/ParticelSystem.h"
#include "../CGVStudio/CGVStudio/MenuManager.h"




Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam1(pWin),Cam2(pWin), fb(0), lr(0), mx(0), my(0)
{
    glfwGetWindowSize(pWin, &windowWidth, &windowHeight);
}

void Application::initialize(Difficulty difficulty) {
    this->difficulty = difficulty;
    Matrix translation, rotationY, rotationX, rotationZ;

    int width = (difficulty == Difficulty::Easy) ? 10 : 15;
    int height = (difficulty == Difficulty::Easy) ? 10 : 20;
    float offset = 0.5f;
    float xOffSet = (difficulty == Difficulty::Hard) ? 5.0f : 0.0f;
    float xOffSet2 = (difficulty == Difficulty::Hard) ? 2.5f : 0.0f;
    float zOffSet = (difficulty == Difficulty::Hard) ? 5.0f : 0.0f;


    // Spieler 1
    player1 = new Player();
    player1->shader(new PhongShader(), false);
    player1->setPosition(Vector(width / 2.0f, 0.5f, height + 2.5f));
    player1->loadModels(ASSET_DIRECTORY "12248_Bird_v1_L2.obj", 0.01f);
    Models.push_back(player1);

    // Spieler 2 (optional)
    if (gameManager.state == MenuState::MultiPlayer) {
        player2 = new Player();
        player2->shader(new PhongShader(), false);
        player2->setPosition(Vector(width / 2.0f, 0.5f, height + 2.5f));
        player2->loadModels(ASSET_DIRECTORY "13463_Australian_Cattle_Dog_v3.obj", 0.02f);
        Models.push_back(player2);
    }

    // Level mit Pfad und Lichtplattformen
    Level level(width, height);
    level.generatePathWithLights();
    const auto path = level.getPath();

    float startX = 0.0f;
    float startY = 0.0f;
    float startZ = 0.0f;

    particleShader = new ParticleShader();

    for (auto* plattform : path) {
        auto* box = new TriangleBoxModel(1, 1, 1);
        auto* pathShader = new PhongShader();
        pathShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY "brick.jpg"));
        box->shader(pathShader, true);
        box->isPath = true;

        Matrix translation;
        float x = startX + plattform->x;
        float z = startZ + plattform->z;
        translation.translation(x, startY, z);
        box->transform(box->transform() * translation);
        box->calculateBoundingBox();
        Models.push_back(box);

        /*
        std::cout << (plattform->isLight ? "Licht" : "Pfad")
            << " Plattform: (" << plattform->x << ", " << plattform->z << ")\n";
        */

        if (plattform->isLight) {
            box->isPath = false;

            auto* fire = new TriangleSphereModel(0.5f);
            Matrix fireTrans;
            fireTrans.translation(x, startY + 1.0f, z);
            fire->transform(fire->transform() * fireTrans);
            auto* fireShader = new PhongShader();
            fireShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY "fire.jpg"));
            fire->shader(fireShader, true);
            fire->calculateBoundingBox();
            Models.push_back(fire);
            rotatingFires.push_back(fire);

            auto* light = new PointLight(Vector(x, startY + 1.0f, z), Color(1.0f, 0.5f, 0.2f));
            light->attenuation(Vector(1.0f, 0.1f, 0.05f));
            ShaderLightMapper::instance().addLight(light);

            fireSystems.push_back(new ParticleSystem(200, Vector(x, startY + 1.0f, z), ParticleSpawnMode::Default));
        }
    }

    // Zusätzliche Feuerkugeln bei Start- und Endplattform
    float fireY = 1.0f;
    float fireRadius = 0.5f;
    float platformDepth = 5.0f;

    float startXPlat = width / 2.0f - offset;
    float startZPlat = -2.5f - offset;
    float endXPlat = width / 2.0f - offset;
    float endZPlat = height + 2.5f - offset;

    float rearZ_start = startZPlat + platformDepth / 2.0f;
    float rearZ_end = endZPlat + platformDepth / 2.0f;

    addFireSphereAndLight(Vector(startXPlat - width / 2.0f + fireRadius, fireY, rearZ_start - platformDepth + fireRadius));
    addFireSphereAndLight(Vector(startXPlat + width / 2.0f - fireRadius, fireY, rearZ_start - platformDepth + fireRadius));
    addFireSphereAndLight(Vector(endXPlat - width / 2.0f + fireRadius, fireY, rearZ_end - fireRadius));
    addFireSphereAndLight(Vector(endXPlat + width / 2.0f - fireRadius, fireY, rearZ_end - fireRadius));

    // Startplattform
    auto* startPlatform = new TrianglePlaneModel(width, 5, 1, 1);
    auto* wallShader = new PhongShader();
    wallShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY "brick_platform.jpg"));
    startPlatform->shader(wallShader, false);
    translation.translation(width / 2.0f - offset, 0.5f, -2.5f - offset);
    startPlatform->transform(startPlatform->transform() * translation);
    startPlatform->calculateBoundingBox();
    Models.push_back(startPlatform);

    BaseModel* pModel = nullptr;

    // Linke Wand
    pModel = new TrianglePlaneModel(width * 2, 20, 1, 1);
    pModel->isWall = true;
    pModel->shader(wallShader, false);
    translation.translation(-0.5, 10.5, 4.5 + zOffSet);
    rotationY.rotationY(toRadian(90));
    rotationZ.rotationZ(toRadian(-90));
    pModel->transform(pModel->transform() * translation * rotationZ * rotationY);
    pModel->setSurfaceNormal(Vector(1, 0, 0));
    pModel->calculateBoundingBox();
    Models.push_back(pModel);

    // Rechte Wand
    pModel = new TrianglePlaneModel(width * 2, 20, 1, 1);
    pModel->isWall = true;
    pModel->shader(wallShader, false);
    translation.translation(9.5 + xOffSet, 10.5, 4.5 + zOffSet);
    rotationY.rotationY(toRadian(90));
    rotationZ.rotationZ(toRadian(90));
    pModel->transform(pModel->transform() * translation * rotationZ * rotationY);
    pModel->setSurfaceNormal(Vector(-1, 0, 0));
    pModel->calculateBoundingBox();
    Models.push_back(pModel);

    // Hintere Wand
    pModel = new TrianglePlaneModel(width, 20, 1, 1);
    pModel->isWall = true;
    pModel->shader(wallShader, false);
    translation.translation(4.5 + xOffSet2, 10.5, -5.5);
    rotationX.rotationX(toRadian(90));
    pModel->transform(pModel->transform() * translation * rotationX);
    pModel->setSurfaceNormal(Vector(0, 0, 1));
    pModel->calculateBoundingBox();
    Models.push_back(pModel);

    // Endplattform
    auto* endPlatform = new TrianglePlaneModel(width, 5, 1, 1);
    endPlatform->shader(wallShader, false);
    translation.translation(width / 2.0f - offset, 0.5f, height + 2.5f - offset);
    endPlatform->transform(endPlatform->transform() * translation);
    endPlatform->calculateBoundingBox();
    Models.push_back(endPlatform);

    endPosition = Vector(width / 2.0f - offset, 0.5f, -2.5f);
    fireSystems.push_back(new ParticleSystem(200, endPosition, ParticleSpawnMode::Ring));
}




void Application::start()
{
    glEnable (GL_DEPTH_TEST); 
    glDepthFunc (GL_LESS); 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  
}

float Application::toRadian(float degrees) {

    return (float) degrees * M_PI /180;

}


void Application::update(float dtime)
{
    if (MenuManager::instance().resetRequested) {
        restartGame();
        MenuManager::instance().resetRequested = false;
    }

    elapsedTime += dtime;

    player1->checkGroundCollision(Models);
    keyPressPlayer1(fb, lr);
    player1->steer(fb, lr);
    fb = 0;
    lr = 0;
    player1->update(dtime, Cam1, Models);

    Vector player1Pos = player1->getPosition();
    Vector diff1 = Vector(player1Pos.X, 0, player1Pos.Z) - Vector(endPosition.X, 0, endPosition.Z);

    if (MenuManager::instance().state == MenuState::MultiPlayer)
    {
        player2->checkGroundCollision(Models);
        keyPressPlayer2(fb, lr);
        player2->steer(fb, lr);
        fb = 0;
        lr = 0;
        player2->update(dtime, Cam2, Models);

        Vector player2Pos = player2->getPosition();
        Vector diff2 = Vector(player2Pos.X, 0, player2Pos.Z) - Vector(endPosition.X, 0, endPosition.Z);

        if (!gameEnded && (diff1.length() < 0.6f || diff2.length() < 0.6f)) {
            gameEnded = true;
            std::cout << "Spiel beendet (Multiplayer)" << std::endl;
        }
    }
    else
    {
        if (!gameEnded && diff1.length() < 0.6f) {
            gameEnded = true;
            std::cout << "Spiel beendet (Singleplayer)" << std::endl;
        }
    }

    for (auto* ps : fireSystems)
        ps->Update(dtime);

    for (BaseModel* model : rotatingFires) {
        Matrix current = model->transform();

        // Mittelpunkt extrahieren
        Vector pos(current.m[12], current.m[13], current.m[14]);

        // Rückübersetzung zum Ursprung
        Matrix toOrigin, backToPos, rotY;
        toOrigin.translation(-pos);
        backToPos.translation(pos);

        // Y-Rotation
        rotY.rotationY(toRadian(45.0f * dtime)); // z. B. 45 Grad pro Sekunde

        // Neue Transformation anwenden
        Matrix newTransform = backToPos * rotY * toOrigin * current;
        model->transform(newTransform);
    }


    Cam1.update();
    Cam2.update();
}


void Application::keyPressPlayer1(float &fb, float &lr) {
    
    
    if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) {
        fb = -1;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) {
        fb = 1;
    }
  
   
    if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
        lr = -1;
    }
  
    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
        lr = 1;
    }
   

}void Application::keyPressPlayer2(float &fb, float &lr) {
    
    
    if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
        fb = -1;
    }
    if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS) {
        fb = 1;
    }
  
   
    if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        lr = -1;
    }
  
    if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
        lr = 1;
    }
   

}

Vector Application::calc3DRay( float x, float y, Vector& Pos)
{   
    //normalisierte Koordinaten berechnen
    int windowWidthHalf = 0;
    int windowHeightHalf = 0;
    glfwGetWindowSize(pWindow, &windowWidthHalf, &windowHeightHalf);
    windowHeightHalf /= 2;
    windowWidthHalf /= 2;

    //Feld auf -1 bis 1 normalisieren
    float xNormal, yNormal;
    xNormal = (x - windowWidthHalf) / windowWidthHalf;
    yNormal = -(y - windowHeightHalf) / windowHeightHalf;
    
    //Richtungsvector(Kamararaum) erzeugen mit inverser Projektionsmatrix
    Matrix projectionCam = this->Cam1.getProjectionMatrix(); 
    Vector normalCursor(xNormal, yNormal, 0);
    Vector direction = projectionCam.invert() * normalCursor;

    //Umwandlung des Richtungsvectors in den Weltraum
    Matrix viewMatrix = this->Cam1.getViewMatrix();
    Vector directionInWeltraum = viewMatrix.invert().transformVec3x3(direction);

    //Schnittpunkt mit der Ebene y=0 bestimmen
    Vector camPos = this->Cam1.position();
    directionInWeltraum.normalize();
    float s;
    camPos.triangleIntersection(directionInWeltraum, Vector(0, 0, 1), Vector(0, 0, 0), Vector(1, 0, 0), s);

    //falls directionWolrld von der Ebene weg zeigt (0,0,0) zurückgeben
    if (s < 0) {
        return Vector(0, 0, 0);
    }

    //Vektor zum Punkt auf der Ebene y=0 berechnen
    Vector positionOnGround = camPos + directionInWeltraum * s;

    //float Ungenauigkeiten umgehen indem der Vektor erneut auf y = 0 gesetzt wird
    return Vector(positionOnGround.X, 0, positionOnGround.Z);

 
    // Pos:Ray Origin
    // return:Ray Direction
    
}

void Application::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ShaderLightMapper::instance().activate();


    if (MenuManager::instance().state == MenuState::MultiPlayer) {

        glViewport(0, 0, windowWidth / 2, windowHeight);
        Cam1.update();
        drawScene(Cam1);
        particleShader->activate(Cam1);
        for (auto* ps : fireSystems)
            ps->Render(Cam1.getProjectionMatrix() * Cam1.getViewMatrix());

        glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight);
        Cam2.update();
        drawScene(Cam2);
        particleShader->activate(Cam2);
        for (auto* ps : fireSystems)
            ps->Render(Cam2.getProjectionMatrix() * Cam2.getViewMatrix());
    }
    else {
        glViewport(0, 0, windowWidth, windowHeight);
        Cam1.update();
        drawScene(Cam1);
        particleShader->activate(Cam1);
        for (auto* ps : fireSystems)
            ps->Render(Cam1.getProjectionMatrix() * Cam1.getViewMatrix());
    }
    if (MenuManager::instance().state == MenuState::Start || MenuManager::instance().state == MenuState::GameWon) {
        glViewport(0, 0, windowWidth, windowHeight);

        Cam1.setPosition(Vector(8.1023f, 4.16865f, 14.3518f));
        Cam1.setTarget(Vector(0.0f, 0.0f, 0.0f));
        Cam1.update();
        drawScene(Cam1);
    }

  

    GLenum Error = glGetError();
    assert(Error == 0);
}

void Application::drawScene(Camera& cam) {
    for (auto it = Models.begin(); it != Models.end(); ++it)
    {
        PhongShader* shader = dynamic_cast<PhongShader*>((*it)->shader());
        if (shader)
        {
            shader->setDarkPath((*it)->isPath);
        }

        (*it)->draw(cam);
    }
}


void Application::end()
{
    for (BaseModel* model : Models)
        delete model;
    Models.clear();

    for (ParticleSystem* ps : fireSystems)
        delete ps;
    fireSystems.clear();

    rotatingFires.clear(); // Die Fire-Sphere-Modelle sind in Models enthalten → NICHT löschen

    // delete player1;
    player1 = nullptr;

    if (player2) {
        // ❌ NICHT MEHR NÖTIG (schon oben gelöscht)
        // delete player2;
        player2 = nullptr;
    }

    ShaderLightMapper::instance().clear();
}


void Application::restartGame() {
    gameEnded = false;
    player1->resetPosition();  
    if (MenuManager::instance().state == MenuState::MultiPlayer) {
        player2->resetPosition();
    }

        MenuManager::instance().lastMode = MenuManager::instance().state;
}

void Application::addFireSphereAndLight(const Vector& pos) {
    PhongShader* fireShader = new PhongShader();
    fireShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY "fire.jpg"));

    TriangleSphereModel* fire = new TriangleSphereModel(0.5f);
    Matrix trans;
    trans.translation(pos);
    fire->transform(fire->transform() * trans);
    fire->shader(fireShader, true);
    fire->calculateBoundingBox();
    Models.push_back(fire);
    rotatingFires.push_back(fire);

    PointLight* light = new PointLight(pos, Color(1.0f, 0.5f, 0.2f));
    light->attenuation(Vector(1.0f, 0.1f, 0.05f));
    ShaderLightMapper::instance().addLight(light);
}

void Application::reinitialize(Difficulty newDifficulty) {
    end();
    initialize(newDifficulty);
    start();
}




