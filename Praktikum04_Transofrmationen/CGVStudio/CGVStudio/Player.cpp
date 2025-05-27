#include "Player.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "../../src/ConstantShader.h"
#include "../../src/TriangleBoxModel.h"
#include <list>

Player::Player() : fb(0, 0, 0)
{
	pBoundingBoxModel = new LineBoxModel(1, 1, 1);
	playerFootBoundingBox = new LineBoxModel(1, 1, 1);
	isFalling = false;
	isGrounded = true;
	BoundingBox.Min = Vector(-0.5f, -0.5f, -0.5f);
	BoundingBox.Max = Vector(0.5f, 0.5f, 0.5f);
}

Player::~Player()
{
	delete pBoundingBoxModel;
	delete playerFootBoundingBox;
}

bool Player::loadModels(const char* player_model)
{
	if (player_model != nullptr) {
		player = new Model(player_model);
		player->shader(pShader);

		Matrix playerRotX, playerRotY, playerMove, playerUrsprung;
		playerRotX.rotationX(toRadian(-90));
		playerRotY.rotationY(toRadian(180));
		playerMove.translation(0, 0.5, 7.5);
		playerUrsprung = this->player->transform();
		playerUrsprung.scale(0.02f);
		player->transform(playerMove * playerUrsprung * playerRotY * playerRotX);
		return true;
	}
	return false;
}

float Player::toRadian(float degrees) {

	return (float)degrees * M_PI / 180;

}

void Player::resetPosition() {
	Matrix currentTransform = player->transform();

	// 1. Translation extrahieren (letzte Spalte der Matrix)
	Vector translation(
		currentTransform.m[12],  // Index 3 (0-basiert: 3, 7, 11)
		currentTransform.m[13],
		currentTransform.m[14]
	);

	// 2. Skalierung berechnen (Länge der Basisvektoren)
	Vector scaleX(currentTransform.m[0], currentTransform.m[1], currentTransform.m[2]);
	Vector scaleY(currentTransform.m[4], currentTransform.m[5], currentTransform.m[6]);
	Vector scaleZ(currentTransform.m[8], currentTransform.m[9], currentTransform.m[10]);
	Vector scale(scaleX.length(), scaleY.length(), scaleZ.length());

	// 3. Rotation extrahieren (Skalierung entfernen)
	Matrix rotationMatrix = currentTransform;

	// Skalierung rückgängig machen
	if (scale.X != 0) {
		rotationMatrix.m[0] /= scale.X; rotationMatrix.m[1] /= scale.X; rotationMatrix.m[2] /= scale.X;
	}
	if (scale.Y != 0) {
		rotationMatrix.m[4] /= scale.Y; rotationMatrix.m[5] /= scale.Y; rotationMatrix.m[6] /= scale.Y;
	}
	if (scale.Z != 0) {
		rotationMatrix.m[8] /= scale.Z; rotationMatrix.m[9] /= scale.Z; rotationMatrix.m[10] /= scale.Z;
	}

	// 4. Neue Transformationsmatrix erstellen (nur Translation zurücksetzen)
	Matrix newTransform;
	newTransform.translation(0, 5.5f, 7.5f);  // Reset-Position

	// Rotation wieder anwenden
	for (int i = 0; i < 3; ++i) {
		newTransform.m[i * 4] = rotationMatrix.m[i * 4] * scale.X;
		newTransform.m[i * 4 + 1] = rotationMatrix.m[i * 4 + 1] * scale.Y;
		newTransform.m[i * 4 + 2] = rotationMatrix.m[i * 4 + 2] * scale.Z;
	}

	player->transform(newTransform);

}

void Player::drawBoundingBox(const BaseCamera& Cam) {
	if (!pBoundingBoxModel) return;
	ConstantShader* pConstShader;
	pConstShader = new ConstantShader();
	pConstShader->color(Color(1, 1, 1));

	pBoundingBoxModel->shader(pConstShader, true);
	pBoundingBoxModel->draw(Cam);
}

