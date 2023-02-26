#pragma once
#include "components.h"
#include <DirectXMath.h>

namespace engine
{
	class ParentBase;
	class GameObject;
	class Scene;

	class ParentBase
	{
	public:
		GameObject& AddChildObject();
		void RemoveChildObject(GameObject& object);
		virtual DirectX::XMMATRIX GetWorldMatrix() const = 0;
		virtual void Release();

	private:
		std::list<GameObject> m_children;
	};

	class GameObject : public ParentBase
	{
		friend class Scene;
	public:
		GameObject();


		bool operator==(const GameObject& other) const;
		void Release() override;
		DirectX::XMMATRIX GetWorldMatrix() const override;


	private:
		uint32_t m_id;
	};

	class Scene : public ParentBase
	{
	public:
		Scene() = default;
		void Release() override;
		DirectX::XMMATRIX GetWorldMatrix() const override;
	private:
		
	};
}
