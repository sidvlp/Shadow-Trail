#include "Player.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "../../src/ConstantShader.h"
#include "../../src/TriangleBoxModel.h"
#include "../CGVStudio/LineModel.h"
#include <list>
#include <algorithm>

template <typename T>
T clamp(T value, T minVal, T maxVal) {
	return std::max(minVal, std::min(value, maxVal));
}


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
		playerMove.translation(startPosition);
		playerUrsprung.scale(0.01f);
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
	newTransform.translation(startPosition);  // Reset-Position


	// Rotation wieder anwenden
	for (int i = 0; i < 3; ++i) {
		newTransform.m[i * 4] = rotationMatrix.m[i * 4] * scale.X;
		newTransform.m[i * 4 + 1] = rotationMatrix.m[i * 4 + 1] * scale.Y;
		newTransform.m[i * 4 + 2] = rotationMatrix.m[i * 4 + 2] * scale.Z;
	}

	player->transform(newTransform);
	updateBoundingBox();
	float modelBottom = BoundingBox.Min.Y;
	float desiredGroundHeight = 0.0f;  // auf Höhe 0 platzieren

	float yOffset = desiredGroundHeight - modelBottom;

	Matrix fixHeight;
	fixHeight.translation(0, 3, 0);
	player->transform(fixHeight * player->transform());

	updateBoundingBox();

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

	Vector bSize = localBox.Max - localBox.Min;
	Vector bCenter = (localBox.Min + localBox.Max) * 0.5f;

	Matrix scaleMat, transMat;
	scaleMat.scale(bSize.X, bSize.Y, bSize.Z);
	transMat.translation(bCenter.X, bCenter.Y, bCenter.Z);

	Matrix modelMatrix = player->transform();
	pBoundingBoxModel->transform(modelMatrix * transMat * scaleMat);


	BoundingBox.Min = Vector(newMin.X, newMin.Y, newMin.Z);
	BoundingBox.Max = Vector(newMax.X, newMax.Y, newMax.Z);
	/*std::cout << "BoundingBox Min: " << newMin.X << ", " << newMin.Y << ", " << newMin.Z << std::endl;
	std::cout << "BoundingBox Max: " << newMax.X << ", " << newMax.Y << ", " << newMax.Z << std::endl;*/
	//std::cout << "---- " << std::endl;
}

AABB& Player::getBoundingBox()
{
	return BoundingBox;
}

void Player::setPosition(const Vector& pos) {
	startPosition = pos;
}

void Player::steer(float ForwardBackward, float LeftRight)
{
	fb.X = ForwardBackward;
	fb.Y = LeftRight;
	
}

void Player::update(float dtime, Camera& cam, std::list<BaseModel*>& models)
{
	Matrix playerMove, playerRot, playerUrsprung;
	playerUrsprung = this->player->transform();

	if (isGrounded) {
		playerRot.rotationZ(fb.Y * dtime * 5);

		Vector intendedMove(0, -fb.X * dtime * 150, 0);
		Vector remainingMove = intendedMove;

		float stepSize = 0.05f; // 5 cm Schrittweite

		Matrix totalTransform = playerUrsprung;

		while (remainingMove.X != 0.0f || remainingMove.Y != 0.0f || remainingMove.Z != 0.0f) {

			// Schrittbewegung berechnen
			Vector stepMove(
				clamp(remainingMove.X, -stepSize, stepSize),
				clamp(remainingMove.Y, -stepSize, stepSize),
				clamp(remainingMove.Z, -stepSize, stepSize)
			);

			// Kollisionsprüfung
			Vector pushback, wallNormal;
			bool collision = checkWallCollision(models, stepMove, pushback, &wallNormal);

			if (collision) {
				float dot = playerDirection.normalize().dot(wallNormal.normalize());

				/*std::cout << "Wand Kollision!" << std::endl;
				std::cout << "PlayerDirection: " << playerDirection.X << std::endl;
				std::cout << "WallNormal:      " << wallNormal << std::endl;
				std::cout << "Dot:             " << dot << std::endl;*/

				resolveCollision(dot, wallNormal, totalTransform, stepSize, models);

				player->transform(totalTransform);
				updateBoundingBox();
				playerDirection = -player->transform().up().normalize();

				break; // Nach Reaktion Bewegung abbrechen
			}

			else {
				// Bewegung anwenden
				totalTransform = totalTransform * Matrix().translation(stepMove.X, stepMove.Y, stepMove.Z);

				// Subtrahiere Step
				remainingMove.X -= stepMove.X;
				remainingMove.Y -= stepMove.Y;
				remainingMove.Z -= stepMove.Z;
			}

		}

		Matrix testTransform = totalTransform * playerRot;

		if (!rotationWouldCauseCollision(models, testTransform)) {
			// Rotation erlaubt → anwenden
			totalTransform = testTransform;
		}
		else {
			// Rotation würde Kollision verursachen → nicht drehen
			std::cout << "Rotation blockiert wegen möglicher Kollision." << std::endl;
		}

		player->transform(totalTransform);
		Matrix finalTransform = player->transform();
		playerDirection = -finalTransform.up().normalize();


	}

	// BoundingBox aktualisieren
	updateBoundingBox();

	// Falling Logik bleibt gleich
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

	cam.setPosition(playerUrsprung.translation() + Vector(0, 2, 3) - player->transform().forward());
	cam.setTarget(playerUrsprung.translation());
}

