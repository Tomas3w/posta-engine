#ifndef POSTAENGINE_NETWORKPACKAGE_UTILITY_H
#define POSTAENGINE_NETWORKPACKAGE_UTILITY_H
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <engine/include/Util/span.h>
#include <vector>
#include <string>

namespace Engine {
	namespace NetworkPackageTypeSize
	{
		constexpr inline uint32_t size(const uint8_t value) { return sizeof(uint8_t); }
		constexpr inline uint32_t size(const uint16_t value) { return sizeof(uint16_t); }
		constexpr inline uint32_t size(const uint32_t value) { return sizeof(uint32_t); }
		constexpr inline uint32_t size(const int8_t value) { return sizeof(int8_t); }
		constexpr inline uint32_t size(const int16_t value) { return sizeof(int16_t); }
		constexpr inline uint32_t size(const int32_t value) { return sizeof(int32_t); }
		constexpr inline uint32_t size(const glm::vec3 value) { return sizeof(glm::vec3); }
		constexpr inline uint32_t size(const glm::quat value) { return sizeof(glm::quat); }
		inline uint32_t size(const std::string& value) { return sizeof(uint32_t) + value.size(); }
		constexpr inline uint32_t size(const float& value) { return sizeof(uint32_t); }
		inline uint32_t size(const std::vector<uint8_t>& value) { return sizeof(uint32_t) + value.size() * sizeof(uint8_t); }
		inline uint32_t size(const std::vector<uint16_t>& value) { return sizeof(uint32_t) + value.size() * sizeof(uint16_t); }
		inline uint32_t size(const std::vector<uint32_t>& value) { return sizeof(uint32_t) + value.size() * sizeof(uint32_t); }

		template<class T>
		constexpr inline uint32_t size_many(const T& value)
		{
			return size(value);
		}

		template<class T, class... Args>
		constexpr inline uint32_t size_many(const T& value, const Args&... args)
		{
			uint32_t v = size(value);
			return v + size_many(args...);
		}
	}

	class NetworkPackage
	{
	public:
		/// Writes bytes to and from the network
		class Writer
		{
		public:
			Writer(uint8_t* ptr);
			void write(const void* ptr, size_t size);
			void read(void* ptr, size_t size);

			template<class T>
			constexpr inline void write_many(const T& value)
			{
				(*this) << value;
			}

			template<class T, class... Args>
			constexpr inline void write_many(const T& value, const Args&... args)
			{
				write_many(value);
				write_many(args...);
			}

			template<class T>
			constexpr inline void read_many(T& value)
			{
				(*this) >> value;
			}

			template<class T, class... Args>
			constexpr inline void read_many(T& value, Args&... args)
			{
				read_many(value);
				read_many(args...);
			}
		private:
			uint8_t* _ptr;
		};

		virtual uint32_t get_type() = 0; // should return the type with a unique id
		virtual Engine::span<uint8_t> serialize() = 0;
		virtual void deserialize(Engine::span<uint8_t>) = 0;

		std::vector<uint8_t> data; 
	};

/// basic writing
void operator<<(Engine::NetworkPackage::Writer& writer, const uint8_t& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const uint16_t& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const uint32_t& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const int8_t& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const int16_t& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const int32_t& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const glm::vec3& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const glm::quat& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const std::string& value); /// Assumes sizeof(char) == 1
void operator<<(Engine::NetworkPackage::Writer& writer, const float& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const std::vector<uint8_t>& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const std::vector<uint16_t>& value);
void operator<<(Engine::NetworkPackage::Writer& writer, const std::vector<uint32_t>& value);

/// basic reading
void operator>>(Engine::NetworkPackage::Writer& writer, uint8_t& value);
void operator>>(Engine::NetworkPackage::Writer& writer, uint16_t& value);
void operator>>(Engine::NetworkPackage::Writer& writer, uint32_t& value);
void operator>>(Engine::NetworkPackage::Writer& writer, int8_t& value);
void operator>>(Engine::NetworkPackage::Writer& writer, int16_t& value);
void operator>>(Engine::NetworkPackage::Writer& writer, int32_t& value);
void operator>>(Engine::NetworkPackage::Writer& writer, glm::vec3& value);
void operator>>(Engine::NetworkPackage::Writer& writer, glm::quat& value);
void operator>>(Engine::NetworkPackage::Writer& writer, std::string& value); /// Assumes sizeof(char) == 1
void operator>>(Engine::NetworkPackage::Writer& writer, float& value);
void operator>>(Engine::NetworkPackage::Writer& writer, std::vector<uint8_t>& value);
void operator>>(Engine::NetworkPackage::Writer& writer, std::vector<uint16_t>& value);
void operator>>(Engine::NetworkPackage::Writer& writer, std::vector<uint32_t>& value);

	template <const uint32_t PACKAGE_TYPE, class T, class... Ts>
	class NetworkPackageTemplate : public NetworkPackage
	{
	public:
		constexpr NetworkPackageTemplate(T& t, Ts&... ts) : value(t), rest(ts...) {}

		constexpr uint32_t size()
		{
			return NetworkPackageTypeSize::size(value) + rest.size();
		}

		uint32_t get_type() { return PACKAGE_TYPE; }
		Engine::span<uint8_t> serialize()
		{
			data.resize(this->size());
			Writer writer(data.data());
			this->write_value_to(writer);
			return Engine::make_span(data);
		}

		constexpr void write_value_to(Writer& writer)
		{
			writer << value;
			rest.write_value_to(writer);
		}

		void deserialize(Engine::span<uint8_t> _data)
		{
			Writer writer(_data.data());
			this->read_value_from(writer);
		}

		constexpr void read_value_from(Writer& writer)
		{
			writer >> value;
			rest.read_value_from(writer);
		}

	private:
		T& value;
		NetworkPackageTemplate<PACKAGE_TYPE, Ts...> rest;
	};

	template <const uint32_t PACKAGE_TYPE, class T>
	class NetworkPackageTemplate<PACKAGE_TYPE, T> : public NetworkPackage
	{
	public:
		constexpr NetworkPackageTemplate(T& t) : value(t) {}

		constexpr uint32_t size()
		{
			return NetworkPackageTypeSize::size(value);
		}
		constexpr void write_value_to(NetworkPackage::Writer& writer)
		{
			writer << value;
		}
		constexpr void read_value_from(NetworkPackage::Writer& writer)
		{
			writer >> value;
		}
		uint32_t get_type() { return PACKAGE_TYPE; }
		Engine::span<uint8_t> serialize()
		{
			data.resize(this->size());
			Writer writer(data.data());
			this->write_value_to(writer);
			return Engine::make_span(data);
		}
		void deserialize(Engine::span<uint8_t> _data)
		{
			Writer writer(_data.data());
			this->read_value_from(writer);
		}
	private:
		T& value;
	};
}

#endif // POSTAENGINE_NETWORKPACKAGE_UTILITY_H
