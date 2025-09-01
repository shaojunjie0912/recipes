#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

using MatType = std::vector<std::vector<int64_t>>;

MatType Multiply(MatType const& A, MatType const& B) {
    assert(A.size() == B[0].size());
    int m = A.size();
    int n = B.size();
    int p = B[0].size();

    MatType C(m, vector<int64_t>(p));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < p; ++j) {
            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}
