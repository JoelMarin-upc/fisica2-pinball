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
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f)
		: PhysicEntity(physics->CreateCircle(_x, _y, _texture.height / 2, angle, dynamic, restitution), physics, _listener, type)
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
	Box(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f)
		: PhysicEntity(physics->CreateRectangle(_x, _y, _texture.width, _texture.height, angle, dynamic, restitution), physics, _listener, type)
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

class Chain : public PhysicEntity
{
public:
	Chain(ModulePhysics* physics, int _x, int _y, int* points, int size, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f, bool reverse = false)
		: PhysicEntity(physics->CreateChain(_x - _texture.width / 2, _y - _texture.height / 2, points, size, angle, dynamic, restitution, reverse), physics, _listener, type)
		, texture(_texture)
	{
		body->type = type;
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x + texture.width / 2, (float)y + texture.height / 2, (float)texture.width, (float)texture.height },
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

public:

};

class ObstacleBox : public Box
{
public:
	ObstacleBox(ModulePhysics* physics, int _x, int _y, float angle, Module* _listener, Texture2D _texture, float restitution, const char letter = NULL)
		: Box(physics, _x, _y, _listener, _texture, EntityType::OBSTACLE, angle, false, restitution)
	{
		if (letter != NULL) body->letter = letter;
	}

public:

};

class ObstacleCircle : public Circle
{
public:
	ObstacleCircle(ModulePhysics* physics, int _x, int _y, float angle, Module* _listener, Texture2D _texture, float restitution, const char letter = NULL)
		: Circle(physics, _x, _y, _listener, _texture, EntityType::OBSTACLE, angle, false, restitution)
	{
		if (letter != NULL) body->letter = letter;
	}

public:

};

class ObstacleChain : public Chain
{
public:
	ObstacleChain(ModulePhysics* physics, int _x, int _y, int* points, int size, float angle, Module* _listener, Texture2D _texture, float restitution, const char letter = NULL, bool reverse = false)
		: Chain(physics, _x, _y, points, size, _listener, _texture, EntityType::OBSTACLE, angle, false, restitution, reverse)
	{
		if (letter != NULL) body->letter = letter;
	}

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
	//App->audio->PlayMusic("Assets/music/gameMusic.ogg", 2.0f);

	// Textures
	ball_t = LoadTexture("Assets/Textures/Pelota_8X.png");
	wall_t = LoadTexture("Assets/Textures/wall.png");
	flipper_left_t = LoadTexture("Assets/Textures/Alas_Izquierda.png");
	flipper_right_t = LoadTexture("Assets/Textures/Alas_Derecha.png");
	obstaculoDerecha_t = LoadTexture("Assets/Textures/Obstaculo_Derecha.png");
	obstaculoIzquierda_t = LoadTexture("Assets/Textures/Obstaculo_Izquierda.png");
	obstaculo1_t = LoadTexture("Assets/Textures/Obstaculo1.png");
	obstaculo2_t = LoadTexture("Assets/Textures/Obstaculo2.png");
	rebotaDragon_t = LoadTexture("Assets/Textures/Rebota_Dragon_.png");
	rebotaFuego_t = LoadTexture("Assets/Textures/Rebota_Fuego.png");
	rebotaHielo_t = LoadTexture("Assets/Textures/Rebota_Hielo.png");
	rebotaRayo_t = LoadTexture("Assets/Textures/Rebota_Rayo.png");
	fondo_t = LoadTexture("Assets/Textures/Pinball.png");

	// Audios
	flipperFX = App->audio->LoadFx("Assets/FX/flipper.wav") - 1;
	saque1FX = App->audio->LoadFx("Assets/FX/saque1.mp3") - 1;
	looseBallFX = App->audio->LoadFx("Assets/FX/hited.mp3") - 1;
	gameOverFX = App->audio->LoadFx("Assets/FX/game-over.wav") - 1;
	gameMusic = App->audio->LoadFx("Assets/music/gameMusic.wav") - 1;
	points1FX = App->audio->LoadFx("Assets/FX/points1.mp3") - 1;
	points2FX = App->audio->LoadFx("Assets/FX/points2.mp3") - 1;
	points3FX = App->audio->LoadFx("Assets/FX/points3.mp3") - 1;
	CreateMap();
	AddBalls(startingBalls);


	App->audio->PlayFx(gameMusic);
	

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");
	

	return true;
}

update_status ModuleGame::PreUpdate()
{
	int x = GetMouseX();
	int y = GetMouseY();
	b2Vec2 mousePos = b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	App->physics->mousePos = mousePos;

	return UPDATE_CONTINUE;
}