void Player::draw(const BaseCamera& Cam)
{
	player->draw(Cam);
	drawBoundingBox(Cam);
	//drawOrientation(Cam); // Hier!

	drawDirectionVector(Cam);

}

void Player::resolveCollision(float dot, const Vector& wallNormal, Matrix& totalTransform, float stepSize, const std::list<BaseModel*>& models)
{
	std::cout << "[resolveCollision] Dot: " << dot << std::endl;

	// Fall 1: Frontal gegen Wand
	if (dot < -0.9f) {
		std::cout << "Frontal gegen Wand → versuche Wegdrehung" << std::endl;
		trySafeRotateZTowardsWall(wallNormal, totalTransform, models, 180.0f);
	}


	// Fall 2: Schräge Kollision → leicht wegdrehen
	else if (dot < -0.3f) {
		std::cout << "Schräge Kollision → leichte Drehung" << std::endl;
		trySafeRotateZTowardsWall(wallNormal, totalTransform, models, 30.0f);

	}

	// Fall 3: Seitlich → entlang Wand gleiten
	else if (std::abs(dot) < 0.9f) {
		std::cout << "Schräge Kollision → versuche adaptive Rotation" << std::endl;
		trySafeRotateZTowardsWall(wallNormal, totalTransform, models, 15.0f);

	}


	// Fall 4: Von hinten gegen Wand
	else if (dot > 0.9f) {
		std::cout << "Rückwärts gegen Wand → Umdrehen" << std::endl;

		Matrix rotateBack;
		rotateBack.rotationZ(toRadian(180));
		totalTransform = totalTransform * rotateBack;
	}
}

