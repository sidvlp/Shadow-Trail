#include "Player.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "../../src/ConstantShader.h"
#include "../../src/TriangleBoxModel.h"
#include "../CGVStudio/LineModel.h"
#include <list>
#include <algorithm>
#include "MenuManager.h"


Player::Player() : fb(0, 0, 0)
{
	debugPlayerBoundingBox = new LineBoxModel(1, 1, 1);
	debugPlayerFootBoundingBox = new LineBoxModel(1, 1, 1);
	isFalling = false;
	isGrounded = true;
	BoundingBox.Min = Vector(-0.5f, -0.5f, -0.5f);
	BoundingBox.Max = Vector(0.5f, 0.5f, 0.5f);
	constantBoundingBoxShader = new ConstantShader();
	constantBoundingBoxShader->color(Color(1, 1, 1));
	debugPlayerBoundingBox->shader(constantBoundingBoxShader, false);
	constantFootBoundingBoxShader = new ConstantShader();
	constantFootBoundingBoxShader->color(Color(0, 1, 0));
	debugPlayerFootBoundingBox->shader(constantFootBoundingBoxShader, false);

}

Player::~Player()
{
	delete debugPlayerBoundingBox;
	delete debugPlayerFootBoundingBox;
	delete constantBoundingBoxShader;
	delete constantFootBoundingBoxShader;
}

bool Player::loadModels(const char* player_model, float scaleFactor)
{
	if (player_model != nullptr) {
		player = new Model(player_model);
		modelScale = scaleFactor;
		player->shader(pShader);

		Matrix playerRotX, playerRotY, playerMove, playerUrsprung;
		playerRotX.rotationX(toRadian(-90));
		playerRotY.rotationY(toRadian(180));
		playerMove.translation(0, 0.5, 7.5);
		playerMove.translation(startPosition);
		playerUrsprung.scale(scaleFactor);
		player->transform(playerMove * playerUrsprung * playerRotY * playerRotX);
		startTransform = player->transform();
		updateBoundingBox();
		return true;
	}
	return false;
}

float Player::toRadian(float degrees) {

	return (float)degrees * M_PI / 180;

}

void Player::resetPosition() {

	player->transform(startTransform);
	updateBoundingBox();

}

/**
* Debug-Methode zum zeichnen der PlayerBoundingBox und der FootBoundingBox
*/
void Player::drawBoundingBox(const BaseCamera& Cam) {
	if (!debugPlayerBoundingBox || !debugPlayerFootBoundingBox) return;


	debugPlayerBoundingBox->draw(Cam);


	debugPlayerFootBoundingBox->draw(Cam);
}

/**
 *
 * Diese Methode berechnet die transformierte Bounding Box des Spielermodells,
 * indem alle acht Eckpunkte der lokalen Bounding Box mit der aktuellen
 * Transformationsmatrix multipliziert werden. Anschließend werden die
 * minimalen und maximalen Koordinaten ermittelt, um die neue globale
 * Bounding Box festzulegen.
 * 
 */
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

	updateFootBoundingBox(localBox, transform);


	Vector bSize = BoundingBox.Max - BoundingBox.Min;
	Vector bCenter = (BoundingBox.Min + BoundingBox.Max) * 0.5f;

	Matrix scaleMat, transMat;
	scaleMat.scale(bSize.X, bSize.Y, bSize.Z);
	transMat.translation(bCenter.X, bCenter.Y, bCenter.Z);

	debugPlayerBoundingBox->transform(transMat * scaleMat);

	BoundingBox.Min = Vector(newMin.X, newMin.Y, newMin.Z);
	BoundingBox.Max = Vector(newMax.X, newMax.Y, newMax.Z);
	/*std::cout << "BoundingBox Min: " << newMin.X << ", " << newMin.Y << ", " << newMin.Z << std::endl;
	std::cout << "BoundingBox Max: " << newMax.X << ", " << newMax.Y << ", " << newMax.Z << std::endl;*/
	//std::cout << "---- " << std::endl;
}

