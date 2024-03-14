#include <posta/Network/NetworkPackage.h>
#include <posta/Util/LoggingMacro.h>
#include <bitset>
// Include htonl and others
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#if __BIG_ENDIAN__
# define htonll(x) (x)
# define ntohll(x) (x)
#else
# define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
# define ntohll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif

using posta::NetworkPackage;

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
void posta::operator<<(NetworkPackage::Writer& writer, const bool& value)
{
	writer.write(&value, 1);
}

void posta::operator<<(NetworkPackage::Writer& writer, const uint8_t& value)
{
	writer.write(&value, sizeof(value));
}

void posta::operator<<(NetworkPackage::Writer& writer, const uint16_t& value)
{
	uint16_t v = htons(value);
	writer.write(&v, sizeof(v));
}

void posta::operator<<(NetworkPackage::Writer& writer, const uint32_t& value)
{
	uint32_t v = htonl(value);
	writer.write(&v, sizeof(v));
}

void posta::operator<<(NetworkPackage::Writer& writer, const uint64_t& value)
{
	uint64_t v = htonll(value);
	writer.write(&v, sizeof(v));
}

void posta::operator<<(NetworkPackage::Writer& writer, const int8_t& value)
{
	writer.write(&value, sizeof(value));
}

void posta::operator<<(NetworkPackage::Writer& writer, const int16_t& value)
{
	uint16_t v = htons(reinterpret_cast<const uint16_t&>(value));
	writer.write(&v, sizeof(v));
}

void posta::operator<<(NetworkPackage::Writer& writer, const int32_t& value)
{
	uint32_t v = htonl(reinterpret_cast<const uint32_t&>(value));
	writer.write(&v, sizeof(v));
}

void posta::operator<<(NetworkPackage::Writer& writer, const int64_t& value)
{
	uint64_t v = htonl(reinterpret_cast<const uint64_t&>(value));
	writer.write(&v, sizeof(v));
}


void posta::operator<<(NetworkPackage::Writer& writer, const glm::quat& value)
{
	//writer.write(&value, sizeof(value));
	writer << value.x;
	writer << value.y;
	writer << value.z;
	writer << value.w;
}

void posta::operator<<(NetworkPackage::Writer& writer, const std::string& value) /// Assumes sizeof(char) == 1
{
	uint32_t size = value.size();
	writer << size;
	writer.write(value.data(), value.size());
}

void posta::operator<<(NetworkPackage::Writer& writer, const float& value)
{
	int _exp;
	float significand = frexp(value, &_exp);
	int8_t mexp = _exp;
	uint8_t exp = reinterpret_cast<uint8_t&>(mexp);

	int32_t integer = static_cast<int32_t>(significand * (1ull << 22));
	bool is_positive = integer >= 0;
	uint32_t uinteger = std::abs(integer);

	//           mantissa           is-positive                  exponent
	uint32_t v = (uinteger << 8) | (is_positive ? 0:1UL << 31) | exp;
	writer << v;
}

void posta::operator<<(NetworkPackage::Writer& writer, const double& value)
{
	int _exp;
	double significand = frexp(value, &_exp);
	int16_t mexp = _exp;
	uint16_t exp = reinterpret_cast<uint16_t&>(mexp);
	if (mexp < 0)
	{
		//LOG("this value ", value, " is mexp < 0, (", mexp, ")");
		exp &= ~(1ul << 15); // exp[15] = 0
		exp &= ~(1ul << 14); // exp[14] = 0
		exp &= ~(1ul << 13); // exp[13] = 0
		exp &= ~(1ul << 12); // exp[12] = 0
		exp &= ~(1ul << 11); // exp[11] = 0
		exp |=  (1ul << 10); // exp[10] = 1
		//LOG("final value: ", std::bitset<16>(exp).to_string());
	}

	int64_t integer = static_cast<int64_t>(significand * (1ull << 52));
	bool is_positive = integer >= 0;
	uint64_t uinteger = std::abs(integer);

	//           mantissa           is-positive                  exponent
	uint64_t v = (uinteger << 11) | (is_positive ? 0:static_cast<uint64_t>(1UL) << 63) | exp;
	writer << v;
}
void posta::operator<<(posta::NetworkPackage::Writer& writer, const glm::vec3& value)
{
	writer << static_cast<float>(value[0]);
	writer << static_cast<float>(value[1]);
	writer << static_cast<float>(value[2]);
}
void posta::operator<<(posta::NetworkPackage::Writer& writer, const glm::vec2& value)
{
	writer << value[0];
	writer << value[1];
}
void posta::operator<<(posta::NetworkPackage::Writer& writer, const glm::dvec3& value)
{
	writer << value[0];
	writer << value[1];
	writer << value[2];
}
void posta::operator<<(posta::NetworkPackage::Writer& writer, const glm::dvec2& value)
{
	writer << value[0];
	writer << value[1];
}

void posta::operator<<(NetworkPackage::Writer& writer, const std::vector<uint8_t>& value) // WARNING: Not tested
{
	uint32_t size = value.size();
	writer << size;
	writer.write(value.data(), size);
}

void posta::operator<<(NetworkPackage::Writer& writer, const std::vector<uint16_t>& value)
{
	uint32_t size = value.size();
	writer << size;
	for (auto& v : value)
		writer << v;
}