// Update: draw background
update_status ModuleGame::Update(double dt)
{
	Ball* ball = GetCurrentBall();

	if (IsKeyPressed(KEY_TAB)) {
		letters = requiredLetters;
	}

	if (!ballLaunched && IsKeyDown(KEY_DOWN)) {
		GetCurrentBall()->body->ApplyImpulse(0.f, -10.f);
		App->audio->PlayFx(saque1FX);
		ballLaunched = true;
	}

	// Flippers
	if (IsKeyDown(KEY_LEFT)) {
		flipperLeft->body->ApplyImpulse(0.f, -5.f);
		App->audio->PlayFx(flipperFX);
	}

	if (IsKeyDown(KEY_RIGHT)) {
		flipperRight->body->ApplyImpulse(0.f, -5.f);
		App->audio->PlayFx(flipperFX);
	}

	if (CheckBonus()) {
		int newBalls = balls.size();
		Ball* b;
		for (int i = 0; i < balls.size(); i++) {
			if (balls[i]->ballNum == currentBall) b = balls[i];
			else delete balls[i];
		}
		balls.clear();
		balls.push_back(b);
		AddBalls(newBalls, currentBall + 1);
	}

	if (IsKeyPressed(KEY_F1)) {
		App->physics->ToggleDebug(ball->body->body);
	}

	if (IsKeyPressed(KEY_C)) {
		App->physics->TogglePhysics();
		LOG("Physics toggled");
	}

	if (IsKeyPressed(KEY_X)) {
		App->physics->ChangeGravity(true);
		LOG("Gravity increased by 1 m/s^2");
	}

	if (IsKeyPressed(KEY_Z)) {
		App->physics->ChangeGravity(false);
		LOG("Gravity decreased by 1 m/s^2");
	}

	if (IsKeyPressed(KEY_M)) {
		App->physics->ChangeRestitution(ball->body->body, true);
		LOG("Restitution coefficient of ball increased by 0.1");
	}

	if (IsKeyPressed(KEY_N)) {
		App->physics->ChangeRestitution(ball->body->body, false);
		LOG("Restitution coefficient of ball decreased by 0.1");
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

update_status ModuleGame::PostUpdate()
{
	App->renderer->DrawText(TextFormat("Best Score:%2i", bestScore), 25, 250, GetFontDefault(), 40, 5, GOLD);
	App->renderer->DrawText(TextFormat("Score:%2i", score), 25, 290, GetFontDefault(), 50, 5, PURPLE);
	App->renderer->DrawText(TextFormat("Previous Score: %2i", prevScore), 25, 340, GetFontDefault(), 20, 3, RED);
	App->renderer->DrawText(TextFormat("Games Played: %2i", gamesPlayed), 25, 370, GetFontDefault(), 20, 3, BLACK);

	if (lost) {
		lost = false;
		Lose();
	}
	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyA->type == EntityType::BALL && bodyB->type == EntityType::DEATHZONE) {
		delete GetCurrentBall();
		balls.erase(balls.begin());
		currentBall++;
		ballLaunched = false;
		App->audio->PlayFx(looseBallFX);
		if (balls.size() == 0) lost = true;
	}
	else if (bodyA->type == EntityType::BALL && bodyB->type == EntityType::OBSTACLE) {
		if (bodyB->letter != NULL) { 
			letters.push_back(bodyB->letter);

			if (bodyB->letter == 'a') {
				score = score + 50;
				App->audio->PlayFx(points3FX);
			}
			if (bodyB->letter == 'b') {
				score = score + 30;
				App->audio->PlayFx(points2FX);
			}
			if (bodyB->letter == 'c') {
				score = score + 10;
				App->audio->PlayFx(points1FX);
			}
			if (bodyB->letter == 'd') {
				score = score + 40;
				App->audio->PlayFx(points2FX);
				
			}
		
		}
	}
}

void ModuleGame::CreateMap()
{
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	
	// deathzone
	entities.emplace_back(new BoxSensor(App->physics, screenWidth / 2 + 170, screenHeight + 150, screenWidth, 300, this, EntityType::DEATHZONE, 0, false));
	//entities.emplace_back(new BoxSensor(App->physics, screenWidth / 2 + 170, -300, screenWidth, 300, this, EntityType::DEATHZONE, 0, false));
	//entities.emplace_back(new BoxSensor(App->physics, screenWidth / 2, screenHeight, 300, screenHeight, this, EntityType::DEATHZONE, 0, false));
	//entities.emplace_back(new BoxSensor(App->physics, screenWidth + 320, 0, 300, screenHeight, this, EntityType::DEATHZONE, 0, false));
	
	// background
	entities.emplace_back(new Chain(App->physics, screenWidth / 2, screenHeight / 2, Pinball, 194, this, fondo_t, EntityType::WALL, 0.f, false));
	
	/*Box* flipperWallLeft = new Box(App->physics, screenWidth / 2 - 105, screenHeight - 120, this, wall_t, EntityType::WALL, 30, false);
	Box* flipperWallRight = new Box(App->physics, screenWidth / 2 + 42, screenHeight - 120, this, wall_t, EntityType::WALL, 330, false);
	entities.emplace_back(flipperWallRight);
	entities.emplace_back(flipperWallLeft);

	b2Vec2 localAnchorLeft(PIXEL_TO_METERS(flipperWallLeft->body->width / 2), 0.f);
	b2Vec2 localAnchorRight(-PIXEL_TO_METERS(flipperWallRight->body->width / 2), 0.f);
	b2Vec2 worldAnchorLeft = flipperWallLeft->body->body->GetWorldPoint(localAnchorLeft);
	b2Vec2 worldAnchorRight = flipperWallRight->body->body->GetWorldPoint(localAnchorRight);
	b2Vec2 worldPixelLeft(METERS_TO_PIXELS(worldAnchorLeft.x), METERS_TO_PIXELS(worldAnchorLeft.y));
	b2Vec2 worldPixelRight(METERS_TO_PIXELS(worldAnchorRight.x), METERS_TO_PIXELS(worldAnchorRight.y));

	flipperLeft = new Flipper(App->physics, this, flipper_left_t, flipperWallLeft, worldPixelLeft, true);
	flipperRight = new Flipper(App->physics, this, flipper_right_t, flipperWallRight, worldPixelRight, false);
	entities.emplace_back(flipperLeft);
	entities.emplace_back(flipperRight);*/

	// obstacles
	entities.emplace_back(new ObstacleCircle(App->physics, screenWidth / 2 + 210, screenHeight / 2 + 110, 0.f, this, rebotaDragon_t, .9f, 'a')); //pelotas
	entities.emplace_back(new ObstacleCircle(App->physics, screenWidth / 2 + 195, screenHeight / 2 - 20, 0.f, this, rebotaFuego_t, .9f, 'b'));
	entities.emplace_back(new ObstacleCircle(App->physics, screenWidth / 2 + 130, screenHeight / 2 + 30, 0.f, this, rebotaHielo_t, .9f, 'c'));
	entities.emplace_back(new ObstacleCircle(App->physics, screenWidth / 2 + 130, screenHeight / 2 - 70, 0.f, this, rebotaRayo_t, .9f, 'd')); //.
	entities.emplace_back(new Chain(App->physics, screenWidth / 2 + 60, screenHeight / 2 - 50, Obstaculo1, 66, this, obstaculo1_t, EntityType::WALL, 0.f, false, .0f, true)); //paredes
	entities.emplace_back(new Chain(App->physics, screenWidth / 2 + 130, screenHeight / 2 + 140, Obstaculo2, 10, this, obstaculo2_t, EntityType::WALL, 0.f, false, .0f, true));
	Chain* flipperWallRight = new Chain(App->physics, screenWidth / 2 + 225, screenHeight - 140, Obstaculo_Derecha, 14, this, obstaculoDerecha_t, EntityType::WALL, 0.f, false, .0f, true);
	Chain* flipperWallLeft = new Chain(App->physics, screenWidth / 2 + 35, screenHeight - 140, Obstaculo_Izquierda, 14, this, obstaculoIzquierda_t, EntityType::WALL, 0.f, false, 0.f, false);
	entities.emplace_back(flipperWallRight);
	entities.emplace_back(flipperWallLeft);

	// flippers
	flipperLeft = new Flipper(App->physics, this, flipper_left_t, flipperWallLeft, b2Vec2(487, 622), true);
	flipperRight = new Flipper(App->physics, this, flipper_right_t, flipperWallRight, b2Vec2(627, 622), false);
	entities.emplace_back(flipperLeft);
	entities.emplace_back(flipperRight);
}

void ModuleGame::AddBalls(int ballCount, int firstId)
{
	int y = 200;
	for (int i = 0; i < ballCount; i++)
	{
		balls.emplace_back(new Ball(App->physics, 760, GetScreenHeight() - y, this, ball_t, i + firstId));
		y -= 80;
	}
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
	for (int i = 0; i < requiredLetters.size(); i++) {
		if (std::find(letters.begin(), letters.end(), requiredLetters[i]) == letters.end()) return false;
	}
	letters = {};
	return true;
}

void ModuleGame::Lose()
{
	App->audio->PlayFx(gameOverFX);
	currentBall = 0;
	AddBalls(startingBalls);
	if (score > bestScore) bestScore = score;
	prevScore = score;
	score = 0;
	gamesPlayed++;
}
