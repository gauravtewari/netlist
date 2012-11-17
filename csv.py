#!/usr/bin/python
bins = [];
with open("b15_C.out") as myfile:
    max = int(myfile.readline());
    for x in range(max+1):
        bins.append(int(myfile.readline()));

print(*bins, sep=", ", end='\n');

with open("b15_C.csv", "w") as myfile:
    for x in range(max+1):
        print(x, ", ", bins[x], file=myfile);



