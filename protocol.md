# Protocol - Draft 1.1
This document describe the communication protocol between client and server

## Summary
Client and server communicate to each other with plain old TCP socket. Each messages between client and server
is stateless, and sent with length prefix. The protocol is split into 2 layer, the bottom layer, and the top
layer. Each doesn't care about what the other said. Bottom doesn't care what the actual message is, and top
doesn't care about the length prefix, etc.

The bottom layer handle things like message framing, and what part of the stream is part of what message. And
the top layer handle the actual body of the message. Also the client state initialization is no different than
the rest of the messages, though the server should always send the full game state at client connection.

## Bottom layer: Message layout with length prefix
Each message body is prefixed with a length. Something like this
```
++--------+------++--------+------++-----
|| Length | Body || Length | Body || ...
++--------+------++--------+------++-----
```
With these rules for length prefix and byte layout:
- If the first byte is less than 252 (0xFC), then, it's the length
- If the first byte is equal to 253 (0xFD), then the following 2 bytes is the length
- If the first byte is equal to 254 (0xFE), then the following 4 bytes is the length
- 255 (0xFF) is reserved. Connection should terminate if ever happen.
    Future me should deal with this problem

### Example
```
0    1    2     3    4     5    6    7    8    9    A    ..   F
+----+----+----++----+----++----+----+----+----+----+----+----+----+----+
| 02 | 7F | 28 || 01 | 01 || A0 | 00 | 3F | 9A | 90 | 45 | .. | F4 | .. |
+----+----+----++----+----++----+----+----+----+----+----+----+----+----+
Len  Dat  Dat   Len  Dat   Len  Dat  Dat  Dat  Dat  Dat  ..   Dat
```
Above stream contains 3 messages:
`0x7F28` with length `0x02`, `0x01` with length `0x01`, and `0x003F9A9045..F4` with length `0xA0`

```
0    1    2    3    4    5    6    7    8    9    A    ..   0112
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
| FC | 01 | 10 | 01 | 01 | A0 | 00 | 3F | 9A | 90 | 45 | .. | F4 | .. |
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
Tag  Len  Len  Dat  Dat  Dat  Dat  Dat  Dat  Dat  Dat  ..   Dat
```
Above stream contains 1 messages:
`0x0101A0003F9A9045..F4` with the length of `0x0110`

## Top layer: Messages
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
    - The next 4 byte (`i32`) indicate the current player id
    - The next 4 byte (`f32`) indicate the current server tick per second
- `0x03` Player update
    - First 4 byte (`i32`) indicate the player id
    - Next 8 bytes (2 `f32`) is the xy position
    - Last 8 bytes (2 `f32`) is the xy velocity