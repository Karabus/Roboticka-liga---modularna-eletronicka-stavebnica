import os

# priečinok so súbormi (môžeš zmeniť na konkrétnu cestu)
folder = "White/N x M - L"

for filename in os.listdir(folder):
    if filename.endswith(". s t l"):
        parts = filename.split(" ")
        print(parts)
        new_name = "".join(parts)
        old_path = os.path.join(folder, filename)
        new_path = os.path.join(folder, new_name)

        os.rename(old_path, new_path)
        print(f"Premenované: {filename} -> {new_name}")