#include "helpers.h"
#include "commands.h"
#include <fcntl.h>

        __attribute__((always_inline, cold))
static inline int ft_parse_error(uint8_t flags, int fd, char *str)
{
        // first line printer
        ft_putstr_fd("ft_ssl: Error: \'", 2);
        ft_putstr_fd(str, 2);
        ft_putstr_fd("\' is an invalid command\n\n", 2);

        // main prompt for help
        ft_putstr_fd("Commands:\nmd5\nsha256\n\nFlags:\n-p -q -r -s\n", 2);
        close(fd);
        return (2);
}

#define HEX_MD5    0x000000000035646dUL 
#define HEX_SHA256 0x0000363532616873UL

int (*func(char *str))(uint8_t flags, int fd, char *filename)
{
        union
        {
                char bytes[8];
                uint64_t integer;
        } token = {0};

        for(int i = 0; str[i] && i < 7; i++)
                token.bytes[i] = str[i];
        if (str[7])
                return (&ft_parse_error);
        switch(token.integer)
        {
                case HEX_MD5 : return &md5; break;
                case HEX_SHA256 : return &sha256; break;
                default : return &ft_parse_error;
        }
}

int main(int ac, char **av)
{
        uint8_t flags = 0;
        int fd = 0;
        char *filename = "NULL";

        if (ac == 1)
                ft_putstr_fd("usage: ft_ssl command [flags] [file/string]\n", 2);
        if (ac == 2)
        {
                fd = 1;
                if (func(av[1]))
                        return (1);
        }
        if (ac >= 3)
        {
                int i;
                for (i = 2; i < ac - 1; i++)
                {
                        if (ft_strncmp("-p", av[i], 3))
                                flags |= P_FLAGS;
                        if (ft_strncmp("-q", av[i], 3))
                                flags |= Q_FLAGS;
                        if (ft_strncmp("-r", av[i], 3))
                                flags |= R_FLAGS;
                        if (ft_strncmp("-s", av[i], 3))
                                flags |= S_FLAGS;
                }
                fd = open(av[i], O_RDONLY);
                if (fd < 0)
                {
                        ft_putstr_fd("error: can't open ", 2);
                        ft_putstr_fd(av[i], 2);
                        ft_putstr_fd("\n", 2);
                }
                filename = av[i];
                if (func(av[1]))
                        return (1);
        }
        return (0); 
}
