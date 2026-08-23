## MD5

MD5 is a legacy cryptographic hash function that produces a 128-bit (16-byte) digest.

High-level steps:
- Padding: append a single `1` bit (0x80), then enough `0` bits so message length (in bits) ≡ 448 (mod 512).
- Length append: append original message length as a 64-bit little-endian integer.
- Initialization: set four 32-bit words A, B, C, D to fixed constants (0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476).
- Process each 512-bit block in 16 little-endian 32-bit words X[0..15] using four rounds of 16 operations. Each step uses one of four nonlinear functions (F,G,H,I), adds a constant T[i], rotates the result left by a fixed amount, and updates the state.
- Finalization: after all blocks, produce digest by concatenating A,B,C,D in little-endian order (128-bit output).

Implementation notes:
- Uses 32-bit word operations and modular addition (wrap-around).
- Byte order: MD5 treats message words in little-endian.
- Performance: simple and fast on 32-bit CPUs.

Security:
- MD5 is considered cryptographically broken: practical collisions exist and it should not be used for security-sensitive purposes (signatures, certificates, etc.). It remains useful for checksums where collision resistance is not required.

Example: MD5("abc") = 900150983cd24fb0d6963f7d28e17f72

---

## SHA-256

SHA-256 (part of the SHA-2 family) outputs a 256-bit (32-byte) digest and is widely used today.

High-level steps:
- Padding: append `1` bit then `0` bits until message length (in bits) ≡ 448 (mod 512).
- Length append: append original message length as a 64-bit big-endian integer.
- Initialization: set eight 32-bit working variables `H0..H7` to fixed constants derived from the fractional parts of square roots of the first primes.
- For each 512-bit block: build a message schedule `W[0..63]` (first 16 words from the block in big-endian, then extend using σ0/σ1 functions). Run 64 rounds of the compression function, using the constants K[0..63], and the operations: ROTR, shifts, Ch, Maj, large Σ and small σ functions. Update `a..h` values and add back to `H0..H7`.
- Finalization: concatenate `H0..H7` in big-endian to form the 256-bit digest.

Implementation notes:
- SHA-256 uses big-endian word interpretation.
- The core primitives are bitwise rotates, shifts, and modular additions on 32-bit words.
- The message schedule increases diffusion over rounds; the K constants are fixed and derived from cube roots.

Security:
- SHA-256 is considered secure for current mainstream use cases (collision/ preimage resistance). No practical collisions are known for the full 256-bit variant as of this writing.

Example: SHA256("abc") = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad

---

### References and further reading
- RFC 1321 — MD5 (informational): algorithm description and reference implementation.
- FIPS 180-4 — Secure Hash Standard (SHA-256 and family), for authoritative specification.

### sources

- [md5 algorithms explained](https://www.geeksforgeeks.org/computer-networks/what-is-the-md5-algorithm/)

- [brief (and gentle) introduction to sha256](https://medium.com/@madan_nv/a-deep-dive-into-sha-256-working-principles-and-applications-a38cccc390d4)

- [step by step tutorial of sha256](https://www.boot.dev/blog/computer-science/how-sha-2-works-step-by-step-sha-256)

- [sha256 visually explained](https://sha256algorithm.com/)
