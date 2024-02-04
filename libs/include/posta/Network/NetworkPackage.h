#ifndef POSTAENGINE_NETWORKPACKAGE_UTILITY_H
#define POSTAENGINE_NETWORKPACKAGE_UTILITY_H
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <posta/Util/span.h>
#include <vector>
#include <string>
#include <fstream>

namespace posta {
	namespace NetworkPackageTypeSize
	{
		constexpr inline uint32_t size(const uint8_t value) { return sizeof(uint8_t); }
		constexpr inline uint32_t size(const uint16_t value) { return sizeof(uint16_t); }
		constexpr inline uint32_t size(const uint32_t value) { return sizeof(uint32_t); }
		constexpr inline uint32_t size(const uint64_t value) { return sizeof(uint64_t); }
		constexpr inline uint32_t size(const int8_t value) { return sizeof(int8_t); }
		constexpr inline uint32_t size(const int16_t value) { return sizeof(int16_t); }
		constexpr inline uint32_t size(const int32_t value) { return sizeof(int32_t); }
		constexpr inline uint32_t size(const int64_t value) { return sizeof(int64_t); }
		//template <const size_t VECSIZE, class T>
		//constexpr inline uint32_t size(const glm::vec<VECSIZE, T, glm::packed_highp> value) { return sizeof(glm::vec<VECSIZE, T, glm::packed_highp>); }
		
		constexpr inline uint32_t size(const glm::vec3 value) { return sizeof(glm::vec3); }
		constexpr inline uint32_t size(const glm::vec2 value) { return sizeof(glm::vec2); }
		constexpr inline uint32_t size(const glm::vec1 value) { return sizeof(glm::vec1); }
		constexpr inline uint32_t size(const glm::dvec3 value) { return sizeof(glm::dvec3); }
		constexpr inline uint32_t size(const glm::dvec2 value) { return sizeof(glm::dvec2); }
		constexpr inline uint32_t size(const glm::dvec1 value) { return sizeof(glm::dvec1); }
		constexpr inline uint32_t size(const glm::quat value) { return sizeof(glm::quat); }
		inline uint32_t size(const std::string& value) { return sizeof(uint32_t) + value.size(); }
		constexpr inline uint32_t size(const float& value) { return sizeof(uint32_t); }
		constexpr inline uint32_t size(const double& value) { return sizeof(uint64_t); }
		template<class T>
		inline uint32_t size(const std::vector<T>& value)
		{
			uint32_t _size = sizeof(uint32_t);
			for (auto& e : value)
				_size += size(e);
			return _size;
		}
		template<> inline uint32_t size<uint8_t>(const std::vector<uint8_t>& value) { return sizeof(uint32_t) + value.size() * sizeof(uint8_t); }
		template<> inline uint32_t size<uint16_t>(const std::vector<uint16_t>& value) { return sizeof(uint32_t) + value.size() * sizeof(uint16_t); }
		template<> inline uint32_t size<uint32_t>(const std::vector<uint32_t>& value) { return sizeof(uint32_t) + value.size() * sizeof(uint32_t); }
		template<> inline uint32_t size<uint64_t>(const std::vector<uint64_t>& value) { return sizeof(uint32_t) + value.size() * sizeof(uint64_t); }
		template<> inline uint32_t size<float>(const std::vector<float>& value) { return sizeof(uint32_t) + value.size() * size((float)0); }

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
		virtual ~NetworkPackage() = default;

		virtual uint32_t get_type() = 0; // should return the type with a unique id
		/// Serializes the data and returns a reference to it
		virtual posta::span<uint8_t> serialize() = 0;
		/// Deserializes the data given as argument, make sure that the data is of the corresponding type before trying to deserialize it
		virtual void deserialize(posta::span<uint8_t>) = 0;
		/// Writes type and data to an ostream, the type is garantueed to be an uint32_t. To retrieve the data read the sizeof(uint32_t) bytes to first find out the type
		virtual posta::span<uint8_t> serialize(std::ostream& output) final
		{
			auto data = serialize();
			uint32_t _type = get_type();
			output.write(reinterpret_cast<const char*>(&_type), sizeof(uint32_t));
			output.write(reinterpret_cast<const char*>(data.data()), data.size());
			return data;
		}

