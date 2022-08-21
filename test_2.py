import os


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
    for (dirpath, dirnames, filenames) in os.walk(mypath + "/data_2"):
        l.extend(filenames)
        break
    

    for j, i in enumerate(l):
        file_name = f"{mypath}/data_2/{i}"
        s_file_name = get_simplified_name(file_name)
        command1 = mypath + f'/src/build/main.exe "{file_name}"'
        command2 = mypath + f'/src/build/main.exe "{s_file_name}"'
        print(file_name.split("/")[-1])
        print("Time for not simplified (microseconds)")
        os.system(command1)
        print("Time for simplified (microseconds)")
        os.system(command2)
        
        print(f"{(j + 1) * 100 // len(l)}% complete")



if __name__ == '__main__':
    # сюда нужно подавать на вход корневую директорию проекта
    # "D:/Programming/C++/Boolean-Schemes-Test"
    calculate(input())

# py -u "d:\Programming\C++\Boolean-Schemes-Test\test_2.py" > "D:\Programming\C++\Boolean-Schemes-Test\time_results.txt"