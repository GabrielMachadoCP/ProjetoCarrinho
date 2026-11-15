#!/usr/bin/env python3
"""Treina e exporta o classificador TinyML de risco do DrenaHub.

O conjunto inicial e sintetico e reproduz uma politica de risco documentada.
Ele existe para que o firmware, o pipeline e a demonstracao possam ser
reproduzidos antes da calibracao fisica. Antes da entrega, o grupo deve coletar
leituras reais do sensor e executar novamente este script com os dados reais.
"""

from __future__ import annotations

import argparse
import csv
import json
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from sklearn.metrics import (
    ConfusionMatrixDisplay,
    accuracy_score,
    classification_report,
    confusion_matrix,
)
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier
from sklearn.preprocessing import StandardScaler


ROOT = Path(__file__).resolve().parents[1]
DATA_DIR = ROOT / "tinyml" / "data"
MODEL_DIR = ROOT / "tinyml" / "model"
EVIDENCE_DIR = ROOT / "tinyml" / "evidencias"
HEADER_PATH = ROOT / "firmware" / "DrenaHub" / "drenahub_model.h"

RANDOM_SEED = 42
CLASS_NAMES = ["NORMAL", "ATENCAO", "CRITICO"]


def policy_label(level: np.ndarray, rise: np.ndarray, obstacle: np.ndarray) -> np.ndarray:
    """Politica usada para rotular o baseline sintetico.

    level: nivel de agua normalizado entre 0 e 1.
    rise: velocidade de crescimento positiva, normalizada entre 0 e 1.
    obstacle: risco de colisao, normalizado entre 0 e 1.
    """

    score = 0.78 * level + 0.17 * rise + 0.05 * obstacle
    labels = np.zeros(level.shape[0], dtype=np.int64)
    labels[(level >= 0.25) | (score >= 0.30)] = 1
    labels[(level >= 0.70) | (score >= 0.64)] = 2
    return labels


def make_dataset(samples: int = 3600) -> tuple[np.ndarray, np.ndarray]:
    rng = np.random.default_rng(RANDOM_SEED)
    level = rng.uniform(0.0, 1.0, samples)
    rise = rng.beta(1.8, 4.0, samples)
    obstacle = rng.uniform(0.0, 1.0, samples)

    # Pequeno ruido representa variacao do ADC e oscilacao mecanica do robo.
    level = np.clip(level + rng.normal(0.0, 0.015, samples), 0.0, 1.0)
    rise = np.clip(rise + rng.normal(0.0, 0.010, samples), 0.0, 1.0)

    x = np.column_stack([level, rise, obstacle]).astype(np.float32)
    y = policy_label(level, rise, obstacle)
    return x, y


def load_dataset(path: Path) -> tuple[np.ndarray, np.ndarray]:
    required = {"water_level", "rise_rate", "obstacle_risk", "label"}
    with path.open("r", encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle)
        if reader.fieldnames is None or not required.issubset(reader.fieldnames):
            raise ValueError(f"CSV precisa conter as colunas: {sorted(required)}")
        rows = list(reader)

    if len(rows) < 90:
        raise ValueError("Use pelo menos 90 amostras reais, com as tres classes representadas.")

    x = np.array(
        [
            [float(row["water_level"]), float(row["rise_rate"]), float(row["obstacle_risk"])]
            for row in rows
        ],
        dtype=np.float32,
    )
    y = np.array([int(row["label"]) for row in rows], dtype=np.int64)
    if set(np.unique(y).tolist()) != {0, 1, 2}:
        raise ValueError("O CSV real deve conter os labels 0, 1 e 2.")
    return x, y


def write_dataset(path: Path, x: np.ndarray, y: np.ndarray) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle, lineterminator="\n")
        writer.writerow(["water_level", "rise_rate", "obstacle_risk", "label", "class_name"])
        for features, label in zip(x, y):
            writer.writerow([*(f"{value:.6f}" for value in features), int(label), CLASS_NAMES[int(label)]])


def c_array(name: str, values: np.ndarray, indent: str = "") -> str:
    flat = values.flatten()
    joined = ", ".join(f"{float(value):.9f}f" for value in flat)
    return f"{indent}static const float {name}[{len(flat)}] = {{{joined}}};"


