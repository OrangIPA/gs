# Protocol - Draft 1
This document describe the communication protocol between client and server

## Summary
Client and server communicate using plain old TCP socket. Each messages between client and server is stateless, and sent with length prefix.
Even the client state initialization is no different than the rest of the messages.

## Message framing with length prefix
Each message contains length and body.
- If the first byte is less than 252 (0xFC), then, it's the length
- If the first byte is equal to 253 (0xFD), then the following 2 bytes is the length
- If the first byte is equal to 254 (0xFE), then the following 4 bytes is the length
- 255 (0xFF) is reserved. Connection should terminate if ever happen.
    Future me should deal with this problem

### Example
```
0    1    2    3    4    5    6    7    8    9    A    ..   F
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
| 02 | 7F | 28 | 01 | 01 | A0 | 00 | 3F | 9A | 90 | 45 | .. | F4 | .. |
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
Len  Dat  Dat  Len  Dat  Len  Dat  Dat  Dat  Dat  Dat  ..   Dat
```
Above stream contains 3 messages:
`0x7F28` with length 0x02, `0x01` with length 0x01, and `0x003F9A9045..F4` with length 0xA0

```
0    1    2    3    4    5    6    7    8    9    A    ..   0112
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
| FC | 01 | 10 | 01 | 01 | A0 | 00 | 3F | 9A | 90 | 45 | .. | F4 | .. |
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
Tag  Len  Len  Dat  Dat  Dat  Dat  Dat  Dat  Dat  Dat  ..   Dat
```
Above stream contains 1 messages:
`0x0101A0003F9A9045..F4` with the length of `0x0110`

## Messages
The first byte indicate the operation, the subsequent bytes is the argument for that operation.
If more argument is supplied than required, the rest is ignored. We use big endian

### Client -> Server
- `0x01` Player move left. No argument
- `0x02` Player move right. No argument
- `0x03` Player stop. No argument

### Server -> Client
- `0x01` Server information, like versions. Reserved, currently no messages
- `0x02` Full game state.
    - The second byte indicate the number of players connected.
    - Third byte and beyond describe the player.
        - Each player takes 20 bytes (1 `i32` + 4 `f32`)
        - First 4 byte (`i32`) is the player id
        - Next 8 bytes (2 `f32`) is the xy position
        - Last 8 bytes (2 `f32`) is the xy velocity
    - If the byte after player ends is `0x00`, then that's the state of the game.
    - If not, then there's more to the state, like the world map.
        Currently, there's no more information, the connection should terminate.
        Future me should deal with this
- `0x03` Player update
    - First 4 byte (`i32`) indicate the player id
    - Next 8 bytes (2 `f32`) is the xy position
    - Last 8 bytes (2 `f32`) is the xy velocity