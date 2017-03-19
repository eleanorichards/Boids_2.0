#include "BoidManager.h"
#include <dinput.h>
#include "GameData.h"
#include <stdio.h>

#include "AntTweakBar.h"


BoidManager::BoidManager(int _numOfBoids, ID3D11Device * _pd3dDevice)
{
	for (int i = 0; i < _numOfBoids; i++)
	{
		m_Boids.push_back(new Boid(_pd3dDevice));
	}

	TwBar* pTweakBar;
	pTweakBar = TwGetBarByName("Boid Manager");

	TwAddVarRW(pTweakBar, "Num of Boids", TW_TYPE_FLOAT, &desiredBoids, "min=0 max=300 step=1 group=Boid");

	TwAddVarRW(pTweakBar, "Separation", TW_TYPE_FLOAT, &separationRadius, "min=-50 max=100 step=0.5 group=Radius");
	TwAddVarRW(pTweakBar, "Cohesion", TW_TYPE_FLOAT, &cohesionRadius, "min=-50 max=100 step=0.5 group=Radius");
	TwAddVarRW(pTweakBar, "Alignment", TW_TYPE_FLOAT, &alignmentRadius, "min=-50 max=100 step=0.5 group=Radius");

	TwAddVarRW(pTweakBar, "Sep", TW_TYPE_FLOAT, &separationModifier, "min=0 max=5 step=0.1 group=Weight");
	TwAddVarRW(pTweakBar, "Coh", TW_TYPE_FLOAT, &cohesionModifier, "min=0 max=5 step=0.1 group=Weight");
	TwAddVarRW(pTweakBar, "Ali", TW_TYPE_FLOAT, &alignmentModifier, "min=0 max=5 step=0.1 group=Weight");
	TwAddVarRW(pTweakBar, "Max Speed", TW_TYPE_FLOAT, &maxSpeed, "min=0 max=50 step=0.1 group=Weight");
	TwAddVarRW(pTweakBar, "Turn Force", TW_TYPE_FLOAT, &maxForce, "min=0 max=10 step=0.01 group=Weight");

	TwAddVarRW(pTweakBar, "Speed", TW_TYPE_FLOAT, &speedModifier, "min=-1 max=1 step=0.01 group=Weight");
	TwAddVarRW(pTweakBar, "Contrast", TW_TYPE_FLOAT, &boidContrast, "min=-1 max=1 step=0.01 group=Weight");

	TwAddVarRW(pTweakBar, "Predators", TW_TYPE_FLOAT, &numOfPredators, "min=0 max=10 step=1 group=Predators");


	//TwAddVarRW(pTweakBar, "Num of Predators", TW_TYPE_FLOAT, &desiredBoids, "min=0 max=250 step=1 group=Boid");
}

BoidManager::~BoidManager()
{
}

void BoidManager::Tick(GameData * _GD)
{
	getUserInput(_GD);
	//Spawn in boids
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); ++it)
	{
		if (!(*it)->isAlive() && it != m_Boids.end())
		{
			if(numOfPredators > predatorsInScene)
			{
				(*it)->Spawn(0.5*Vector3::One, _GD, true);
				predatorsInScene++;
			}
			if (desiredBoids > boidsInScene)
			{
				(*it)->Spawn(0.5*Vector3::One, _GD, false);
				boidsInScene++;
			}
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
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); ++it)
	{
		if((*it)->isAlive())
		(*it)->Draw(_DD);
	}
}

void BoidManager::getUserInput(GameData * _GD)
{
	if (_GD->m_keyboardState[DIK_9] && !(_GD->m_prevKeyboardState[DIK_9]))
	{
		for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); ++it)
		{
			(*it)->setColour(boidContrast, 0, 0);
		}
	}

}

void BoidManager::moveBoid(Boid* _boid, GameData * _GD)
{
	if (_boid->isAlive() && !_boid->isPredator())
	{
		//get distance of nearby boids
		Vector3 coh = cohesion(_boid) * cohesionModifier;
		Vector3 sep = separation(_boid) * separationModifier;
		Vector3 ali = alignment(_boid) * alignmentModifier;

		Vector3 esc = escape(_boid) * escapeModifier;

		_boid->setVelocity((_boid->getVelocity() + sep + coh + ali + esc)  /*+ (speedModifier * Vector3::One)*/);
	}
	else if (_boid->isAlive() && _boid->isPredator())
	{
		//get distance of nearby boids
		Vector3 coh = cohesion(_boid) * (cohesionModifier + 1);
		Vector3 sep = separation(_boid) * separationModifier;
		Vector3 ali = alignment(_boid) * alignmentModifier;

		_boid->setVelocity((_boid->getVelocity() + coh)  /*+ (speedModifier * Vector3::One)*/);
	}

}

//towards the centre of mass of other boids
Vector3 BoidManager::cohesion(Boid* _boid)
{
	Vector3 target = Vector3::Zero;
	int count = 0;

	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		if ((*it)->isAlive() && _boid->isAlive() && _boid != (*it) && !(*it)->isPredator())
		{
			float distance = Vector3::DistanceSquared(_boid->GetPos(), (*it)->GetPos());

			if (distance > 0 && distance < cohesionRadius * 10)
			{
				target += (*it)->GetPos();
				count++;
			}
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

Vector3 BoidManager::escape(Boid * _boid)
{
	Vector3 predatorLocation = Vector3::Zero;

	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		float distance = Vector3::DistanceSquared(_boid->GetPos(), (*it)->GetPos());
		if (distance > 0 && distance < 500)
		if ((*it)->isPredator())
		{
			{
				predatorLocation = (*it)->GetPos();
				return seek(predatorLocation, _boid->GetPos(), _boid->getVelocity());

			}
		}
		else
		{
			return Vector3::Zero;
		}
	}
}

void BoidManager::set2D(bool _is2D)
{
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		(*it)->set2D(_is2D);
	}
}

//steer away from nearby boids
Vector3 BoidManager::separation(Boid* _boid)
{
	Vector3 steer = Vector3::Zero;
	int boidsInRange = 0;

	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		if (_boid != *it && _boid->isAlive() && (*it)->isAlive() && !(*it)->isPredator())
		{
			float distance = Vector3::DistanceSquared(_boid->GetPos(), (*it)->GetPos());
			if (distance > 0 && distance < separationRadius * 10)
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
		if (_boid != *it && _boid->isAlive() && (*it)->isAlive() && !(*it)->isPredator())
		{
			float distance = Vector3::DistanceSquared(_boid->GetPos(), (*it)->GetPos());

			if (distance > 0 && distance < alignmentRadius * 10)
			{
				sum += (*it)->getVelocity();
				boidsInRange++;
			}
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


