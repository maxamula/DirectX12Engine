#pragma once

using namespace engine;

REGISTER_SCRIPT(TestScript);

class TestScript : public IScript
{
public:
	TestScript(GameObject& obj)
		: IScript(obj)
	{}

	// Called once at the start of the game
	void Begin() override
	{
		
	}

	// Called every frame
	void Update(float fElapsedTime) override
	{
		
	} 
};