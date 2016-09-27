import pwnbox

a = open('log','r')
data = a.read().split('\n')
a.close()

data = [i.split(' ') for i in data]

data = data[1:-1]

data = [[int(a[0].replace(',','')), int(a[1].replace(',','')), int(a[2])] for a in data]

count = 0
for i in data:
    for j in data:
	if i[0] == j[0] and i[2] == j[2] and i[1] != j[1]:
	    print "G : %d\nGD1 : %d\n GD2: %d\n N : %d\n" % (i[0],i[1],j[1],i[2])

print "count : %d\n" % count
#print data[0]

dic= {}

for i in range(len(data)):
    dic[data[i][2]] = i
    #print data[i][2]


def parse(string):
    retVal = string.strip().replace('[','').replace(']','')
    if retVal == '':
	return []
    else:
	return [int(i) for i in retVal.split(' ')]


def get_log(g,ga,p):
    a = 1
    k = g
    while a < p:
	if k == ga:
	    return a
	a += 1
	k = k * g % p
    return -1
    

x = dic.keys()

a = open('factorD','r')
y = parse(a.readline())

dic2 = {}
for i in x:
    dic2[i] = y
    y = parse(a.readline())
#print dic2
a.close()

for i in dic2:
    for j in dic2[i]:
	#print i,j
	assert i % j == 0

dic3 = {}


for i in data:
    if len(dic2[i[2]]) != 0:
	#print "GOOD"
	for j in dic2[i[2]]:
	    #print "g : %d, g^d %% %d == %d" % (i[0] % j, j, i[1] % j)
	    if i[0] % j != 0 and i[0] % j !=1 :
		modp = get_log(i[0] % j, 1, j)
		modv = get_log(i[0] % j, i[1]%j, j)
		print "d mod %d == %d" % (modp, modv)
		dic3[modp] = modv
	print "-"

remains= []
moduls = []
for i in dic3:
    moduls.append(i)
    remains.append(dic3[i])

print moduls
print remains

print pwnbox.number.crt(remainders=remains,moduli=moduls,coprime=False)
