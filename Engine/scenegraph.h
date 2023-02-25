#pragma once
#include "components.h"
#include <DirectXMath.h>
#include <boost/serialization/access.hpp>

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
		friend class boost::serialization::access;
	public:
		GameObject();


		bool operator==(const GameObject& other) const;
		void Release() override;
		DirectX::XMMATRIX GetWorldMatrix() const override;


	private:
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & m_id;
			ar& m_id;
			ar& m_id;
			ar& m_id;
			ar& m_id;
			ar& m_id;
			ar& m_id;
			ar& m_id;
			ar& m_id;
			ar& m_id;
			ar& m_id;
		}

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
