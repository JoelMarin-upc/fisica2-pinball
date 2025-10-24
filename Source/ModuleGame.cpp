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

protected:
	PhysBody* body;
	Module* listener;
	EntityType type;
	ModulePhysics* physics;
};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type)
		: PhysicEntity(physics->CreateCircle(_x, _y, _texture.height), physics, _listener, type)
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
	Box(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type)
		: PhysicEntity(physics->CreateRectangle(_x, _y, _texture.width, _texture.height), physics, _listener, type)
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
	BoxSensor(ModulePhysics* physics, int _x, int _y, int width, int height, Module* _listener, EntityType type)
		: PhysicEntity(physics->CreateRectangleSensor(_x, _y, width, height), physics, _listener, type)
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
	Ball(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: Circle(physics, _x, _y, _listener, _texture, EntityType::BALL)
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

private:
	
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

	ball = LoadTexture("Assets/wheel.png");
	entities.emplace_back(new BoxSensor(App->physics, GetScreenWidth() / 2, GetScreenHeight(), GetScreenWidth(), 500, this, EntityType::DEATHZONE));

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		balls.emplace_back(new Ball(App->physics, GetMouseX(), GetMouseY(), this, ball));
	}

	for (PhysicEntity* entity : entities)
	{
		entity->Update();
	}

	for (Ball* b : balls)
	{
		b->Update();
	}

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyA->type == EntityType::BALL && bodyB->type == EntityType::DEATHZONE) {
		delete balls[currentBall];
		balls.erase(balls.begin());
	}
}