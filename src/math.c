#include "../include/math.h"

void mat4(Mat4 *m0) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (i == 0) {
        (*m0)[i][j] = 1;
      } else {
        (*m0)[i][j] = 0;
      }
    }
  }
}

void ortographic_projection(Mat4 *m0, float left, float right, float top,
                            float bottom) {
  (*m0)[0][0] = 2 / (right - left);
  (*m0)[1][1] = 2 / (top - bottom);
  (*m0)[2][2] = 2;
  (*m0)[3][0] = -(right + left) / (right - left);
  (*m0)[3][1] = -(top + bottom) / (top - bottom);
  (*m0)[3][2] = -1;
  (*m0)[3][3] = 1;
}

void translate(Mat4 *m0, float x, float y, float z) {
  (*m0)[3][0] = x;
  (*m0)[3][1] = y;
  (*m0)[3][2] = z;
}

void scale(Mat4 *m0, float x, float y, float z) {
  (*m0)[0][0] = x;
  (*m0)[1][1] = y;
  (*m0)[2][2] = z;
}

/// Perform matrix multiplication and store result in m0
void mul(Mat4 *m0, Mat4 *m1) {
  Mat4 result;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result[i][j] = 0;
      for (int k = 0; k < 4; k++) {
        result[i][j] += (*m0)[i][k] * (*m1)[k][j];
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      (*m0)[i][j] = result[i][j];
    }
  }
}
