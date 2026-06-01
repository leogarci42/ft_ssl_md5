#include "helpers.h"
#include "commands.h"
#include <fcntl.h>

__attribute__((always_inline, cold))
static inline void ft_parse_error(char *str)
{
        // first line printer
        ft_putstr_fd("ft_ssl: Error: \'", 2);
        ft_putstr_fd(str, 2);
        ft_putstr_fd("\' is an invalid command\n\n", 2);

        // main prompt for help
        ft_putstr_fd("Commands:\nmd5\nsha256\n\nFlags:\n-p -q -r -s\n", 2);
}

int main(int ac, char **av)
{
        uint8_t flags = 0;
        int fd = 0;

        if (ac == 1)
                ft_putstr_fd("usage: ft_ssl command [flags] [file/string]\n", 2);
        else if (ft_strncmp("md5", av[1], 4) && ft_strncmp("sha256", av[1], 7))
                return (ft_parse_error(av[1]), 2);
        if (ac == 2)
        {
                fd = 1;
                if (ft_strncmp("md5", av[1], 4))
                        return (md5(flags, fd));
                if (ft_strncmp("sha256", av[1], 7))
                        return (sha256(flags, fd));
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
        }
        return (0); 
}
