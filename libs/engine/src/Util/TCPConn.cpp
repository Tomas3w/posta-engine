#include <engine/include/Util/TCPConn.h>
// Include htonl and others
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

using Engine::TCPConn;

std::string Engine::ip_to_string(Uint32 ip)
{
	return std::to_string((ip << 24) >> 24) + "." + std::to_string((ip << 16) >> 24) + "." + std::to_string((ip << 8) >> 24) + "." + std::to_string(ip >> 24);
}

Uint32 Engine::string_to_ip(std::string s)
{
	Uint32 ip = 0;
	s += '.';
	char* i = s.data();
	short n = 24;
	for (char& c : s)
	{
		if (c == '.')
		{
			c = '\0';
			ip |= std::stoi(i) << n;
			c = '.';
			n -= 8;
			i = (&c) + 1;
		}
	}
	return htonl(ip);
}

TCPConn::TCPConn()
{
	_socket = nullptr;
	socket_set = nullptr;
	buffer_cursor = 0;
}

TCPConn::TCPConn(TCPsocket socket)
{
	set_socket(socket);
	buffer_cursor = 0;
}

TCPConn::~TCPConn()
{
	if (_socket != nullptr)
	{
		SDLNet_FreeSocketSet(socket_set);
		SDLNet_TCP_Close(_socket);
	}
}

void TCPConn::set_socket(TCPsocket socket)
{
	if (_socket != nullptr)
	{
		SDLNet_FreeSocketSet(socket_set);
		SDLNet_TCP_Close(_socket);
	}
	socket_set = SDLNet_AllocSocketSet(1);
	_socket = socket;
	SDLNet_TCP_AddSocket(socket_set, _socket);
}

TCPsocket TCPConn::get_socket()
{
	return _socket;
}

bool TCPConn::send(Engine::NetworkPackage& package)
{
	return send(package.get_type(), package.serialize());
}

bool TCPConn::send(uint32_t data_type, Engine::span<uint8_t> data)
{
	uint32_t data_size = data.size();
	uint32_t n_data_size = htonl(data_size);
	uint32_t n_data_type = htonl(data_type);
	// send data_size
	int bytes_sent = SDLNet_TCP_Send(_socket, &n_data_size, sizeof(n_data_size));
	if (bytes_sent < static_cast<int>(sizeof(n_data_size)))
		return false;
	// send data_type
	bytes_sent = SDLNet_TCP_Send(_socket, &n_data_type, sizeof(n_data_type));
	if (bytes_sent < static_cast<int>(sizeof(n_data_type)))
		return false;
	// send data
	bytes_sent = SDLNet_TCP_Send(_socket, data.data(), data.size());
	if (bytes_sent < static_cast<int>(data.size()))
		return false;
	return true;
}

bool TCPConn::recv(uint32_t& data_type, Engine::span<uint8_t> data)
{
	if (SDLNet_CheckSockets(socket_set, 0) > 0)
	{
		// buffering the recieved data
		int bytes_recv = SDLNet_TCP_Recv(_socket, &buffer[buffer_cursor], std::min(buffer.size() - buffer_cursor, data.size()));
		buffer_cursor += bytes_recv;
	}

	if (buffer_cursor < static_cast<int>(sizeof(uint32_t))) // checking that the package size has been gathered
	{
		if (buffer_cursor < 0)
			buffer_cursor = 0;
		return false;
	}
	// checking that the package received is complete
	uint32_t expected_data_size = ntohl(reinterpret_cast<uint32_t&>(*buffer.data()));
	if (buffer_cursor < static_cast<int>(expected_data_size + sizeof(uint32_t)))
		return false;
	// copying data to data_type and data
	memcpy (data.data(), buffer.data() + sizeof(uint32_t) * 2, expected_data_size);
	memcpy (&data_type , buffer.data() + sizeof(uint32_t)    , sizeof(uint32_t));
	data_type = ntohl(data_type);
	// moving buffer back to start
	uint32_t expected_message_size = expected_data_size + sizeof(uint32_t) * 2;
	memmove(buffer.data(), buffer.data() + expected_message_size, buffer.size() - expected_message_size);
	buffer_cursor -= expected_message_size;
	return true;
}

