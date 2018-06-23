#ifndef UNET_DEVICE_H
#define UNET_DEVICE_H

#include <string>

#include <string.h>
#include <unistd.h>

#include "base.h"
#include "error.h"
#include "buffer.h"
#include "eth.h"
#include "ip.h"
#include "arp.h"

namespace unet
{
	class device : private nocopy
	{
		std::string name;
		int fd = -1;
		uint32_t addr = 0;
		uint8_t hw[6];
		unet::arp arp;

		void move(device &src)
		{
			name = std::move(src.name);

			if (fd >= 0) { ::close(fd); }
			fd = src.fd;
			addr = src.addr;
			memcpy(hw, src.hw, sizeof(hw));

			src.fd = -1;
			src.addr = 0;
			memset(src.hw, 0, sizeof(src.hw));
		}

	public:
		device() {}
		~device() { close(); }

		device(device &&src) { move(src); }
		device &operator=(device &&src) { move(src); return *this; }

		std::error_code open(const char *addr, const char *route, const char *hwaddr, const char *name = "");
		void close();

		std::error_code loop_rx(eth &recvr);
		std::error_code read(buffer &buf);
		ssize_t write(const slice &buf);

		ssize_t transmit(buffer &buf, const uint8_t *dmac, eth_type type);

		const uint8_t *hwaddr() const { return hw; }
	};
}

#endif

