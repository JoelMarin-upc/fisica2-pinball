
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleGame.h"

#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	renderer = new ModuleRender(this);
	audio = new ModuleAudio(this, true);
	physics = new ModulePhysics(this);
	scene_intro = new ModuleGame(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(physics);
	AddModule(audio);
	
	// Scenes
	AddModule(scene_intro);

	// Rendering happens at the end
	AddModule(renderer);
}

Application::~Application()
{
	for (auto it = list_modules.rbegin(); it != list_modules.rend(); ++it)
	{
		Module* item = *it;
		delete item;
	}
	list_modules.clear();
	
}

bool Application::Init()
{
	bool ret = true;

	SetTargetFPS(targetFPS);


	// Call Init() in all modules
	for (auto it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		Module* module = *it;
		ret = module->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");

	for (auto it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		Module* module = *it;
		ret = module->Start();
	}
	
	return ret;
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	// --- T-key toggle FPS ---
	if (IsKeyPressed(KEY_T))
	{
		targetFPS = (targetFPS == 60) ? 30 : 60;
		SetTargetFPS(targetFPS); // raylib caps FPS automatically
		LOG("Target FPS switched to %d", targetFPS);
	}

	// --- Delta time ---
	static double lastTime = GetTime(); // seconds
	double currentTime = GetTime();
	double dt = currentTime - lastTime; // seconds since last frame
	lastTime = currentTime;

	// --- Fixed timestep physics ---
	static double accumulator = 0.0;
	const float physicsStep = 1.0f / 60.0f; // 60 Hz fixed physics step
	accumulator += dt;
	while (accumulator >= physicsStep)
	{
		physics->Step(physicsStep);
		accumulator -= physicsStep;
	}

	// --- Module updates ---

	// PreUpdate
	for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
		if ((*it)->IsEnabled())
			ret = (*it)->PreUpdate();

	// Update
	for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
	{
		Module* module = *it;
		if (!module->IsEnabled()) continue;

		// Only pass dt to game module
		if (module == scene_intro)
			ret = module->Update(dt);
		else
			ret = module->Update();
	}

	// PostUpdate
	for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
		if ((*it)->IsEnabled())
			ret = (*it)->PostUpdate();

	if (WindowShouldClose())
		ret = UPDATE_STOP;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (auto it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
	{
		Module* item = *it;
		ret = item->CleanUp();
	}
	
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.emplace_back(mod);
}