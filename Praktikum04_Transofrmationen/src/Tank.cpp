//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Tank.h"


Tank::Tank():fb(0,0,0), angleCorrection(0.0f)
{

}

Tank::~Tank()
{
    delete chassis;
    delete cannon;
}

bool Tank::loadModels(const char* ChassisFile, const char* CannonFile)
{

    if (ChassisFile != nullptr && CannonFile != nullptr) {
        chassis = new Model(ChassisFile);
        chassis->shader(pShader);
        cannon = new Model(CannonFile);
        cannon->shader(pShader);
        return true; 
    }

    return false;
    
}

void Tank::steer( float ForwardBackward, float LeftRight)
{
    fb.X = ForwardBackward;
    fb.Y = LeftRight;
}

void Tank::aim(const Vector& Target )
{   
    //Abfangen ob der übergeneme Vektor 0 0 0 ist, da es hier keine Richtung gibt in die das Rohr zeigen kann
    if (Target.X == 0 && Target.Y == 0 && Target.Z == 0) {
        return;
    }


    //aktzuelle Position der Kanone holen
    Matrix cannonPos = this->cannon->transform();

    //delta Werte zwischen Kanone und Target Berechnen
    float deltaX =  -Target.X + cannonPos.m03;
    float deltaZ =  -Target.Z + cannonPos.m23;

    //mit arctan den Winkel der Kanone berechnen
    angleCannon = atan2f(deltaZ, -deltaX);
    
}

void Tank::update(float dtime)
{
    Matrix chassisMatrix, chassisMov , chassisRot, chassisUrsprung;
    Matrix cannonMatrix;
  
    //chassis
    chassisUrsprung = chassis->transform(); 
    chassisRot.rotationY(fb.Y * dtime);
    chassisMov.translation(fb.X*dtime*2,0,0);

    chassisMatrix = chassisUrsprung * chassisMov * chassisRot;
    this->chassis->transform(chassisMatrix);


    //cannon
    Matrix rotCannonCorrection;
    angleCorrection -= fb.Y * dtime;
    rotCannonCorrection.rotationY(angleCorrection + angleCannon);
    
    cannonMatrix = chassisUrsprung * rotCannonCorrection;
    this->cannon->transform(cannonMatrix);


}

void Tank::draw(const BaseCamera& Cam)
{
    chassis->draw(Cam);
    cannon->draw(Cam);
}
