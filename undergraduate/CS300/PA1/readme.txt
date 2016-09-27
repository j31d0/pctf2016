Programming Assignment #1

20150349
박지희
Park ji hee

PA1.py에 Assignemnt에 해당하는 프로그램이 들어있다. main()에서 argmunet로 input과 output을 받으며,  input 이름이 "input_{number}.txt' 꼴이 아니면 실행하지 않고 종료된다.



알고리즘 설명

먼저 input array를 받아 각각의 숫자를 Node로 만들어 저장한다. 각 Node에는 up,down,left,right 가 있으며 경계에 있는 node에는 그 다음 값이 None으로 설정된다. (예_ [0][0] 위치에 있는 Node는 left,up이 None)
또한, 오른쪽 과 아래쪽의 경계에는 Dummy Node가 있다.

예를 들면, 
[[1,2,3],[4,5]]라는 array를 기준으로 tableau를 생성하면


      None None None
       |    |    |
None - 1 -  2 -  3  -  Dummy - None
       |    |    |       |
None - 4 -  5 - Dummy - Dummy - None
       |    |    |
None -Dummy-Dummy-Dummy -None
       |    |    |
     None  None  None


이렇게 linked Node가 생성되어, Dummy가 경계를 차지하게 된다.

extract를 수행할 때는, [0][0]의 위치에서 deleteOne을 실행하게 된다.  deleteOne은
  - 자신의 위치가 오른쪽 및 아래 경계일 경우 Dummy를 재설정해주고 자신을 Dummy로 바꾼 후 return
  - 자신의 위치의 오른쪽이나 밑에 Dummy가 아닌 Node가 있을 때는 둘 중의 작은 값과 자신을 swap한 후 바뀐 위치에서 deleteOne을 실행

 
 insert를 수행할 때는, Dummy Node 중 삽입될 곳을 먼저 찾는다. Dummy 중 up과 left가 Dummy가 아닐 경우 그 곳에서 insert_recursive를 실행하고, 아닌 경우 가로 크기와 세로 크기를 비교해 더 작은 쪽의 끝에서 insert_recursive를 실행한다.

 insert_recursive는, 자신의 위와 왼쪽에 있는 값을 비교해 더 큰 쪽의 값보다 자신이 작을 때,  자신을 swap한 후 바뀐 위치에서 다시 insert_recursive를 실행한다. recursion은 자신의 위와 왼쪽의 값이 모두 자신보다 작거나 [0][0]의 위치에 왔을 때 끝나게 된다.

 extract의 시간 복잡도는 한번의 함수 실행에 O(1)이 들고, 함수가 한번 실행될 때 마다 위치가 오른쪽으로 한번 또는 아래쪽으로 한번 이동하므로 최대 m+n만큼의 위치 이동이 지나면 무조건 Dummy의 위치로 오게 되어 recursion tree가 끝나게 된다. 따라서 extract는 O(m+n)이다.

 insert는 Dummy Node에서 위치를 찾는 것과 insert_recursive가 실행되는 시간을 더해야 하는데, 먼저 Dummy Node의 갯수가 최대 2(m+n)이기 때문에 모두 돌아보는데는 O(m+n)의 시간이 걸린다.
 insert_recursive를 수행하는 데에는, tableau의 임의의 점에서 시작하여 [0][0]까지 이동하는 데에는 extract와 비슷하게 최대 m+n번의 이동이 든다. 
 따라서 extract와 insert는 모두 O(m+n)의 시간복잡도를 가지게 된다.
