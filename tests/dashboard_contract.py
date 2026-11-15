#!/usr/bin/env python3
"""Smoke test estático do contrato entre firmware e Central DrenaHub."""

from pathlib import Path


SOURCE = Path("firmware/DrenaHub/DrenaHub.ino").read_text(encoding="utf-8")

REQUIRED_TOKENS = {
    "painel de alerta": 'id="alertPanel"',
    "botão de reconhecimento": 'id="ackButton"',
    "contador de ocorrências": 'id="eventCount"',
    "rota de reconhecimento": 'server.on("/alert/ack"',
    "presença de água na API": 'water_present',
    "objeto de alerta na API": 'json += "\\\"alert\\\":{"',
    "parada autônoma": 'currentAction = waterPresent',
    "avanço em caminho livre": "moveForward();",
    "desvio por distância": "distanceCm <= OBSTACLE_AVOID_DISTANCE_CM",
    "limiar de detecção": "WATER_DETECTION_THRESHOLD = 0.30f",
    "histerese de saída": "WATER_CLEAR_THRESHOLD = 0.15f",
}

missing = [name for name, token in REQUIRED_TOKENS.items() if token not in SOURCE]
if missing:
    raise SystemExit("Contrato incompleto: " + ", ".join(missing))

print("Contrato firmware/Central: OK")
