#pragma once
#include "components.h"
#include "utils.h"
#include <DirectXMath.h>

namespace engine
{
	class ParentBase;
	class GameObject;
	class Scene;

	class ENGINE_API ParentBase
	{
	public:
		virtual GameObject& CreateObject() = 0;
		virtual void Destroy() = 0;
		virtual DirectX::XMMATRIX GetWorldMatrix() const = 0;
		virtual void _DetachChild(GameObject* object) = 0;

	protected:
		
		std::vector<GameObject*> m_children;
	};

	class ENGINE_API GameObject : public ParentBase
	{
	public:
		[[nodiscard]] virtual Transformation& GetTransformation() const = 0;

	private:

	};

	class ENGINE_API Scene : public ParentBase
	{
	public:
		static Scene& FromBinary(uint8_t* bin);
	private:

	};

	ENGINE_API Scene& CreateScene();
}
