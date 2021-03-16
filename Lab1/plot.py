import matplotlib.pyplot as plt

n = int(input())
d = dict()
for i in range(n):
    s = int(input())
    d[s] = d.get(s,0) + 1

keys = list(d.keys())
values = list(d.values())
plt.bar(keys, values,color='maroon')
plt.show()
