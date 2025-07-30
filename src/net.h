#pragma once

#include <sys/types.h>
#include <stdint.h>

#define CLIENT_DISCONNECT -2

int send_message(int fd, size_t len, uint8_t *message);

/**
 * @brief  Retrive tcp messages by TLV protocol without type
 *
 * The protocol is, each message have length, and body.
 * If the first byte is less than 252 (0xFC) then the first byte is the length
 * If the first byte equal to 253 (0xFD) then the following 2 byte is the length
 * If the first byte equal to 254 (0xFE) then the following 4 byte is the length
 * 255 (0xFF) is reserved
 * 
 * ## example
 * ```
 * 0    1    2    3    4    5    6    7    8    9    A    ..   F
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 * | 02 | 7F | 28 | 01 | 01 | A0 | 00 | 3F | 9A | 90 | 45 | .. | F4 | .. |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 * Len  Dat  Dat  Len  Dat  Len  Dat  Dat  Dat  Dat  Dat  ..   Dat
 * ```
 * Above stream contains 3 messages:
 * `0x7F28` with length 0x02, `0x01` with length 0x01, and `0x003F9A9045..F4` with length 0xA0
 * 
 * ```
 * 0    1    2    3    4    5    6    7    8    9    A    ..   0113
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 * | FC | 01 | 10 | 01 | 01 | A0 | 00 | 3F | 9A | 90 | 45 | .. | F4 | .. |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 * Tag  Len  Len  Dat  Dat  Dat  Dat  Dat  Dat  Dat  Dat  ..   Dat
 * ```
 * Above stream contains 1 messages:
 * `0x0101A0003F9A9045..F4` with the length of `0x0110`
 *
 * @param fd socket file descriptor for the tcp connection
 * @param[out] out_message return owned uint8_t array pointer
 * @param[out] len return 1 byte length of the out_message
 */
int recv_message(int fd, size_t *len, uint8_t **out_message);