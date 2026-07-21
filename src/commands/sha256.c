#include "commands.h"

typedef struct s_sha256_ctx
{
        uint64_t count;
        uint32_t state[8];
        uint8_t  buffer[64];
} t_sha256_ctx;

        __attribute__((always_inline, hot))
static inline void sha256_transform(uint32_t *state, const uint8_t *block)
{
        // first 32 bits of the fractional parts of the cube roots of the first 64 primes
        static const uint32_t K[64] = {
                0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };
        uint32_t W[64];
        uint32_t a, b, c, d, e, f, g, h;
        uint32_t T1, T2;

        for (int i = 0; i < 16; i++)
                W[i] = ((uint32_t)block[i*4] << 24) | ((uint32_t)block[i*4+1] << 16)
                     | ((uint32_t)block[i*4+2] << 8) | ((uint32_t)block[i*4+3]);
        for (int i = 16; i < 64; i++)
        {
                uint32_t s0 = ROTR(W[i-15], 7)  ^ ROTR(W[i-15], 18) ^ (W[i-15] >> 3);
                uint32_t s1 = ROTR(W[i-2],  17) ^ ROTR(W[i-2],  19) ^ (W[i-2]  >> 10);
                W[i] = W[i-16] + s0 + W[i-7] + s1;
        }
        a = state[0]; b = state[1]; c = state[2]; d = state[3];
        e = state[4]; f = state[5]; g = state[6]; h = state[7];
        for (int i = 0; i < 64; i++)
        {
                uint32_t S1    = ROTR(e, 6) ^ ROTR(e, 11) ^ ROTR(e, 25);
                uint32_t ch    = (e & f) ^ (~e & g);
                uint32_t S0    = ROTR(a, 2) ^ ROTR(a, 13) ^ ROTR(a, 22);
                uint32_t maj   = (a & b) ^ (a & c) ^ (b & c);
                T1 = h + S1 + ch + K[i] + W[i];
                T2 = S0 + maj;
                h = g; g = f; f = e; e = d + T1;
                d = c; c = b; b = a; a = T1 + T2;
        }
        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
        state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

        __attribute__((always_inline, hot))
static inline void sha256_update(t_sha256_ctx *ctx, const uint8_t *input, size_t input_len)
{
        size_t i = 0;
        size_t index    = (size_t)((ctx->count >> 3) & 0x3F);
        ctx->count     += ((uint64_t)input_len << 3);
        size_t part_len = 64 - index;

        if (input_len >= part_len)
        {
                for (size_t j = 0; j < part_len; j++)
                        ctx->buffer[index + j] = input[j];
                sha256_transform(ctx->state, ctx->buffer);
                i = part_len;
                for (; i + 63 < input_len; i += 64)
                        sha256_transform(ctx->state, &input[i]);
                index = 0;
        }
        for (size_t j = 0; i < input_len; i++, j++)
                ctx->buffer[index + j] = input[i];
}

        __attribute__((always_inline, cold))
static inline int ft_process_input_sha(int fd, t_sha256_ctx *ctx, uint8_t flags)
{
        uint8_t  buf[4096];
        ssize_t  bytes_read;

        while ((bytes_read = read(fd, buf, sizeof(buf))) > 0)
        {
                if (fd == 0 && (flags & P_FLAGS))
                        write(1, buf, bytes_read);
                sha256_update(ctx, buf, bytes_read);
        }
        if (bytes_read < 0)
                return (-1);
        return (0);
}

        __attribute__((always_inline, cold))
static inline void sha256_init(t_sha256_ctx *ctx)
{
        ctx->count = 0;

        // first 32 bits of the fractional parts of the square roots of the first 8 primes
        ctx->state[0] = 0x6a09e667;
        ctx->state[1] = 0xbb67ae85;
        ctx->state[2] = 0x3c6ef372;
        ctx->state[3] = 0xa54ff53a;
        ctx->state[4] = 0x510e527f;
        ctx->state[5] = 0x9b05688c;
        ctx->state[6] = 0x1f83d9ab;
        ctx->state[7] = 0x5be0cd19;
}

        __attribute__((always_inline))
static inline void sha256_final(uint8_t *digest, t_sha256_ctx *ctx)
{
        uint8_t  bits[8];
        uint32_t index;
        uint32_t pad_len;

        // store bit count as big-endian (SHA uses big-endian, unlike MD5)
        for (int i = 7; i >= 0; i--)
                bits[7 - i] = (uint8_t)(ctx->count >> (i * 8));
        index   = (uint32_t)((ctx->count >> 3) & 0x3F);
        pad_len = (index < 56) ? (56 - index) : (120 - index);
        static const uint8_t padding[64] = { 0x80 };
        sha256_update(ctx, padding, pad_len);
        sha256_update(ctx, bits, 8);

        // output is big-endian
        for (int i = 0; i < 8; i++)
        {
                digest[i*4]     = (uint8_t)(ctx->state[i] >> 24);
                digest[i*4 + 1] = (uint8_t)(ctx->state[i] >> 16);
                digest[i*4 + 2] = (uint8_t)(ctx->state[i] >> 8);
                digest[i*4 + 3] = (uint8_t)(ctx->state[i]);
        }
}

        __attribute__((always_inline))
static inline void print_sha256_output(uint8_t flags, const char hash[64], const char *target, int is_string)
{
        if ((flags & Q_FLAGS) || is_string == -1)
        {
                write(1, hash, 64);
                write(1, "\n", 1);
                return;
        }
        if (flags & R_FLAGS)
        {
                write(1, hash, 64);
                write(1, " ", 1);
                if (is_string)
                        write(1, "\"", 1);
                write(1, target, __builtin_strlen(target));
                if (is_string)
                        write(1, "\"", 1);
                write(1, "\n", 1);
        }
        else
        {
                write(1, "SHA256 (", 8);
                if (is_string)
                        write(1, "\"", 1);
                write(1, target, __builtin_strlen(target));
                write(1, is_string ? "\") = " : ") = ", is_string ? 5 : 4);
                write(1, hash, 64);
                write(1, "\n", 1);
        }
}

int sha256(uint8_t flags, int fd, char *filename)
{
        t_sha256_ctx ctx;
        uint8_t      digest[32];
        char         hash[64];

        sha256_init(&ctx);
        if (ft_process_input_sha(fd, &ctx, flags) < 0)
                return (1);
        sha256_final(digest, &ctx);
        static const char hex[] = "0123456789abcdef";
        for (int i = 0; i < 32; i++)
        {
                hash[i * 2]     = hex[(digest[i] >> 4) & 0xF];
                hash[i * 2 + 1] = hex[digest[i] & 0xF];
        }
        print_sha256_output(flags, hash, filename, 0);
        return (0);
}
