import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Укажите путь к вашему файлу log
file_path = r"/home/pyc_nya/projects/vuz2/sem4/os/lab1/linux/lab.log"  # Путь к вашему файлу лога

# Загрузка данных из файла
df = pd.read_csv(file_path, sep=" ", skiprows=1, header=None, 
                 names=["Timestamp", "File_Size", "Cluster_Size", "Operations_Count", "Duration"])

# Уникальные размеры файлов в данных
file_sizes = df["File_Size"].unique()

# Создание директории для сохранения графиков
output_dir = r"/home/pyc_nya/projects/vuz2/sem4/os/lab1/linux/graphs"
os.makedirs(output_dir, exist_ok=True)

# Максимальное количество кластерных размеров на одном графике
max_clusters_per_plot = 6

# Генерация графиков для каждого размера файла
for file_size in file_sizes:
    df_filtered = df[df["File_Size"] == file_size]
    cluster_sizes = sorted(df_filtered["Cluster_Size"].unique())

    # Разбиваем размеры кластеров на группы по max_clusters_per_plot элементов
    cluster_groups = [cluster_sizes[i:i + max_clusters_per_plot] for i in range(0, len(cluster_sizes), max_clusters_per_plot)]

    for group_index, cluster_group in enumerate(cluster_groups):
        df_grouped = df_filtered[df_filtered["Cluster_Size"].isin(cluster_group)].groupby(["Cluster_Size", "Operations_Count"])["Duration"].mean().reset_index()

        fig, ax = plt.subplots(figsize=(10, 6))
        for cluster_size in cluster_group:
            subset = df_grouped[df_grouped["Cluster_Size"] == cluster_size]
            ax.plot(subset["Operations_Count"], subset["Duration"], marker="o", label=f"Cluster: {cluster_size}")

        ax.set_xscale("log", base=2)  # Логарифмическая шкала по оси X
        ax.set_xlabel("Количество операций (Operations Count)")
        ax.set_ylabel("Среднее время выполнения (мс)")
        ax.set_title(f"Зависимость времени выполнения от количества операций\nРазмер файла: {file_size} байт (Группа {group_index + 1})")
        ax.legend()
        ax.grid(True)

        # Сохранение графика
        output_path = os.path.join(output_dir, f"performance_{file_size}_group{group_index + 1}.png")
        plt.savefig(output_path)
        plt.close(fig)

print(f"Графики сохранены в папке: {output_dir}")