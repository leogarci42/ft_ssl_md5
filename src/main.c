#include "helpers.h"
#include "commands.h"
#include <fcntl.h>

        __attribute__((always_inline, cold))
static inline int ft_parse_error(uint8_t flags, int fd, char *str)
{
        (void)flags;
        (void)fd;

        // first line printer
        ft_putstr_fd("ft_ssl: Error: \'", 2);
        ft_putstr_fd(str, 2);
        ft_putstr_fd("\' is an invalid command\n\n", 2);

        // main prompt for help
        ft_putstr_fd("Commands:\nmd5\nsha256\n\nFlags:\n-p -q -r -s\n", 2);
        return (2);
}

int (*func(char *str))(uint8_t flags, int fd, char *filename)
{
        size_t i = 0;
        union
        {
                char bytes[8];
                uint64_t integer;
        } token = {0};

        for(i = 0; str[i] && i < 8; i++)
                token.bytes[i] = str[i];
        if (i > 6 || str[i] != '\0')
                return (&ft_parse_error); 
        switch(token.integer)
        {
                case HEX_MD5 : return &md5; break;
                case HEX_SHA256 : return &sha256; break;
                default : return &ft_parse_error;
        }
}

static inline void cleanup_fd(int *fd)
{
        if (fd && *fd > 2) 
                close(*fd);
}

        __attribute__((always_inline, cold))
static inline int set_flags(uint8_t *flags, size_t ac, char **av)
{
        size_t i = 2;
        
        for (i = 2; i < ac - 1; i++)
        {
                if (av[i] && av[i][0] == '-' && av[i][1] != '\0')
                {
                        switch(av[i][1])
                        {
                                case 'p' : *flags |= P_FLAGS; continue ;
                                case 'q' : *flags |= Q_FLAGS; continue ;
                                case 'r' : *flags |= R_FLAGS; continue ;
                                case 's' : *flags |= S_FLAGS; continue ;
                        }
                }

        }
        return (i);
}

int main(int ac, char **av)
{
        uint8_t flags = 0;
        __attribute__((cleanup(cleanup_fd)))int fd = 1;
        char *filename = "NULL";

        if (ac == 1)
                ft_putstr_fd("usage: ft_ssl command [flags] [file/string]\n", 2);
        int (*function)(uint8_t, int, char *) = func(av[1]);
        if (function == &ft_parse_error)
                return (function(flags, fd, av[1]));
        if (ac == 2)
                function(flags, fd, filename);
        if (ac >= 3)
        {
                int i = set_flags(&flags, (size_t)ac, av);
                fd = open(av[i], O_RDONLY);
                if (fd < 0)
                {
                        ft_putstr_fd("error: can't open ", 2);
                        ft_putstr_fd(av[i], 2);
                        ft_putstr_fd("\n", 2);
                }
                filename = av[i];
                function(flags, fd, filename);
        }
        return (0); 
}
