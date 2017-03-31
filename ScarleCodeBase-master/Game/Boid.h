#pragma once
#include "VBGO.h"
#include "vertex.h"


class Boid : public VBGO
{
public:
	Boid(ID3D11Device* _pd3dDevice);
	~Boid();

	void Spawn(Vector3 _scale, GameData* _GD, bool _isPredator);

	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;

	//SETTERS
	void SetAlive(bool isAlive);
	void setVelocity(Vector3 velocity);
	void setRotation();
	void setColour(float r, float g, float b);
	void set2D(bool _is2D) { is2D = _is2D; };
	void setArmyToggle(bool _isArmySimOn);

	//GETTERS
	Vector3 getVelocity() { return m_vel; }
	
	bool isAlive() { return m_alive; }
	bool isPredator() { return m_predator; }



private:
	Vector3 m_target; // I am travelling towards this location
	Vector3 m_dir;
	Vector3 m_up;
	Vector3 m_acc;
	Vector3 travelDirection;
	Vector3 initialDirection;
	Vector3 initialLocation;


	bool inBoundingBox = true;
	bool m_alive;
	bool is2D = false;
	bool m_predator = false;
	bool isArmySimOn = false;

	int max = 100;
	int min = 50;

	float boxSize = 100;
	float accelerationLimit = 10;
	float speed = 1;
	float colour = 0.0f;
	int startMax = 50;
	int startMin = -50;

	myVertex* m_vertices;

};