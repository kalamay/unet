#include <iostream>
#include <err.h>

#include "device.h"

int
main(void)
{
	unet::device dev;
	std::error_code ec;

	ec = dev.open("10.0.0.4", "10.0.0.0/24", "00:0c:29:6d:50:25");
	if (ec) {
		fio::err() << "failed to open device: " << ec << fio::endl;
		return 1;
	}

	unet::eth eth;
	ec = dev.loop_rx(eth);
	if (ec) {
		fio::err() << "failed to read from device: " << ec << fio::endl;
		return 1;
	}
}

