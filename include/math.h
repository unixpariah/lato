typedef float Mat4[4][4];

void mat4(Mat4 *m0);

void ortographic_projection(Mat4 *mat4, float left, float right, float top,
                            float bottom);

void translate(Mat4 *m0, float x, float y, float z);

void scale(Mat4 *m0, float x, float y, float z);

void mul(Mat4 *m0, Mat4 *m1);
