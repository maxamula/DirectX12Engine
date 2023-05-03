#include "scenegraph.h"
#include <entt/entt.hpp>
#include "utils.h"
#include "script.h"


namespace engine
{
	class GameObjectImpl;
	class SceneImpl;

	class ENGINE_API GameObjectImpl : public GameObject
	{
		friend class SceneImpl;
		friend Scene& Scene::FromBinary(uint8_t* bin);
	public:
		GameObject& CreateObject() override
		{
			GameObjectImpl* child = new GameObjectImpl(m_reg, *this);
			m_children.push_back(child);
			//LOG_DEBUG("dsfdfsfd");
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

		[[nodiscard]] DirectX::XMMATRIX GetWorldMatrix() const override
		{
			return GetTransformation().GetMatrix() * m_parent.GetWorldMatrix();
		}

		// components

		[[nodiscard]] Transformation& GetTransformation() const override
		{
			return m_reg.get<Transformation>(m_id);
		}

		void SetScript(ScriptPtr script) const 
		{
			m_reg.emplace_or_replace<Script>(m_id, std::move(script));
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
		friend Scene& Scene::FromBinary(uint8_t* bin);
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
			while (!m_children.empty())
			{
				m_children.back()->Destroy();
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
			else
			{
				throw std::runtime_error("Object not found in scene");
			}
		}

		[[nodiscard]] DirectX::XMMATRIX GetWorldMatrix() const override
		{
			return DirectX::XMMatrixIdentity();
		}

	private:
		entt::registry m_registry;
	};

	Scene& Scene::FromBinary(uint8_t* bin)
	{
		utils::BinaryReader br(bin);
		SceneImpl* scene = new SceneImpl();
		const uint32_t objectCount = br.ReadUInt32();
		for (uint32_t i = 0; i < objectCount; i++)
		{
			const uint16_t componentCount = br.ReadUInt16();
			GameObjectImpl& obj = dynamic_cast<GameObjectImpl&>(scene->CreateObject());
			for (uint32_t j = 0; j < componentCount; i++)
			{
				const COMPONENT_TYPE::type type = static_cast<COMPONENT_TYPE::type>(br.ReadUInt16());
				switch (type)
				{
				case COMPONENT_TYPE::TRANSFORM:
				{
					Transformation& t = obj.GetTransformation();
					t.position = { br.ReadFloat(), br.ReadFloat(), br.ReadFloat() };
					t.rotation = { br.ReadFloat(), br.ReadFloat(), br.ReadFloat() };
					t.scale = { br.ReadFloat(), br.ReadFloat(), br.ReadFloat() };
					break;
				}
				case COMPONENT_TYPE::SCRIPT:
				{
					const uint64_t scriptId = br.ReadUInt64();
					ScriptPtr pScript = GetScriptTable()[scriptId].creator(obj);
					obj.SetScript(std::move(pScript));
					break;
				}
				}
			}
		}
		return *scene;
	}

	Scene& CreateScene()
	{
		return *new SceneImpl();
	}
}