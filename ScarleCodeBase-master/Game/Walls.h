#pragma once
#include "VBGO.h"
#include "vertex.h"

class Walls : public VBGO
{
public:
	Walls() {};
	virtual ~Walls() {};

	//virtual void Draw(DrawData* _DD) override;
	void init(int _size, ID3D11Device* _GD);

private:
	virtual void Transform() {};

	int m_size;
	myVertex* m_vertices;

};