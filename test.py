import os


def get_percent(name1, name2):
    i1, i2 = 0, 0
    with open(name1, "r") as f1: 
        with open(name2, "r") as f2:         
            for i, j in enumerate(f1.readlines()):
                if "OUTPUT" in j:
                    i1 = i
                    break
            for i, j in enumerate(f2.readlines()):
                if "OUTPUT" in j:
                    i2 = i
                    break
    
    with open(name1, "r") as f1: 
        with open(name2, "r") as f2:         
            l1 = len(f1.readlines()) - i1
            l2 = len(f2.readlines()) - i2
            return max(l1 - l2, 0) * 100 // l1


def get_simplified_name(name):
    i = len(name) - 1
    while(i > 0 and name[i] != '/'):
        i -= 1
    return name[:i] + "/simplified/simplified_" + name[i + 1:]


def writeresults(l, file_name):
    with open(file_name, "w") as f1: 
        f1.writelines(l)


def calculate(mypath):
    l = []
    for (dirpath, dirnames, filenames) in os.walk(mypath + "/data"):
        l.extend(filenames)
        break
    
    res = []

    for j, i in enumerate(l):
        file_name = f"{mypath}/data/{i}"
        s_file_name = get_simplified_name(file_name)
        command = mypath + f'/src/build/main.exe "{file_name}"'
        os.system(command)
        per = str(get_percent(file_name, s_file_name))
        res.append(f"{i:20}" + ": Simplified by " + per + "%\n")
        print(f"{(j + 1) * 100 // len(l)}% complete")

    writeresults(res, "results.txt")


if __name__ == '__main__':
    # "D:/Programming/C++/Boolean-Schemes-Test"
    calculate("D:/Programming/C++/Boolean-Schemes-Test")