#pragma once
#include "VBGO.h"
#include "vertex.h"

class Obstacle : public VBGO
{
public:
	Obstacle(ID3D11Device* _pd3dDevice);
	~Obstacle();

private:
	myVertex* m_vertices;

};

