#include "scenegraph.h"
#include <entt/entt.hpp>


namespace engine
{
	class GameObjectImpl;
	class SceneImpl;

	class ENGINE_API GameObjectImpl : public GameObject
	{
		friend class SceneImpl;
	public:
		GameObject& CreateObject() override
		{
			GameObject* child = new GameObjectImpl(m_reg, *this);
			m_children.push_back(child);
			return *child;
		}

		void Destroy() override
		{
			// destroy all children
			for (GameObject* obj : m_children)
			{
				obj->Destroy();
			}
			// destroy from registry and remove from parent
			m_parent._DetachChild(this);
			// delete current object
			delete this;
		}

		// removes entity from ierarhy and from registry but does not delete it
		void _DetachChild(GameObject* object) override
		{
			auto it = std::find(m_children.begin(), m_children.end(), object);
			if (it != m_children.end())
			{
				m_children.erase(it);
				m_reg.destroy(dynamic_cast<GameObjectImpl*>(object)->m_id);
			}
		}

		DirectX::XMMATRIX GetWorldMatrix() const override
		{
			return DirectX::XMMatrixIdentity();
		}
	private:
		GameObjectImpl(entt::registry& reg, ParentBase& parent)
			: m_parent(parent), m_reg(reg)
		{
			m_id = m_reg.create();
			m_reg.emplace<Transformation>(m_id);
		}
		
		ParentBase& m_parent;		// will never change
		entt::registry& m_reg;
		entt::entity m_id;
	};

	class ENGINE_API SceneImpl : public Scene
	{
	public:
		SceneImpl() = default;
		GameObject& CreateObject() override
		{
			GameObject* child = new GameObjectImpl(m_registry, *this);
			m_children.push_back(child);
			return *child;
		}

		void Destroy() override
		{
			// destroy all children
			for (GameObject* obj : m_children)
			{
				obj->Destroy();
			}		
			// delete current object
			delete this;
		}

		void _DetachChild(GameObject* object) override
		{
			auto it = std::find(m_children.begin(), m_children.end(), object);
			if (it != m_children.end())
			{
				m_children.erase(it);
				m_registry.destroy(dynamic_cast<GameObjectImpl*>(object)->m_id);
			}
		}

		DirectX::XMMATRIX GetWorldMatrix() const override
		{
			return DirectX::XMMatrixIdentity();
		}

	private:
		entt::registry m_registry;
	};

	Scene& CreateScene()
	{
		return *new SceneImpl();
	}
}