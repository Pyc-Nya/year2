#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Шаг 1. Анализ логов → JSON + PNG с границами доверительного интервала ±3σ
--------------------------------------------------------------
• модели степеней 0…4; F‑тест; «фиктивный» старший член;
• графики для deg=1,2 с доверительным интервалом ±3σ;
• модельные данные → model_results.json.
"""
from __future__ import annotations
import json, os, math, numpy as np, pandas as pd, statsmodels.api as sm
from scipy.stats import f as f_dist
import matplotlib.pyplot as plt
from typing import Final, TypedDict, List

# ---------- настройка ----------
LOG_PATH:    Final[str] = r'/home/pyc_nya/projects/vuz2/sem4/aisd/lab3/result.log'
OUT_DIR:     Final[str] = r'/home/pyc_nya/projects/vuz2/sem4/aisd/lab3/report/report_results'
EPS_RATIO:   Final[float] = 0.02
PNG_WIDTH:   Final[int] = 600         # в пикселях
# --------------------------------

class ModelDict(TypedDict):
    degree: int
    coef:   List[float]
    var:    float
    sd:     float
    code:   str
    comment:str


def assess_spurious(coef: list[float], eps: float = EPS_RATIO) -> str:
    nz = [abs(c) for c in coef if c]
    if len(nz) <= 1:
        return ''
    a_k, a_prev = nz[-1], max(nz[:-1])
    if a_prev and a_k / a_prev < eps:
        return (f"коэффициент при n^{len(nz)-1} всего {a_k/a_prev:.2%} "
                f"от предыдущих — вероятно накладные расходы")
    return ''


def analyse_operation(group: pd.DataFrame) -> list[ModelDict]:
    x, y = group['size'].values, group['time_ns'].values
    N, prev_rss, models = len(y), None, []
    for deg in range(5):
        X = np.vander(x, deg+1, increasing=True)
        model = sm.OLS(y, X).fit()
        rss = model.ssr
        df_res = N - (deg + 1)
        var, sd = rss / df_res, math.sqrt(rss / df_res)
        coef = list(model.params) + [0]*(5-deg-1)

        if deg == 0:
            code = '--'
        else:
            F = ((prev_rss - rss) / 1) / (rss / df_res)
            F05, F01 = f_dist.ppf(0.95, 1, df_res), f_dist.ppf(0.99, 1, df_res)
            code = ('+' if F > F05 else '-') + ('+' if F > F01 else '-')
        if deg and coef[deg] < 0:
            code = '*'+code

        models.append(ModelDict(
            degree = deg,
            coef   = coef,
            var    = var,
            sd     = sd,
            code   = code,
            comment= assess_spurious(coef)
        ))
        prev_rss = rss
    return models


def save_plot(x: np.ndarray, y: np.ndarray, models: list[ModelDict], op: str) -> str:
    xs = np.linspace(x.min(), x.max(), 200)
    plt.figure(figsize=(PNG_WIDTH/100, PNG_WIDTH/100 * 0.75))
    plt.scatter(x, y, s=20, label='Измерения')
    # Строим линии регрессии и интервалы ±3σ для deg=1 и deg=2
    for m in models:
        if m['degree'] in (1, 2):
            # предсказанное значение
            ys = sum(m['coef'][i] * xs**i for i in range(m['degree']+1))
            # границы доверительного интервала ±3σ
            band = 3 * m['sd']
            lower = ys - band
            upper = ys + band
            plt.plot(xs, ys, label=f'deg={m["degree"]}')
            plt.fill_between(xs, lower, upper, alpha=0.2,
                             label=f'±3σ (deg={m["degree"]})')
    plt.title(op)
    plt.xlabel('Размер n')
    plt.ylabel('Время (ns)')
    plt.legend()
    plt.tight_layout()
    fn = f"{op}.png"
    plt.savefig(os.path.join(OUT_DIR, fn), dpi=150)
    plt.close()
    return fn


def main() -> None:
    df = pd.read_csv(LOG_PATH, sep=r"\s+", names=['time_ns','op','size'], engine='python')
    report: dict[str, dict] = {}
    for op, group in df.groupby('op'):
        models = analyse_operation(group)
        img = save_plot(group['size'].values, group['time_ns'].values, models, op)
        report[op] = {'models': models, 'image': img}
    with open(os.path.join(OUT_DIR, 'model_results.json'), 'w', encoding='utf-8') as f:
        json.dump(report, f, ensure_ascii=False, indent=2)
    print("JSON + PNG готовы с границами доверительного интервала.")

if __name__ == "__main__":
    main()