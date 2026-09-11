# ft_ssl (mandatory part)

This repository contains a C implementation of the mandatory part of the ft_ssl project (MD5 and SHA256 commands).

Features implemented (mandatory):
- `md5` command
- `sha256` command
- Flags supported: `-p`, `-q`, `-r`, `-s` (multiple `-s` supported)
- Read from stdin, files, and strings

Not implemented (bonus):
- Interactive command parsing bonus
- Whirlpool algorithm (bonus)

Build

```sh
make
```

Example usage

```sh
# read from stdin
echo "42 is nice" | ./ft_ssl md5

# print stdin and append checksum
echo "text" | ./ft_ssl md5 -p

# hash a string
./ft_ssl md5 -s "hello"

# hash a file
./ft_ssl md5 file.txt

# sha256 example
./ft_ssl sha256 -s "42 is nice"
```

Notes

- The command dispatch rejects unknown commands with a helpful message.
- String inputs are handled by passing `-s "string"` and the implementation processes the string directly (no temporary files).
- When `-p` is used the program echoes stdin content to stdout before printing the digest.

Testing

```sh
make test        # unit tests + CLI regression series (71 checks total)
make test-lib    # self-check of the ftest test library
make debug       # ASan/UBSan build; `make test` afterwards runs everything
                 # under the sanitizers
```

- [`tests/`](tests/README.md) — what is tested and how to add tests
- [`ftest/`](ftest/README.md) — the C/C++ test library used by the testers
  (suites, stdout capture, fd leak detection, heap-growth detection,
  subprocess runner with timeouts)

If you want me to also implement the bonus parts (interactive parser or Whirlpool), tell me and I will add them.