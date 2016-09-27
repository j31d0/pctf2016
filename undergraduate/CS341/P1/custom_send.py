#!/usr/bin/python

import pwnbox
import struct
import argparse

parser = argparse.ArgumentParser(add_help = False)

parser.add_argument("-h", "--host", help = "host ip")
parser.add_argument("-p", "--port", help = "port num", type=int)
parser.add_argument("-l", "--length", help = "length")
parser.add_argument("-i", "--incomplete", help = "don't send all packet", action="store_true")
parser.add_argument("-b", "--byte", help = "send byte by byte", action ="store_true")
test_length = 1000 * 1000 * 5 - 8
#server = '1.255.54.63'
#port =  10292

#jeawon
#server = '52.78.156.255'
#port =  14141

#server = '143.248.56.16'
#port = 3000

#server = '143.248.2.13'
#port = 20030

#server = '1.255.54.63'
#port = 7878

server = '110.76.76.141'
port = 8080

args = parser.parse_args()

test_length = eval(args.length)
server = args.host
port = args.port


p = pwnbox.pipe.SocketPipe(server,port,log_to = None, logging = None)

def check_sum(payload):
    if len(payload) & 1:
	new_payload = payload+'\x00'
    else:
	new_payload = payload
    sum_ = 0
    for i in range(0,len(payload),2):
	sum_ += struct.unpack("<H",new_payload[i:i+2])[0]
	sum_ = (sum_ >> 16) + (sum_ & 0xffff)
    
    return struct.pack("<H",((~sum_) & 0xffff))
    
payload = "a" * test_length
header_len = 8

header = struct.pack("<BBH", 0,0,0)
header += struct.pack(">I",(len(payload)+header_len))


payload = header + payload

payload = payload[0:2] + check_sum(payload) + payload[4:]


if args.incomplete:
    p.write(payload[:-2])
elif args.byte:
    for i in payload:
	p.write(i)
else:
    p.write(payload)
print "SEND"

data = p.read_byte(len(payload))
print "recved data : %d" % len(data)


p.close()
