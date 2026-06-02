#include "commands.h"

typedef struct s_md5_ctx
{
        uint64_t count;
        uint32_t state[4];
        uint8_t  buffer[64];
} t_md5_ctx;

        __attribute__((always_inline, hot))
static inline void md5_transform(uint32_t *state, uint8_t *buffer)
{
        uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
        // hardcoded value that correspond to T[i(1 ... 64)] = 2^32 * |sin(i)| 
        static const uint32_t T[64] = {
                0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
        };
        uint32_t X[16];

        for (int i = 0; i < 16; i++) 
                X[i] = ((uint32_t)block[i*4]) | ((uint32_t)block[i*4 + 1] << 8) | ((uint32_t)block[i*4 + 2] << 16) | ((uint32_t)block[i*4 + 3] << 24);
        //1st round
        MD5_STEP(F, a, b, c, d, X[0],  7,  T[0]);
        MD5_STEP(F, d, a, b, c, X[1],  12, T[1]);
        MD5_STEP(F, c, d, a, b, X[2],  17, T[2]);
        MD5_STEP(F, b, c, d, a, X[3],  22, T[3]);
        MD5_STEP(F, a, b, c, d, X[4],  7,  T[4]);
        MD5_STEP(F, d, a, b, c, X[5],  12, T[5]);
        MD5_STEP(F, c, d, a, b, X[6],  17, T[6]);
        MD5_STEP(F, b, c, d, a, X[7],  22, T[7]);
        MD5_STEP(F, a, b, c, d, X[8],  7,  T[8]);
        MD5_STEP(F, d, a, b, c, X[9],  12, T[9]);
        MD5_STEP(F, c, d, a, b, X[10], 17, T[10]);
        MD5_STEP(F, b, c, d, a, X[11], 22, T[11]);
        MD5_STEP(F, a, b, c, d, X[12], 7,  T[12]);
        MD5_STEP(F, d, a, b, c, X[13], 12, T[13]);
        MD5_STEP(F, c, d, a, b, X[14], 17, T[14]);
        MD5_STEP(F, b, c, d, a, X[15], 22, T[15]);
        //2nd round
        MD5_STEP(G, a, b, c, d, X[1],  5,  T[16]);
        MD5_STEP(G, d, a, b, c, X[6],  9,  T[17]);
        MD5_STEP(G, c, d, a, b, X[11], 14, T[18]);
        MD5_STEP(G, b, c, d, a, X[0],  20, T[19]);
        MD5_STEP(G, a, b, c, d, X[5],  5,  T[20]);
        MD5_STEP(G, d, a, b, c, X[10], 9,  T[21]);
        MD5_STEP(G, c, d, a, b, X[15], 14, T[22]);
        MD5_STEP(G, b, c, d, a, X[4],  20, T[23]);
        MD5_STEP(G, a, b, c, d, X[9],  5,  T[24]);
        MD5_STEP(G, d, a, b, c, X[14], 9,  T[25]);
        MD5_STEP(G, c, d, a, b, X[3],  14, T[26]);
        MD5_STEP(G, b, c, d, a, X[8],  20, T[27]);
        MD5_STEP(G, a, b, c, d, X[13], 5,  T[28]);
        MD5_STEP(G, d, a, b, c, X[2],  9,  T[29]);
        MD5_STEP(G, c, d, a, b, X[7],  14, T[30]);
        MD5_STEP(G, b, c, d, a, X[12], 20, T[31]);
        //3rd round
        MD5_STEP(H, a, b, c, d, X[5],  4,  T[32]);
        MD5_STEP(H, d, a, b, c, X[8],  11, T[33]);
        MD5_STEP(H, c, d, a, b, X[11], 16, T[34]);
        MD5_STEP(H, b, c, d, a, X[14], 23, T[35]);
        MD5_STEP(H, a, b, c, d, X[1],  4,  T[36]);
        MD5_STEP(H, d, a, b, c, X[4],  11, T[37]);
        MD5_STEP(H, c, d, a, b, X[7],  16, T[38]);
        MD5_STEP(H, b, c, d, a, X[10], 23, T[39]);
        MD5_STEP(H, a, b, c, d, X[13], 4,  T[40]);
        MD5_STEP(H, d, a, b, c, X[0],  11, T[41]);
        MD5_STEP(H, c, d, a, b, X[3],  16, T[42]);
        MD5_STEP(H, b, c, d, a, X[6],  23, T[43]);
        MD5_STEP(H, a, b, c, d, X[9],  4,  T[44]);
        MD5_STEP(H, d, a, b, c, X[12], 11, T[45]);
        MD5_STEP(H, c, d, a, b, X[15], 16, T[46]);
        MD5_STEP(H, b, c, d, a, X[2],  23, T[47]);
        //4th round
        MD5_STEP(I, a, b, c, d, X[0],  6,  T[48]);
        MD5_STEP(I, d, a, b, c, X[7],  10, T[49]);
        MD5_STEP(I, c, d, a, b, X[14], 15, T[50]);
        MD5_STEP(I, b, c, d, a, X[5],  21, T[51]);
        MD5_STEP(I, a, b, c, d, X[12], 6,  T[52]);
        MD5_STEP(I, d, a, b, c, X[3],  10, T[53]);
        MD5_STEP(I, c, d, a, b, X[10], 15, T[54]);
        MD5_STEP(I, b, c, d, a, X[1],  21, T[55]);
        MD5_STEP(I, a, b, c, d, X[8],  6,  T[56]);
        MD5_STEP(I, d, a, b, c, X[15], 10, T[57]);
        MD5_STEP(I, c, d, a, b, X[6],  15, T[58]);
        MD5_STEP(I, b, c, d, a, X[13], 21, T[59]);
        MD5_STEP(I, a, b, c, d, X[4],  6,  T[60]);
        MD5_STEP(I, d, a, b, c, X[11], 10, T[61]);
        MD5_STEP(I, c, d, a, b, X[2],  15, T[62]);
        MD5_STEP(I, b, c, d, a, X[9],  21, T[63]);

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
}

        __attribute__((always_inline, hot))
