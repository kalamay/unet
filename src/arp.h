#ifndef UNET_ARP_H
#define UNET_ARP_H

#include <set>
#include <cstring>
#include <cstdint>

#include "fio/fio.h"
#include "base.h"
#include "slice.h"

#define UNET_ARP_HLEN       8    /* Total octets in header. */
#define UNET_ARP_DLEN       20   /* Total octets in IPv4 data. */

#define UNET_ARP_HARDWARE \
	X(NETROM,     0)  /* from KA9Q: NET/ROM pseudo */ \
	X(ETHER,      1)  /* Ethernet 10Mbps */ \
	X(EETHER,     2)  /* Experimental Ethernet */ \
	X(AX25,       3)  /* AX.25 Level 2 */ \
	X(PRONET,     4)  /* PROnet token ring */ \
	X(CHAOS,      5)  /* Chaosnet */ \
	X(IEEE802,    6)  /* IEEE 802.2 Ethernet/TR/TB */ \
	X(ARCNET,     7)  /* ARCnet */ \
	X(APPLETLK,   8)  /* APPLEtalk */ \
	X(DLCI,       15) /* Frame Relay DLCI */ \
	X(ATM,        19) /* ATM */ \
	X(METRICOM,   23) /* Metricom STRIP (new IANA id) */ \
	X(IEEE1394,   24) /* IEEE 1394 IPv4 - RFC 2734 */ \
	X(EUI64,      27) /* EUI-64 */ \
	X(INFINIBAND, 32) /* InfiniBand */ \

#define UNET_ARP_PROTO \
	X(IP4,        0x800)

#define UNET_ARP_OPCODE \
	X(REQUEST,    1)  /* ARP request */ \
	X(REPLY,      2)  /* ARP reply */ \
	X(RREQUEST,   3)  /* RARP request */ \
	X(RREPLY,     4)  /* RARP reply */ \
	X(IN_REQUEST, 8)  /* InARP request */ \
	X(IN_REPLY,   9)  /* InARP reply */ \
	X(NAK,        10) /* (ATM)ARP NAK */ \

namespace unet
{
	enum arphrd : uint16_t
	{
#define X(name, val) ARPHRD_##name = val,
		UNET_ARP_HARDWARE
#undef X
	};

	enum arpproto : uint16_t
	{
#define X(name, val) ARPPROTO_##name = val,
		UNET_ARP_PROTO
#undef X
	};

	enum arpop : uint16_t
	{
#define X(name, val) ARPOP_##name = val,
		UNET_ARP_OPCODE
#undef X
	};

	const char *arphrd_name(arphrd type);
	const char *arpproto_name(arpproto type);
	const char *arpop_name(arpop type);

	struct arp_hdr : private nocopy, private nomove
	{
		uint16_t hwtype;
		uint16_t protype;
		uint8_t  hwsize;
		uint8_t  prosize;
		uint16_t opcode;
		uint8_t  data[0];
	} __attribute__((packed));

	struct arp_ip
	{
		uint8_t  smac[6];
		uint32_t sip;
		uint8_t  dmac[6];
		uint32_t dip;
	} __attribute__((packed));

	class arp_cache
	{
		struct entry
		{
			uint32_t sip;
			uint16_t hwtype;
			mutable uint8_t smac[6];
			mutable bool resolved;

			entry(uint32_t sip, uint16_t hwtype = ARPHRD_ETHER)
				: sip(sip), hwtype(hwtype), resolved(false) {}

			void setmac(const uint8_t *mac) const
			{
				memcpy(smac, mac, sizeof(smac));
				resolved = true;
			}

			bool operator<(const entry &other) const
			{
				return hwtype < other.hwtype || (hwtype == other.hwtype && sip < other.sip);
			}

			bool operator==(const entry &other) const
			{
				return hwtype == other.hwtype && sip == other.sip;
			}
		};

		std::set<entry> set;

	public:
		bool add(const arp_hdr &hdr, const arp_ip &data);
		bool add(uint32_t ip, const uint8_t *mac, arphrd hwtype = ARPHRD_ETHER);
		bool update(const arp_hdr &hdr, const arp_ip &data);
		bool update(uint32_t ip, const uint8_t *mac, arphrd hwtype = ARPHRD_ETHER);
		const uint8_t *find(uint32_t ip, arphrd hwtype = ARPHRD_ETHER);
	};

	class arp
	{
		arp_cache cache;

	public:
		void recv(const slice &val);

		void request(slice &val, uint32_t sip, const uint8_t *smac, uint32_t dip, const uint8_t *dmac);
		const uint8_t *find_hwaddr(uint32_t ip, arphrd hwtype = ARPHRD_ETHER);
	};

	static_assert(sizeof(arp_hdr) == UNET_ARP_HLEN, "arp_hdr size invalid");
	static_assert(sizeof(arp_ip) == UNET_ARP_DLEN, "arp_ip size invalid");
};

fio::ostream &operator<<(fio::ostream &os, const unet::arp_hdr &v);

#endif

