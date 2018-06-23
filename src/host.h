#ifndef UNET_HOST_H
#define UNET_HOST_H

#include <arpa/inet.h>

namespace unet
{
	template <typename T>
	constexpr uint16_t hton16(T h) { return htons(static_cast<uint16_t>(h)); }

	template <typename T>
	constexpr uint16_t ntoh16(T n) { return ntohs(static_cast<uint16_t>(n)); }

	template <typename T>
	constexpr uint32_t hton32(T h) { return htonl(static_cast<uint32_t>(h)); }

	template <typename T>
	constexpr uint32_t ntoh32(T n) { return ntohl(static_cast<uint32_t>(n)); }
}

#endif

