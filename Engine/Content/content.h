#pragma once

namespace engine::content
{
	enum ASSET_TYPE
	{
		UNKNOWN,
		ANIMATION,
		AUDIO,
		MATERIAL,
		MESH,
		SKELETON,
		TEXTURE,
		COUNT
	};

	enum PRIMITIVE_TOPOLOGY
	{

	};

	class IResource
	{
	public:
		virtual ~IResource() = default;

		virtual void Load(const void* const data) = 0;
		virtual void Release() = 0;
	};

	class GeometryResource : public IResource
	{
	public:
		GeometryResource() = default;
		~GeometryResource() = default;

		void Load(const void* const data) override;
		void Release() override;
	private:
		uint64_t m_hande;
	};

	uint64_t CreateResourse(const void* const data, ASSET_TYPE type);
	void ReleaseResourse(uint64_t handle);
}

