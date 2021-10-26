groups = [[]]
with open(input("File: ")) as f:
    line = ""
    for line in f:
        if line == "\n":
            groups.append([])
        else:
            groups[-1].append(float(line))
   
for g in groups:
    print(sum(g)/len(g))
