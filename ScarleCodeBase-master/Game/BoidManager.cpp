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

	for (int i = 0; i < 10; i++)
	{
		m_Obstacles.push_back(new Obstacle(_pd3dDevice));
	}

	TwBar* bTweakBar;
	bTweakBar = TwGetBarByName("Boid");

	TwAddVarRW(bTweakBar, "Boid num",			TW_TYPE_FLOAT, &desiredBoids,		"min=0 max=800 step=1");

	TwAddVarRW(bTweakBar, "Separation",		TW_TYPE_FLOAT, &separationRadius,	"min=-50 max=100 step=0.5 group=Radius");
	TwAddVarRW(bTweakBar, "Cohesion",		TW_TYPE_FLOAT, &cohesionRadius,		"min=-50 max=100 step=0.5 group=Radius");
	TwAddVarRW(bTweakBar, "Alignment",		TW_TYPE_FLOAT, &alignmentRadius,	"min=-50 max=100 step=0.5 group=Radius");

	TwAddVarRW(bTweakBar, "Sep",			TW_TYPE_FLOAT, &separationModifier,	"min=0 max=5 step=0.1 group=Weight");
	TwAddVarRW(bTweakBar, "Coh",			TW_TYPE_FLOAT, &cohesionModifier,	"min=0 max=5 step=0.1 group=Weight");
	TwAddVarRW(bTweakBar, "Ali",			TW_TYPE_FLOAT, &alignmentModifier,	"min=0 max=5 step=0.1 group=Weight");
	TwAddVarRW(bTweakBar, "Max Speed",		TW_TYPE_FLOAT, &maxSpeed,			"min=0 max=50 step=0.1 group=Weight");
	TwAddVarRW(bTweakBar, "Turn Force",		TW_TYPE_FLOAT, &maxForce,			"min=0 max=10 step=0.01 group=Weight");

	TwBar* pTweakBar;
	pTweakBar = TwGetBarByName("Predator");

	TwAddVarRW(pTweakBar, "Predator num",		TW_TYPE_FLOAT, &numOfPredators,		"min=0 max=10 step=1 group=Predators");
	TwAddVarRW(pTweakBar, "Escape Speed",	TW_TYPE_FLOAT, &escapeModifier,		"min=-100 max=100 step=1 group=Predators");
	TwAddVarRW(pTweakBar, "Escape Radius",	TW_TYPE_FLOAT, &escapeRadius,		"min=-100 max=100 step=1 group=Predators");

	TwAddVarRW(pTweakBar, "Amplitude",		TW_TYPE_FLOAT, &amplitude,			"min=-100 max=100 step=0.1 group=Predators");
	TwAddVarRW(pTweakBar, "Frequency",		TW_TYPE_FLOAT, &frequency,			"min=-100 max=100 step=0.1 group=Predators");


	TwAddVarRW(pTweakBar, "Obstacle num", TW_TYPE_FLOAT, &obstaclesDesired, "min=0 max=10 step=1 group=Obstacles");
	TwAddVarRW(pTweakBar, "Spawn Location", TW_TYPE_DIR3D, &obstacleSpawn, "group=Obstacles");

}

BoidManager::~BoidManager()
{
}

void BoidManager::Tick(GameData * _GD)
{
	//Spawn in boids
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); ++it)
	{
		//if not already spawned (i.e. alive)
		if (!(*it)->isAlive())
		{
			if(numOfPredators > predatorsInScene) //if predator added
			{
				(*it)->Spawn(2*Vector3::One, _GD, true);
				predatorsInScene++;
			}
			if (desiredBoids > boidsInScene) //if normal boid added
			{
				(*it)->Spawn(0.5*Vector3::One, _GD, false);
				boidsInScene++;
			}
		}
		else if ((*it)->isAlive())
		{
			if (predatorsInScene > numOfPredators && (*it)->isPredator()) //if predator taken out
			{
				
				(*it)->SetAlive(false);
				predatorsInScene--;
				
			}
			if (boidsInScene > desiredBoids && !(*it)->isPredator()) //if normal boid taken out
			{
				(*it)->SetAlive(false);
				boidsInScene--;
			}

			

			//Tick & move every frame
			(*it)->Tick(_GD);
			moveBoid((*it), _GD);
		}
	}
	for (list<Obstacle*>::iterator Oit = m_Obstacles.begin(); Oit != m_Obstacles.end(); ++Oit)
	{
		if (obstaclesDesired > obstaclesInScene)
		{
			(*Oit)->Spawn(Vector3::One, _GD, Vector3::Zero);
			obstaclesInScene++;

		}
		(*Oit)->Tick(_GD);

	}
}