void posta::operator<<(NetworkPackage::Writer& writer, const std::vector<uint32_t>& value)
{
	uint32_t size = value.size();
	writer << size;
	for (auto& v : value)
		writer << v;
}
void posta::operator<<(NetworkPackage::Writer& writer, const std::vector<uint64_t>& value)
{
	uint32_t size = value.size();
	writer << size;
	for (auto& v : value)
		writer << v;
}

void posta::operator<<(posta::NetworkPackage::Writer& writer, const std::vector<float>& value)
{
	uint32_t size = value.size();
	writer << size;
	for (auto& v : value)
		writer << v;
}

// basic reading
void posta::operator>>(NetworkPackage::Writer& writer, bool& value)
{
	writer.read(&value, 1);
}

void posta::operator>>(NetworkPackage::Writer& writer, uint8_t& value)
{
	writer.read(&value, sizeof(value));
}

void posta::operator>>(NetworkPackage::Writer& writer, uint16_t& value)
{
	writer.read(&value, sizeof(value));
	value = ntohs(value);
}

void posta::operator>>(NetworkPackage::Writer& writer, uint32_t& value)
{
	writer.read(&value, sizeof(value));
	value = ntohl(value);
}
void posta::operator>>(NetworkPackage::Writer& writer, uint64_t& value)
{
	writer.read(&value, sizeof(value));
	value = ntohll(value);
}

void posta::operator>>(NetworkPackage::Writer& writer, int8_t& value)
{
	writer.read(&value, sizeof(value));
}

void posta::operator>>(NetworkPackage::Writer& writer, int16_t& value)
{
	writer.read(&value, sizeof(value));
	uint16_t* n = reinterpret_cast<uint16_t*>(&value);
	uint16_t nn = ntohs(*n);
	value = (*reinterpret_cast<int16_t*>(&nn));
}

void posta::operator>>(NetworkPackage::Writer& writer, int32_t& value)
{
	writer.read(&value, sizeof(value));
	uint32_t* n = reinterpret_cast<uint32_t*>(&value);
	uint32_t nn = ntohl(*n);
	value = (*reinterpret_cast<int32_t*>(&nn));
}

void posta::operator>>(NetworkPackage::Writer& writer, int64_t& value)
{
	writer.read(&value, sizeof(value));
	uint64_t* n = reinterpret_cast<uint64_t*>(&value);
	uint64_t nn = ntohl(*n);
	value = (*reinterpret_cast<int64_t*>(&nn));
}


void posta::operator>>(NetworkPackage::Writer& writer, glm::quat& value)
{
	//writer.read(&value, sizeof(value));
	writer >> value.x;
	writer >> value.y;
	writer >> value.z;
	writer >> value.w;
}

void posta::operator>>(NetworkPackage::Writer& writer, std::string& value) /// Assumes sizeof(char) == 1
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	writer.read(value.data(), size);
}

void posta::operator>>(NetworkPackage::Writer& writer, float& value)
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
}

void posta::operator>>(NetworkPackage::Writer& writer, double& value)
{
	uint64_t v;
	writer >> v;
	bool is_positive = !static_cast<bool>((v >> 63) & 1u);
	v &= ~(static_cast<uint64_t>(1ul) << 63);
	uint64_t uinteger = v >> 11;
	
	int64_t integer = uinteger;

	uint16_t mexp = (v << 53) >> 53;
	int exp = 0;
	if (mexp & (1ul << 10))
	{
		exp = static_cast<int16_t>(mexp) - 2048;
		//exp &= ~(1ul << 10); // exp[10] = 0
		//exp = -exp;
		//LOG("exp ", exp);
	}
	else
		exp = mexp;
	double significand = integer / static_cast<double>(1ull << 52);
	value = ldexp(significand, exp) * (is_positive ? 1:-1);
}
void posta::operator>>(posta::NetworkPackage::Writer& writer, glm::vec3& value)
{
	writer >> value[0];
	writer >> value[1];
	writer >> value[2];
}
void posta::operator>>(posta::NetworkPackage::Writer& writer, glm::vec2& value)
{
	writer >> value[0];
	writer >> value[1];
}
void posta::operator>>(posta::NetworkPackage::Writer& writer, glm::dvec3& value)
{
	writer >> value[0];
	writer >> value[1];
	writer >> value[2];
}
void posta::operator>>(posta::NetworkPackage::Writer& writer, glm::dvec2& value)
{
	writer >> value[0];
	writer >> value[1];
}

void posta::operator>>(NetworkPackage::Writer& writer, std::vector<uint8_t>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	writer.read(value.data(), sizeof(uint8_t) * size);
}

void posta::operator>>(NetworkPackage::Writer& writer, std::vector<uint16_t>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	
	for (size_t i = 0; i < size; i++)
		writer >> value[i];
}

void posta::operator>>(NetworkPackage::Writer& writer, std::vector<uint32_t>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	
	for (size_t i = 0; i < size; i++)
		writer >> value[i];
}

void posta::operator>>(NetworkPackage::Writer& writer, std::vector<uint64_t>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	
	for (size_t i = 0; i < size; i++)
		writer >> value[i];
}

void posta::operator>>(NetworkPackage::Writer& writer, std::vector<float>& value)
{
	uint32_t size;
	writer >> size;
	value.resize(size);
	
	for (size_t i = 0; i < size; i++)
		writer >> value[i];
}

/*
void posta::operator<<(posta::NetworkPackage::Writer& writer, const VirtualNetworkPackageKind* value)
{
	writer << value->which;
	value->write_value_to(writer);
}
*/

