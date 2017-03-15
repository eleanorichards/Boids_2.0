#pragma once
#include "gameobject.h"

#include <string>
#include "Model.h"
#include "Boid.h"
#include <list>
#include <vector>

using namespace std;
using namespace DirectX;	

class BoidManager : public GameObject
{
public:
	BoidManager(int _numOfBoids, ID3D11Device* _pd3dDevice);
	~BoidManager();
	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;
	//virtual void DrawScreenSpace(DrawData2D* _DD2D);

	void getUserInput(GameData * _GD);
	void moveBoid(Boid* _boid, GameData * _GD);

	Vector3 separation(Boid* _boid);
	Vector3 alignment(Boid* _boid);
	Vector3 cohesion(Boid* _boid);

	Vector3 seek(Vector3 _target, Vector3 _pos, Vector3 _vel);

	//GETTERS
	int getNumOfBoids() { return boidsInScene; }

private:
	list<Boid*> m_Boids;
	Boid* _Boid;

	Vector3 initialLocation;
	Vector3 travelDirection;
	Vector3 randomDirection;

	//alignment and cohesion radius should be similar/the same
	float alignmentRadius = 25;
	float cohesionRadius = 25;
	float separationRadius = 10;

	float alignmentModifier = 1.5;
	float separationModifier = 1;
	float cohesionModifier = 1;
	
	float maxSpeed = 15.0f;
	float maxForce = 0.5f;

	float boidsInScene = 0;
	float desiredBoids = 0;

	int max = 10;
	int min = -10;
	int startMax = 100;
	int startMin = -100;

	bool placeBoid = false;

};