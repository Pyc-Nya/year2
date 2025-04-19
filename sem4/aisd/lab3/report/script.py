import matplotlib.pyplot as plt
import collections

# Путь к лог-файлу
log_file = r'C:\projects\year2\sem4\aisd\lab3\result.log'

# Структура для хранения данных: {operation: {size: [times]}}
data = collections.defaultdict(lambda: collections.defaultdict(list))

# Чтение и парсинг лог-файла
with open(log_file, 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) != 3:
            continue  # пропустить некорректные строки
        time_ns, operation, size = int(parts[0]), parts[1], int(parts[2])
        data[operation][size].append(time_ns)

# Построение графиков
plt.figure(figsize=(12, 8))
for operation, size_times in data.items():
    sizes = sorted(size_times.keys())
    avg_times = [sum(size_times[s]) / len(size_times[s]) for s in sizes]
    plt.plot(sizes, avg_times, label=operation)

plt.title("Зависимость времени выполнения операций от размера множества")
plt.xlabel("Размер множества")
plt.ylabel("Среднее время (наносекунды)")
plt.legend()
plt.grid(True)
plt.tight_layout()

# Сохранение в PNG-файл
plt.savefig(r'C:\projects\year2\sem4\aisd\lab3\report\report_results\graph.png', dpi=300)
