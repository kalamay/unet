#ifndef UNET_FMT_H
#define UNET_FMT_H

#define UNET_MAC_FMT "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"
#define UNET_MAC_ARG(m) m[0],m[1],m[2],m[3],m[4],m[5]
#define UNET_MAC_NARG 6

#define UNET_IP4_FMT "%hhu.%hhu.%hhu.%hhu"
#define UNET_IP4_ARG(m) m&0xff,(m>>8)&0xff,(m>>16)&0xff,(m>>24)&0xff
#define UNET_IP4_NARG 4

#endif

