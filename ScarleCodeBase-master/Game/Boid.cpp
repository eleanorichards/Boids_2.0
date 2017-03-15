#include "Boid.h"
#include "GameData.h"
#include <iostream>

Boid::Boid(ID3D11Device * _pd3dDevice) 
{
	//initialise as not alive
	m_alive = false;

	Matrix rotMat;
	rotMat = Matrix::Identity;

	m_fudge = Matrix::CreateRotationY(XM_PIDIV2);



	//All the Draw stuff
	int vert = 0;
	int numVerts = 3;
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
	m_vertices[vert++].Pos = Vector3(0, 0, 0);
	m_vertices[vert].Color = Color(1.0f, 0.0f, 1.0f, 1.0f);
	m_vertices[vert++].Pos = Vector3(0, 0, 4);
	m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
	m_vertices[vert++].Pos = Vector3(5, 0, 2);

	for (int i = 0; i < m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}


	BuildIB(_pd3dDevice, indices);
	BuildVB(_pd3dDevice, numVerts, m_vertices);

	delete[] indices;    //this is no longer needed as this is now in the index Buffer
	delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
	m_vertices = nullptr;
	
	//set m_up
	m_up = Vector3::Transform(Vector3::Up, m_fudge.Invert() * m_worldMat) - m_pos;
}

Boid::~Boid()
{
}

void Boid::Spawn(Vector3 _pos, Vector3 _scale, GameData* _GD)
{
	initialDirection = Vector3(((float)(rand() % max) - min), ((float)(rand() % max) - min), (((float)(rand() % max) - min)))*0.1;
	
	m_alive = true; // turn this enemy ON ;)
	m_pos = _pos;
	m_scale = _scale;
	m_vel = initialDirection;
}

void Boid::Tick(GameData * _GD)
{
	if (m_alive)
	{
		if (m_pos.x >= 220 || m_pos.x <= -220 || m_pos.y >= 220 || m_pos.y <= -220 || m_pos.z >= 220 || m_pos.z <= -220)
		{
			//move to opposite end of box
			m_pos *= (-0.9);
		}
		else 
		{
			setAcceleration((m_vel) * _GD->m_dt);
			m_pos += m_acc;
		}
		setRotation();
	}
}

void Boid::Draw(DrawData * _DD)
{
	if (m_alive)
	{
		VBGO::Draw(_DD);
	}
}

void Boid::SetAlive(bool isAlive)
{
	m_alive = isAlive;
}

//limit and then set velocity
void Boid::setVelocity(Vector3 velocity)
{
	m_vel = velocity;
}

void Boid::setPosition(Vector3 position)
{
	m_pos = position;
}

void Boid::setRotation()
{
	Matrix scaleMat = Matrix::CreateScale(m_scale);
	Matrix rotTransMat = Matrix::CreateWorld(m_pos, m_vel, m_up);
	Matrix transMat = Matrix::CreateTranslation(m_pos);
	m_worldMat = m_fudge * scaleMat * rotTransMat * transMat;

}

void Boid::setAcceleration(Vector3 _acceleration)
{
	//cap acceleration in any direction to the set MAX
	if (_acceleration.x > maxAcceleration)
	{
		_acceleration.x = maxAcceleration;
	}
	if (_acceleration.y > maxAcceleration)
	{
		_acceleration.y = maxAcceleration;
	}
	if (_acceleration.z > maxAcceleration)
	{
		_acceleration.z = maxAcceleration;
	}
	if (_acceleration.x < minAcceleration)
	{
		_acceleration.x = minAcceleration;
	}
	if (_acceleration.y < minAcceleration)
	{
		_acceleration.y = minAcceleration;
	}
	if (_acceleration.z < minAcceleration)
	{
		_acceleration.z = minAcceleration;
	}

	m_acc = _acceleration;
}

void Boid::setColour(float r, float g, float b)
{
	m_vertices[1].Color = Color(r, g, b, 1.0f);
}