void Player::updateBoundingBox()
{
	AABB localBox = player->boundingBox();
	Matrix transform = player->transform();

	Vector corners[8] = {
		Vector(localBox.Min.X, localBox.Min.Y, localBox.Min.Z),
		Vector(localBox.Min.X, localBox.Min.Y, localBox.Max.Z),
		Vector(localBox.Min.X, localBox.Max.Y, localBox.Min.Z),
		Vector(localBox.Min.X, localBox.Max.Y, localBox.Max.Z),
		Vector(localBox.Max.X, localBox.Min.Y, localBox.Min.Z),
		Vector(localBox.Max.X, localBox.Min.Y, localBox.Max.Z),
		Vector(localBox.Max.X, localBox.Max.Y, localBox.Min.Z),
		Vector(localBox.Max.X, localBox.Max.Y, localBox.Max.Z)
	};


	for (int i = 0; i < 8; ++i)
		corners[i] = transform * corners[i];

	Vector newMin = corners[0];
	Vector newMax = corners[0];

	for (int i = 1; i < 8; ++i) {
		newMin.X = std::min(newMin.X, corners[i].X);
		newMin.Y = std::min(newMin.Y, corners[i].Y);
		newMin.Z = std::min(newMin.Z, corners[i].Z);

		newMax.X = std::max(newMax.X, corners[i].X);
		newMax.Y = std::max(newMax.Y, corners[i].Y);
		newMax.Z = std::max(newMax.Z, corners[i].Z);
	}

	Vector bSize = newMax - newMin;
	Matrix scaleMat;
	scaleMat.scale(bSize.X, 0.1, bSize.Z);

	Vector bcenter = (newMin + newMax) * 0.5f;
	Matrix transMat;
	transMat.translation(bcenter.X, bcenter.Y / 2, bcenter.Z);
	pBoundingBoxModel->transform(transMat * scaleMat);

	BoundingBox.Min = Vector(newMin.X, newMin.Y, newMin.Z);
	BoundingBox.Max = Vector(newMax.X, newMax.Y, newMax.Z);
	//	std::cout << "BoundingBox Min: " << newMin.X << ", " << newMin.Y << ", " << newMin.Z << std::endl;
	//std::cout << "BoundingBox Max: " << newMax.X << ", " << newMax.Y << ", " << newMax.Z << std::endl;
	//std::cout << "---- " << std::endl;
}



AABB& Player::getBoundingBox()
{
	return BoundingBox;
}



void Player::steer(float ForwardBackward, float LeftRight)
{
	fb.X = ForwardBackward;
	fb.Y = LeftRight;
	//std::cout << "fb.X" << fb.X << std::endl;
	//std::cout << "fb.Y" << fb.Y << std::endl;
}

void Player::update(float dtime, Camera& cam)
{
	Matrix playerMove, playerRot, playerUrsprung;

	playerUrsprung = this->player->transform();

	if (isGrounded) {
		if (fb.X != 0) {
			playerRot.rotationZ(fb.Y * dtime * 5);
			playerMove.translation(0, -fb.X * dtime * 150, 0);
			this->player->transform(playerUrsprung * playerMove * playerRot);
		}
		else {
			playerRot.rotationZ(fb.Y * dtime * 5);
			this->player->transform(playerUrsprung * playerRot);
		}
	}
	updateBoundingBox();

	if (isFalling) {

		std::cout << "Falling" << std::endl;

		Matrix trans = player->transform();
		Matrix playerFall;
		playerFall.translation(0, 0, -5);
		player->transform(trans * playerFall);
		updateBoundingBox();

		if (BoundingBox.Min.Y < -10.0f) {
			isFalling = false;
			isGrounded = true;
			resetPosition();


		}
	}

	cam.setPosition(playerUrsprung.translation() + Vector(0, 5, 5) - player->transform().forward() * 50);
	cam.setTarget(playerUrsprung.translation());
}

void Player::draw(const BaseCamera& Cam)
{
	player->draw(Cam);
	drawBoundingBox(Cam);



}

