#pragma once

#include <unistd.h>
#include <stdint.h>

// FLAGS
#define P_FLAGS (1 << 0)
#define Q_FLAGS (1 << 1)
#define R_FLAGS (1 << 2)
#define S_FLAGS (1 << 3)

//MD5 rotation function
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define LEFT_ROTATE(x, s) (((x) << (s)) | ((x) >> (32 - (s))))
#define MD5_STEP(f, a, b, c, d, x, s, ac) { \
    (a) += f((b), (c), (d)) + (x) + (ac); \
    (a) = LEFT_ROTATE((a), (s)); \
    (a) += (b); \
}

// commands

int md5(uint8_t flags, int fd, char *filename);
int sha256(uint8_t flags, int fd, char *filename);
