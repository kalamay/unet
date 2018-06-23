#include "eth.h"
#include "fmt.h"
#include "host.h"

using namespace unet;

static const uint8_t broadcast_hw[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

const uint8_t *unet::broadcast_hwaddr = broadcast_hw;

const char *unet::eth_name(eth_type type)
{
	switch (type) {
#define X(name, val) case ETH_##name: return #name;
		UNET_ETH_TYPE
#undef X
	}
	return "(unknown)";
}

void eth::recv(const slice &buf)
{
	const eth_hdr &hdr = buf.as<eth_hdr>();
	if (hdr.has_type(ETH_ARP)) {
		_arp.recv(buf.trim_left(UNET_ETH_HLEN));
	}
#if 0
	else {
		fio::out() << hdr << fio::endl;
	}
#endif
}

fio::ostream &operator<<(fio::ostream &os, const eth_hdr &v)
{
	char buf[256];
	int len = snprintf(buf, sizeof(buf), "eth (%s " UNET_MAC_FMT "->" UNET_MAC_FMT ")",
			eth_name(static_cast<eth_type>(v.type())),
			UNET_MAC_ARG(v.smac),
			UNET_MAC_ARG(v.dmac));
	return os.write(buf, len);
}