bool Player::checkGroundCollision(std::list<BaseModel*>& models)
{
	for (BaseModel* model : models) {
		if (model != this) {
			const AABB& modelBox = model->getBoundingBox();
			bool groundCollision = (this->BoundingBox.Min.X <= modelBox.Max.X && this->BoundingBox.Max.X >= modelBox.Min.X) &&
				(this->BoundingBox.Min.Y <= modelBox.Max.Y && this->BoundingBox.Max.Y >= modelBox.Min.Y) &&
				(this->BoundingBox.Min.Z <= modelBox.Max.Z && this->BoundingBox.Max.Z >= modelBox.Min.Z);
			if (groundCollision) {
				//std::cout << "Col
				isGrounded = true;
				isFalling = false;

				return true;
			}
		}
	}
	isGrounded = false;
	isFalling = true;
	std::cout << "No collision" << std::endl;
	return false;
}

bool Player::checkIfOnEndPlatform(std::list<BaseModel*>& models)
{
	for (BaseModel* model : models) {
		if (model != this && model->isEndPlatform) {
			const AABB& modelBox = model->getBoundingBox();
			bool collision = (this->BoundingBox.Min.X <= modelBox.Max.X && this->BoundingBox.Max.X >= modelBox.Min.X) &&
				(this->BoundingBox.Min.Y <= modelBox.Max.Y && this->BoundingBox.Max.Y >= modelBox.Min.Y) &&
				(this->BoundingBox.Min.Z <= modelBox.Max.Z && this->BoundingBox.Max.Z >= modelBox.Min.Z);

			if (collision) {
				std::cout << "Ende erreicht!" << std::endl;
				return true;
			}
		}
	}
	return false;
}




//bool Player::checkGroundCollision(const AABB& a, const AABB& b) {
//	bool groundCollision = (a.Min.X <= b.Max.X && a.Max.X >= b.Min.X) &&
//		(a.Min.Y <= b.Max.Y && a.Max.Y >= b.Min.Y) &&
//		(a.Min.Z <= b.Max.Z && a.Max.Z >= b.Min.Z);
//	if (groundCollision) {
//	/*		isGrounded = true;
//			isFalling = false;
//			fallVelocity = 0.0f;*/
//			return true;
//		
//	}
//	//else {
//	//	isGrounded = false;
//	//	isFalling = true;
//	//	fallVelocity = 0.0f;
//	//}
//
//
//	return false;
//}





//AABB localBox = player->boundingBox();
//Matrix transform = player->transform();
//
//Vector corners[8] = {
//	Vector(localBox.Min.X, localBox.Min.Y, localBox.Min.Z),
//	Vector(localBox.Min.X, localBox.Min.Y, localBox.Max.Z),
//	Vector(localBox.Min.X, localBox.Max.Y, localBox.Min.Z),
//	Vector(localBox.Min.X, localBox.Max.Y, localBox.Max.Z),
//	Vector(localBox.Max.X, localBox.Min.Y, localBox.Min.Z),
//	Vector(localBox.Max.X, localBox.Min.Y, localBox.Max.Z),
//	Vector(localBox.Max.X, localBox.Max.Y, localBox.Min.Z),
//	Vector(localBox.Max.X, localBox.Max.Y, localBox.Max.Z)
//};
//
//
//for (int i = 0; i < 8; ++i)
//	corners[i] = transform * corners[i];
//
//Vector newMin = corners[0];
//Vector newMax = corners[0];
//
//for (int i = 1; i < 8; ++i) {
//	newMin.X = std::min(newMin.X, corners[i].X);
//	newMin.Y = std::min(newMin.Y, corners[i].Y);
//	newMin.Z = std::min(newMin.Z, corners[i].Z);
//
//	newMax.X = std::max(newMax.X, corners[i].X);
//	newMax.Y = std::max(newMax.Y, corners[i].Y);
//	newMax.Z = std::max(newMax.Z, corners[i].Z);
//}
//
//Vector bSize = newMax - newMin;
//Matrix scaleMat;
//scaleMat.scale(bSize.X, bSize.Y, bSize.Z);
//
//Vector bcenter = (newMin + newMax) * 0.5f;
//Matrix transMat;
//transMat.translation(bcenter.X, bcenter.Y, bcenter.Z);
//pBoundingBoxModel->transform(transMat * scaleMat);