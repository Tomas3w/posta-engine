#ifndef CONNECTION_H
#define CONNECTION_H
#include <engine/include/App.h>
#include <engine/include/Util/span.h>
#include <engine/include/Util/NetworkPackage.h>
#include <array>

std::string ip_to_string(Uint32 ip);
Uint32 string_to_ip(std::string s);

class Connection
{
public:
	static constexpr size_t BUFFER_SIZE = 2048; // max bytes for a packet, including the size and the type of the packet
	Connection();
	Connection(TCPsocket socket);
	Connection(const Connection&) = delete;
	Connection operator=(const Connection&) = delete;
	~Connection();

	void set_socket(TCPsocket socket);
	TCPsocket get_socket();

	bool send(Engine::NetworkPackage& package);
	bool send(uint32_t data_type, Engine::span<uint8_t> data); // returns false in case of closed connection
	/// returns true only if an entire packet is received and then the data type is stored in data_type and the data
	/// in data
	bool recv(uint32_t& data_type, Engine::span<uint8_t> data);
private:
	std::array<uint8_t, BUFFER_SIZE> buffer;
	int buffer_cursor;

	TCPsocket _socket;
	SDLNet_SocketSet socket_set;
};

#endif // CONNECTION_H
