#include <cassert>
#include <cmath>
#include <iostream>

#include "../firmware/DrenaHub/drenahub_model.h"

static int classify(float level, float rise, float obstacle) {
  const float input[3] = {level, rise, obstacle};
  float probabilities[3] = {0.0f, 0.0f, 0.0f};
  const int result = DrenaHubModel::predict(input, probabilities);

  float sum = probabilities[0] + probabilities[1] + probabilities[2];
  assert(std::fabs(sum - 1.0f) < 0.001f);
  return result;
}

int main() {
  assert(classify(0.05f, 0.02f, 0.10f) == 0);
  assert(classify(0.40f, 0.20f, 0.20f) == 1);
  assert(classify(0.86f, 0.30f, 0.10f) == 2);
  assert(classify(0.58f, 0.95f, 0.60f) == 2);

  std::cout << "TinyML smoke test: OK\n";
  return 0;
}
