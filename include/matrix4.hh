#ifndef MATRIX4_HH
#define MATRIX4_HH

#include <iostream>

class matrix4
{
public:
    matrix4();
    matrix4 operator*(const matrix4 &rhs);
    void operator*=(const matrix4 &rhs);
    static matrix4 identity();

    void frustum(matrix4 &mat,
        const float &left, const float &right,
        const float &bottom, const float &top,
        const float &z_near, const float &z_far);
    void look_at(matrix4 &mat,
        const float &eyeX, const float &eyeY, const float &eyeZ,
        const float &centerX, const float &centery, const float &centerZ,
        float upX, float upY, float upZ);

    friend std::ostream &operator<<(std::ostream &out, const matrix4 &m);
private:
    float _mat[16];
};

#endif /* MATRIX4_HH */
