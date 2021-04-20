from random import randint
M = 100
s = int(input())
if(s == 1):
    for j in range(randint(1,M)):
        for k in range(randint(1,M)):
            print(randint(1,M),end=" ")
        print("")
else:
    # query data generation

    for i in range(M):
        print(randint(1,M))