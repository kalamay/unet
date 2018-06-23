#include "device.h"
#include "fmt.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/if_tun.h>

using namespace unet;

#define cmd(...) __extension__ ({ \
	char buf[4096]; \
	snprintf(buf, sizeof(buf), __VA_ARGS__); \
	system(buf); \
})

static int if_route(const char *dev, const char *cidr)
{
	return cmd("ip route add dev %s %s", dev, cidr);
}

static int if_address(const char *dev, const char *cidr)
{
	return cmd("ip address add dev %s local %s", dev, cidr);
}

static int if_up(const char *dev)
{
	return cmd("ip link set dev %s up", dev);
}

std::error_code device::open(const char *a, const char *r, const char *hwa, const char *dev)
{
	uint32_t new_addr = 0;
	uint8_t new_hwaddr[6];
	int s = -1;
	std::error_code ec;

	if (inet_pton(AF_INET, a, &new_addr) != 1) {
		ec = error::invalid_ipaddr;
		goto done;
	}

	if (sscanf(hwa, UNET_MAC_FMT, UNET_MAC_ARG(&new_hwaddr)) != UNET_MAC_NARG) {
		ec = error::invalid_hwaddr;
		goto done;
	}

	if ((s = ::open("/dev/net/tun", O_RDWR)) < 0) {
		ec = std::error_code(errno, std::system_category());
		goto done;
	}

	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

	if (*dev) {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}

	if (ioctl(s, TUNSETIFF, (void *) &ifr) < 0) {
		ec = std::error_code(errno, std::system_category());
		goto done;
	}

	if (if_up(ifr.ifr_name) != 0) {
		//print_err("ERROR when setting up if\n");
		ec = std::error_code(errno, std::system_category());
		goto done;
	}

	if (if_route(ifr.ifr_name, r) != 0) {
		//print_err("ERROR when setting route for if\n");
		ec = std::error_code(errno, std::system_category());
		goto done;
	}

	if (if_address(ifr.ifr_name, a) != 0) {
		//print_err("ERROR when setting addr for if\n");
		ec = std::error_code(errno, std::system_category());
		goto done;
	}

	name.clear();
	name.append(ifr.ifr_name);
	std::swap(fd, s);
	addr = new_addr;
	memcpy(hw, new_hwaddr, sizeof(hw));

done:
	if (s >= 0) {
		while (::close(s) < 0 && errno == EINTR) {}
	}
	return ec;
}

void device::close()
{
	if (fd < 0) { return; }
	while (::close(fd) < 0 && errno == EINTR) {}
	name.clear();
	fd = -1;
	addr = 0;
	memset(hw, 0, sizeof(hw));
}

std::error_code device::read(buffer &buf)
{
	slice end = buf.end();
	ssize_t n = ::read(fd, end.value(), end.length());
	if (n < 0) { return std::error_code(errno, std::system_category()); }
	if (n == 0) { return std::error_code(EBADF, std::system_category()); }
	buf.bump((size_t)n);
	return std::error_code();
}

ssize_t device::write(const slice &buf)
{
	return ::write(fd, buf.value(), buf.length());
}

