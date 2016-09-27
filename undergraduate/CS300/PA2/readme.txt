Programming Assignment #2

20150349
박지희
Park ji hee

PA2_20150349.py에 Assignment에 해당하는 프로그램이 들어있다. main()에서 argument로 input 이름을 받고 output 파일에 답을 출력한다. Python 2.7 버전을 사용하였다.

알고리즘 설명

문제는 단어의 갯수 n, 최대 가로 길이 p, 각 단어의 길이 l[0],l[1],...l[n-1] 이 있을 때, cost가 가장 작게 단어를 배치하는 것이다. 

문제의 subproblem을 잡아 그 optimal을 먼저 구하고, 다음으로 원래 문제의 해를 구하기로 하였다.

최적해 C = f(n,p,l) 이라고 할 때, 마지막 줄에 있는 단어들이 i ~ n 라고 하면 C - (p - (l[i] + l[i+1] + ... + l[n] + (n-i)) = f(n-i,p,l / {l[i], ... ,l[n]}) 이라는 subproblem optimality를 이용하여 다이나믹 프로그래밍으로 문제를 해결하였다.

따라서 subproblem이 모두 알려져 있다고 가정할 때, 
f(n,p,l)은 minimum of {f(n-i,p,l / {l[i], ... ,l[n]}) + (p - (l[i] + l[i+1] + ... + l[n] + (n-i))} 이라는 것을 알 수 있고, 이를 DP로 구현하였다.

Time complexity

내부 while문이 ll > 0 일때까지 실행될 수 있기 때문에 최대 O(n)이고, 밖의 for 문이 n번 수행되므로 Time complexity는
f(n) = O(n)*n = O(n^2)이다. 