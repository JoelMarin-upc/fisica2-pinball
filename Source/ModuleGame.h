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
	update_status Update(double dt);
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void CreateMap();
	void AddBalls(int ballCount, int firstId = 0);
	Ball* GetCurrentBall();
	bool CheckBonus();
	void Lose();

	std::vector<PhysicEntity*> entities;
	std::vector<Ball*> balls;
	int currentBall = 0;
	bool ballLaunched = false;
	int startingBalls = 3;
	int score = 0;
	int prevScore = 0;
	int bestScore = 0;
	int gamesPlayed = 0;
	bool lost = false;

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
	int gameOverFX;
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

	int Pinball[194] = {
	576, 744,
	613, 744,
	613, 718,
	633, 688,
	745, 620,
	749, 449,
	745, 438,
	737, 427,
	727, 418,
	720, 414,
	713, 411,
	709, 404,
	708, 398,
	709, 391,
	712, 384,
	719, 376,
	724, 367,
	729, 355,
	731, 342,
	731, 324,
	729, 309,
	725, 295,
	720, 281,
	712, 265,
	703, 253,
	693, 242,
	680, 233,
	671, 229,
	667, 221,
	667, 214,
	669, 206,
	675, 201,
	683, 200,
	692, 200,
	702, 203,
	713, 208,
	727, 215,
	737, 223,
	749, 233,
	760, 244,
	770, 255,
	777, 266,
	782, 279,
	785, 291,
	784, 716,
	806, 716,
	810, 240,
	799, 220,
	784, 195,
	765, 175,
	744, 157,
	723, 144,
	703, 135,
	678, 126,
	650, 121,
	617, 120,
	591, 123,
	570, 130,
	550, 137,
	529, 149,
	514, 159,
	497, 172,
	480, 191,
	465, 211,
	454, 229,
	445, 249,
	438, 272,
	435, 292,
	434, 315,
	435, 336,
	439, 359,
	443, 378,
	451, 398,
	461, 416,
	474, 434,
	482, 444,
	483, 450,
	483, 456,
	481, 462,
	478, 467,
	473, 470,
	466, 472,
	459, 472,
	452, 477,
	445, 486,
	442, 497,
	441, 606,
	554, 687,
	575, 718,
	575, 743,
	420, 892,
	-136, 872,
	-110, -68,
	1048, -66,
	1061, 773,
	423, 892,
	575, 744
	};
};