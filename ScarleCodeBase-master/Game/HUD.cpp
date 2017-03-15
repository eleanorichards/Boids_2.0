#include "HUD.h"
#include "TextGO2D.h"
#include <sstream>
#include "BoidManager.h"
#include "DrawData2D.h"
#include "AntTweakBar.h"

HUD::HUD()
{
	BoidManager* pBoidManager;
}

HUD::~HUD()
{
}

void HUD::Draw(DrawData * _DD)
{

	//TextGO2D boidNumText("Boids: (Q)" + pBoidManager->getNumOfBoidsAsString() + "\nAlignment (1-2): " + pBoidManager->getAlignmentAsString() + "\nSeparation (3-4): " + pBoidManager->getSeparationAsString() + "\nCohesion (5-6): " + pBoidManager->getCohesionAsString());
	//boidNumText.SetPos(Vector2(0.0f, 60.0f));
	//boidNumText.SetColour(Color((float*)&DirectX::Colors::Green));
	//boidNumText.SetScale(0.4f);
	//boidNumText.Draw(_DD);
	
}




