#pragma once



#include <stdio.h>
#include "../../src/Model.h"
#include "../../src/LineBoxModel.h"
#include <list>

class Player : public Model
{
public:
	Player();
	virtual ~Player();
	bool loadModels(const char* player_model);
	void steer(float ForwardBackward, float LeftRight);
	void update(float dtime, Camera& cam);
	virtual void draw(const BaseCamera& Cam);
	bool checkGroundCollision(std::list<BaseModel*>& models);
	bool checkIfOnEndPlatform(std::list<BaseModel*>& models);
	//bool Player::checkGroundCollision(const AABB& a, const AABB& b);
	float toRadian(float degrees);
	void resetPosition();
	void drawBoundingBox(const BaseCamera& Cam);
	void updateBoundingBox();
	AABB& getBoundingBox();
	Vector getPosition() const;

protected:
	Model* player;
	Vector direction;
	Vector fb;
	LineBoxModel* pBoundingBoxModel;
	LineBoxModel* playerFootBoundingBox;
	bool isGrounded;
	bool isFalling;
	float fallVelocity;
	const float GRAVITY = -9.81f;

};

