#include "device.h"
#include "fmt.h"

using namespace unet;

std::error_code device::loop_rx(eth &recvr)
{
	for (;;) {
		auto *buf = unet::buffer::create(2024);
		auto ec = read(*buf);
		if (ec) {
			delete buf;
			return ec;
		}
		recvr.recv(buf->begin());
		delete buf;
	}
}

ssize_t device::transmit(buffer &buf, const uint8_t *dmac, eth_type type)
{
	slice val = buf.begin();
	eth_hdr &hdr = val.as<eth_hdr>();

	memcpy(hdr.dmac, dmac, sizeof(hdr.dmac));
	memcpy(hdr.smac, hw, sizeof(hdr.smac));
	hdr.set_type(type);

	return write(val);
}

