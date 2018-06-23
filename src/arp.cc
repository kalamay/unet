#include "arp.h"
#include "buffer.h"
#include "host.h"
#include "fmt.h"
#include "eth.h"

#include <stdio.h>

using namespace unet;

bool arp_cache::add(const arp_hdr &hdr, const arp_ip &data)
{
	return add(data.sip, data.smac, static_cast<arphrd>(hdr.hwtype));
}

bool arp_cache::add(uint32_t ip, const uint8_t *mac, arphrd hwtype)
{
	auto it = set.emplace(hwtype, ip);
	it.first->setmac(mac);
	return it.second;
}

bool arp_cache::update(const arp_hdr &hdr, const arp_ip &data)
{
	return update(data.sip, data.smac, static_cast<arphrd>(hdr.hwtype));
}

bool arp_cache::update(uint32_t ip, const uint8_t *mac, arphrd hwtype)
{
	entry e(ip, hwtype);
	auto it = set.find(e);
	if (it != set.end()) {
		it->setmac(mac);
		return true;
	}
	return false;
}

const uint8_t *arp_cache::find(uint32_t ip, arphrd hwtype)
{
	entry e(ip, hwtype);
	const auto it = set.find(e);
	return it == set.end() && it->resolved ? nullptr : it->smac;
}

void arp::recv(const slice &val)
{
	const arp_hdr &hdr = val.as<arp_hdr>();
	fio::out() << hdr << fio::endl;
}

void arp::request(slice &val, uint32_t sip, const uint8_t *smac, uint32_t dip, const uint8_t *dmac)
{
	arp_hdr &arp = val.as<arp_hdr>();
	arp_ip &payload = *reinterpret_cast<arp_ip *>(arp.data);

	memcpy(payload.smac, smac, sizeof(payload.smac));
	payload.sip = hton32(sip);

	memcpy(payload.dmac, dmac, sizeof(payload.dmac));
	payload.dip = hton32(dip);

	arp.opcode = hton16(ARPOP_REQUEST);
	arp.hwtype = hton16(ARPHRD_ETHER);
	arp.protype = hton16(ETH_IP);
	arp.hwsize = 6;
	arp.prosize = 4;
}

const uint8_t *arp::find_hwaddr(uint32_t ip, arphrd hwtype)
{
	return cache.find(ip, hwtype);
}

const char *unet::arphrd_name(arphrd type)
{
	switch (type) {
#define X(name, val) case ARPHRD_##name: return #name;
		UNET_ARP_HARDWARE
#undef X
	}
	return "(unknown)";
}

const char *unet::arpproto_name(arpproto type)
{
	switch (type) {
#define X(name, val) case ARPPROTO_##name: return #name;
		UNET_ARP_PROTO
#undef X
	}
	return "(unknown)";
}

const char *unet::arpop_name(arpop type)
{
	switch (type) {
#define X(name, val) case ARPOP_##name: return #name;
		UNET_ARP_OPCODE
#undef X
	}
	return "(unknown)";
}

fio::ostream &operator<<(fio::ostream &os, const unet::arp_hdr &v)
{
	char buf[512];
	int len = snprintf(buf, sizeof(buf), "arp (hrd=%s pro=%s op=%s",
			unet::arphrd_name(static_cast<unet::arphrd>(ntoh16(v.hwtype))),
			unet::arpproto_name(static_cast<unet::arpproto>(ntoh16(v.protype))),
			unet::arpop_name(static_cast<unet::arpop>(ntoh16(v.opcode))));
	os.write(buf, len);
	if (v.protype == hton16(ARPPROTO_IP4)) {
		const arp_ip &d = *reinterpret_cast<const arp_ip *>(v.data);
		len = snprintf(buf, sizeof(buf), UNET_IP4_FMT "@" UNET_MAC_FMT "->" UNET_IP4_FMT "@" UNET_MAC_FMT,
				UNET_IP4_ARG(d.sip), UNET_MAC_ARG(d.smac),
				UNET_IP4_ARG(d.dip), UNET_MAC_ARG(d.dmac));
		os << ' ';
		os.write(buf, len);
	}
	return os << ')';
}

