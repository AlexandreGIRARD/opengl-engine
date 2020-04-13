#include <matrix4.hh>

matrix4::matrix4()
{
    _mat[16] = {};
}

matrix4 matrix4::operator*(const matrix4 &rhs)
{
    matrix4 new_mat{};
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float ij;
            for (int k = 0; k < 4; k++)
                ij += _mat[i*4 + k] * rhs._mat[k*4 + j];
            new_mat._mat[i*4 + j] = ij;
        }
    }
    return new_mat;
}

void matrix4::operator*=(const matrix4 &rhs)
{
    *this = (*this) * rhs;
}

matrix4 matrix4::identity()
{
    auto id = matrix4();
    id._mat[0] = 1.0;
    id._mat[5] = 1.0;
    id._mat[10] = 1.0;
    id._mat[15] = 1.0;
    return id;
}

std::ostream &operator<<(std::ostream &out, const matrix4 &m)
{
    return out << m._mat[0] << " " << m._mat[1] << " " << m._mat[2] << " " << m._mat[3] << "\n" <<
           m._mat[4] << " " << m._mat[5] << " " << m._mat[6] << " " << m._mat[7] << "\n" <<
           m._mat[8] << " " << m._mat[9] << " " << m._mat[10] << " " << m._mat[11] << "\n" <<
           m._mat[12] << " " << m._mat[13] << " " << m._mat[14] << " " << m._mat[15] << "\n";
}
