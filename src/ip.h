#ifndef UNET_IP_H
#define UNET_IP_H

#include <cstdint>

#include <set>
#include <cstring>

#define UNET_IP4_HLEN       20   /* Total octets in header. */

namespace unet
{
	struct ip4_hdr
	{
		uint8_t ver_ihl;
		uint8_t tos;
		uint16_t len;
		uint16_t id;
		uint16_t frag_off;
		uint8_t ttl;
		uint8_t proto;
		uint16_t check;
		uint32_t saddr;
		uint32_t daddr;
		uint8_t data[0];

		uint8_t ver() const { return ver_ihl >> 4; }
		uint8_t ihl() const { return ver_ihl | 0xf; }
	} __attribute__((packed));

	class ip
	{
	public:
		void recv(const slice &buf);
	};

	static_assert(sizeof(ip4_hdr) == UNET_IP4_HLEN, "ip4_hdr size invalid");
};

#endif

