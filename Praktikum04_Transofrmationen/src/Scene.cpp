//
//  Scene.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 06.11.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include "Scene.h"
#include <string.h>
#include "model.h"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#define ASSET_EXTRA "../"
#else
#define ASSET_DIR "../assets/"
#define ASSET_EXTRA ""
#endif //WIN32 

SceneNode::SceneNode() : m_pParent(NULL), m_pModel(NULL), m_Scaling(1,1,1)
{
   setLocalTransform(Vector(0, 0, 0), Vector(0, 1, 0), 0.0f);
}

SceneNode::SceneNode( const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel)
{
    setName(Name);
    setLocalTransform( Translation, RotationAxis, RotationAngle);
    setScaling(Scale);
    setParent(pParent);
    setModel(pModel);
}

const Matrix& SceneNode::getLocalTransform() const
{
    return m_LocalTransform;
}

void SceneNode::setLocalTransform( const Vector& Translation, const Vector& RotationAxis, const float RotationAngle)
{
    Matrix translation, rotation;

    rotation.rotationAxis(RotationAxis, RotationAngle);
    translation.translation(Translation);
    
    setLocalTransform(translation * rotation);//CHECK: Transform reinfolge richtig ? 
}

void SceneNode::setLocalTransform( const Matrix& LocalTransform)
{
    m_LocalTransform = LocalTransform;
}

Matrix SceneNode::getGlobalTransform() const
{
    Matrix scaling, lokal, global;
    //get Scaling und lokal Transformation vektoren.
    scaling.scale(this->getScaling());
    lokal = this->getLocalTransform();
    //this node global transform
    global = lokal * scaling;

    SceneNode* tmp = this->m_pParent;

    //solang es parent gibt, für alle nodes die global transform berechnen abhängig vom parent. 
    while (tmp != nullptr) {
        global = tmp->getLocalTransform() * global;
        tmp = tmp->m_pParent;
    }


    return global; 
}

const SceneNode* SceneNode::getParent() const
{
    return m_pParent;
}

void SceneNode::setParent( SceneNode* pNode)
{
    this->m_pParent = pNode;
}

const std::set<SceneNode*>& SceneNode::getChildren() const
{
    return m_Children;
}

void SceneNode::addChild(SceneNode* pChild)
{
    m_Children.insert(pChild);
}

void SceneNode::removeChild(SceneNode* pChild)
{
    m_Children.erase(pChild);
}

void SceneNode::setModel( Model* pModel)
{
    m_pModel = pModel;
}

const Model* SceneNode::getModel() const
{
    return m_pModel;
}

void SceneNode::setName( const std::string& Name)
{
    m_Name = Name;
}

const std::string& SceneNode::getName() const
{
    return m_Name;
}

const Vector& SceneNode::getScaling() const
{
    return m_Scaling;
}
void SceneNode::setScaling( const Vector& Scaling)
{
    m_Scaling = Scaling;
}

void SceneNode::draw(const BaseCamera& Cam)
{
    if (m_pModel != nullptr) {
        m_pModel->transform(this->getGlobalTransform());
        m_pModel->draw(Cam);
    }
    for (SceneNode* tmp : m_Children) {
        tmp->draw(Cam);
    }
}


Scene::Scene()
{
}

Scene::~Scene()
{
    m_Models.clear();
}



bool Scene::addSceneFile( const char* Scenefile)
{
    std::ifstream file(Scenefile);
    std::string fileLine;

    if (!file) {
        std::cout << "Failed to load file" << std::endl;
        return false;
    }

    while (std::getline(file, fileLine)) {
        //checken ob es ein Model ist
        if (fileLine.front() == 'M') {
            char ModelID[256];
            char ModelFile[256];
            //modelID und Filename einlesen und speichern
            sscanf(fileLine.c_str(), "MODEL ID=%s FILE=%s", ModelID, ModelFile);
            std::stringstream ss;
            //dir + model name vorbereiten.
            ss << ASSET_EXTRA << ModelFile;
            //model erzeugen
            Model* pModel = new Model(ss.str().c_str(), false);
            pModel->shader(pShader,true);
            //model hinzufügen in die liste
            m_Models.insert(std::pair<std::string, Model*>(ModelID, pModel));

        }
        else {
            //zeile ist NODE
            Vector Position, Rotation, Scale;
            float angle;
            char nodeID[256] = "";
            char nodeParentID[256] ="";
            char modelID[256] = "";
            //alles einlesen und speichern
            sscanf(fileLine.c_str(),
                "NODE ID=%s PARENTID=%s MODELID=%s TRANSLATION=%f %f %f ROTATIONAXIS=%f %f %f ROTATIONANGLE=%f SCALE=%f %f %f",
                nodeID, nodeParentID, modelID, 
                &Position.X, &Position.Y, &Position.Z, 
                &Rotation.X, &Rotation.Y, &Rotation.Z, 
                &angle, &Scale.X, &Scale.Y, &Scale.Z);

            //nach Parentnode suchen, wenn keiner vorhanden ist es der root node.
            SceneNode* parentNode = nullptr;
            if (nodeParentID != "") {
                parentNode = searchNode(nodeParentID, m_Root);
            }
            if (parentNode == nullptr) {
                parentNode = &m_Root;
            }
            //child erzeugen und den parent zuweisen
            SceneNode* tmp = new SceneNode(nodeID, Position, Rotation, angle, Scale, parentNode, m_Models[modelID]);
            parentNode->addChild(tmp);
        }
    }

    return true;
}

SceneNode* Scene::searchNode(std::string id, const SceneNode& toSearch) {

    for (SceneNode* tmp : toSearch.getChildren()) {
        if (tmp->getName() == id) {
            return tmp;
        }
        SceneNode* search = searchNode(id, *tmp);
        if (search != nullptr) {
            return search;
        }
    }
    return nullptr;
}


void Scene::draw( const BaseCamera& Cam)
{
    m_Root.draw(Cam);
}