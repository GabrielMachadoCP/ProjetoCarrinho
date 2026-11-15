#!/usr/bin/env python3
"""Coleta a telemetria serial do DrenaBot e adiciona labels reais ao CSV."""

from __future__ import annotations

import argparse
import csv
import time
from pathlib import Path

import serial


LABELS = {"NORMAL": 0, "ATENCAO": 1, "CRITICO": 2}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Coleta dados reais do DrenaBot")
    parser.add_argument("--port", required=True, help="Ex.: COM3, /dev/ttyUSB0")
    parser.add_argument("--label", required=True, choices=LABELS)
    parser.add_argument("--seconds", type=int, default=60)
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("tinyml/data/drenahub_real.csv"),
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    exists = args.output.exists()
    deadline = time.monotonic() + args.seconds
    collected = 0

    with serial.Serial(args.port, args.baud, timeout=1) as device, args.output.open(
        "a", encoding="utf-8", newline=""
    ) as output:
        writer = csv.writer(output, lineterminator="\n")
        if not exists:
            writer.writerow(["water_level", "rise_rate", "obstacle_risk", "label", "class_name"])

        while time.monotonic() < deadline:
            line = device.readline().decode("utf-8", errors="ignore").strip()
            if not line.startswith("telemetry,"):
                continue
            parts = line.split(",")
            if len(parts) != 8:
                continue
            _, _, _, water_level, rise_rate, _, obstacle_risk, _ = parts
            writer.writerow(
                [water_level, rise_rate, obstacle_risk, LABELS[args.label], args.label]
            )
            output.flush()
            collected += 1
            print(f"{collected:04d} | {args.label} | agua={water_level} subida={rise_rate} obst={obstacle_risk}")

    print(f"Coleta concluida: {collected} linhas em {args.output}")


if __name__ == "__main__":
    main()
