import pwnbox
import sha
import itertools

def egcd(a, b):
    if a == 0:
        return (b, 0, 1)
    else:
        g, y, x = egcd(b % a, a)
        return (g, x - (b // a) * y, y)

def modinv(a, m):
    g, x, y = egcd(a, m)
    if g != 1:
        raise Exception('modular inverse does not exist')
    else:
        return x % m

N = 81546073902331759271984999004451939555402085006705656828495536906802924215055062358675944026785619015267809774867163668490714884157533291262435378747443005227619394842923633601610550982321457446416213545088054898767148483676379966942027388615616321652290989027944696127478611206798587697949222663092494873481

Flag_enc = 16155172062598073107968676378352115117161436172814227581212799030353856989153650114500204987192715640325805773228721292633844470727274927681444727510153616642152298025005171599963912929571282929138074246451372957668797897908285264033088572552509959195673435645475880129067211859038705979011490574216118690919

modinv2 = modinv(2, N)


class connect():
    def __init__(self):
        self.pipe = pwnbox.pipe.SocketPipe("rabit.pwning.xxx", 7763, logging = False)
        self.pow()
    def loop(self, target):
        self.pipe.read_until("Give a ciphertext: ")
        self.pipe.write(str(target) + "\n")
        v = self.pipe.read_until("\n").split(' ')[-1]
        return int(v)
    def interact():
        self.pipe.interact()
    def pow(self):
        self.pipe.read_until("Before we begin, a quick proof of work:\n")
        prefix = self.pipe.read_until(",").split()[-1][:-1]
        self.pipe.read_until("\n")
        for i in itertools.product(map(chr, range(256)), repeat=5):
            res = prefix + ''.join(i)
            if sha.new(res).digest()[-3:] == "\xff" * 3:
                self.pipe.write(res + "\n")
                print "[*] pow fin"
                return;

div4 = modinv2 * modinv2
import sys
conn = connect()
k = 1
low = 0
high = N
for i in xrange(1000):
    k *= 4
    a = conn.loop((Flag_enc * k) % N)
    if a == 0:
        high = (high + low) / 2
    else:
        low = (high + low) / 2
    print str(a), hex((high + low) / 2)
