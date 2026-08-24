#!/usr/bin/env bash
set -u
cd "$(dirname "$0")/.."

BIN=./ft_ssl
fail=0
total=0

check() {
    local name="$1"; shift
    local expected="$1"; shift
    total=$((total + 1))
    local got
    got="$(timeout 5 "$@" 2>/dev/null)"
    if [ "$got" = "$expected" ]; then
        echo "ok   $name"
    else
        fail=$((fail + 1))
        echo "FAIL $name"
        echo "     expected: [$expected]"
        echo "     got:      [$got]"
    fi
}

check_no_segfault() {
    local name="$1"; shift
    total=$((total + 1))
    timeout 5 "$@" </dev/null >/dev/null 2>&1
    local status=$?
    if [ "$status" -eq 139 ] || [ "$status" -eq 134 ] || [ "$status" -eq 124 ]; then
        fail=$((fail + 1))
        echo "FAIL $name (exit=$status: crash or hang)"
    else
        echo "ok   $name (exit=$status)"
    fi
}

echo "== segfault regression =="
check_no_segfault "no arguments"          $BIN
check_no_segfault "flags only (-q)"      $BIN md5 -q
check_no_segfault "flags only (-q -r)"   $BIN sha256 -q -r
check_no_segfault "-s missing argument"  $BIN md5 -s
check_no_segfault "invalid command"      $BIN bogus
check_no_segfault "empty command name"   $BIN ""
check_no_segfault "long invalid command" $BIN thiscommandiswaytoolongfortheswitch

echo "== golden outputs =="
seed=$(mktemp)
printf 'abc' > "$seed"

check "md5 -s string"       "900150983cd24fb0d6963f7d28e17f72" $BIN md5 -q -s abc
check "sha256 -s string"    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" $BIN sha256 -q -s abc
check "md5 file -r"         "900150983cd24fb0d6963f7d28e17f72 $seed" \
                            bash -c "printf 'abc' > '$seed' && $BIN md5 -r '$seed'"
check "sha256 file -r"      "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad $seed" \
                            bash -c "printf 'abc' > '$seed' && $BIN sha256 -r '$seed'"
check "md5 stdin via -p"    "abcMD5 ((stdin)) = 900150983cd24fb0d6963f7d28e17f72" \
                            bash -c "printf 'abc' | $BIN md5 -p"
check "sha256 stdin via -p" "abcSHA256 ((stdin)) = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" \
                            bash -c "printf 'abc' | $BIN sha256 -p"
check "md5 empty string"    "d41d8cd98f00b204e9800998ecf8427e" $BIN md5 -q -s ""
check "flags only reads stdin" "d41d8cd98f00b204e9800998ecf8427e" bash -c "printf '' | $BIN md5 -q"
check "p flag no double hash"  "abc900150983cd24fb0d6963f7d28e17f72" bash -c "printf 'abc' | $BIN md5 -qp"

rm -f "$seed"
echo
echo "$((total - fail))/$total passed"
[ "$fail" -eq 0 ]