		std::vector<uint8_t> data; 
	};

/// basic writing
void operator<<(posta::NetworkPackage::Writer& writer, const uint8_t& value);
void operator<<(posta::NetworkPackage::Writer& writer, const uint16_t& value);
void operator<<(posta::NetworkPackage::Writer& writer, const uint32_t& value);
void operator<<(posta::NetworkPackage::Writer& writer, const uint64_t& value);
void operator<<(posta::NetworkPackage::Writer& writer, const int8_t& value);
void operator<<(posta::NetworkPackage::Writer& writer, const int16_t& value);
void operator<<(posta::NetworkPackage::Writer& writer, const int32_t& value);
void operator<<(posta::NetworkPackage::Writer& writer, const int64_t& value);
void operator<<(posta::NetworkPackage::Writer& writer, const float& value);
void operator<<(posta::NetworkPackage::Writer& writer, const double& value);
void operator<<(posta::NetworkPackage::Writer& writer, const glm::vec3& value);
void operator<<(posta::NetworkPackage::Writer& writer, const glm::vec2& value);
void operator<<(posta::NetworkPackage::Writer& writer, const glm::dvec3& value);
void operator<<(posta::NetworkPackage::Writer& writer, const glm::dvec2& value);
//void operator<<(posta::NetworkPackage::Writer& writer, const glm::vec3& value);
void operator<<(posta::NetworkPackage::Writer& writer, const glm::quat& value);
void operator<<(posta::NetworkPackage::Writer& writer, const std::string& value); /// Assumes sizeof(char) == 1
void operator<<(posta::NetworkPackage::Writer& writer, const std::vector<uint8_t>& value);
void operator<<(posta::NetworkPackage::Writer& writer, const std::vector<uint16_t>& value);
void operator<<(posta::NetworkPackage::Writer& writer, const std::vector<uint32_t>& value);
void operator<<(posta::NetworkPackage::Writer& writer, const std::vector<uint64_t>& value);
void operator<<(posta::NetworkPackage::Writer& writer, const std::vector<float>& value);
template<class T>
void operator<<(posta::NetworkPackage::Writer& writer, const std::vector<T>& value)
{
	uint32_t size = value.size();
	writer << size;
	for (auto& v : value)
		writer << v;
}

/// basic reading
void operator>>(posta::NetworkPackage::Writer& writer, uint8_t& value);
void operator>>(posta::NetworkPackage::Writer& writer, uint16_t& value);
void operator>>(posta::NetworkPackage::Writer& writer, uint32_t& value);
void operator>>(posta::NetworkPackage::Writer& writer, uint64_t& value);
void operator>>(posta::NetworkPackage::Writer& writer, int8_t& value);
void operator>>(posta::NetworkPackage::Writer& writer, int16_t& value);
void operator>>(posta::NetworkPackage::Writer& writer, int32_t& value);
void operator>>(posta::NetworkPackage::Writer& writer, int64_t& value);
void operator>>(posta::NetworkPackage::Writer& writer, float& value);
void operator>>(posta::NetworkPackage::Writer& writer, double& value);
void operator>>(posta::NetworkPackage::Writer& writer, glm::vec3& value);
void operator>>(posta::NetworkPackage::Writer& writer, glm::vec2& value);
void operator>>(posta::NetworkPackage::Writer& writer, glm::dvec3& value);
void operator>>(posta::NetworkPackage::Writer& writer, glm::dvec2& value);
//void operator>>(posta::NetworkPackage::Writer& writer, glm::vec3& value);
void operator>>(posta::NetworkPackage::Writer& writer, glm::quat& value);
void operator>>(posta::NetworkPackage::Writer& writer, std::string& value); /// Assumes sizeof(char) == 1
void operator>>(posta::NetworkPackage::Writer& writer, std::vector<uint8_t>& value);
void operator>>(posta::NetworkPackage::Writer& writer, std::vector<uint16_t>& value);
void operator>>(posta::NetworkPackage::Writer& writer, std::vector<uint32_t>& value);
void operator>>(posta::NetworkPackage::Writer& writer, std::vector<uint64_t>& value);
void operator>>(posta::NetworkPackage::Writer& writer, std::vector<float>& value);
template<class T>
void operator>>(posta::NetworkPackage::Writer& writer, std::vector<T>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	
	for (size_t i = 0; i < size; i++)
		writer >> value[i];
}

	template <const uint32_t PACKAGE_TYPE, class T, class... Ts>
	class NetworkPackageTemplate : public NetworkPackage
	{
	public:

		constexpr NetworkPackageTemplate(T& t, Ts&... ts) : value(t), rest(ts...) {}

		constexpr uint32_t size()
		{
			return NetworkPackageTypeSize::size(value) + rest.size();
		}

		uint32_t get_type() override { return PACKAGE_TYPE; }

		using NetworkPackage::serialize;
		posta::span<uint8_t> serialize() override
		{
			data.resize(this->size());
			Writer writer(data.data());
			this->write_value_to(writer);
			return posta::make_span(data);
		}

		constexpr void write_value_to(Writer& writer)
		{
			writer << value;
			rest.write_value_to(writer);
		}

		void deserialize(posta::span<uint8_t> _data) override
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
		uint32_t get_type() override { return PACKAGE_TYPE; }

		using NetworkPackage::serialize;
		posta::span<uint8_t> serialize() override
		{
			data.resize(this->size());
			Writer writer(data.data());
			this->write_value_to(writer);
			return posta::make_span(data);
		}
		void deserialize(posta::span<uint8_t> _data) override
		{
			Writer writer(_data.data());
			this->read_value_from(writer);
		}
	private:
		T& value;
	};
}

#endif // POSTAENGINE_NETWORKPACKAGE_UTILITY_H
