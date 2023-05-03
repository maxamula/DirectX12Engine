#pragma once
#include <vector>
#include <functional>
#include <mutex>
#include <stack>
#include <map>

namespace engine::utils
{
	class BinaryReader
	{
	public:
		BinaryReader(void* data)
			: m_data((uint8_t*)data)
		{}

		inline void* CurrentReadPos()
		{
			return &m_data[m_offset];
		}

		inline void Skip(uint64_t offset)
		{
			m_offset += offset;
		}

		template <typename T>
		T ReadBytes()
		{
			T data = *(T*)(m_data + m_offset);
			m_offset += sizeof(T);
			return data;
		}

		std::string ReadString(uint32_t count)
		{
			std::string data = std::string((char*)(m_data + m_offset), count);
			m_offset += count;
			return data;
		}
		inline bool ReadBool(){ return ReadBytes<bool>(); }
		inline double ReadDouble(){ return ReadBytes<double>(); }
		inline float ReadFloat(){ return ReadBytes<float>(); }
		inline int64_t ReadInt64(){ return ReadBytes<int64_t>(); }
		inline int32_t ReadInt32(){ return ReadBytes<int32_t>(); }
		inline int16_t ReadInt16(){ return ReadBytes<int16_t>(); }
		inline int8_t ReadInt8(){ return ReadBytes<int8_t>(); }
		inline uint64_t ReadUInt64(){ return ReadBytes<uint64_t>(); }
		inline uint32_t ReadUInt32(){ return ReadBytes<uint32_t>(); }
		inline uint16_t ReadUInt16(){ return ReadBytes<uint16_t>(); }
		inline uint8_t ReadUInt8(){ return ReadBytes<uint8_t>(); }

		inline void SetOffset(uint64_t offset){ m_offset = offset; }
		inline uint64_t GetOffset()
		{
			return m_offset;
		}
	private:
		uint8_t* m_data = nullptr;
		uint64_t m_offset = 0;	
	};

	class BinaryWriter
	{
	public:
		BinaryWriter(uint64_t size)
			: m_data(new uint8_t[size]), m_detached(false)
		{}
		BinaryWriter(void* data)
			: m_data((uint8_t*)data), m_detached(true)
		{}
		~BinaryWriter()
		{
			if (!m_detached)
				delete[] m_data;
		}

		template <typename T>
		void WriteBytes(T data)
		{
			*(T*)(m_data + m_offset) = data;
			m_offset += sizeof(T);
		}

		void WriteString(std::string data)
		{
			memcpy(m_data + m_offset, data.c_str(), data.size());
			m_offset += data.size();
		}
		inline void WriteBool(bool data) { WriteBytes(data); }
		inline void WriteDouble(double data) { WriteBytes(data); }
		inline void WriteFloat(float data) { WriteBytes(data); }
		inline void WriteInt64(int64_t data) { WriteBytes(data); }
		inline void WriteInt32(int32_t data) { WriteBytes(data); }
		inline void WriteInt16(int16_t data) { WriteBytes(data); }
		inline void WriteInt8(int8_t data) { WriteBytes(data); }
		inline void WriteUInt64(uint64_t data) { WriteBytes(data); }
		inline void WriteUInt32(uint32_t data) { WriteBytes(data); }
		inline void WriteUInt16(uint16_t data) { WriteBytes(data); }
		inline void WriteUInt8(uint8_t data) { WriteBytes(data); }
		inline void WriteNull(uint64_t count) { m_offset += count; }
		inline void Detach() { m_detached = true; }
		inline void SetOffset(uint64_t offset)
		{
			m_offset = offset;
		}
		inline uint64_t GetOffset()
		{
			return m_offset;
		}
	private:
		uint8_t* m_data = nullptr;
		uint64_t m_offset = 0;
		bool m_detached = false;
		uint8_t m_pad[3]{};
	};

	template<typename ...Args>
	class Event
	{
	public:
		typedef std::function<void(Args...)> EventCallback;

		void operator+=(const EventCallback& callback)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_callbacks.emplace_back(callback);
		}

		void operator -= (const EventCallback& callback)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_callbacks.erase(std::remove(m_callbacks.begin(), m_callbacks.end(), callback), m_callbacks.end());
		}

		void FireOnce(const EventCallback& callback)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_callbacksOnce.push(callbck);
		}

		void operator()(Args... args)
		{
			std::lock_guard<std::mutex> lock(m_mutex);	
			for (auto& callback : m_callbacks)
				callback(args...);
			while (!m_callbacksOnce.empty())
			{
				m_callbacksOnce.top()(args...);
				m_callbacksOnce.pop();
			}
		}

		void Clear()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_callbacks.clear();
		}

	private:
		std::vector<EventCallback> m_callbacks;
		std::stack<EventCallback> m_callbacksOnce;
		std::mutex m_mutex;
	};


	template<typename T>
	class ItemManager 
	{
	public:
		int Add(const T& item)
		{
			uint64_t id;
			if (m_freelist.empty()) 
			{
				id = m_nextId++;
			}
			else 
			{
				id = m_freelist.top();
				m_freelist.pop();
			}
			m_items[id] = item;
			return id;
		}

		void Remove(const int id) 
		{
			m_items.erase(id);
			m_freelist.push(id);
		}

		const T& Get(const int id) const 
		{
			auto it = m_items.find(id); 
			if (it != m_items.end())
				return it->second;
			else
				throw new std::exception("Item not found");
		}

	private:
		uint64_t m_nextId = 1;
		std::stack<uint64_t> m_freelist;
		std::map<uint64_t, T> m_items;
	};
}