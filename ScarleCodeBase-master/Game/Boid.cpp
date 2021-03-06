#include "Boid.h"
#include "GameData.h"

#include "AntTweakBar.h"


Boid::Boid(ID3D11Device * _pd3dDevice) 
{
	//initialise as not alive
	m_alive = false;

	Matrix rotMat;
	rotMat = Matrix::Identity;

	m_fudge = Matrix::CreateRotationY(XM_PIDIV2);

	//All the Draw stuff
	int vert = 0;
	int numVerts = 6;
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	if (!m_predator)
	{
		//Vert bits
		//X tri
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0, -2, 0);
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(5, 0, 2); //this is the point
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0, 2, 4);

		//Y tri
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0, 2, 0);
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(5, 0, 2); // this is the point
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0, -2, 4);
	}
	else if (m_predator)
	{
		//Vert bits
		//X tri
		m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0, -2, 0);
		m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(5, 0, 2); //this is the point
		m_vertices[vert].Color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0, 2, 4);

		//Y tri
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0, 2, 0);
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(5, 0, 2); // this is the point
		m_vertices[vert].Color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		m_vertices[vert++].Pos = Vector3(0, -2, 4);
	}

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

void Boid::Spawn(Vector3 _scale, GameData* _GD, bool _isPredator, bool _isHerder)
{
	initialDirection = Vector3(((float)(rand() % max) - min), ((float)(rand() % max) - min), (((float)(rand() % max) - min)))*0.1;
	initialLocation = Vector3((float)(rand() % (startMax - startMin + 1) + startMin),
		((float)(rand() % (startMax - startMin + 1) + startMin)),
		((float)(rand() % (startMax - startMin + 1) + startMin)));

	m_alive = true; // turn this enemy ON ;)
	m_pos = initialLocation;
	m_scale = _scale;
	m_vel = initialDirection;
	m_predator = _isPredator;
	m_herder = _isHerder;
}

void Boid::Tick(GameData * _GD)
{
	if (m_alive)
	{
		//move to opposite end of box if out of range
		if (m_pos.x >= boxSize || m_pos.x <= -boxSize || m_pos.y >= boxSize || m_pos.y <= -boxSize || m_pos.z >= boxSize || m_pos.z <= -boxSize)
		{
			m_pos *= (-0.95);
		}
		else 
		{
			if (is2D)
			{
				m_pos.y = 0.0f;
			}
			m_pos += ((m_vel)* _GD->m_dt);
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


void Boid::setRotation()
{
	Matrix scaleMat = Matrix::CreateScale(m_scale);
	Matrix rotTransMat = Matrix::CreateWorld(m_pos, m_vel, m_up);
	Matrix transMat = Matrix::CreateTranslation(m_pos);
	m_worldMat = m_fudge * scaleMat * rotTransMat * transMat;

}

//void Boid::setColour(float r, float g, float b)
//{
//	m_vertices[3].Color.AdjustContrast(r);
//	m_vertices[4].Color.AdjustContrast(r);
//	m_vertices[5].Color.AdjustContrast(r);
//	m_vertices[6].Color.AdjustContrast(r);
//}

void Boid::setArmyToggle(bool _isArmySimOn)
{
	if (_isArmySimOn)
	{
		isArmySimOn = true;
	}
	else
	{
		isArmySimOn = false;
	}
}


