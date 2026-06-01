#include "commands.h"

typedef struct s_md5_ctx
{
	uint64_t count;
	uint32_t state[4];
        uint8_t  buffer[64];
} t_md5_ctx;

__attribute__((always_inline, cold))
static inline int ft_strlen(char *str)
{
        int i;
        for (i = 0; str[i]; i++);
        return (i);
}

void md5_update(t_md5_ctx *ctx, const uint8_t *input, size_t input_len)
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
		md5_update(ctx, buf, bytes_read);
        if (bytes_read < 0)
		return (-1);
        return (0);
}

__attribute__((always_inline))
static inline void cleanup_fd(int *fd)
{
	if (fd && *fd > 2) 
		close(*fd);
}

__attribute__((always_inline, hot))
int md5(uint8_t flags, __attribute__((cleanup(cleanup_fd))) int fd)
{
	t_md5_ctx ctx;
	uint8_t   digest[16];

	(void)flags;
	md5_init(&ctx);
	if (ft_process_input(fd, &ctx) < 0)
	{
		close(fd);
		return (1);
	}
	close(fd);
	md5_final(digest, &ctx);
	return (0);
}
