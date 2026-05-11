import os
from multiprocessing import process

input_path = r"C:\Users\shorv\Downloads\3D tlac\Celkove pocty.txt"
output_file = r"C:\Users\shorv\Downloads\3D tlac\Celkove pocty.txt"

vysledky = []

'''
for file in os.listdir(folder_path):
    if file.lower().endswith(".stl"):
        # hľadanie vzoru napr. 1x10, 2x4 atď.
        oldFilenameList = file.split("_")
        newFilename = oldFilenameList[-1]
        os.rename(os.path.join(folder_path, file), os.path.join(folder_path, newFilename))
        riadok = f"{str(newFilename)} - 40x - biela"
        vysledky.append(riadok)


# (voliteľné) zoradenie
vysledky.sort()

print(vysledky)
with open(output_file, "a", encoding="utf-8") as f:
    for r in vysledky:
        f.write(r + "\n")

print(f"Hotovo, zapísaných {len(vysledky)} riadkov.")
'''
'''
text = []
pocet = 0
with open(input_path, "r", encoding="utf-8") as f:
    for line in f:
        line = line.strip().split(" - ")
        if len(line)>1:
            line[-1] = "zlta"
        text.append(" - ".join(line))

with open(output_file, "w", encoding="utf-8") as f:
    for r in text:
        f.write(r + "\n")
        pocet += 1
'''
'''text = []
pocet = 0
with open(input_path, "r", encoding="utf-8") as f:
    for line in f:
        line = line.strip().split(" - ")
        print(line)
        if len(line)<2:
            line.append("\n")
            text.append("".join(line))
        else:
            x = int(line[1][:-1])//20
            line.append(str(x)+"x")
            text.append(" - ".join(line))

with open(output_file, "w", encoding="utf-8") as f:
    f.write("Nazov - celkovy pocet - farba - pocet pre stavebnica")
    for r in text:
        f.write(r + "\n")
        pocet += 1
'''
pocet = 0
with open(input_path, "r", encoding="utf-8") as f:
    for line in f:
        line = line.strip().split(" - ")
        print(line)
        if len(line)>2 and line[-1][:-1].isnumeric():
            pocet += int(line[-1][:-1])

print(f"Hotovo, zapísaných {pocet} riadkov.")