static inline void md5_update(t_md5_ctx *ctx, const uint8_t *input, size_t input_len)
{
        size_t i = 0;
        size_t index = (size_t)((ctx->count >> 3) & 0x3F);
        ctx->count += ((uint64_t)input_len << 3);
        size_t part_len = 64 - index;

        if (input_len >= part_len)
        {
                for (size_t j = 0; j < part_len; j++)
                        ctx->buffer[index + j] = input[j];
                md5_transform(ctx->state, ctx->buffer);
                i = part_len;
                for (; i + 63 < input_len; i += 64)
                        md5_transform(ctx->state, &input[i]);
                index = 0;
        }
        for (size_t j = 0; i < input_len; i++, j++)
                ctx->buffer[index + j] = input[i];
}

        __attribute__((always_inline, cold))
static inline int ft_process_input(int fd, t_md5_ctx *ctx)
{
        uint8_t buf[4096]; 
        ssize_t bytes_read;

        while ((bytes_read = read(fd, buf, sizeof(buf))) > 0)
        {
                if (fd == 0 && (flags & P_FLAGS))
                        write(1, buf, bytes_read);
                md5_update(ctx, buf, bytes_read);
        }
        if (bytes_read < 0)
                return (-1);
        return (0);
}

        __attribute__((always_inline, cold))
static inline void md5_init(t_md5_ctx *ctx)
{
        ctx->count = 0;

        //norm defined in RFC 1321
        ctx->state[0] = 0x67452301;
        ctx->state[1] = 0xEDFCBA45;
        ctx->state[2] = 0x98CBADFE;
        ctx->state[3] = 0x13DCE476;
}

        __attribute__((always_inline, cold))
static inline void cleanup_fd(int *fd)
{
        if (fd && *fd > 2) 
                close(*fd);
}

        __attribute__((always_inline))
static inline void md5_final(uint8_t *digest, t_md5_ctx *ctx)
{
        uint8_t  bits[8];
        uint32_t index;
        uint32_t pad_len;

        for (int i = 0; i < 8; i++)
                bits[i] = (uint8_t)(ctx->count >> (i * 8));
        index = (uint32_t)((ctx->count >> 3) & 0x3F);
        pad_len = (index < 56) ? (56 - index) : (120 - index);
        static const uint8_t padding[64] = { 0x80 };
        md5_update(ctx, padding, pad_len);
        md5_update(ctx, bits, 8);
        for (int i = 0; i < 4; i++)
        {
                digest[i * 4] = (uint8_t)(ctx->state[i]);
                digest[i * 4 + 1] = (uint8_t)(ctx->state[i] >> 8);
                digest[i * 4 + 2] = (uint8_t)(ctx->state[i] >> 16);
                digest[i * 4 + 3] = (uint8_t)(ctx->state[i] >> 24);
        }
}

        __attribute__((always_inline))
static inline void ft_putstr(const char *str)
{
        int i = 0;
        if (!str)
                return;
        while (str[i])
                i++;
        write(1, str, i);
}

        __attribute__((always_inline))
static void print_md5_output(uint8_t flags, const char hash[32], const char *target, int is_string)
{
        if ((flags & Q_FLAGS) || is_string == -1)
        {
                write(1, hash, 32);
                write(1, "\n", 1);
                return;
        }
        if (flags & R_FLAGS)
        {
                write(1, hash, 32);
                write(1, " ", 1);
                if (is_string) write(1, "\"", 1);
                ft_putstr(target);
                if (is_string) write(1, "\"", 1);
                write(1, "\n", 1);
        }
        else
        {
                ft_putstr(is_string ? "MD5 (\"" : "MD5 (");
                ft_putstr(target);
                ft_putstr(is_string ? "\") = " : ") = ");
                write(1, hash, 32);
                write(1, "\n", 1);
        }
}

int md5(uint8_t flags, __attribute__((cleanup(cleanup_fd))) int fd, char *filename)
{
        t_md5_ctx ctx;
        uint8_t   digest[16];

        (void)flags;
        md5_init(&ctx);
        if (ft_process_input(fd, &ctx) < 0)
                return (1);
        md5_final(digest, &ctx);
        return (0);
}
