import re
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Путь к файлу с логами
LOG_FILE = r'C:\projects\year2\sem4\os\lab3\win\result2.log'

# Шаблон для разбора строк вида "Threads: <T>, Time: <sec>"
pattern = re.compile(r'Threads:\s*(\d+),\s*Time:\s*([\d.]+)')

# Собираем все замеры
threads = []
times   = []
with open(LOG_FILE, 'r') as f:
    for line in f:
        m = pattern.search(line)
        if m:
            threads.append(int(m.group(1)))
            times.append(float(m.group(2)))

# Создаём DataFrame
df = pd.DataFrame({'threads': threads, 'time': times})

# Группируем по числу потоков и считаем среднее и σ
stats = df.groupby('threads')['time'].agg(['mean', 'std']).reset_index()

# Данные для регрессии
x = stats['threads'].values
y = stats['mean'].values

# Рисуем график
plt.figure(figsize=(8, 5))
plt.errorbar(
    x, y,
    yerr=stats['std'],
    fmt='o-', capsize=5,
    label='Среднее время ±σ'
)

plt.xlabel('Число потоков')
plt.ylabel('Время выполнения (с)')
plt.title('Время выполнения')
plt.xticks(x, rotation=45)
plt.grid(True)
plt.legend(loc='best')
plt.tight_layout()

plt.show()
