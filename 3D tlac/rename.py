import os

# priečinok so súbormi (môžeš zmeniť na konkrétnu cestu)
folder = "White/Arm"

for filename in os.listdir(folder):
    if filename.endswith(".stl"):
        parts = filename.split("_")

        if len(parts) >= 3:
            new_name = parts[-1]  # vezme poslednú časť (napr. C.stl)

            old_path = os.path.join(folder, filename)
            new_path = os.path.join(folder, new_name)

            os.rename(old_path, new_path)
            print(f"Premenované: {filename} -> {new_name}")