void Player::updateFootBoundingBox(const AABB& localBox, const Matrix& transform) {
	const float footHeightRatio = 0.2f;
	const float footWidthRatio = 0.4f;  

	float totalWidth = localBox.Max.X - localBox.Min.X;
	float totalDepth = localBox.Max.Y - localBox.Min.Y;
	float totalHeight = localBox.Max.Z - localBox.Min.Z;

	float footHeight = totalHeight * footHeightRatio;
	float footWidth = totalWidth * footWidthRatio;
	float footDepth = totalDepth * footWidthRatio;

	Vector center = (localBox.Min + localBox.Max) * 0.5f;

	AABB localFootBox;
	localFootBox.Min = Vector(center.X - footWidth / 2.0f,	center.Y - footDepth / 2.0f, localBox.Min.Z);
	localFootBox.Max = Vector(center.X + footWidth / 2.0f, center.Y + footDepth / 2.0f, localBox.Min.Z + footHeight);

	Vector footBoxCorners[8] = {
		Vector(localFootBox.Min.X, localFootBox.Min.Y, localFootBox.Min.Z),
		Vector(localFootBox.Min.X, localFootBox.Min.Y, localFootBox.Max.Z),
		Vector(localFootBox.Min.X, localFootBox.Max.Y, localFootBox.Min.Z),
		Vector(localFootBox.Min.X, localFootBox.Max.Y, localFootBox.Max.Z),
		Vector(localFootBox.Max.X, localFootBox.Min.Y, localFootBox.Min.Z),
		Vector(localFootBox.Max.X, localFootBox.Min.Y, localFootBox.Max.Z),
		Vector(localFootBox.Max.X, localFootBox.Max.Y, localFootBox.Min.Z),
		Vector(localFootBox.Max.X, localFootBox.Max.Y, localFootBox.Max.Z)
	};

	for (int i = 0; i < 8; ++i) {
		footBoxCorners[i] = transform * footBoxCorners[i];
	}

	footBoundingBox.Min = footBoundingBox.Max = footBoxCorners[0];
	for (int i = 1; i < 8; ++i) {
		footBoundingBox.Min.X = std::min(footBoundingBox.Min.X, footBoxCorners[i].X);
		footBoundingBox.Min.Y = std::min(footBoundingBox.Min.Y, footBoxCorners[i].Y);
		footBoundingBox.Min.Z = std::min(footBoundingBox.Min.Z, footBoxCorners[i].Z);

		footBoundingBox.Max.X = std::max(footBoundingBox.Max.X, footBoxCorners[i].X);
		footBoundingBox.Max.Y = std::max(footBoundingBox.Max.Y, footBoxCorners[i].Y);
		footBoundingBox.Max.Z = std::max(footBoundingBox.Max.Z, footBoxCorners[i].Z);
	}

	Vector footSize = footBoundingBox.Max - footBoundingBox.Min;
	Vector footCenter = (footBoundingBox.Min + footBoundingBox.Max) * 0.5f;

	Matrix footScaleMat, footTransMat;
	footScaleMat.scale(footSize.X, footSize.Y, footSize.Z);
	footTransMat.translation(footCenter.X, footCenter.Y, footCenter.Z);

	debugPlayerFootBoundingBox->transform(footTransMat * footScaleMat);
}



void Player::setStartPosition(const Vector& pos) {
	startPosition = pos;
}

void Player::steer(float ForwardBackward, float LeftRight)
{
	fb.X = ForwardBackward;
	fb.Y = LeftRight;
	
}

void Player::update(float dtime, Camera& cam, std::list<BaseModel*>& models)
{
	Matrix playerMove, playerRot, currentTransform;
	currentTransform = this->player->transform();

	if (isGrounded) {
		playerRot.rotationZ(fb.Y * dtime * 5); //Y und Z vertauscht

		const float BASE_SPEED = 1.5f;
		float speed = BASE_SPEED / modelScale;   
		Vector intendedMove(0, -fb.X * dtime * speed, 0); //Y und Z vertauscht

		Matrix totalTransform = currentTransform;


			Vector wallNormal;
			bool collision = checkWallCollision(models, &wallNormal);

			if (collision) {
				float dot = playerDirection.normalize().dot(wallNormal.normalize());

				/*std::cout << "Wand Kollision!" << std::endl;
				std::cout << "PlayerDirection: " << playerDirection.X << std::endl;
				std::cout << "WallNormal:      " << wallNormal << std::endl;
				std::cout << "Dot:             " << dot << std::endl;*/

				resolveCollision(dot, wallNormal, totalTransform, models);

				//player->transform(totalTransform);
				//updateBoundingBox();
				//playerDirection = -player->transform().up().normalize(); //Negativer Up-Vektor ist der Forward-Vektor

			}

			else {
				totalTransform = totalTransform * Matrix().translation(intendedMove);
			}

		
		Matrix testTRotationTransform = totalTransform * playerRot;

		if (!rotationWouldCauseCollision(models, testTRotationTransform)) {
			totalTransform = testTRotationTransform;
		}
		else {
		//	std::cout << "Rotation blockiert wegen möglicher Kollision." << std::endl;
		}

		player->transform(totalTransform);
		playerDirection = -totalTransform.up().normalize(); //Negativer Up-Vektor ist der Forward-Vektor


	}

	updateBoundingBox();
	updateBoundingBox();

	if (isFalling) {
		//std::cout << "Falling" << std::endl;

		Matrix trans = player->transform();
		Matrix playerFall;
		playerFall.translation(0, 0, -500 * dtime); //Y und Z sind vertauscht
		player->transform(trans * playerFall);
		updateBoundingBox();

		if (BoundingBox.Min.Y < -5.0f) {
			isFalling = false;
			isGrounded = true;

			if (MenuManager::instance().state == MenuState::SinglePlayer) {
				lives--;

				if (lives > 0) {
					resetPosition();

				}
				else {
					lives = 3;
					MenuManager::instance().state = MenuState::GameOver;
				}
			}
			else {
				resetPosition();
			}
		
		}
	}

	cam.setPosition(player->transform().translation() + Vector(0, 2, 3));
	cam.setTarget(player->transform().translation());
}

