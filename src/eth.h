#ifndef UNET_ETH_H
#define UNET_ETH_H

#include <cstdint>

#include "fio/fio.h"
#include "base.h"
#include "slice.h"
#include "arp.h"
#include "ip.h"
#include "host.h"

#define UNET_ETH_ALEN       6    /* Octets in one ethernet addr */
#define UNET_ETH_HLEN       14   /* Total octets in header. */
#define UNET_ETH_ZLEN       60   /* Min. octets in frame sans FCS */
#define UNET_ETH_DATA_LEN   1500 /* Max. octets in payload */
#define UNET_ETH_FRAME_LEN  1514 /* Max. octets in frame sans FCS */
#define UNET_ETH_FCS_LEN    4    /* Octets in the FCS */

#define UNET_ETH_TYPE \
	X(AARP,        0x80F3) /* Appletalk AARP */ \
	X(AF_IUCV,     0xFBFB) /* IBM af_iucv */ \
	X(ALL,         0x0003) /* Every packet */ \
	X(AOE,         0x88A2) /* ATA over Ethernet */ \
	X(ARCNET,      0x001A) /* 1A for ArcNet */ \
	X(ARP,         0x0806) /* Address Resolution packet */ \
	X(ATALK,       0x809B) /* Appletalk DDP */ \
	X(ATMFATE,     0x8884) /* Frame-based ATM Transport over Ethernet */ \
	X(ATMMPOA,     0x884c) /* MultiProtocol Over ATM */ \
	X(AX25,        0x0002) /* Dummy protocol id for AX.25 */ \
	X(BATMAN,      0x4305) /* B.A.T.M.A.N.-Advanced packet */ \
	X(BPQ,         0x08FF) /* G8BPQ AX.25 Ethernet Packet */ \
	X(CAIF,        0x00F7) /* ST-Ericsson CAIF protocol */ \
	X(CAN,         0x000C) /* CAN: Controller Area Network */ \
	X(CANFD,       0x000D) /* CANFD: CAN flexible data rat */ \
	X(CONTROL,     0x0016) /* Card specific control frames */ \
	X(CUST,        0x6006) /* DEC Customer use */ \
	X(DDCMP,       0x0006) /* DEC DDCMP: Internal only */ \
	X(DEC,         0x6000) /* DEC Assigned proto */ \
	X(DIAG,        0x6005) /* DEC Diagnostics */ \
	X(DNA_DL,      0x6001) /* DEC DNA Dump/Load */ \
	X(DNA_RC,      0x6002) /* DEC DNA Remote Console */ \
	X(DNA_RT,      0x6003) /* DEC DNA Routing */ \
	X(DSA,         0x001B) /* Distributed Switch Arch */ \
	X(ECONET,      0x0018) /* Acorn Econet */ \
	X(EDSA,        0xDADA) /* Ethertype DSA */ \
	X(FCOE,        0x8906) /* Fibre Channel over Ethernet */ \
	X(FIP,         0x8914) /* FCoE Initialization Protocol */ \
	X(HDLC,        0x0019) /* HDLC frames */ \
	X(IEEE1588,    0x88F7) /* IEEE 1588 Timesync */ \
	X(IEEE802154,  0x00F6) /* IEEE802.15.4 frame */ \
	X(IEEE8021AD,  0x88A8) /* 802.1ad Service VLAN */ \
	X(IEEE8021AH,  0x88E7) /* 802.1ah Backbone Service Tag */ \
	X(IEEE8021Q,   0x8100) /* 802.1Q VLAN Extended Header */ \
	X(IEEE80221,   0x8917) /* IEEE 802.21 Media Independent Handover Protocol */ \
	X(IEEE802_2,   0x0004) /* 802.2 frames */ \
	X(IEEE802_3,   0x0001) /* Dummy type for 802.3 frames */ \
	X(IEEE802_EX1, 0x88B5) /* 802.1 Local Experimental 1 */ \
	X(IEEEPUP,     0x0a00) /* Xerox IEEE802.3 PUP packet */ \
	X(IEEEPUPAT,   0x0a01) /* Xerox IEEE802.3 PUP Addr Trans packet */ \
	X(IP,          0x0800) /* Internet Protocol packet */ \
	X(IPV6,        0x86DD) /* IPv6 over bluebook */ \
	X(IPX,         0x8137) /* IPX over DIX */ \
	X(IRDA,        0x0017) /* Linux-IrDA */ \
	X(LAT,         0x6004) /* DEC LAT */ \
	X(LINK_CTL,    0x886c) /* HPNA, wlan link local tunnel */ \
	X(LOCALTALK,   0x0009) /* Localtalk pseudo type */ \
	X(LOOP,        0x0060) /* Ethernet Loopback packet */ \
	X(LOOPBACK,    0x9000) /* Ethernet loopback packet, per IEEE 802.3 */ \
	X(MOBITEX,     0x0015) /* Mobitex */ \
	X(MPLS_MC,     0x8848) /* MPLS Multicast traffic */ \
	X(MPLS_UC,     0x8847) /* MPLS Unicast traffic */ \
	X(MVRP,        0x88F5) /* 802.1Q MVRP */ \
	X(PAE,         0x888E) /* Port Access Entity (IEEE 802.1X) */ \
	X(PAUSE,       0x8808) /* IEEE Pause frames (IEEE 802.3 31B) */ \
	X(PHONET,      0x00F5) /* Nokia Phonet frames */ \
	X(PPPTALK,     0x0010) /* Dummy type for Atalk over PP */ \
	X(PPP_DISC,    0x8863) /* PPPoE discovery messages */ \
	X(PPP_MP,      0x0008) /* Dummy type for PPP MP frames */ \
	X(PPP_SES,     0x8864) /* PPPoE session messages */ \
	X(PRP,         0x88FB) /* IEC 62439-3 PRP/HSRv0 */ \
	X(PUP,         0x0200) /* Xerox PUP packet */ \
	X(PUPAT,       0x0201) /* Xerox PUP Addr Trans packet */ \
	X(QINQ1,       0x9100) /* deprecated QinQ VLAN */ \
	X(QINQ2,       0x9200) /* deprecated QinQ VLAN */ \
	X(QINQ3,       0x9300) /* deprecated QinQ VLAN */ \
	X(RARP,        0x8035) /* Reverse Addr Res packet */ \
	X(SCA,         0x6007) /* DEC Systems Comms Arch */ \
	X(SLOW,        0x8809) /* Slow Protocol (IEEE 802.3ad 43B) */ \
	X(SNAP,        0x0005) /* Internal only */ \
	X(TDLS,        0x890D) /* TDLS */ \
	X(TEB,         0x6558) /* Trans Ether Bridging */ \
	X(TIPC,        0x88CA) /* TIPC */ \
	X(TRAILER,     0x001C) /* Trailer switch tagging */ \
	X(TR_802_2,    0x0011) /* 802.2 frames */ \
	X(TSN,         0x22F0) /* TSN (IEEE 1722) packet */ \
	X(WAN_PPP,     0x0007) /* Dummy type for WAN PPP frame */ \
	X(WCCP,        0x883E) /* Web-cache coordination protocol */ \
	X(X25,         0x0805) /* CCITT X.25 */ \
	X(XDSA,        0x00F8) /* Multiplexed DSA protocol */

namespace unet
{
	extern const uint8_t *broadcast_hwaddr;

	enum eth_type : uint16_t
	{
#define X(name, val) ETH_##name = val,
		UNET_ETH_TYPE
#undef X
	};

	const char *eth_name(eth_type type);

	struct eth_hdr : private nocopy, private nomove
	{
		uint8_t  dmac[6];
		uint8_t  smac[6];
		uint16_t _type;
		uint8_t  payload[0];

		constexpr uint16_t type() const { return ntoh16(_type); }
		constexpr bool has_type(uint16_t val) const { return _type == hton16(val); }
		void set_type(uint16_t val) { _type = hton16(val); }
	} __attribute__((packed));

	class eth
	{
		unet::arp _arp;
		unet::ip _ip;
	public:
		void recv(const slice &buf);

		unet::arp &arp() { return _arp; }
		unet::ip &ip() { return _ip; }
	};

	static_assert(sizeof(eth_hdr) == UNET_ETH_HLEN, "eth_hdr size invalid");
};

fio::ostream &operator<<(fio::ostream &os, const unet::eth_hdr &v);

#endif