def export_header(
    path: Path,
    scaler: StandardScaler,
    model: MLPClassifier,
    metrics: dict,
) -> None:
    w1 = model.coefs_[0]  # [input][hidden]
    b1 = model.intercepts_[0]
    w2 = model.coefs_[1]  # [hidden][output]
    b2 = model.intercepts_[1]

    lines = [
        "#pragma once",
        "",
        "// Arquivo gerado por tinyml/train_drenahub_model.py.",
        f"// Dataset: {metrics['dataset']}.",
        f"// Accuracy no conjunto de teste: {metrics['accuracy']:.4f}",
        "",
        "#include <math.h>",
        "",
        "namespace DrenaHubModel {",
        "",
        "constexpr int INPUT_COUNT = 3;",
        f"constexpr int HIDDEN_COUNT = {w1.shape[1]};",
        "constexpr int OUTPUT_COUNT = 3;",
        "",
        c_array("SCALER_MEAN", scaler.mean_),
        c_array("SCALER_SCALE", scaler.scale_),
        c_array("W1", w1),
        c_array("BIAS1", b1),
        c_array("W2", w2),
        c_array("BIAS2", b2),
        "",
        "inline float clamp01(float value) {",
        "  if (value < 0.0f) return 0.0f;",
        "  if (value > 1.0f) return 1.0f;",
        "  return value;",
        "}",
        "",
        "inline int predict(const float input[INPUT_COUNT], float probabilities[OUTPUT_COUNT]) {",
        "  float normalized[INPUT_COUNT];",
        "  float hidden[HIDDEN_COUNT];",
        "  float logits[OUTPUT_COUNT];",
        "",
        "  for (int i = 0; i < INPUT_COUNT; ++i) {",
        "    normalized[i] = (clamp01(input[i]) - SCALER_MEAN[i]) / SCALER_SCALE[i];",
        "  }",
        "",
        "  for (int j = 0; j < HIDDEN_COUNT; ++j) {",
        "    float value = BIAS1[j];",
        "    for (int i = 0; i < INPUT_COUNT; ++i) {",
        "      value += normalized[i] * W1[i * HIDDEN_COUNT + j];",
        "    }",
        "    hidden[j] = value > 0.0f ? value : 0.0f;",
        "  }",
        "",
        "  float maxLogit = -1000000.0f;",
        "  for (int k = 0; k < OUTPUT_COUNT; ++k) {",
        "    float value = BIAS2[k];",
        "    for (int j = 0; j < HIDDEN_COUNT; ++j) {",
        "      value += hidden[j] * W2[j * OUTPUT_COUNT + k];",
        "    }",
        "    logits[k] = value;",
        "    if (value > maxLogit) maxLogit = value;",
        "  }",
        "",
        "  float sum = 0.0f;",
        "  for (int k = 0; k < OUTPUT_COUNT; ++k) {",
        "    probabilities[k] = expf(logits[k] - maxLogit);",
        "    sum += probabilities[k];",
        "  }",
        "",
        "  int bestClass = 0;",
        "  float bestProbability = -1.0f;",
        "  for (int k = 0; k < OUTPUT_COUNT; ++k) {",
        "    probabilities[k] /= sum;",
        "    if (probabilities[k] > bestProbability) {",
        "      bestProbability = probabilities[k];",
        "      bestClass = k;",
        "    }",
        "  }",
        "  return bestClass;",
        "}",
        "",
        "}  // namespace DrenaHubModel",
        "",
    ]
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines), encoding="utf-8")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Treina e exporta o TinyML do DrenaHub")
    parser.add_argument(
        "--input",
        type=Path,
        help="CSV real com water_level,rise_rate,obstacle_risk,label. Sem este argumento usa baseline sintetico.",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if args.input:
        x, y = load_dataset(args.input)
        dataset_name = "real_sensor_collection"
    else:
        x, y = make_dataset()
        dataset_name = "synthetic_baseline"
    x_train, x_test, y_train, y_test = train_test_split(
        x,
        y,
        test_size=0.25,
        random_state=RANDOM_SEED,
        stratify=y,
    )

    scaler = StandardScaler()
    x_train_scaled = scaler.fit_transform(x_train)
    x_test_scaled = scaler.transform(x_test)

    model = MLPClassifier(
        hidden_layer_sizes=(6,),
        activation="relu",
        solver="lbfgs",
        alpha=0.0005,
        max_iter=3000,
        random_state=RANDOM_SEED,
    )
    model.fit(x_train_scaled, y_train)
    predictions = model.predict(x_test_scaled)

    report = classification_report(
        y_test,
        predictions,
        labels=[0, 1, 2],
        target_names=CLASS_NAMES,
        output_dict=True,
        zero_division=0,
    )
    matrix = confusion_matrix(y_test, predictions, labels=[0, 1, 2])
    metrics = {
        "dataset": dataset_name,
        "seed": RANDOM_SEED,
        "samples_total": int(len(x)),
        "samples_train": int(len(x_train)),
        "samples_test": int(len(x_test)),
        "accuracy": float(accuracy_score(y_test, predictions)),
        "confusion_matrix": matrix.tolist(),
        "classification_report": report,
        "warning": (
            "Modelo treinado com leituras reais fornecidas pelo grupo."
            if args.input
            else "Baseline sintetico; validar e retreinar com leituras reais antes da entrega."
        ),
    }

    if not args.input:
        write_dataset(DATA_DIR / "drenahub_training_synthetic.csv", x, y)
    MODEL_DIR.mkdir(parents=True, exist_ok=True)
    (MODEL_DIR / "metrics.json").write_text(
        json.dumps(metrics, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )
    export_header(HEADER_PATH, scaler, model, metrics)

    EVIDENCE_DIR.mkdir(parents=True, exist_ok=True)
    display = ConfusionMatrixDisplay(matrix, display_labels=CLASS_NAMES)
    display.plot(cmap="Blues", colorbar=False)
    matrix_kind = "real" if args.input else "baseline"
    title_kind = "dados reais" if args.input else "baseline sintetico"
    plt.title(f"DrenaHub TinyML - {title_kind}")
    plt.tight_layout()
    plt.savefig(EVIDENCE_DIR / f"matriz_confusao_{matrix_kind}.png", dpi=180)
    plt.close()

    print(json.dumps(metrics, indent=2, ensure_ascii=False))
    print(f"Header exportado para: {HEADER_PATH}")


if __name__ == "__main__":
    main()