void Player::draw(const BaseCamera& Cam)
{
	player->draw(Cam);
//	drawBoundingBox(Cam);
	//drawOrientation(Cam); // Hier!

	//drawDirectionVector(Cam);

}

void Player::resolveCollision(float dot, const Vector& wallNormal, Matrix& totalTransform, const std::list<BaseModel*>& models)
{
	//std::cout << "[resolveCollision] Dot: " << dot << std::endl;

	// Fall 1: Frontal gegen Wand
	if (dot < -0.9f) {
		//std::cout << "Frontal gegen Wand → versuche Wegdrehung" << std::endl;
		trySafeRotate(wallNormal, totalTransform, models, 180.0f);
	}


	// Fall 2: Schräge Kollision → leicht wegdrehen
	else if (dot < -0.3f) {
		//std::cout << "Schräge Kollision → leichte Drehung" << std::endl;
		trySafeRotate(wallNormal, totalTransform, models, 30.0f);

	}

	// Fall 3: Seitlich → entlang Wand gleiten
	else if (std::abs(dot) < 0.9f) {
		//std::cout << "Schräge Kollision → versuche adaptive Rotation" << std::endl;
		trySafeRotate(wallNormal, totalTransform, models, 15.0f);

	}


	// Fall 4: Von hinten gegen Wand
	else if (dot > 0.9f) {
		std::cout << "Rückwärts gegen Wand → Umdrehen" << std::endl;

		Matrix rotateBack;
		rotateBack.rotationZ(toRadian(180));
		totalTransform = totalTransform * rotateBack;
	}
}

void Player::trySafeRotate(const Vector& wallNormal, Matrix& totalTransform, const std::list<BaseModel*>& models, float maxAngleDeg)
{
	Vector currentDir = playerDirection.normalize();
	Vector cross = currentDir.cross(wallNormal);
	float direction = (cross.Z >= 0) ? 1.0f : -1.0f; //Z und Y sind vertauscht -> Drehung um Y-Achse

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

			bool groundCollision =
				(footBoundingBox.Min.X <= modelBox.Max.X && footBoundingBox.Max.X >= modelBox.Min.X) &&
				(footBoundingBox.Min.Y <= modelBox.Max.Y && footBoundingBox.Max.Y >= modelBox.Min.Y) &&
				(footBoundingBox.Min.Z <= modelBox.Max.Z && footBoundingBox.Max.Z >= modelBox.Min.Z);

			if (groundCollision) {
				isGrounded = true;
				isFalling = false;
				return true;
			}
		}
	}

	isGrounded = false;
	isFalling = true;
	//std::cout << "No ground collision" << std::endl;
	return false;
}


Vector Player::getPosition() const {
	const Matrix& transform = player->transform();
	return Vector(transform.translation());
}


/**
 *
 * Diese Methode durchläuft alle übergebenen Modelle und überprüft,
 * ob die Axis-Aligned Bounding Box (AABB) des Spielers mit der
 * Bounding Box eines Wandmodells überschneidet. Falls eine
 * Kollision erkannt wird, wird die Normalenrichtung der
 * Wandoberfläche zurückgegeben.
 *
 * @param models      Liste aller Modelle in der Szene, die auf Kollision
 *                    überprüft werden sollen.
 * @param wallNormalOut Zeiger auf einen Vektor, in den die Oberflächennormale
 *                      der kollidierten Wand geschrieben wird.
 *
 * @return true, wenn eine Kollision mit einer Wand vorliegt,
 *         andernfalls false.
 *
 */
bool Player::checkWallCollision(const std::list<BaseModel*>& models, Vector* wallNormalOut)
{

	for (BaseModel* model : models) {
		if (model != this && model->isWall) {
			const AABB& wallBox = model->getBoundingBox();

			bool collision = (BoundingBox.Min.X <= wallBox.Max.X && BoundingBox.Max.X >= wallBox.Min.X) &&
				(BoundingBox.Min.Y <= wallBox.Max.Y && BoundingBox.Max.Y >= wallBox.Min.Y) &&
				(BoundingBox.Min.Z <= wallBox.Max.Z && BoundingBox.Max.Z >= wallBox.Min.Z);

			if (collision) {
				//std::cout << "Wand Kollision!" << std::endl;

					*wallNormalOut = model->getSurfaceNormal();

				return true;
			}
		}
	}

	return false;
}

bool Player::rotationWouldCauseCollision(const std::list<BaseModel*>& models, const Matrix& testTransform)
{
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

	for (int i = 0; i < 8; ++i)
		corners[i] = testTransform * corners[i];

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
