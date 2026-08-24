#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "helpers.h"
#include "commands.h"

static int g_total = 0;
static int g_fail  = 0;

#define HASH_LEN_MD5    32
#define HASH_LEN_SHA256 64
#define CAPTURE_MAX     256

static size_t capture(int (*fn)(uint8_t, int, char *), uint8_t flags,
                      int fd, char *arg, char *out, size_t out_size)
{
        fflush(stdout);
        FILE   *tmp   = tmpfile();
        int     saved = dup(STDOUT_FILENO);

        if (!tmp || saved < 0)
        {
                perror("capture setup");
                exit(2);
        }
        dup2(fileno(tmp), STDOUT_FILENO);
        fn(flags, fd, arg);
        fflush(stdout);
        dup2(saved, STDOUT_FILENO);
        close(saved);
        fseek(tmp, 0, SEEK_END);
        long n = ftell(tmp);
        if (n < 0)
                n = 0;
        if ((size_t)n > out_size - 1)
                n = out_size - 1;
        fseek(tmp, 0, SEEK_SET);
        size_t got = fread(out, 1, (size_t)n, tmp);
        out[got] = '\0';
        fclose(tmp);
        return (got);
}

static void check_str(const char *name, const char *got, const char *expected)
{
        g_total++;
        if (strcmp(got, expected) == 0)
                printf("ok   %s\n", name);
        else
        {
                g_fail++;
                printf("FAIL %s\n     expected: %s\n     got:      %s\n",
                        name, expected, got);
        }
}

static void run_case(const char *algo_name, int (*fn)(uint8_t, int, char *),
                     const char *label, const char *input, size_t len,
                     const char *expected_hex)
{
        char name[128];
        char captured[CAPTURE_MAX];
        char input_copy[1200001];
        char expected[HASH_LEN_SHA256 + 4];

        memcpy(input_copy, input, len);
        input_copy[len] = '\0';
        snprintf(expected, sizeof(expected), "%s\n", expected_hex);
        snprintf(name, sizeof(name), "%s/%s", algo_name, label);
        capture(fn, Q_FLAGS, -1, input_copy, captured, sizeof(captured));
        check_str(name, captured, expected);
}

static void run_block_edge_cases(int (*fn)(uint8_t, int, char *), const char *name)
{
        static const struct
        {
                size_t len;
                const char *md5;
                const char *sha256;
        } vectors[] = {
                { 0,    "d41d8cd98f00b204e9800998ecf8427e", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" },
                { 1,    "0cc175b9c0f1b6a831c399e269772661", "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb" },
                { 3,    "47bce5c74f589f4867dbd57e9ca9f808", "9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0" },
                { 55,   "ef1772b6dff9a122358552954ad0df65", "9f4390f8d30c2dd92ec9f095b65e2b9ae9b0a925a5258e241c9f1e910f734318" },
                { 56,   "3b0c8ac703f828b04c6c197006d17218", "b35439a4ac6f0948b6d6f9e3c6af0f5f590ce20f1bde7090ef7970686ec6738a" },
                { 63,   "b06521f39153d618550606be297466d5", "7d3e74a05d7db15bce4ad9ec0658ea98e3f06eeecf16b4c6fff2da457ddc2f34" },
                { 64,   "014842d480b571495a4a0363793f7367", "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb" },
                { 65,   "c743a45e0d2e6a95cb859adae0248435", "635361c48bb9eab14198e76ea8ab7f1a41685d6ad62aa9146d301d4f17eb0ae0" },
                { 119,  "8a7bd0732ed6a28ce75f6dabc90e1613", "31eba51c313a5c08226adf18d4a359cfdfd8d2e816b13f4af952f7ea6584dcfb" },
                { 120,  "5f61c0ccad4cac44c75ff505e1f1e537", "2f3d335432c70b580af0e8e1b3674a7c020d683aa5f73aaaedfdc55af904c21c" },
                { 1000, "cabe45dcc9ae5b66ba86600cca6b8ba8", "41edece42d63e8d9bf515a9ba6932e1c20cbc9f5a5d134645adb5db1b9737ea3" },
        };
        static char a_buf[1200001];

        memset(a_buf, 'a', sizeof(a_buf));
        for (size_t i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++)
        {
                char label[32];

                snprintf(label, sizeof(label), "'a' x %zu", vectors[i].len);
                run_case(name, fn, label, a_buf, vectors[i].len,
                        strcmp(name, "md5") == 0 ? vectors[i].md5 : vectors[i].sha256);
        }
}

static void test_md5_classic(void)
{
        run_case("md5", md5, "fox", "The quick brown fox jumps over the lazy dog", 43,
                 "9e107d9d372bb6826bd81d3542a419d6");
}

static void test_sha256_classic(void)
{
        run_case("sha256", sha256, "nopq",
                 "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56,
                 "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

static void test_file_mode(void)
{
        static const char path[] = "/tmp/opencode/ft_ssl_test_file.bin";
        char     captured[CAPTURE_MAX];
        int      fd = open(path, O_RDONLY);

        g_total++;
        if (fd < 0)
        {
                g_fail++;
                printf("FAIL file/md5: cannot open %s\n", path);
                return;
        }
        capture(md5, Q_FLAGS, fd, (char *)path, captured, sizeof(captured));
        close(fd);
        check_str("file/md5 ('a' x 1000)", captured, "cabe45dcc9ae5b66ba86600cca6b8ba8\n");
}

static void test_helpers(void)
{
        char captured[CAPTURE_MAX];
        FILE *tmp;
        int   saved;

        check_str("strncmp/n=0", ft_strncmp("abc", "abc", 0) == 0 ? "eq" : "ne", "eq");
        check_str("strncmp/equal", ft_strncmp("abc", "abc", 3) == 0 ? "eq" : "ne", "eq");
        check_str("strncmp/prefix", ft_strncmp("abc", "abd", 2) == 0 ? "eq" : "ne", "eq");
        check_str("strncmp/unsigned", ft_strncmp("\x80", "\x01", 1) > 0 ? "gt" : "le", "gt");

        fflush(stdout);
        tmp    = tmpfile();
        saved  = dup(STDOUT_FILENO);
        dup2(fileno(tmp), STDOUT_FILENO);
        ft_putstr_fd((char *)"hello", 1);
        fflush(stdout);
        dup2(saved, STDOUT_FILENO);
        close(saved);
        fseek(tmp, 0, SEEK_SET);
        size_t n = fread(captured, 1, sizeof(captured) - 1, tmp);
        captured[n] = '\0';
        fclose(tmp);
        check_str("putstr_fd/hello", captured, "hello");
}

int main(void)
{
        mkdir("/tmp/opencode", 0755);

        FILE *seed = fopen("/tmp/opencode/ft_ssl_test_file.bin", "wb");

        if (!seed)
        {
                perror("seed file");
                return (2);
        }
        for (int i = 0; i < 1000; i++)
                fputc('a', seed);
        fclose(seed);

        test_md5_classic();
        test_sha256_classic();
        run_block_edge_cases(md5, "md5");
        run_block_edge_cases(sha256, "sha256");
        test_file_mode();
        test_helpers();

        printf("\n%d/%d passed\n", g_total - g_fail, g_total);
        return (g_fail != 0);
}
