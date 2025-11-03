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
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void CreateMap();
	void AddBalls(int ballCount, int firstId = 0);
	Ball* GetCurrentBall();
	bool CheckBonus();

public:
	std::vector<PhysicEntity*> entities;
	std::vector<Ball*> balls;
	int currentBall = 0;
	bool ballLaunched = false;
	int startingBalls = 3;

	Flipper* flipperLeft;
	Flipper* flipperRight;

	Texture2D ball_t;
	Texture2D wall_t;
	Texture2D flipper_left_t;
	Texture2D flipper_right_t;
	Texture2D obstaculoDerecha_t;
	Texture2D obstaculoIzquierda_t;
	Texture2D obstaculo1_t;
	Texture2D obstaculo2_t;
	Texture2D rebotaDragon_t;
	Texture2D rebotaFuego_t;
	Texture2D rebotaHielo_t;
	Texture2D rebotaRayo_t;
	Texture2D fondo_t;

	std::vector<char> letters;
	std::vector<char> requiredLetters = {'a','b','c','d'};
	int flipperFX;
	int holeFX;
	int points1FX;
	int points2FX;
	int points3FX;
	int saque1FX;
	int saque2FX;
	int looseBallFX;
	int gameMusic;

private:
	int Obstaculo_Derecha[14] = {
	15, 100,
	63, 47,
	62, 6,
	60, 6,
	45, 40,
	3, 85,
	14, 99
	};

	int Obstaculo_Izquierda[14] = {
	51, 100,
	3, 48,
	4, 6,
	6, 6,
	22, 41,
	63, 85,
	52, 99
	};

	int Obstaculo1[66] = {
	62, 231,
	54, 210,
	47, 187,
	43, 162,
	43, 142,
	45, 120,
	50, 102,
	56, 87,
	64, 72,
	75, 54,
	83, 44,
	92, 34,
	95, 28,
	94, 18,
	89, 10,
	83, 5,
	76, 6,
	64, 11,
	50, 19,
	40, 29,
	28, 44,
	20, 60,
	13, 77,
	8, 98,
	6, 115,
	6, 131,
	7, 152,
	10, 170,
	17, 189,
	26, 204,
	37, 217,
	50, 227,
	60, 231
	};

	int Obstaculo2[10] = {
	33, 79,
	62, 28,
	34, 3,
	3, 26,
	32, 79
	};

	int Pinball_fondo[210] = {
	361, 716,
	381, 716,
	385, 240,
	373, 217,
	358, 196,
	341, 177,
	320, 159,
	297, 144,
	268, 131,
	240, 123,
	213, 121,
	184, 122,
	156, 126,
	131, 135,
	112, 143,
	91, 156,
	72, 171,
	54, 189,
	38, 212,
	26, 235,
	17, 257,
	11, 282,
	8, 303,
	8, 329,
	12, 354,
	17, 374,
	21, 388,
	31, 406,
	41, 425,
	48, 436,
	54, 443,
	57, 448,
	57, 454,
	56, 461,
	52, 467,
	47, 471,
	38, 472,
	30, 475,
	24, 480,
	20, 488,
	18, 495,
	17, 504,
	16, 607,
	127, 686,
	150, 718,
	150, 752,
	187, 752,
	187, 718,
	208, 688,
	320, 620,
	324, 450,
	320, 438,
	314, 429,
	306, 421,
	297, 416,
	291, 414,
	286, 408,
	283, 400,
	284, 392,
	287, 385,
	292, 378,
	298, 370,
	302, 358,
	305, 344,
	306, 329,
	305, 314,
	302, 299,
	296, 283,
	288, 268,
	279, 254,
	268, 242,
	258, 234,
	249, 228,
	244, 225,
	241, 219,
	241, 213,
	243, 208,
	246, 204,
	250, 201,
	256, 200,
	264, 200,
	274, 203,
	286, 208,
	298, 214,
	311, 223,
	324, 233,
	333, 241,
	341, 250,
	347, 259,
	354, 268,
	357, 276,
	359, 284,
	360, 290,
	360, 714,
	230, 865,
	-96, 843,
	-134, 492,
	-104, 81,
	-31, -85,
	435, -83,
	511, 38,
	540, 740,
	370, 874,
	231, 865,
	360, 715
	};
};