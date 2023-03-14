#include "scenegraph.h"

namespace Engine
{
	public ref class GameObject
	{
	internal:
		engine::GameObject& m_gameObject;

		GameObject(engine::GameObject& obj)
			: m_gameObject(obj)
		{}
	public:
		GameObject^ CreateObject()
		{
			return gcnew GameObject(m_gameObject.CreateObject());
		}

		void Destroy()
		{
			m_gameObject.Destroy();
		}
	};

	public ref class Scene
	{
	internal:
		engine::Scene& m_scene;
	public:
		Scene()
			: m_scene(engine::CreateScene())
		{}

		GameObject^ CreateObject()
		{
			return gcnew GameObject(m_scene.CreateObject());
		}

		void Destroy()
		{
			m_scene.Destroy();
		}
	};
}