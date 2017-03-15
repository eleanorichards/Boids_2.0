#include "BoidManager.h"
#include <dinput.h>
#include "GameData.h"
#include <sstream>
#include "TextGO2D.h"
#include "DrawData2D.h"
#include <iostream>
#include <stdio.h>

BoidManager::BoidManager(int _numOfBoids, ID3D11Device * _pd3dDevice)
{
	for (int i = 0; i < _numOfBoids; i++)
	{
		m_Boids.push_back(new Boid(_pd3dDevice));
	}
}

BoidManager::~BoidManager()
{
}

void BoidManager::Tick(GameData * _GD)
{
	getUserInput(_GD);
	//Spawn in boids
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		if (!(*it)->isAlive() && placeBoid && it != m_Boids.end())
		{
			(*it)->Spawn(initialLocation, 0.5*Vector3::One, _GD);
			placeBoid = false;
		}
		if ((*it)->isAlive())
		{
			(*it)->Tick(_GD);
			moveBoid((*it), _GD);
		}
	}
}

void BoidManager::Draw(DrawData * _DD)
{
	//for (auto& it = boids.begin(); it != boids.end(); it++)
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		if((*it)->isAlive())
		(*it)->Draw(_DD);
	}
}

void BoidManager::getUserInput(GameData * _GD)
{
	if (_GD->m_keyboardState[DIK_Q] && placeBoid == false)
	{
		initialLocation = Vector3(((float)(rand() % startMax) - startMin), ((float)(rand() % startMax) - startMin), (((float)(rand() % startMax) - startMin)));
		boidsInScene++;
		placeBoid = true;
	}
	if (alignmentModifier >= 1 || separationModifier >= -1 || cohesionModifier >= -1)
	{

		if (_GD->m_keyboardState[DIK_1] && !(_GD->m_prevKeyboardState[DIK_1]))
			alignmentModifier -= 0.5f;
		else if (_GD->m_keyboardState[DIK_2] && !(_GD->m_prevKeyboardState[DIK_2]))
			alignmentModifier += 0.5f;
		else if (_GD->m_keyboardState[DIK_3] && !(_GD->m_prevKeyboardState[DIK_3]))
			separationRadius -= 0.5f;
		else if (_GD->m_keyboardState[DIK_4] && !(_GD->m_prevKeyboardState[DIK_4]))
			separationRadius += 0.5f;
		else if (_GD->m_keyboardState[DIK_5] && !(_GD->m_prevKeyboardState[DIK_5]))
			cohesionModifier -= 0.5f;
		else if (_GD->m_keyboardState[DIK_6] && !(_GD->m_prevKeyboardState[DIK_6]))
			cohesionModifier += 0.5f;
	}
	if (_GD->m_keyboardState[DIK_9] && !(_GD->m_prevKeyboardState[DIK_9]))
	{
		for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
		{
			(*it)->setColour(0, 1, 0);
		}
	}

}

void BoidManager::moveBoid(Boid* _boid, GameData * _GD)
{
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		Vector3 coh = cohesion(_boid) * cohesionModifier;
		Vector3 sep = separation(_boid);
		Vector3 ali = alignment(_boid) * alignmentModifier;

		_boid->setVelocity(_boid->getVelocity() + sep + coh + ali);
	}
}

//towards the centre of mass of other boids
Vector3 BoidManager::cohesion(Boid* _boid)
{
	Vector3 target = Vector3::Zero;
	int count = 0;

	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		float distance = Vector3::Distance(_boid->GetPos(), (*it)->GetPos());
		if (distance > 0 && distance < cohesionRadius)
		{
			target += (*it)->GetPos();
			count++;
		}
	}
	if (count > 0)
	{
		target /= count;
		return seek(target, _boid->GetPos(), _boid->getVelocity());
	}
	else
	{
		return Vector3::Zero;
	}
}

Vector3 BoidManager::seek(Vector3 _target, Vector3 _pos, Vector3 _vel)
{
	Vector3 desired = _target - _pos;
	desired.Normalize();
	desired *= maxSpeed;
	desired = XMVector3ClampLength(desired, 0.0f, maxSpeed);

	Vector3 steer = desired - _vel;
	steer = XMVector3ClampLength(steer, 0.0f, maxForce);

	return steer;

}

//steer away from nearby boids
Vector3 BoidManager::separation(Boid* _boid)
{
	Vector3 steer = Vector3::Zero;
	int boidsInRange = 0;

	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		if (_boid != *it && _boid->isAlive() && (*it)->isAlive())
		{
			float distance = Vector3::Distance(_boid->GetPos(), (*it)->GetPos());
			if (distance > 0 && distance < separationRadius)
			{
				Vector3 steerAway = _boid->GetPos() - (*it)->GetPos();
				steerAway.Normalize();
				steerAway /= distance;
				steer += steerAway;
				boidsInRange++;
			}
		}
	}
	if (boidsInRange > 0)
	{
		steer /= boidsInRange;
	}
	if (steer != Vector3::Zero)
	{
		steer.Normalize();
		steer *= maxSpeed;
		steer -= _boid->getVelocity();
		steer = XMVector3ClampLength(steer, 0.0f, maxForce);
	}
	return steer;

}

//calculate average velocity of all nearby boids
Vector3 BoidManager::alignment(Boid* _boid)
{
	int boidsInRange = 0;
	Vector3 sum = Vector3::Zero;

	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		float distance = Vector3::Distance(_boid->GetPos(), (*it)->GetPos());
		if (distance > 0 && distance < alignmentRadius)
		{
			sum += (*it)->getVelocity();
			boidsInRange++;
		}
	}
	if (boidsInRange > 0)
	{
		sum /= boidsInRange;
		sum.Normalize();
		sum *= maxSpeed;

		Vector3 steer = sum - _boid->getVelocity();
		steer = XMVector3ClampLength(steer, 0.0f, maxForce);
		return steer;
	}
	else
	{
		return Vector3::Zero;

	}

}


std::string BoidManager::getNumOfBoidsAsString()
{
	std::stringstream bString;
	bString << boidsInScene;
	return bString.str();
}

std::string BoidManager::getAlignmentAsString()
{
	std::stringstream aString;
	aString << alignmentModifier;
	return aString.str();
}


std::string BoidManager::getSeparationAsString()
{
	std::stringstream bString;
	bString << separationRadius;
	return bString.str();
}


std::string BoidManager::getCohesionAsString()
{
	std::stringstream cString;
	cString << cohesionModifier;
	return cString.str();
}

void BoidManager::DrawScreenSpace(DrawData2D* _DD2D)
{
	/*TextGO2D boidNumText("Boids: (Q)" + getNumOfBoidsAsString() + "\nAlignment (1-2): " + getAlignmentAsString() + "\nSeparation (3-4): " + getSeparationAsString() + "\nCohesion (5-6): " + getCohesionAsString());
	boidNumText.SetPos(Vector2(0.0f, 60.0f));
	boidNumText.SetColour(Color((float*)&DirectX::Colors::Green));
	boidNumText.SetScale(0.4f);
	boidNumText.Draw(_DD2D);	*/
}
