import sys

def main():
        inputFileName = sys.argv[1]
        outputFileName = inputFileName.replace('input', 'output')

        #########################################
        # TODO: Implement dynamic programming to
        # minimize the sum over all lines of square of
        # the number of white spaces at the end of the line.
        #########################################
        inputFile = open(inputFileName, 'r')
        n,p = map(int, inputFile.readline().split(' '))
        l= map(int, inputFile.readline().split(' ')) #length of each word

        A = [-1] * (n+1) # A[i] is optimal cost by using only words with
                         # 1, 2, ... i

        A[0] = 0

        for i in range(1,n+1,1):
                ll = i 
                dd = l[ll - 1]
                while dd < p and ll > 0:
                        # so, A[i] = minimum of
                        # {A[j] + (p - (l[j] + l[j+1] + .... l[i] + (i-j)))}
                        # where (sum of l[j] < p)
                        if A[i] == -1 or A[i] > A[ll- 1] + (p - dd):
                                A[i] = A[ll - 1] + (p - dd)
                        ll -= 1
                        dd += 1 + l[ll - 1]


        outputFile = open(outputFileName,'w')
        outputFile.write(str(A[n]))
        outputFile.close()

if __name__ == "__main__":
         main()
