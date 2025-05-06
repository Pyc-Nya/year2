#!/usr/bin/env python3
"""
Readers–Writers log visualizer.

Поместите файл в каталог, где лежат папки writers_logs/ и readers_logs/.
При запуске создаёт ./results/ и сохраняет 4 PNG‑графика.

Версия 2025‑05‑06: числовые оси теперь используют автоматический
целочисленный шаг, чтобы избежать предупреждения
'Locator attempting to generate ... ticks'.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path
from typing import DefaultDict, Dict, List, Tuple
from collections import defaultdict

import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator

# --------------------------------------------------------------------------- #
# -------------------------  НАСТРОЙКА ЛОКАТОРОВ  --------------------------- #
# --------------------------------------------------------------------------- #
def integer_locator(ax, axis: str = "y", *, max_ticks: int = 20) -> None:
    """
    Ставит на ось (x или y) локатор с целым шагом, подобранным так,
    чтобы число делений не превышало `max_ticks`.
    """
    lo, hi = ax.get_ylim() if axis == "y" else ax.get_xlim()
    rng = abs(hi - lo)
    if rng == 0:
        step = 1
    else:
        step = max(1, int(round(rng / max_ticks)))
    loc = MultipleLocator(step)
    (ax.yaxis if axis == "y" else ax.xaxis).set_major_locator(loc)


# --------------------------------------------------------------------------- #
# -----------------------------  ПАРСЕР  ------------------------------------ #
# --------------------------------------------------------------------------- #
BASE = Path(__file__).resolve().parent
WRITERS_DIR = BASE / "writers_logs"
READERS_DIR = BASE / "readers_logs"
RESULTS_DIR = BASE / "results"
RESULTS_DIR.mkdir(exist_ok=True)

WRITE_RE = re.compile(
    r"^WRITER_(?P<wid>\d{2})\s+WRITE\s+PAGE_(?P<page>\d+)\s+(?P<ts>\d+)$"
)
READ_RE = re.compile(
    r"^READER_(?P<rid>\d{2})\s+READ\s+PAGE_(?P<page>\d+)\s+(?P<ts>\d+)$"
)

Event = Tuple[str, int, int, int]  # (type, ts, page, writer)


def _parse_logs() -> List[Event]:
    events: List[Event] = []

    # writers
    for p in sorted(WRITERS_DIR.glob("writer_*.log")):
        for line in p.read_text(encoding="utf-8").splitlines():
            if (m := WRITE_RE.match(line)):
                events.append(
                    ("write", int(m["ts"]), int(m["page"]), int(m["wid"]))
                )

    # readers
    for p in sorted(READERS_DIR.glob("reader_*.log")):
        for line in p.read_text(encoding="utf-8").splitlines():
            if (m := READ_RE.match(line)):
                events.append(("read", int(m["ts"]), int(m["page"]), -1))

    if not events:
        sys.exit("Логи не содержат событий WRITE/READ.")

    events.sort(key=lambda e: (e[1], 0 if e[0] == "write" else 1))  # ts, type
    return events


# --------------------------------------------------------------------------- #
# -----------------------------  АНАЛИТИКА  --------------------------------- #
# --------------------------------------------------------------------------- #
def _build_stats(events: List[Event]):
    owner: Dict[int, int] = {}  # page → last writer
    total_reads: Dict[int, int] = defaultdict(int)
    per_writer_page: Dict[int, DefaultDict[int, int]] = defaultdict(
        lambda: defaultdict(int)
    )

    timeline = []  # (page, ts, type, writer)
    write_to_firstread: List[int] = []
    _last_write_ts: Dict[Tuple[int, int], int | None] = {}
    reads_after_write: Dict[Tuple[int, int, int], int] = defaultdict(int)

    for ev_type, ts, page, wid in events:
        if ev_type == "write":
            owner[page] = wid
            timeline.append((page, ts, "write", wid))
            _last_write_ts[(wid, page)] = ts
        else:
            writer = owner.get(page)
            if writer is None:
                continue
            total_reads[writer] += 1
            per_writer_page[writer][page] += 1
            timeline.append((page, ts, "read", writer))

            key = (writer, page)
            if key in _last_write_ts and _last_write_ts[key] is not None:
                dt = ts - _last_write_ts[key]
                write_to_firstread.append(dt)
                reads_after_write[(writer, page, _last_write_ts[key])] += 1
                _last_write_ts[key] = None

    return (
        total_reads,
        per_writer_page,
        timeline,
        write_to_firstread,
        reads_after_write,
    )


# --------------------------------------------------------------------------- #
# -----------------------------  ГРАФИКИ  ----------------------------------- #
# --------------------------------------------------------------------------- #
def _plot_total_reads(total_reads: Dict[int, int]):
    labels, values = zip(*sorted(total_reads.items(), key=lambda kv: -kv[1]))
    fig, ax = plt.subplots()
    ax.bar(range(len(labels)), values)
    ax.set_xticks(range(len(labels)))
    ax.set_xticklabels([f"{w:02d}" for w in labels])
    integer_locator(ax, "y")  # целые деления по Y
    ax.set_xlabel("Writer ID")
    ax.set_ylabel("Total READs")
    ax.set_title("Total READs per Writer")
    plt.tight_layout()
    plt.savefig(RESULTS_DIR / "total_reads_by_writer.png")
    plt.close()


def _plot_heatmap(per_writer_page: Dict[int, DefaultDict[int, int]]):
    import numpy as np
    import matplotlib.ticker as mticker
    
    writers: list[int] = sorted(per_writer_page.keys())
    pages:   list[int] = sorted({p for v in per_writer_page.values() for p in v})
    data = np.zeros((len(writers), len(pages)), dtype=int)

    for i, w in enumerate(writers):
        for j, p in enumerate(pages):
            data[i, j] = per_writer_page[w][p]

    # --- построение графика --------------------------------------------------
    fig, ax = plt.subplots()
    im = ax.imshow(data, aspect="auto")
    
    # подписи осей
    ax.set_yticks(range(len(writers)), labels=[f"{w:02d}" for w in writers])
    ax.set_xticks(range(len(pages)),   labels=pages)
    ax.set_xlabel("Page")
    ax.set_ylabel("Writer ID")
    ax.set_title("READs Heatmap (Writer × Page)")

    # цветовая шкала с целыми делениями
    cbar = fig.colorbar(im, ax=ax, label="READs")
    cbar.ax.yaxis.set_major_locator(mticker.MaxNLocator(integer=True))

    plt.tight_layout()
    plt.savefig(RESULTS_DIR / "page_heatmap.png", dpi=300)
    plt.close(fig)


def _plot_timeline(timeline: List[Tuple[int, int, str, int]]):
    pages = sorted({p for p, *_ in timeline})
    fig, ax = plt.subplots(figsize=(10, 0.5 * len(pages)))
    # один проход без вложенных циклов
    for p, ts, typ, _ in timeline:
        y = pages.index(p)
        ax.plot(
            ts,
            y,
            marker="s" if typ == "write" else ".",
            linestyle="",
        )
    ax.set_yticks(range(len(pages)))
    ax.set_yticklabels(pages)
    ax.yaxis.set_major_locator(MultipleLocator(1))
    ax.set_xlabel("Time (ms since boot)")
    ax.set_ylabel("Page")
    ax.set_title("Timeline of WRITE (■) and READ (•)")
    plt.tight_layout()
    plt.savefig(RESULTS_DIR / "timeline_page_events.png")
    plt.close()


# --------------------------------------------------------------------------- #
# --------------------------------  MAIN  ----------------------------------- #
# --------------------------------------------------------------------------- #
def main():
    events = _parse_logs()
    total_reads, per_writer_page, timeline, _, _ = _build_stats(events)

    _plot_total_reads(total_reads)
    _plot_heatmap(per_writer_page)
    _plot_timeline(timeline)

    print(f"Графики сохранены в {RESULTS_DIR.relative_to(BASE)}")


if __name__ == "__main__":
    main()
