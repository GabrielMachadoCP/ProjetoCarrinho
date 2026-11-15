#!/usr/bin/env python3
"""Smoke test estático do contrato entre firmware e Central DrenaHub."""

from pathlib import Path


SOURCE = Path("firmware/DrenaHub/DrenaHub.ino").read_text(encoding="utf-8")

REQUIRED_TOKENS = {
    "painel de alerta": 'id="alertPanel"',
    "botão de confirmação": "CONFIRMAR DRENAGEM OBSTRUÍDA",
    "contador de ocorrências": 'id="eventCount"',
    "rota de reconhecimento": 'server.on("/alert/ack"',
    "confirmação do modo autônomo": "confirmAutonomous()",
    "rota de parada de emergência": 'server.on("/emergency-stop"',
    "botão de parada de emergência": "PARADA DE EMERGÊNCIA",
    "presença de água na API": 'water_present',
    "objeto de alerta na API": 'json += "\\\"alert\\\":{"',
    "parada autônoma por ocorrência": "if (waterStopActive)",
    "referência de umidade residual": "residualMoistureBaseline",
    "nova subida de água": "WATER_RETRIGGER_DELTA = 0.12f",
    "umidade residual na API": 'json += "\\\"residual_moisture\\\":"',
    "avanço em caminho livre": "moveForward();",
    "desvio por distância": "distanceCm <= OBSTACLE_AVOID_DISTANCE_CM",
    "distância simples de desvio": "OBSTACLE_AVOID_DISTANCE_CM = 15.0f",
    "limiar de detecção": "WATER_DETECTION_THRESHOLD = 0.30f",
    "histerese de saída": "WATER_CLEAR_THRESHOLD = 0.15f",
}

missing = [name for name, token in REQUIRED_TOKENS.items() if token not in SOURCE]
if missing:
    raise SystemExit("Contrato incompleto: " + ", ".join(missing))

interface_order = [
    SOURCE.index('id="autoButton"'),
    SOURCE.index("Movimentação manual"),
    SOURCE.index('id="waterState"'),
]
if interface_order != sorted(interface_order):
    raise SystemExit("Ordem da Central incorreta: modos, controle manual e sensores")

autonomous = SOURCE.split("void runAutonomousMode()", 1)[1].split(
    "// API E ROTAS WEB", 1
)[0]
unexpected_gates = [
    token
    for token in ("waterAlertActive", "suppressWaterAlertUntilDry", "currentRisk")
    if token in autonomous
]
if unexpected_gates:
    raise SystemExit(
        "Patrulha contém bloqueios extras: " + ", ".join(unexpected_gates)
    )

auto_route = SOURCE.split('server.on("/mode/auto"', 1)[1].split(
    'server.on("/emergency-stop"', 1
)[0]
for token in ("updateSensorsAndInference(true);", "runAutonomousMode();"):
    if token not in auto_route:
        raise SystemExit("Modo autônomo não inicia imediatamente: " + token)

ack_route = SOURCE.split('server.on("/alert/ack"', 1)[1].split(
    'server.on("/move/forward"', 1
)[0]
for token in (
    "waterAlertActive = false;",
    "waterStopActive = false;",
    "residualMoistureMode = waterPresent;",
    "runAutonomousMode();",
):
    if token not in ack_route:
        raise SystemExit("Confirmação não libera a patrulha: " + token)

print("Contrato firmware/Central: OK")
