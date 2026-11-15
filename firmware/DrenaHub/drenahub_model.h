#pragma once

// Arquivo gerado por tinyml/train_drenahub_model.py.
// Dataset: synthetic_baseline.
// Accuracy no conjunto de teste: 0.9967

#include <math.h>

namespace DrenaHubModel {

constexpr int INPUT_COUNT = 3;
constexpr int HIDDEN_COUNT = 6;
constexpr int OUTPUT_COUNT = 3;

static const float SCALER_MEAN[3] = {0.501956171f, 0.310856123f, 0.508439198f};
static const float SCALER_SCALE[3] = {0.290859332f, 0.176318195f, 0.290002690f};
static const float W1[18] = {-28.771230812f, -16.336770914f, -26.537481447f, 1.775176046f, -0.411858584f, -23.880763066f, 0.003569644f, -1.499380968f, -0.011717850f, 12.436616867f, 0.505637699f, 0.016135594f, -0.044625517f, -2.829332195f, -0.006537388f, 5.697214676f, 8.191794889f, 0.049663264f};
static const float BIAS1[6] = {20.583556606f, -19.896359791f, -13.395807286f, -8.072241744f, -16.090095961f, -12.517520582f};
static const float W2[18] = {7.887364919f, 14.801127776f, -22.784717238f, -5.978124728f, -14.622469971f, 20.035088227f, 3.375135839f, -13.695377187f, 9.249428110f, -8.989948129f, -0.444165300f, 11.427630592f, 29.479138464f, -2.321261361f, -27.831902467f, 31.548962054f, 6.356963439f, -38.627490915f};
static const float BIAS2[3] = {-46.862064224f, 5.743412644f, 40.632001299f};

inline float clamp01(float value) {
  if (value < 0.0f) return 0.0f;
  if (value > 1.0f) return 1.0f;
  return value;
}

inline int predict(const float input[INPUT_COUNT], float probabilities[OUTPUT_COUNT]) {
  float normalized[INPUT_COUNT];
  float hidden[HIDDEN_COUNT];
  float logits[OUTPUT_COUNT];

  for (int i = 0; i < INPUT_COUNT; ++i) {
    normalized[i] = (clamp01(input[i]) - SCALER_MEAN[i]) / SCALER_SCALE[i];
  }

  for (int j = 0; j < HIDDEN_COUNT; ++j) {
    float value = BIAS1[j];
    for (int i = 0; i < INPUT_COUNT; ++i) {
      value += normalized[i] * W1[i * HIDDEN_COUNT + j];
    }
    hidden[j] = value > 0.0f ? value : 0.0f;
  }

  float maxLogit = -1000000.0f;
  for (int k = 0; k < OUTPUT_COUNT; ++k) {
    float value = BIAS2[k];
    for (int j = 0; j < HIDDEN_COUNT; ++j) {
      value += hidden[j] * W2[j * OUTPUT_COUNT + k];
    }
    logits[k] = value;
    if (value > maxLogit) maxLogit = value;
  }

  float sum = 0.0f;
  for (int k = 0; k < OUTPUT_COUNT; ++k) {
    probabilities[k] = expf(logits[k] - maxLogit);
    sum += probabilities[k];
  }

  int bestClass = 0;
  float bestProbability = -1.0f;
  for (int k = 0; k < OUTPUT_COUNT; ++k) {
    probabilities[k] /= sum;
    if (probabilities[k] > bestProbability) {
      bestProbability = probabilities[k];
      bestClass = k;
    }
  }
  return bestClass;
}

}  // namespace DrenaHubModel
