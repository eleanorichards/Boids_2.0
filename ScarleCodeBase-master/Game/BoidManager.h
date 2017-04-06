#pragma once
#include "gameobject.h"

#include <string>
#include "Model.h"
#include "Boid.h"
#include <list>
#include <vector>
#include "Obstacle.h"

using namespace std;
using namespace DirectX;	

class BoidManager : public GameObject
{
public:
	BoidManager(int _numOfBoids, ID3D11Device* _pd3dDevice);
	~BoidManager();

	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;

	void moveBoid(Boid* _boid, GameData * _GD);

	Vector3 separation(Boid* _boid);
	Vector3 alignment(Boid* _boid);
	Vector3 cohesion(Boid* _boid);
	Vector3 seek(Vector3 _target, Vector3 _pos, Vector3 _vel);

	Vector3 escape(Boid* _boid);

	Vector3 wiggle(Boid* _boid, GameData * _GD);

	Vector3 movePlayer(GameData * _GD, Boid* _boid);

	//GETTERS
	int getNumOfBoids() { return boidsInScene; };
	bool get2D() { return is2D; };
	bool getArmyToggle() { return armySim; };

	//SETTERS
	void set2D(bool _is2D);
	void setArmyToggle(bool _isArmySimOn);

private:
	list<Boid*> m_Boids;
	Boid* _Boid;

	list<Obstacle*> m_Obstacles;

	Vector3 initialLocation;
	Vector3 travelDirection;
	Vector3 randomDirection;

	Vector3 obstacleSpawn;
	//Vector3 wiggly = Vector3::Zero;
	//alignment and cohesion radius should be similar/the same
	float alignmentRadius = 20;
	float cohesionRadius = 20;
	float separationRadius = 10;
	float escapeRadius = 20;

	float alignmentModifier = 1;
	float separationModifier = 1.1f;
	float cohesionModifier = 1;
	float escapeModifier = 2.0f;
	float predatorCohesionModifier = 1.5f;

	float maxSpeed = 10.0f;
	float maxForce = 0.1f;

	float frequency = 0.1;
	float amplitude = 0.1;

	float boidsInScene = 0;
	float predatorsInScene = 0;
	float herdersInScene = 0;
	float desiredBoids = 0;
	float boidContrast = 0.0f;
	float speedModifier = 0.0f;
	float numOfPredators = 0.0f;
	float numOfHerders = 0.0f;

	float obstaclesInScene = 0;
	float obstaclesDesired = 0;

	int max = 10;
	int min = -10;
	int startMax = 50;
	int startMin = -50;
	
	bool is2D = false;
	bool armySim = false;

};