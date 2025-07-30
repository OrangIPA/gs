#pragma once

#include <sys/types.h>
#include <stdint.h>

#define CLIENT_DISCONNECT -2

int send_message(int fd, uint8_t *message, size_t len);

/**
 * @brief  Retrive tcp messages by TLV protocol without type
 *
 * The protocol is, each message have length, and body.
 * The first bytes is the length and the rest is the body.
 *
 * since max value of 8 bit uint is 255 (0xFF), message length can't exceed 248 (0xF8)
 * bytes 248 (0xF8) and above is reserved for futureproofing (i'm thinking of doing it utf-8 style)
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
 * @param fd socket file descriptor for the tcp connection
 * @param[out] out_message return owned uint8_t array pointer
 * @param[out] len return 1 byte length of the out_message
 */
int recv_message(int fd, size_t *len, uint8_t **out_message);