#pragma once
#include "window.h"

namespace engine
{
	class Window;
	class Application
	{
		public:
		Application();
		virtual ~Application();

		void Run();
		Window& CreateRenderWindow();

		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual void Shutdown() = 0;

		//static Application* GetInstance() { return s_instance; }
	};
	void Init();
}

