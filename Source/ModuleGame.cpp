#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, ModulePhysics* _physics, Module* _listener, EntityType type)
		: body(_body)
		, physics(_physics)
		, listener(_listener)
		, type(type)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

public:
	PhysBody* body;
	Module* listener;
	EntityType type;
	ModulePhysics* physics;
};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true)
		: PhysicEntity(physics->CreateCircle(_x, _y, _texture.height / 2, angle, dynamic), physics, _listener, type)
		, texture(_texture)
	{
		body->type = type;
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}

private:
	Texture2D texture;

};

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true)
		: PhysicEntity(physics->CreateRectangle(_x, _y, _texture.width, _texture.height, angle, dynamic), physics, _listener, type)
		, texture(_texture)
	{
		body->type = type;
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;

};

class BoxSensor : public PhysicEntity
{
public:
	BoxSensor(ModulePhysics* physics, int _x, int _y, int width, int height, Module* _listener, EntityType type, float angle = 0.f, bool dynamic = true)
		: PhysicEntity(physics->CreateRectangleSensor(_x, _y, width, height, angle, dynamic), physics, _listener, type)
	{
		body->type = type;
	}

	void Update() override
	{

	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:

};

class Ball : public Circle
{
public:
	Ball(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, int ballNum)
		: Circle(physics, _x, _y, _listener, _texture, EntityType::BALL), ballNum(ballNum)
	{
		
	}

	/*void Update() override
	{
		
	}*/

	~Ball() {
		auto pbody = body->body;
		/*delete body;
		if (body) body = nullptr;*/
		physics->DestroyBody(pbody);
	}

public:
	int ballNum;

};

class Flipper : public Box
{
public:
	Flipper(ModulePhysics* physics, Module* _listener, Texture2D _texture, PhysicEntity* attachTo, b2Vec2 anchorPoint, bool left)
		: Box(physics, anchorPoint.x + ((left ? 1 : -1) * (_texture.width / 2)), anchorPoint.y, _listener, _texture, EntityType::FLIPPER)
	{
		float lowerAngle, upperAngle;
		if (left) {
			lowerAngle = -30;
			upperAngle = 30;
		}
		else {
			lowerAngle = -30;
			upperAngle = 30;
		}
		physics->CreateRevoluteJoint(body->body, attachTo->body->body, anchorPoint.x, anchorPoint.y, lowerAngle, upperAngle);
	}

	/*void Update() override
	{

	}*/

public:

};

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleGame::~ModuleGame()
{

}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	App->audio->PlayMusic("Assets/music/gameMusic.wav");

	ball_t = LoadTexture("Assets/wheel.png");
	wall_ver_t = LoadTexture("Assets/wall_vertical.png");
	wall_hor_t = LoadTexture("Assets/wall_horizontal.png");
	flipper_left_t = LoadTexture("Assets/flipper_left.png");
	flipper_right_t = LoadTexture("Assets/flipper_right.png");
	flipperFX = App->audio->LoadFx("Assets/FX/flipper.wav") - 1;
	CreateMap();
	AddBalls(3);

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");
	

	return true;
}

// Update: draw background
update_status ModuleGame::Update(double dt)
{
	// Ball launch
	if (!ballLaunched && IsKeyDown(KEY_SPACE))
	{
		GetCurrentBall()->body->ApplyImpulse(0.f, -10.f);
		ballLaunched = true;
	}

	// Flippers
	if (IsKeyDown(KEY_LEFT))
	{
		flipperLeft->body->ApplyImpulse(0.f, -10.f);
		App->audio->PlayFx(flipperFX);
	}

	if (IsKeyDown(KEY_RIGHT))
	{
		flipperRight->body->ApplyImpulse(0.f, -10.f);
		App->audio->PlayFx(flipperFX);
	}

	// Bonus / balls logic
	if (CheckBonus())
	{
		int newBalls = startingBalls - currentBall;
		Ball* ball = nullptr;
		for (int i = 0; i < balls.size(); i++)
		{
			if (balls[i]->ballNum == currentBall) ball = balls[i];
			else delete balls[i];
		}
		balls.clear();
		balls.push_back(ball);
		AddBalls(newBalls, currentBall + 1);
	}

	// Update all entities (just update positions, no extra dt multiplication)
	for (PhysicEntity* entity : entities)
		entity->Update();

	for (Ball* b : balls)
		b->Update();

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyA->type == EntityType::BALL && bodyB->type == EntityType::DEATHZONE) {
		delete GetCurrentBall();
		balls.erase(balls.begin());
		currentBall++;
		ballLaunched = false;
	}
}

