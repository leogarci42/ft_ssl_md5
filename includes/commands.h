#pragma once

#include <unistd.h>

// FLAGS
#define P_FLAGS (1 << 0)
#define Q_FLAGS (1 << 1)
#define R_FLAGS (1 << 2)
#define S_FLAGS (1 << 3)

// commands

int md5(uint8_t flags, int fd);
int sha256(uint8_t flags, int fd);
