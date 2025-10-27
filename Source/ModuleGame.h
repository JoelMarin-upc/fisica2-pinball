#pragma once

#include "Globals.h"
#include "Module.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicEntity;
class Ball;
class Flipper;


class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void CreateMap();
	void AddBalls(int ballCount);
	Ball* GetCurrentBall();

public:
	std::vector<PhysicEntity*> entities;
	std::vector<Ball*> balls;
	int currentBall = 0;
	bool ballLaunched = false;
	
	Flipper* flipperLeft;
	Flipper* flipperRight;

	Texture2D ball_t;
	Texture2D wall_hor_t;
	Texture2D wall_ver_t;
	Texture2D flipper_left_t;
	Texture2D flipper_right_t;
	int flipperFX;

};
