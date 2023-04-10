#include "scenegraph.h"

namespace Engine
{
	public ref class Transformation
	{
	internal:
		engine::Transformation& m_transform;
	public:
		Transformation(engine::Transformation& transform)
			: m_transform(transform)
		{	}
		property float X { float get() { return m_transform.position.x; } void set(float value) { m_transform.position.x = value; } }
		property float Y { float get() { return m_transform.position.y; } void set(float value) { m_transform.position.y = value; } }
		property float Z { float get() { return m_transform.position.z; } void set(float value) { m_transform.position.z = value; } }
		property float RotX { float get() { return m_transform.rotation.x; } void set(float value) { m_transform.rotation.x = value; } }
		property float RotY { float get() { return m_transform.rotation.y; } void set(float value) { m_transform.rotation.y = value; } }
		property float RotZ { float get() { return m_transform.rotation.z; } void set(float value) { m_transform.rotation.z = value; } }
		property float ScaleX { float get() { return m_transform.scale.x; } void set(float value) { m_transform.scale.x = value; } }
		property float ScaleY { float get() { return m_transform.scale.y; } void set(float value) { m_transform.scale.y = value; } }
		property float ScaleZ { float get() { return m_transform.scale.z; } void set(float value) { m_transform.scale.z = value; } }
	};


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

		Transformation^ GetTransformation()
		{
			engine::Transformation& transform = m_gameObject.GetTransformation();
			return gcnew Transformation(transform);
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