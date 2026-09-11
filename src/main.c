#include "helpers.h"
#include "commands.h"
#include <fcntl.h>

        __attribute__((always_inline, cold))
static inline int ft_parse_error(uint8_t flags, int fd, char *str)
{
        (void)flags;
        (void)fd;

        ft_putstr_fd("ft_ssl: Error: \'", 2);
        ft_putstr_fd(str, 2);
        ft_putstr_fd("\' is an invalid command\n\n", 2);
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
static inline int set_flags(uint8_t *flags, size_t ac, char **av, int (*function)(uint8_t, int, char *))
{
        int i = 2;

        for (; i < (int)ac; i++)
        {
                if (!av[i] || av[i][0] != '-' || av[i][1] == '\0')
                        break;
                for (size_t j = 1; av[i][j]; j++)
                {
                        switch (av[i][j])
                        {
                                case 'p':
                                        function((*flags) | P_FLAGS, 0, "(stdin)");
                                        break;
                                case 'q':
                                        *flags |= Q_FLAGS; break;
                                case 'r':
                                        *flags |= R_FLAGS; break;
                                case 's':
                                        if (i + 1 >= (int)ac)
                                        {
                                                ft_putstr_fd("ft_ssl: ", 2);
                                                ft_putstr_fd(av[1], 2);
                                                ft_putstr_fd(": -s: No such file or directory\n", 2);
                                                return (i + 1);
                                        }
                                        i++;
                                        function((*flags) | S_FLAGS, -1, av[i]);
                                        j = __builtin_strlen(av[i - 1]);
                                        break;
                                default:
                                        break;
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

        if (ac == 1 || !av[1])
        {
                ft_putstr_fd("usage: ft_ssl command [flags] [file/string]\n", 2);
                return (2);
        }
        int (*function)(uint8_t, int, char *) = func(av[1]);
        if (function == &ft_parse_error)
                return (function(flags, fd, av[1]));
        if (ac == 2)
        {
                filename = "(stdin)";
                function(flags, 0, filename);
                return (0);
        }
        if (ac >= 3)
        {
                int i = set_flags(&flags, (size_t)ac, av, function);

                if (i >= ac || !av[i])
                {
                        int consumed = (av[ac - 1] && av[ac - 1][0] != '-');

                        for (int k = 2; !consumed && k < ac; k++)
                                if (av[k] && av[k][0] == '-')
                                        consumed = (__builtin_strchr(av[k], 'p') != NULL);
                        if (!consumed)
                                function(flags, 0, "(stdin)");
                        return (0);
                }
                fd = open(av[i], O_RDONLY);
                if (fd < 0)
                {
                        ft_putstr_fd("ft_ssl: ", 2);
                        ft_putstr_fd(av[1], 2);
                        ft_putstr_fd(": ", 2);
                        ft_putstr_fd(av[i], 2);
                        ft_putstr_fd(": No such file or directory\n", 2);
                }
                else
                {
                        filename = av[i];
                        function(flags, fd, filename);
                }
        }
        return (0); 
}
