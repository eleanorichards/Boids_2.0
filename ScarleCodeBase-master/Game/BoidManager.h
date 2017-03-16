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

	void getUserInput(GameData * _GD);
	void moveBoid(Boid* _boid, GameData * _GD);

	Vector3 separation(Boid* _boid, float distance);
	Vector3 alignment(Boid* _boid, float distance);
	Vector3 cohesion(Boid* _boid, float distance);
	Vector3 seek(Vector3 _target, Vector3 _pos, Vector3 _vel);

	//GETTERS
	int getNumOfBoids() { return boidsInScene; }

	void set2D(bool _is2D);


private:
	list<Boid*> m_Boids;
	Boid* _Boid;

	Vector3 initialLocation;
	Vector3 travelDirection;
	Vector3 randomDirection;

	//alignment and cohesion radius should be similar/the same
	float alignmentRadius = 30;
	float cohesionRadius = 40;
	float separationRadius = 10;

	float alignmentModifier = 1;
	float separationModifier = 1;
	float cohesionModifier = 1;
	
	float maxSpeed = 15.0f;
	float maxForce = 0.1f;

	float boidsInScene = 0;
	float desiredBoids = 0;

	int max = 10;
	int min = -10;
	int startMax = 50;
	int startMin = -50;

};