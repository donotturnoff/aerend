groups = [[]]
with open(input("File: ")) as f:
    line = ""
    for line in f:
        if line == "\n":
            groups.append([])
        else:
            groups[-1].append(float(line))
   
for i in range(1, len(groups)):
    g = groups[i]
    print("       {:n} & {:0.5f} & {:0.5f} \\\\".format(i, sum(g)/len(g), np.std(g, ddof=1)))