void BoidManager::Draw(DrawData * _DD)
{
	//draw if alive
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); ++it)
	{
		if((*it)->isAlive())
		(*it)->Draw(_DD);
	}
	for (list<Obstacle*>::iterator it = m_Obstacles.begin(); it != m_Obstacles.end(); ++it)
	{
		if ((*it)->isAlive())
			(*it)->Draw(_DD);
	}
}

//apply 3 base rules + escape (from predators)
//Predators only apply cohesion
void BoidManager::moveBoid(Boid* _boid, GameData * _GD)
{
	if (_boid->isAlive() && !_boid->isPredator()) 
	{
		Vector3 coh = cohesion(_boid) * cohesionModifier;
		Vector3 sep = separation(_boid) * separationModifier;
		Vector3 ali = alignment(_boid) * alignmentModifier;

		Vector3 esc = -escape(_boid) * escapeModifier;

		_boid->setVelocity((_boid->getVelocity() + sep + coh + ali + esc)  /*+ (speedModifier * Vector3::One)*/);
	}
	else if (_boid->isAlive() && _boid->isPredator())
	{
		Vector3 coh = cohesion(_boid) * cohesionModifier;
		Vector3 wig = wiggle(_boid, _GD);
		_boid->setVelocity((_boid->getVelocity() + coh + wig)  /*+ (speedModifier * Vector3::One)*/);
	}

}

//find the centre of mass of other boids
Vector3 BoidManager::cohesion(Boid* _boid)
{
	Vector3 target = Vector3::Zero;
	int count = 0;

	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		if ((*it)->isAlive() && _boid->isAlive() && _boid != (*it) )
		{
			float distance = Vector3::DistanceSquared(_boid->GetPos(), (*it)->GetPos());
			if (distance > 0 && distance < cohesionRadius * 10)
			{
				if (!(*it)->isPredator())
				{
					target += (*it)->GetPos();
					count++;
				}
				else if ((*it)->isPredator() && distance < 100 && !_boid->isPredator()) //if predator is too close to boid, EAT HIM
				{
					_boid->SetAlive(false);
					desiredBoids--;
				}
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


//steer towards the centre of mass (cohesion)
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


//escape from predators
Vector3 BoidManager::escape(Boid * _boid)
{
	Vector3 repelLocation = Vector3::Zero;
	int count = 0;
	//check distance from predators
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		if ((*it)->isAlive() && _boid->isAlive() && _boid != (*it) 
			&& !_boid->isPredator() && (*it)->isPredator())
		{
			
			float distance = Vector3::DistanceSquared(_boid->GetPos(), (*it)->GetPos());
			if (distance > 0 && distance < escapeRadius * 10)
			{
				repelLocation += (*it)->GetPos();
				count++;
			}
			
		}
	}
	//check distance from obstacles
	for (list<Obstacle*>::iterator Oit = m_Obstacles.begin(); Oit != m_Obstacles.end(); ++Oit)
	{
		if ((*Oit)->isAlive() && _boid->isAlive())
		{
			float distance = Vector3::DistanceSquared(_boid->GetPos(), (*Oit)->GetPos());
			if (distance > 0 && distance < escapeRadius * 10)
			{
				repelLocation += (*Oit)->GetPos();
				count++;

			}
		}
	}




	if (count > 0)
	{
		return seek(repelLocation, _boid->GetPos(), _boid->getVelocity());
		repelLocation /= count;
	}
	else
	{
		return Vector3::Zero;
	}
}

Vector3 BoidManager::wiggle(Boid * _boid, GameData * _GD)
{
	//Vector3 wiggle = Vector3::Zero;
	Vector3 wiggly = Vector3::Zero;

	//wiggly.x = amplitude * sin(2 * XM_PI*frequency*_GD->m_dt);

	// = a * Mathf.Sin(2 * Mathf.Pi*freq*_GD->m_dt
	return wiggly;
}


//pass is2D through to Boids
void BoidManager::set2D(bool _is2D)
{
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		(*it)->set2D(_is2D);
	}
}


//move away from nearby boids
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


//calculate & match average velocity of all nearby boids
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


