import numpy as np
import scipy.stats as ss

alpha = 0.05

test_names = [
    "a single large opaque rectangle, composited using \\textsf{src\\_over}",
    "a single large opaque rectangle",
    "a single large transparent rectangle",
    "two overlapping opaque rectangles",
    "two overlapping transparent rectangles",
    "two overlapping opaque rectangles with a small amount of text in one",
    "two overlapping transparent rectangles with a small amount of text in one",
    "two overlapping transparent rectangles with a small amount of text in both",
    "two overlapping transparent rectangles with a large amount of text in one",
    "two overlapping transparent rectangles with a large amount of text in both"
]

compositor_names = [
    "base",
    "special_cases",
    "fewer_memory_accesses",
    "premultiplied",
    "float_division",
    "shift_division",
    "compiler_flags",
    "encapsulated",
    "std_copy",
    "base_new",
    "inlined_src_over",
    "inlined_src_over_drm",
    "inlined_src_over_no_drm",
]

comparisons = [
    ("base", "special_cases"),
    ("special_cases", "fewer_memory_accesses"),
    ("fewer_memory_accesses", "premultiplied"),
    ("premultiplied", "float_division"),
    ("premultiplied", "shift_division"),
    ("shift_division", "compiler_flags"),
    ("base", "compiler_flags"),
    ("compiler_flags", "encapsulated"),
    ("encapsulated", "std_copy"),
    ("std_copy", "base_new"),
    ("base_new", "inlined_src_over"),
    ("inlined_src_over", "inlined_src_over_drm"),
    ("inlined_src_over_drm", "inlined_src_over_no_drm"),
]


for (comp1, comp2) in comparisons:
    print("% Comparing", comp2, "to", comp1)
    tests1 = [[]]
    tests2 = [[]]
    with open(comp1) as f:
        line = ""
        for line in f:
            if line == "\n":
                tests1.append([])
            else:
                tests1[-1].append(float(line))

    with open(comp2) as f:
        line = ""
        for line in f:
            if line == "\n":
                tests2.append([])
            else:
                tests2[-1].append(float(line))

    comp_no1 = compositor_names.index(comp1)+1
    comp_no2 = compositor_names.index(comp2)+1

    print("     \\begin{table}[H]")
    print("      \\centering")
    print("      \\begin{tabular}{|c|c|c|c|c|c|c|c|}")
    print("       \\hline")
    print("       Test & $\\mu_{:n}$ & $\\mu_{:n}$ & $\\sigma_{:n}$ & $\\sigma_{:n}$ & t & p & Conclusion \\\\".format(comp_no1, comp_no2, comp_no1, comp_no2))
    print("       \\hline")

    for i in range(len(tests1)):
        t1 = tests1[i]
        t2 = tests2[i]
        name = test_names[i]
        mu1 = np.mean(t1)
        mu2 = np.mean(t2)
        sigma1 = np.std(t1, ddof=1)
        sigma2 = np.std(t2, ddof=1)
        t, p = ss.ttest_ind(t1, t2, equal_var=False, alternative="greater")
        conclusion = "reject" if p <= alpha else "accept"
        print("       {:n} & {:0.5f} & {:0.5f} & {:0.5f} & {:0.5f} & {:0.5f} & {:0.5f} & {:s} $H_0$ \\\\".format(i, mu1, mu2, sigma1, sigma2, t, p, conclusion))

    print("       \\hline")
    print("      \\end{tabular}")
    print("     \\end{table}")
