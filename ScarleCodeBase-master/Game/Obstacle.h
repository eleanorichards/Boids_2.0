#pragma once
#include "VBGO.h"
#include "vertex.h"

class Obstacle : public VBGO
{
public:
	Obstacle(ID3D11Device* _pd3dDevice);
	~Obstacle();

	void Spawn(Vector3 _scale, GameData* _GD, Vector3 _location);

	virtual void Draw(DrawData* _DD) override;
	virtual void Tick(GameData* _GD) override;

	bool isAlive() { return m_alive; }

private:
	myVertex* m_vertices;
	bool m_alive = false;
	Vector3 initialLocation;

	int startMax = 50;
	int startMin = -50;

};

