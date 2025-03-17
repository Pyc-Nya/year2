#!/usr/bin/env python3
import os
import pandas as pd
import matplotlib.pyplot as plt

# Путь к лог-файлу
file_path = r"/home/pyc_nya/projects/vuz2/sem4/os/lab1/linux/lab.log"

# Загрузка данных из файла.
# Предполагается, что каждая строка лога имеет формат:
# [YYYY-MM-DD HH:MM:SS] File_Size Cluster_Size Operations_Count Duration
# При этом строка разбивается по пробелам на 6 токенов: "[YYYY-MM-DD", "HH:MM:SS]", "File_Size", "Cluster_Size", "Operations_Count", "Duration"
df = pd.read_csv(file_path, sep=" ", header=None,
                 names=["Date", "Time", "File_Size", "Cluster_Size", "Operations_Count", "Duration"])

# Приведение числовых столбцов к типу numeric (в случае, если обнаружатся ошибки преобразования — будут заменены на NaN)
df["File_Size"] = pd.to_numeric(df["File_Size"], errors='coerce')
df["Cluster_Size"] = pd.to_numeric(df["Cluster_Size"], errors='coerce')
df["Operations_Count"] = pd.to_numeric(df["Operations_Count"], errors='coerce')
df["Duration"] = pd.to_numeric(df["Duration"], errors='coerce')

# Удаление строк с отсутствующими значениями в числовых полях
df = df.dropna(subset=["File_Size", "Cluster_Size", "Operations_Count", "Duration"])

# Создание директории для сохранения графиков
output_dir = r"/home/pyc_nya/projects/vuz2/sem4/os/lab1/linux/graphs2"
os.makedirs(output_dir, exist_ok=True)

# Обработка данных для каждого уникального размера файла
for file_size in df["File_Size"].unique():
    df_filtered = df[df["File_Size"] == file_size]
    
    # График 1: Время vs Размер кластера
    # Группируем данные по 'Cluster_Size' и вычисляем среднее время выполнения
    df_cluster = df_filtered.groupby("Cluster_Size")["Duration"].mean().reset_index()
    
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(df_cluster["Cluster_Size"], df_cluster["Duration"], marker="o", linestyle="-")
    ax.set_xlabel("Размер кластера (байт)")
    ax.set_ylabel("Среднее время выполнения (мс)")
    ax.set_title(f"Время vs Размер кластера\nРазмер файла: {file_size} байт")
    ax.grid(True)
    
    output_path1 = os.path.join(output_dir, f"duration_vs_cluster_{file_size}.png")
    plt.savefig(output_path1)
    plt.close(fig)
    
    # График 2: Время vs Количество операций
    # Группируем данные по 'Operations_Count' и вычисляем среднее время выполнения
    df_ops = df_filtered.groupby("Operations_Count")["Duration"].mean().reset_index()
    
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(df_ops["Operations_Count"], df_ops["Duration"], marker="o", linestyle="-")
    ax.set_xscale("log", base=2)  # Логарифмическая шкала по оси X с основанием 2
    ax.set_xlabel("Количество операций")
    ax.set_ylabel("Среднее время выполнения (мс)")
    ax.set_title(f"Время vs Количество операций\nРазмер файла: {file_size} байт")
    ax.grid(True)
    
    output_path2 = os.path.join(output_dir, f"duration_vs_operations_{file_size}.png")
    plt.savefig(output_path2)
    plt.close(fig)

print(f"Графики сохранены в папке: {output_dir}")
