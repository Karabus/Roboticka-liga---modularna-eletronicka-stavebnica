input_path = r"Celkove pocty.txt"

text=''
pocet = 0
with open(input_path, "r", encoding="utf-8") as f:
    for line in f:
        line = line.strip().split(" - ")
        print(line)
        if len(line)<2:
            continue
        else:
            print(line[3][:-1])
            if line[3][:-1].isdigit():
                pocet += int(line[3][:-1])

print("Counted pocet =", pocet)