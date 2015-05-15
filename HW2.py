import xml.dom.minidom
import sys
import csv
import time
from matrixops import algorithm_Fl_Warh

def resist(inp, outp):
    dom = xml.dom.minidom.parse(inp)
    dom.normalize()
    # перейдем к schematics
    node = dom.childNodes[0]
    fl = 1
    u = v = t = t_inv = n = -7
    d = []
    k_nonet = 0
    for k in node.childNodes:
        st = k.attributes or {}
        name = k.nodeName
        if name == "net":
            for (x, y) in st.items():
                if x == "id":
                    n = int(y)
            # последний элемент с "net"
            k_nonet = k
    # заполним d
    for i in range(n):
        d.append([])
        for j in range(n):
            if i != j:
                d[i].append(float('+inf'))
            else:
                d[i].append(0)
    for k_nonet in node.childNodes:
        st = k_nonet.attributes or {}
        name = k_nonet.nodeName
        if name == "capactor" or name == "resistor":
            for (x, y) in st.items():
                if x == "resistance":
                    t = float(y)
                elif x == "net_from":
                    u = int(y) - 1
                elif x == "net_to":
                    v = int(y) - 1
            d[u][v] = d[v][u] = 1 / (1 / d[u][v] + 1 / t)
        elif name == "diode":
            for (x, y) in st.items():
                if x == "resistance":
                    t = float(y)
                elif x == "net_from":
                    u = int(y) - 1
                elif x == "net_to":
                    v = int(y) - 1
                elif x == "reverse_resistance":
                    t_inv = float(y)
            d[u][v] = 1 / (1 / d[u][v] + 1 / t)
            d[v][u] = 1 / (1 / d[v][u] + 1 / t_inv)
    # Теперь d заполнен начальными значениями
    start = time.time()
    dot = algorithm_Fl_Warh(d)
    finish = (time.time() - start)
    
    start2 = time.time()
    for m in range(n):
        for i in range(n):
            for j in range(n):
                summ = d[i][m] + d[m][j]
                if summ == 0 and d[i][j] == 0:
                    d[i][j] = 1 / (float("+inf"))
                elif summ == 0:
                    d[i][j] = 1 / (1 / d[i][j] + float("+inf"))
                elif d[i][j] == 0:
                    d[i][j] = 1 / (float("+inf") + 1 / summ)
                elif (1 / d[i][j] + 1 / summ) == 0:
                    d[i][j] = float("+inf")
                else:
                    d[i][j] = 1 / (1 / d[i][j] + 1 / summ)
    finish2 = (time.time() - start2)
    
    # Выведем отношение времени работы
    if finish!=0:
        print(finish2/finish)
    else:
        print("Time equals zero")
    
    # Вывод результатов в таблицу
    with open(outp, "w") as f:
        for line_n in range(n):
            for column_n in range(n-1):
                f.write(str(round(d[line_n][column_n], 6)) + ", ")
            f.write(str(round(d[line_n][n-1], 6)) + "\n")

if __name__ == "__main__":
    resist(sys.argv[1], sys.argv[2])
