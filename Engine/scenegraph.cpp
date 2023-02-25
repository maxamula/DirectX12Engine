#include "scenegraph.h"
#include <entt/entt.hpp>


namespace engine
{
	namespace
	{
		entt::registry g_registry;
	}

	GameObject::GameObject()
	{
		m_id = (uint32_t)g_registry.create();
		g_registry.emplace<Transformation>(entt::entity(m_id));
	}

	DirectX::XMMATRIX GameObject::GetWorldMatrix() const
	{
		return DirectX::XMMatrixIdentity();
	}

	bool GameObject::operator==(const GameObject& other) const
	{
		return m_id == other.m_id;
	}

	void GameObject::Release()
	{
		this->ParentBase::Release();
		g_registry.destroy(entt::entity(m_id));
	}

	DirectX::XMMATRIX Scene::GetWorldMatrix() const
	{
		return DirectX::XMMatrixIdentity();
	}

	void Scene::Release()
	{
		ParentBase::Release();
	}

	GameObject& ParentBase::AddChildObject()
	{
		m_children.emplace_back();
		return m_children.back();
	}

	void ParentBase::RemoveChildObject(GameObject& object)
	{
		auto it = std::find(m_children.begin(), m_children.end(), object);
		if (it != m_children.end())
		{
			m_children.erase(it);
		}
	}

	void ParentBase::Release()
	{
		// Release all children
		for (auto& child : m_children)
			child.Release();
		m_children.clear();
	}
}