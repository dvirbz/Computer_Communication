

def main(argv):
    if len(argv) != 2:
        print("USAGE: %s <correct output number>" % argv[0])
        return 1
    os.system(f"WFQ.exe < Inputs/input{argv[1]}.txt")
    path1 = f"./Outputs/output{argv[1]}.txt"
    path2 = "./output.txt"
    count = 0
    with open(path1) as f:
        data1 = f.readlines()
    with open(path2) as f:
        data2 = f.readlines()
    data2 = [data2[i] for i in range(len(data2)) if len(data2[i].split(' ')) > 6 and data2[i][0].isnumeric()]
    line_number = 0
    for line1, line2 in zip(data1, data2):
        line_number += 1
        if line1 != line2:
            count += 1
            print(f"line {line_number}: lines mismatch! lines are:")
            print(f"correct line: {line1}")
            print(f"output line: {line2}\n")
    if len(data1) > len(data2):
        print(f"output file missing {len(data1) - len(data2)} lines")
    if len(data1) < len(data2):
        print(f"output file has {len(data2) - len(data1)} excess lines")
    print("Done!"f" A Total of {count} mistakes!")


if __name__ == '__main__':
    import sys
    import os
    sys.exit(main(sys.argv))

