#include <posta/Util/NetworkPackage.h>
// Include htonl and others
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

using Engine::NetworkPackage;

NetworkPackage::Writer::Writer(uint8_t* ptr)
{
	_ptr = ptr;
}

void NetworkPackage::Writer::write(const void* ptr, size_t size)
{
	memcpy(_ptr, ptr, size);
	_ptr += size;
}

void NetworkPackage::Writer::read(void* ptr, size_t size)
{
	memcpy(ptr, _ptr, size);
	_ptr += size;
}

// basic writing
void Engine::operator<<(NetworkPackage::Writer& writer, const uint8_t& value)
{
	writer.write(&value, sizeof(value));
}

void Engine::operator<<(NetworkPackage::Writer& writer, const uint16_t& value)
{
	uint16_t v = htons(value);
	writer.write(&v, sizeof(v));
}

void Engine::operator<<(NetworkPackage::Writer& writer, const uint32_t& value)
{
	uint32_t v = htonl(value);
	writer.write(&v, sizeof(v));
}

void Engine::operator<<(NetworkPackage::Writer& writer, const int8_t& value)
{
	writer.write(&value, sizeof(value));
}

void Engine::operator<<(NetworkPackage::Writer& writer, const int16_t& value)
{
	uint16_t v = htons(reinterpret_cast<const uint16_t&>(value));
	writer.write(&v, sizeof(v));
}

void Engine::operator<<(NetworkPackage::Writer& writer, const int32_t& value)
{
	uint32_t v = htonl(reinterpret_cast<const uint32_t&>(value));
	writer.write(&v, sizeof(v));
}

void Engine::operator<<(NetworkPackage::Writer& writer, const glm::vec3& value)
{
	//writer.write(&value, sizeof(value));
	writer << value.x;
	writer << value.y;
	writer << value.z;
}

void Engine::operator<<(NetworkPackage::Writer& writer, const glm::quat& value)
{
	//writer.write(&value, sizeof(value));
	writer << value.x;
	writer << value.y;
	writer << value.z;
	writer << value.w;
}

void Engine::operator<<(NetworkPackage::Writer& writer, const std::string& value) /// Assumes sizeof(char) == 1
{
	uint32_t size = value.size();
	writer << size;
	writer.write(value.data(), value.size());
}

void Engine::operator<<(NetworkPackage::Writer& writer, const float& value)
{
	int _exp;
	float significand = frexp(value, &_exp);
	int8_t mexp = _exp;
	uint8_t exp = reinterpret_cast<uint8_t&>(mexp);

	int32_t integer = static_cast<int32_t>(significand * pow(2, 22));
	bool is_positive = integer >= 0;
	uint32_t uinteger = abs(integer);

	//           mantissa           is-positive                  exponent
	uint32_t v = (uinteger << 8) | (is_positive ? 0:1UL << 31) | exp;
	writer << v;
}

void Engine::operator<<(NetworkPackage::Writer& writer, const std::vector<uint8_t>& value) // WARNING: Not tested
{
	uint32_t size = value.size();
	writer << size;
	writer.write(value.data(), size);
}

void Engine::operator<<(NetworkPackage::Writer& writer, const std::vector<uint16_t>& value)
{
	uint32_t size = value.size();
	writer << size;
	for (auto& v : value)
		writer << v;
}

void Engine::operator<<(NetworkPackage::Writer& writer, const std::vector<uint32_t>& value)
{
	uint32_t size = value.size();
	writer << size;
	for (auto& v : value)
		writer << v;
}

// basic reading
void Engine::operator>>(NetworkPackage::Writer& writer, uint8_t& value)
{
	writer.read(&value, sizeof(value));
}

void Engine::operator>>(NetworkPackage::Writer& writer, uint16_t& value)
{
	writer.read(&value, sizeof(value));
	value = ntohs(value);
}

void Engine::operator>>(NetworkPackage::Writer& writer, uint32_t& value)
{
	writer.read(&value, sizeof(value));
	value = ntohl(value);
}

void Engine::operator>>(NetworkPackage::Writer& writer, int8_t& value)
{
	writer.read(&value, sizeof(value));
}

void Engine::operator>>(NetworkPackage::Writer& writer, int16_t& value)
{
	writer.read(&value, sizeof(value));
	uint16_t* n = reinterpret_cast<uint16_t*>(&value);
	uint16_t nn = ntohs(*n);
	value = (*reinterpret_cast<int16_t*>(&nn));
}

void Engine::operator>>(NetworkPackage::Writer& writer, int32_t& value)
{
	writer.read(&value, sizeof(value));
	uint32_t* n = reinterpret_cast<uint32_t*>(&value);
	uint32_t nn = ntohl(*n);
	value = (*reinterpret_cast<int32_t*>(&nn));
}

void Engine::operator>>(NetworkPackage::Writer& writer, glm::vec3& value)
{
	//writer.read(&value, sizeof(value));
	writer >> value.x;
	writer >> value.y;
	writer >> value.z;
}

void Engine::operator>>(NetworkPackage::Writer& writer, glm::quat& value)
{
	//writer.read(&value, sizeof(value));
	writer >> value.x;
	writer >> value.y;
	writer >> value.z;
	writer >> value.w;
}

void Engine::operator>>(NetworkPackage::Writer& writer, std::string& value) /// Assumes sizeof(char) == 1
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	writer.read(value.data(), size);
}

void Engine::operator>>(NetworkPackage::Writer& writer, float& value)
{
	uint32_t v;
	writer >> v;
	bool is_positive = !static_cast<bool>((v >> 31) & 1u);
	v &= ~(1ul << 31);
	uint32_t uinteger = v >> 8;
	
	int32_t integer = uinteger;
	
	int8_t exp = (v << 24) >> 24;
	float significand = integer / static_cast<float>(pow(2, 22));
	value = ldexp(significand, exp) * (is_positive ? 1:-1);
	/*
	int _exp;
	float significand = frexp(value, &_exp);
	int8_t exp;
	exp = _exp;

	int32_t integer = static_cast<int32_t>(significand * pow(2, 22));
	bool is_positive = integer >= 0;
	uint32_t uinteger = abs(integer);

	//           mantissa          is-positive   exponent
	uint32_t v = (uinteger << 8) | (is_positive ? 1UL << 31:0) | exp;
	writer << v;
	*/
}

void Engine::operator>>(NetworkPackage::Writer& writer, std::vector<uint8_t>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	writer.read(value.data(), sizeof(uint8_t) * size);
}

void Engine::operator>>(NetworkPackage::Writer& writer, std::vector<uint16_t>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	
	for (size_t i = 0; i < size; i++)
		writer >> value[i];
}

void Engine::operator>>(NetworkPackage::Writer& writer, std::vector<uint32_t>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	
	for (size_t i = 0; i < size; i++)
		writer >> value[i];
}

