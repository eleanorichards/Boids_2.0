#pragma once
#include "VBGO.h"
#include "vertex.h"

class Predator : public VBGO
{
public:
	Boid(ID3D11Device* _pd3dDevice);
	~Boid();

	void Spawn(Vector3 _pos, Vector3 _scale, GameData* _GD);

	virtual void Tick(GameData* _GD) override;
	virtual void Draw(DrawData* _DD) override;

	//SETTERS
	void SetAlive(bool isAlive);
	void setVelocity(Vector3 velocity);
	void setPosition(Vector3 position);
	void setRotation();
	void setAcceleration(Vector3 _acceleration);
	void setSpeed(float _speed) { speed = _speed; };
	void setColour(float r, float g, float b);
	void set2D(bool _is2D) { is2D = _is2D; };

	//GETTERS
	Vector3 getVelocity() { return m_vel; }
	Vector3 getAccleration() { return m_acc; }

	bool isAlive() { return m_alive; }


private:
	Vector3 m_target; // I am travelling towards this location
	Vector3 m_dir;
	Vector3 m_up;
	Vector3 m_acc;
	Vector3 travelDirection;
	Vector3 initialDirection;

	bool inBoundingBox = true;
	bool m_alive;
	bool is2D = false;
	int max = 100;
	int min = 50;

	float boxSize = 150;
	float accelerationLimit = 10;
	float speed = 1;
	float speedModifier = 0.0f;


	myVertex* m_vertices;

};