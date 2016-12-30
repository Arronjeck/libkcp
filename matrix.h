//
// Created by 理 傅 on 2016/12/30.
//

#ifndef KCP_MATRIX_H
#define KCP_MATRIX_H

#include <stdint.h>

class matrix {
private:
    uint8_t **data;
public:
    matrix(const matrix &) = delete;

    matrix &operator=(const matrix &) = delete;

    // newMatrix returns a matrix of zeros.
    static matrix *newMatrix(int rows, int cols);

    // IdentityMatrix returns an identity matrix of the given size.
    static matrix *identityMatrix(int size);

    static void free(matrix *m);

    // Multiply multiplies this matrix (the one on the left) by another
    // matrix (the one on the right) and returns a new matrix with the result.
    matrix *Multiply(matrix *right);

private:
    matrix() = default;

    ~matrix() = default;
};


#endif //KCP_MATRIX_H