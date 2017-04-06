#include "BoidManager.h"
#include <dinput.h>
#include "GameData.h"
#include <stdio.h>

#include "AntTweakBar.h"

//SET ARMY
void TW_CALL RunCBARMY(void *_boidManager)
{
	BoidManager* pBoidManager = (BoidManager*) _boidManager;
	if (!pBoidManager->getArmyToggle())
	{
		pBoidManager->setArmyToggle(true);
		if (!pBoidManager->get2D())
		{
			pBoidManager->set2D(true);

		}
	}
	else
	{
		pBoidManager->setArmyToggle(false);
	}
}

//SET 2D
void TW_CALL RunCB2D(void *_boidManager)
{
	BoidManager* pBoidManager = (BoidManager*)_boidManager;
	if (!pBoidManager->get2D())
	{
		pBoidManager->set2D(true);

	}
	else
	{
		pBoidManager->set2D(false);
	}
}

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
	bTweakBar = TwGetBarByName("Boid Behaviour");

	TwAddVarRW(bTweakBar, "Boids",			TW_TYPE_FLOAT, &desiredBoids,		"min=0 max=800 step=1");

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

	TwAddVarRW(pTweakBar, "Predators",		TW_TYPE_FLOAT, &numOfPredators,		"min=0 max=10 step=1 group=Predators");
	TwAddVarRW(pTweakBar, "Escape Speed",	TW_TYPE_FLOAT, &escapeModifier,		"min=-100 max=100 step=1 group=Predators");
	TwAddVarRW(pTweakBar, "Escape Radius",	TW_TYPE_FLOAT, &escapeRadius,		"min=-100 max=100 step=1 group=Predators");

	TwAddVarRW(pTweakBar, "Herder", TW_TYPE_FLOAT, &numOfHerders, "min=0 max=1 step=1 group=Predators");


	TwAddVarRW(pTweakBar, "Amplitude",		TW_TYPE_FLOAT, &amplitude,			"min=-100 max=100 step=0.1 group=Predators");
	TwAddVarRW(pTweakBar, "Frequency",		TW_TYPE_FLOAT, &frequency,			"min=-100 max=100 step=0.1 group=Predators");


	TwAddVarRW(pTweakBar, "Obstacles",		TW_TYPE_FLOAT, &obstaclesDesired,	"min=0 max=10 step=1 group=Obstacles");
	TwAddVarRW(pTweakBar, "Spawn Location", TW_TYPE_DIR3D, &obstacleSpawn,		"group=Obstacles");

	TwAddButton(pTweakBar, "Army Sim", RunCBARMY, this, "group=Room");
	TwAddButton(pTweakBar, "Toggle 2D", RunCB2D, this, "group=Room");

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
				(*it)->Spawn(2*Vector3::One, _GD, true, false);
				predatorsInScene++;
			}
			if (desiredBoids > boidsInScene) //if normal boid added
			{
				(*it)->Spawn(0.8*Vector3::One, _GD, false, false);
				boidsInScene++;
			}
			if ( numOfHerders > herdersInScene) //if herder added
			{
				(*it)->Spawn(2.5*Vector3::One, _GD, false, true);
				herdersInScene++;
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
			if (herdersInScene > numOfHerders && !(*it)->isHerder()) //if normal boid taken out
			{
				(*it)->SetAlive(false);
				herdersInScene--;
			}
			//Tick & move every frame
			//if (herdersInScene == 1)
			//{
			

			//}
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
	if (_boid->isAlive() && !_boid->isPredator() && !_boid->isHerder()) 
	{
		if (armySim)
		{
			cohesionModifier = 0.3f;
			escapeModifier *= -0.9;
		}
		Vector3 coh = cohesion(_boid) * cohesionModifier;
		Vector3 sep = separation(_boid) * separationModifier;
		Vector3 ali = alignment(_boid) * alignmentModifier;

		Vector3 esc = -escape(_boid) * escapeModifier;
		_boid->setVelocity((_boid->getVelocity() + sep + coh + ali + esc)  /*+ (speedModifier * Vector3::One)*/);
	}
	if (_boid->isAlive() && _boid->isPredator() && !_boid->isHerder())
	{
		Vector3 coh = cohesion(_boid) * predatorCohesionModifier;
		Vector3 wig = wiggle(_boid, _GD);
		_boid->setVelocity((_boid->getVelocity() + coh + wig)  /*+ (speedModifier * Vector3::One)*/);
	}
	if (_boid->isAlive() && !_boid->isPredator() && _boid->isHerder())
	{
		Vector3 mov = movePlayer(_GD, _boid);
		_boid->setVelocity(_boid->getVelocity() + mov);
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
				if (!(*it)->isPredator() && !(*it)->isHerder())
				{
					target += (*it)->GetPos();
					count++;
					if (_boid->isPredator() && count >= 15) // if predator is "surrounded"
					{
						_boid->SetAlive(false); //kill the predator
					}
				}
				//if predator is too close to boid, EAT THE BOID
				else if ((*it)->isPredator() && distance < 100 && !_boid->isPredator()) 
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
	desired.Normalize(); // set between 0-1
	desired *= maxSpeed; // adjsutable in AntTweakBar
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
			&& !_boid->isPredator() && !_boid->isHerder() && (*it)->isPredator() 
			|| (*it)->isHerder())
		{
			//if distance is less than the escape radius
			float distance = Vector3::DistanceSquared(_boid->GetPos(), (*it)->GetPos());
			if (distance > 0 && distance < escapeRadius * 10)
			{
				//add predator location to repel locations
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

	//wiggly.z = _boid->getVelocity().z * amplitude * sin(2 * XM_PI*frequency*_GD->m_dt);

	return wiggly;
}


//pass is2D through to Boids
void BoidManager::set2D(bool _is2D)
{
	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		(*it)->set2D(_is2D);
	}
	if (is2D)
		is2D = false;
	else
		is2D = true;
}

void BoidManager::setArmyToggle(bool _isArmySimOn)
{
	Vector3 startLocation = Vector3(0.0f, 0.0f, 10.0f);
	numOfPredators = 5;
	desiredBoids = 500;

	for (list<Boid*>::iterator it = m_Boids.begin(); it != m_Boids.end(); it++)
	{
		//set up start locations
		if (!(*it)->isPredator() && !(*it)->isHerder())
		{
			(*it)->SetPos(startLocation);
			startLocation.z += 20.0f;
			startLocation.x += 20.0f;
			//tell each boid army is on so cohesion is turned off
		}
		(*it)->setArmyToggle(_isArmySimOn);
	}

	if (armySim)
		armySim = false;
	else
		armySim = true;
}

Vector3 BoidManager::movePlayer(GameData * _GD, Boid* _boid)
{
	Vector3 target = Vector3::Zero;
	float incrementor = 50;
	//WORKS MOST EFFECTIVELY IN 2D
	if (_GD->m_keyboardState[DIK_MINUS] & 0x80)//RAISE
	{
		target.y -= incrementor;
	}
	if (_GD->m_keyboardState[DIK_ADD] & 0x80)//LOWER
	{
		target.y += incrementor;
	}
	if (_GD->m_keyboardState[DIK_UP] & 0x80)//UP
	{
		target.z -= incrementor;
	}
	if (_GD->m_keyboardState[DIK_DOWN] & 0x80)//DOWN
	{
		target.z += incrementor;
	}	
	if (_GD->m_keyboardState[DIK_LEFT] & 0x80)//LEFT
	{
		target.x -= incrementor;
	}
	if (_GD->m_keyboardState[DIK_RIGHT] & 0x80)//RIGHT
	{
		target.x += incrementor;
	}
	//similar to capping of ali, coh and sep. 
	if (target != Vector3::Zero)
	{
		target.Normalize();
		target *= (maxSpeed + 10);
		target -= _boid->getVelocity();
		target = XMVector3ClampLength(target, 0.0f, maxForce);
	}

	return target;
}


//move away from boids within separation radius
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