void Player::trySafeRotateZTowardsWall(const Vector& wallNormal, Matrix& totalTransform, const std::list<BaseModel*>& models, float maxAngleDeg)
{
	Vector currentDir = playerDirection.normalize();
	Vector cross = currentDir.cross(wallNormal);
	float direction = (cross.Z >= 0) ? 1.0f : -1.0f;

	float stepSize = 1.0f;
	float currentAngle = maxAngleDeg * direction;

	while (std::abs(currentAngle) > 0.0f) {
		Matrix testRotation;
		testRotation.rotationZ(toRadian(currentAngle));
		Matrix testTransform = totalTransform * testRotation;

		if (!rotationWouldCauseCollision(models, testTransform)) {
			totalTransform = testTransform;
			std::cout << "Rotation erfolgreich mit " << currentAngle << " Grad." << std::endl;
			return;
		}

		currentAngle -= stepSize * direction;
		if (std::abs(currentAngle) < stepSize)
			break;
	}

	std::cout << "Keine sichere Rotation möglich." << std::endl;
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

Vector Player::getPosition() const {
	const Matrix& transform = player->transform();
	return Vector(transform.m[12], transform.m[13], transform.m[14]);
}

bool Player::checkWallCollision(const std::list<BaseModel*>& models, const Vector& movement, Vector& pushback, Vector* wallNormalOut)
{
	AABB futureBox = BoundingBox;
	futureBox.Min += movement;
	futureBox.Max += movement;

	for (BaseModel* model : models) {
		if (model != this && model->isWall) {
			const AABB& wallBox = model->getBoundingBox();

			bool collision = (futureBox.Min.X <= wallBox.Max.X && futureBox.Max.X >= wallBox.Min.X) &&
				(futureBox.Min.Y <= wallBox.Max.Y && futureBox.Max.Y >= wallBox.Min.Y) &&
				(futureBox.Min.Z <= wallBox.Max.Z && futureBox.Max.Z >= wallBox.Min.Z);

			if (collision) {
				std::cout << "Wand Kollision!" << std::endl;

				pushback = Vector(0, 0, 0);
				float pushStrength = 2.0f;

				if (wallNormalOut != nullptr)
					*wallNormalOut = model->getSurfaceNormal();


				/*	if (movement.X != 0)
						pushback.X = (movement.X > 0) ? -pushStrength : pushStrength;
					if (movement.Y != 0)
						pushback.Y = (movement.Y > 0) ? -pushStrength : pushStrength;
					if (movement.Z != 0)
						pushback.Z = (movement.Z > 0) ? -pushStrength : pushStrength;*/

				return true;
			}
		}
	}

	return false;
}

bool Player::rotationWouldCauseCollision(const std::list<BaseModel*>& models, const Matrix& testTransform)
{
	// Simuliere die BoundingBox mit testTransform
	AABB localBox = player->boundingBox();

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

	// Transformiere die Ecken
	for (int i = 0; i < 8; ++i)
		corners[i] = testTransform * corners[i];

	// Neue AABB berechnen
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

	// Prüfe Kollision mit Walls
	for (BaseModel* model : models) {
		if (model != this && model->isWall) {
			const AABB& wallBox = model->getBoundingBox();

			bool collision = (newMin.X <= wallBox.Max.X && newMax.X >= wallBox.Min.X) &&
				(newMin.Y <= wallBox.Max.Y && newMax.Y >= wallBox.Min.Y) &&
				(newMin.Z <= wallBox.Max.Z && newMax.Z >= wallBox.Min.Z);

			if (collision) {
				std::cout << "Rotation würde Kollision verursachen!" << std::endl;
				return true;
			}
		}
	}

	return false;
}

void Player::drawDirectionVector(const BaseCamera& Cam) {
	ConstantShader shader;
	shader.color(Color(1, 1, 0));  // Gelb

	Vector pos = getPosition();
	Vector end = pos + playerDirection * 2.0f;

	LineModel dirLine(pos, end);
	dirLine.shader(&shader);
	dirLine.draw(Cam);
}

void Player::drawOrientation(const BaseCamera& Cam) {
	// Hole die Transformationsmatrix
	const Matrix& transform = player->transform();

	// Hole den Ursprung
	Vector pos(transform.m[12], transform.m[13], transform.m[14]);

	// Hole die Richtungsvektoren (und normiere sie zur Sicherheit)
	Vector forward = transform.forward().normalize();
	Vector up = transform.up().normalize();
	Vector right = transform.right().normalize();

	// Faktor für die Länge der gezeichneten Linien
	float axisLength = 2.0f;

	// Linien-Endpunkte
	Vector forwardEnd = pos + forward * axisLength;
	Vector upEnd = pos + up * axisLength;
	Vector rightEnd = pos + right * axisLength;

	// Shader für die Linien
	ConstantShader shader;

	// Forward (rot)
	shader.color(Color(1, 0, 0));
	LineModel forwardLine(pos, forwardEnd);
	forwardLine.shader(&shader);
	forwardLine.draw(Cam);

	// Up (grün)
	shader.color(Color(0, 1, 0));
	LineModel upLine(pos, upEnd);
	upLine.shader(&shader);
	upLine.draw(Cam);

	// Right (blau)
	shader.color(Color(0, 0, 1));
	LineModel rightLine(pos, rightEnd);
	rightLine.shader(&shader);
	rightLine.draw(Cam);

}