void ModuleGame::CreateMap()
{
	// deathzone
	entities.emplace_back(new BoxSensor(App->physics, GetScreenWidth() / 2, GetScreenHeight() + 150, GetScreenWidth(), 300, this, EntityType::DEATHZONE, 0, false));
	
	// walls
	entities.emplace_back(new Box(App->physics, 15, GetScreenHeight() - 250, this, wall_ver_t, EntityType::OBSTACLE, 0, false));
	entities.emplace_back(new Box(App->physics, 100, GetScreenHeight() - 220, this, wall_ver_t, EntityType::OBSTACLE, 0, false));
	entities.emplace_back(new Box(App->physics, -30, GetScreenHeight() - 80, this, wall_hor_t, EntityType::OBSTACLE, 0, false));
	entities.emplace_back(new Box(App->physics, 85, GetScreenHeight() - 530, this, wall_ver_t, EntityType::OBSTACLE, 30, false));
	
	Box* flipperWallLeft = new Box(App->physics, GetScreenWidth() / 2 - 260, GetScreenHeight() - 160, this, wall_hor_t, EntityType::OBSTACLE, 30, false);
	Box* flipperWallRight = new Box(App->physics, GetScreenWidth() / 2 + 260, GetScreenHeight() - 160, this, wall_hor_t, EntityType::OBSTACLE, 330, false);
	entities.emplace_back(flipperWallLeft);
	entities.emplace_back(flipperWallRight);
	
	b2Vec2 localAnchorLeft(PIXEL_TO_METERS(flipperWallLeft->body->width / 2), 0.f);
	b2Vec2 localAnchorRight(-PIXEL_TO_METERS(flipperWallRight->body->width / 2), 0.f);
	b2Vec2 worldAnchorLeft = flipperWallLeft->body->body->GetWorldPoint(localAnchorLeft);
	b2Vec2 worldAnchorRight = flipperWallRight->body->body->GetWorldPoint(localAnchorRight);
	b2Vec2 worldPixelLeft(METERS_TO_PIXELS(worldAnchorLeft.x), METERS_TO_PIXELS(worldAnchorLeft.y));
	b2Vec2 worldPixelRight(METERS_TO_PIXELS(worldAnchorRight.x), METERS_TO_PIXELS(worldAnchorRight.y));
	
	flipperLeft = new Flipper(App->physics, this, flipper_left_t, flipperWallLeft, worldPixelLeft, true);
	flipperRight = new Flipper(App->physics, this, flipper_right_t, flipperWallRight, worldPixelRight, false);
	entities.emplace_back(flipperLeft);
	entities.emplace_back(flipperRight);

}

void ModuleGame::AddBalls(int ballCount, int firstId)
{
	int y = 400;
	for (int i = 0; i < ballCount; i++)
	{
		balls.emplace_back(new Ball(App->physics, 45, GetScreenHeight() - y, this, ball_t, i));
		y -= 80;
	}
	//currentBall = balls.size() - 1;
}

Ball* ModuleGame::GetCurrentBall()
{
	Ball* ball;
	for (int i = 0; i < balls.size(); i++) {
		ball = balls[i];
		if (ball->ballNum == currentBall) {
			return ball;
		}
	}
	return nullptr;
}

bool ModuleGame::CheckBonus()
{
	return false;
}
