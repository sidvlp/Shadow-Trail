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
	bool loadModels(const char* player_model, float scaleFactor);
	void steer(float ForwardBackward, float LeftRight);
	void drawDirectionVector(const BaseCamera& Cam);

	void update(float dtime, Camera& cam, std::list<BaseModel*>& models);
	virtual void draw(const BaseCamera& Cam);
	void resolveCollision(float dot, const Vector& wallNormal, Matrix& totalTransform, const std::list<BaseModel*>& models);
	void trySafeRotate(const Vector& wallNormal, Matrix& totalTransform, const std::list<BaseModel*>& models, float maxAngleDeg);
	void trySafeRotateZ(float desiredAngleDeg, Matrix& totalTransform, const std::list<BaseModel*>& models);
	Vector getWallNormal(const AABB& wallBox, const AABB& playerBox);
	Vector computePenetrationCorrection(const AABB& playerBox, const AABB& wallBox);
	bool checkGroundCollision(std::list<BaseModel*>& models);
	void updateFootBoundingBox();
	bool checkIfOnEndPlatform(std::list<BaseModel*>& models);
	float toRadian(float degrees);
	void resetPosition();
	void drawBoundingBox(const BaseCamera& Cam);
	void updateBoundingBox();
	void updateFootBoundingBox(const AABB& localBox, const Matrix& transform);
	AABB& getBoundingBox();
	Vector getPosition() const;
	bool checkWallCollision(const std::list<BaseModel*>& models, Vector& pushback, Vector* wallNormalOut);
	void setPosition(const Vector& pos);



	bool rotationWouldCauseCollision(const std::list<BaseModel*>& models, const Matrix& testTransform);


	void drawOrientation(const BaseCamera& Cam);

protected:
	Model* player;
	Vector direction;
	Vector fb;
	LineBoxModel* pBoundingBoxModel;
	LineBoxModel* playerFootBoundingBox;
	AABB footBoundingBox;
	bool isGrounded;
	bool isFalling;
	float fallVelocity;
	const float GRAVITY = -9.81f;
	Vector playerDirection;
	Vector startPosition;
	float modelScale = 1.0f;

};

