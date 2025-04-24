//
//  TriangleBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "TriangleBoxModel.h"

TriangleBoxModel::TriangleBoxModel(float Width, float Height, float Depth)
{

	float x = Width / 2;
	float y = Height / 2;
	float z = Depth / 2;


	BoundingBox.Min = Vector(-x, -y, -z);
	BoundingBox.Max = Vector(x, y, z);
	/*  
	  f---g		//abcd n(0,0,1) //bfgc n(0,1,0)	
	 /|  /|		
	b---c |		//aehd n(0,-1,0) //efgh n(0,0,-1)
	| e-|-h
	|/  |/		//dcgh n(1,0,0) //aefb (-1,0,0)		
	a---d

	  5---6		
	 /|  /|
	1---2 |		
	| 4-|-7			
	|/  |/		
	0---3

	1---2                2         1---2
	|   |  wird			/|   und   |  /
	|   |             /  |         |/
	0---3            0---3         0
*/

	Vector a(-x,-y,z); 
	Vector b(-x, y, z);
	Vector c(x, y, z);
	Vector d(x, -y, z);

	Vector e(-x, -y, -z);
	Vector f(-x, y, -z);
	Vector g(x, y, -z);
	Vector h(x, -y, -z);
	
	
	VB.begin();


	makePlane(a, b, c, d, Vector(0, 0, 1));

	makePlane(h, g, f, e, Vector(0, 0, -1));

	makePlane(b, f, g, c, Vector(0, 1, 0));

	makePlane(e, a, d, h, Vector(0, -1, 0));

	makePlane(d, c, g, h, Vector(0, 0, 1));

	makePlane(e, f, b, a, Vector(-1, 0, 0));

	VB.end();

	IB.begin();
	for (int i = 0; i < 6;i++) {
			
			//counter clockwise.
			//triangle1
			int index = i * 4;
			IB.addIndex(index);
			IB.addIndex(index+3);
			IB.addIndex(index+2);
			//triangle2
			IB.addIndex(index);
			IB.addIndex(index+2);
			IB.addIndex(index+1);
	}

	IB.end();
	
	
}


void TriangleBoxModel::makePlane(const Vector& v0, const Vector& v1, const Vector& v2, const Vector& v3, const Vector& n) {

	VB.addNormal(n);
	VB.addTexcoord0(0,1);
	VB.addVertex(v0);
	VB.addTexcoord0(0, 0);
	VB.addVertex(v1);
	VB.addTexcoord0(1, 0);
	VB.addVertex(v2);
	VB.addTexcoord0(1, 1);
	VB.addVertex(v3);
}

void TriangleBoxModel::draw(const BaseCamera& Cam)
{
    BaseModel::draw(Cam);
	

	VB.activate();
	IB.activate();

	glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);

	IB.deactivate();
	VB.deactivate();
	
}

void TriangleBoxModel::boundingBox()  {
	//// Ursprüngliche Eckpunkte (lokal)
	//Vector corners[8] = {
	//	Vector(-0.5f, -0.5f,  0.5f),  // a
	//	Vector(-0.5f,  0.5f,  0.5f),  // b
	//	Vector(0.5f,  0.5f,  0.5f),  // c
	//	Vector(0.5f, -0.5f,  0.5f),  // d
	//	Vector(-0.5f, -0.5f, -0.5f),  // e
	//	Vector(-0.5f,  0.5f, -0.5f),  // f
	//	Vector(0.5f,  0.5f, -0.5f),  // g
	//	Vector(0.5f, -0.5f, -0.5f)   // h
	//};

	//// Transformationsmatrix
	//const Matrix& m = this->transform();

	//// Alle Eckpunkte transformieren
	//
	//for (int i = 0; i < 8; i++) {
	//	corners[i] = m * corners[i];
	//}

	//// Min/Max finden
	//Vector newMin = corners[0];
	//Vector newMax = corners[0];
	//for (int i = 1; i < 8; i++) {
	//	newMin.X = std::min(newMin.X, corners[i].X);
	//	newMin.Y = std::min(newMin.Y, corners[i].Y);
	//	newMin.Z = std::min(newMin.Z, corners[i].Z);

	//	newMax.X = std::max(newMax.X, corners[i].X);
	//	newMax.Y = std::max(newMax.Y, corners[i].Y);
	//	newMax.Z = std::max(newMax.Z, corners[i].Z);
	//}

	//
	//BoundingBox.Min = Vector(newMin.X, newMin.Y, newMin.Z);
	//BoundingBox.Max = Vector(newMax.X, newMax.Y, newMax.Z);
	//std::cout << "BoundingBox Min: " << newMin.X << ", " << newMin.Y << ", " << newMin.Z << std::endl;
	//std::cout << "BoundingBox Max: " << newMax.X << ", " << newMax.Y << ", " << newMax.Z << std::endl;
	//std::cout << "---- " << std::endl;
}

void TriangleBoxModel::calculateBoundingBox() 
{
//	BoundingBox = boundingBox();
	
	Vector corners[8] = {
		Vector(-0.5f, -0.5f,  0.5f),  // a
		Vector(-0.5f,  0.5f,  0.5f),  // b
		Vector(0.5f,  0.5f,  0.5f),  // c
		Vector(0.5f, -0.5f,  0.5f),  // d
		Vector(-0.5f, -0.5f, -0.5f),  // e
		Vector(-0.5f,  0.5f, -0.5f),  // f
		Vector(0.5f,  0.5f, -0.5f),  // g
		Vector(0.5f, -0.5f, -0.5f)   // h
	};

	// Transformationsmatrix
	const Matrix& m = this->transform();

	// Alle Eckpunkte transformieren

	for (int i = 0; i < 8; i++) {
		corners[i] = m * corners[i];
	}

	// Min/Max finden
	Vector newMin = corners[0];
	Vector newMax = corners[0];
	for (int i = 1; i < 8; i++) {
		newMin.X = std::min(newMin.X, corners[i].X);
		newMin.Y = std::min(newMin.Y, corners[i].Y);
		newMin.Z = std::min(newMin.Z, corners[i].Z);

		newMax.X = std::max(newMax.X, corners[i].X);
		newMax.Y = std::max(newMax.Y, corners[i].Y);
		newMax.Z = std::max(newMax.Z, corners[i].Z);
	}


	BoundingBox.Min = Vector(newMin.X, newMin.Y, newMin.Z);
	BoundingBox.Max = Vector(newMax.X, newMax.Y, newMax.Z);
	/*std::cout << "BoundingBox Min: " << newMin.X << ", " << newMin.Y << ", " << newMin.Z << std::endl;
	std::cout << "BoundingBox Max: " << newMax.X << ", " << newMax.Y << ", " << newMax.Z << std::endl;
	std::cout << "---- " << std::endl;*/
}

AABB& TriangleBoxModel::getBoundingBox()
{
	return BoundingBox;
}




