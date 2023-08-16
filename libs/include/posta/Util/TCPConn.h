#ifndef POSTAENGINE_TCPCONN_UTILITY_H
#define POSTAENGINE_TCPCONN_UTILITY_H
//#include <posta/App.h>
#include <SDL2/SDL_net.h>
#include <posta/Util/span.h>
#include <posta/Util/NetworkPackage.h>
#include <array>

namespace posta {
	std::string ip_to_string(Uint32 ip);
	Uint32 string_to_ip(std::string s);

	class TCPConn
	{
	public:
		static constexpr size_t BUFFER_SIZE = 2048; // max bytes for a packet, including the size and the type of the packet
		TCPConn();
		TCPConn(TCPsocket socket);
		TCPConn(const TCPConn&) = delete;
		TCPConn operator=(const TCPConn&) = delete;
		~TCPConn();

		void set_socket(TCPsocket socket);
		TCPsocket get_socket();

		bool send(posta::NetworkPackage& package);
		bool send(uint32_t data_type, posta::span<uint8_t> data); // returns false in case of closed connection
		/// returns true only if an entire packet is received and then the data type is stored in data_type and the data
		/// in data
		bool recv(uint32_t& data_type, posta::span<uint8_t> data);
	private:
		std::array<uint8_t, BUFFER_SIZE> buffer;
		int buffer_cursor;

		TCPsocket _socket;
		SDLNet_SocketSet socket_set;
	};
}

#endif // POSTAENGINE_TCPCONN_UTILITY_H