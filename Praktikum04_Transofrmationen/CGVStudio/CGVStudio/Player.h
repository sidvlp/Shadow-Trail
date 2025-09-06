#pragma once



#include <stdio.h>
#include "../../src/Model.h"
#include "../../src/LineBoxModel.h"
#include <list>
#include "../../src/ConstantShader.h"

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
	bool checkGroundCollision(std::list<BaseModel*>& models);
	float toRadian(float degrees);
	void resetPosition();
	void drawBoundingBox(const BaseCamera& Cam);
	void updateBoundingBox();
	void updateFootBoundingBox(const AABB& localBox, const Matrix& transform);
	Vector getPosition() const;
	bool checkWallCollision(const std::list<BaseModel*>& models, Vector* wallNormalOut);
	void setStartPosition(const Vector& pos);

	bool rotationWouldCauseCollision(const std::list<BaseModel*>& models, const Matrix& testTransform);

	void drawOrientation(const BaseCamera& Cam);
	int getLives() const { return lives; }
	void setLives(int l) { lives = l; }

protected:
	Model* player;
	Vector direction;
	Vector fb;
	LineBoxModel* debugPlayerBoundingBox;
	LineBoxModel* debugPlayerFootBoundingBox;
	AABB footBoundingBox;
	bool isGrounded;
	bool isFalling;
	Vector playerDirection;
	Vector startPosition;
	float modelScale = 1.0f;
	Matrix startTransform;
	ConstantShader* constantBoundingBoxShader;
	ConstantShader* constantFootBoundingBoxShader;
	int lives = 3;

};

