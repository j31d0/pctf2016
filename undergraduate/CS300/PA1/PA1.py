import sys


class Node:
    def __init__(self,value,left = None ,right = None,up = None,down = None):
        self.value = value
	self.left = left
	self.right = right
	self.up = up
	self.down = down


    def printNode(self):
        x = self
	s = ""
	while x.value != None:
	    i = x
	    while i.value !=  None:
		s = s + str(i.value) + ' '
		i = i.right
	    s = s + '\n'
	    x = x.down
	return s
    
    def printNode_Npath(self):
        x = self
	s = ""
	while x != None:
	    i = x
	    while i !=  None:
		if i.value != None:
		    s = s + str(i.value) + ' '
		else:
		    s = s + "N" + ' '
		i = i.right
	    s = s + '\n'
	    x = x.down
	return s


    def deleteOne(self):
        if self.right.value == None and self.down.value == None:
	    self.value = None
	    self.right.down = None
	    self.down.right = None
	    if self.up == None or self.up.value == None:
		self.right = None
	    if self.left == None or self.left.value == None:
		self.down = None
	    return None
        elif self.down.value == None or (self.right.value != None and self.right.value < self.down.value):
	    temp = self.value
	    self.value = self.right.value
	    self.right.value = temp
	    self.right.deleteOne()
	else:
	    temp = self.value
	    self.value = self.down.value
	    self.down.value = temp
	    self.down.deleteOne()
	return self

    def insert_recursive(self):
        if self.up == None and self.left == None:
	    return
	elif self.up == None:
	    if self.left.value > self.value:
		tmp = self.value
		self.value = self.left.value
		self.left.value = tmp
		self.left.insert_recursive()
	elif self.left == None:
	    if self.up.value > self.value:
		tmp = self.value
		self.value = self.up.value
		self.up.value = tmp
		self.up.insert_recursive()
	else:
	    toSwap = self.up
	    if self.left.value > toSwap.value:
		toSwap = self.left
	    if toSwap.value > self.value:
		tmp = self.value
		self.value = toSwap.value
		toSwap.value = tmp
		toSwap.insert_recursive() 
	return

    def insertOne(self,value):
	h = 0
	leaf = self
	while leaf.value != None:
	    leaf = leaf.down
	    h += 1
	lend = leaf
	w = 0
	while leaf.up != None:
	    if leaf.left and leaf.up and leaf.left.value != None and leaf.up.value != None:
		leaf.value = value
		leaf.right.down = Node(None, up = leaf.right, left = leaf.down)
		leaf.down.right = leaf.right.down
		leaf.insert_recursive()
		return self
	    elif leaf.right:
		w += 1
		leaf = leaf.right
	    else:
		leaf = leaf.up
	if h > w:
	    leaf.value = value
	    leaf.right = Node(None,left = leaf)
	    leaf.right.down = Node(None, left = leaf.down, up = leaf.right)
	    leaf.down.right = leaf.right.down
	    leaf.insert_recursive()
	else:
	    lend.value = value
	    lend.down = Node(None, up = lend)
	    lend.down.right = Node(None, left = lend.down, up = lend.right)
	    lend.right.down = lend.down.right
	    lend.insert_recursive()

	return self


    def __str__(self):
        return "<Node>\n"+self.printNode()+"<\\Node>"


def cons_from_array(A):
    if len(A) == 0: return None
    a1 = []
    for i in range(len(A)):
	a1.append([Node(k) for k in A[i]])
    for i in range(len(a1)):
	for j in range(len(a1[i])):
	    if i < len(a1) - 1 and j < len(a1[i+1]):
		a1[i][j].down = a1[i+1][j]
	    if j < len(a1[i]) - 1:
		a1[i][j].right = a1[i][j+1]
	    if j != 0:
		a1[i][j].left = a1[i][j-1]
	    if i != 0:
		a1[i][j].up = a1[i-1][j]

    root = a1[0][0]
    leav = root
    while leav.down:
	leav = leav.down
    
    leav.down = Node(None, up = leav)
    leav.down.right = Node(None, left = leav.down)

    emptySub = leav.down.right
    proot = emptySub

    while leav.up or leav.right:
	if leav.right:
	    emptySub.right = Node(None, left = emptySub)
	    emptySub.up = leav.right
	    leav.right.down = emptySub
	    emptySub = emptySub.right
	    leav = leav.right
	else:
	    emptySub.up = Node(None, down = emptySub, left = leav)
	    leav.right = emptySub.up
	    emptySub = emptySub.up
	    leav = leav.up
    
    emptySub.up = Node(None, down = emptySub, left = leav)
    leav.right = emptySub.up


    
    return root







def main():
    if len(sys.argv) != 2:
	print "usage : python %s input_#.txt" % sys.argv[0]
        exit(0)

    input_name = sys.argv[1]
    if (not input_name.endswith('.txt')) or (not input_name.startswith('input_')):
	print "[ERROR] invalid file name format : %s" % input_name
	exit(0)
    input_no = input_name[6:-4]
    if (not input_no.isdigit()):
	print "[ERROR] invalid file name format : %s" % input_name
	exit(0)

    
    input_no = int(input_no)

    fin = open(input_name,'r')
    buf = fin.readline()
    buf = buf.split(' ')
    m = int(buf[0])
    n = int(buf[1])
    k = int(buf[2])
    commandBuf = []
    for i in range(k):
	buf = fin.readline()
	commandBuf.append(buf.strip().split(' '))
    #print commandBuf

    tmp_array = []
    for i in range(n):
	sub_array = []
	buf = fin.readline().strip().split(' ')
        for j in range(m):
	    if buf[j] == '#':
		break
	    else:
		sub_array.append(int(buf[j]))
	tmp_array.append(sub_array)

    fin.close()
    target = cons_from_array(tmp_array)
    
    for i in commandBuf:
	#print target
	if i[0] == 'E':
	    target = target.deleteOne()
	elif i[0] == 'I':
	    target = target.insertOne(int(i[1]))
    fout = open('output_%d.txt' % input_no, 'w')
    fout.write(target.printNode())
    fout.close()


if __name__ == "__main__":
    main()

