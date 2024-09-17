#pragma once
#include <cmath>
#include <array>
#include <cassert>
#include <optional>
#include <cstdio>
#include <common.h>
#include <glm/glm.hpp>

namespace renderer::core::math
{

namespace detail
{

constexpr static f64 PI = 3.14159265358979323846;
using mat_type          = std::array<std::array<f64, 3>, 3>;

/*
solve cubic polynomial ax^3 + bx^2 + cx + d = 0
using cardanos method
*/
inline std::array<f64, 3>
solve_cubic(f64 a, f64 b, f64 c, f64 d)
{
  const f64 p            = (3 * a * c - b * b) / (3 * a * a);
  const f64 q            = (2 * b * b * b - 9 * a * b * c + 27 * a * a * d) / (27 * a * a * a);
  const f64 discriminant = (q * q / 4) + (p * p * p / 27);

  std::array<f64, 3> roots;

  if (discriminant > 0)
  {
    puts("roots of polynomial are complex");
    assert(false);
  }
  else if (discriminant == 0)
  {
    const f64 u = std::cbrt(-q / 2);
    roots[0]    = (2 * u - b / (3 * a));
    roots[1]    = (-u - b / (3 * a));
    roots[0]    = (-u - b / (3 * a));
  }
  else
  {
    const f64 r     = std::sqrt(-p / 3);
    const f64 theta = std::acos(-q / (2 * r * r * r));
    roots[0]        = (2 * r * std::cos(theta / 3) - b / (3 * a));
    roots[1]        = (2 * r * std::cos((theta + 2 * PI) / 3) - b / (3 * a));
    roots[2]        = (2 * r * std::cos((theta + 4 * PI) / 3) - b / (3 * a));
  }
  return roots;
}

inline f64
determinant3x3(const mat_type& M)
{
  return M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) - M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0]) +
         M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
}

inline std::array<f64, 4>
ccoeff(const mat_type& A)
{
  const f64 &a11 = A[0][0], a12 = A[0][1], a13 = A[0][2];
  const f64 &a21 = A[1][0], a22 = A[1][1], a23 = A[1][2];
  const f64 &a31 = A[2][0], a32 = A[2][1], a33 = A[2][2];

  const f64 trace    = a11 + a22 + a33;
  const f64 minorSum = (a11 * a22 + a11 * a33 + a22 * a33) - (a12 * a21 + a13 * a31 + a23 * a32);
  const f64 detA     = determinant3x3(A);
  // λ^3 - trace(λ^2) - minorSum(λ) + detA = 0
  return {-1, trace, -minorSum, detA};
}

using gssslv_type = std::array<std::array<f64, 3>, 2>;

inline void
swap_row(gssslv_type& mat, usize i, usize j)
{
  for (usize k = 0; k <= mat.size(); k++)
  {
    const double temp = mat[i][k];
    mat[i][k]         = mat[j][k];
    mat[j][k]         = temp;
  }
}

inline int
forward_elim(gssslv_type& mat)
{
  for (usize k = 0; k < mat.size(); k++)
  {
    usize i_max = k;
    usize v_max = mat[i_max][k];

    for (usize i = k + 1; i < mat.size(); i++)
    {
      if (std::abs(mat[i][k]) > v_max)
      {
        v_max = mat[i][k], i_max = i;
      }
    }
    if (!mat[k][i_max])
    {
      return k;
    }
    if (i_max != k)
    {
      swap_row(mat, k, i_max);
    }
    for (usize i = k + 1; i < mat.size(); i++)
    {
      const f64 f = mat[i][k] / mat[k][k];
      for (usize j = k + 1; j <= mat.size(); j++)
      {
        mat[i][j] -= mat[k][j] * f;
      }
      mat[i][k] = 0;
    }
  }
  return -1;
}

inline std::array<f64, 2>
backward_substitution(gssslv_type& mat)
{
  std::array<f64, 2> x;
  for (int i = mat.size() - 1; i >= 0; i--)
  {
    x[i] = mat[i][mat.size()];
    for (usize j = i + 1; j < mat.size(); j++)
    {
      x[i] -= mat[i][j] * x[j];
    }
    x[i] = x[i] / mat[i][i];
  }
  return x;
}

inline std::optional<std::array<f64, 2>>
gaussian_elimination(gssslv_type& mat)
{
  const int singular_flag = forward_elim(mat);
  if (singular_flag != -1)
  {
    return std::nullopt;
  }
  return backward_substitution(mat);
}

inline std::optional<std::array<f64, 3>>
get_eigenvectors(mat_type& matrix, f64 eigen_value)
{
  matrix[0][0] = matrix[0][0] - eigen_value;
  matrix[1][1] = matrix[1][1] - eigen_value;
  matrix[2][2] = matrix[2][2] - eigen_value;

  gssslv_type guessed_mat;
  /*
  we assume eigenvector.x = 1
  so we only need so solve Y and Z

  so we turn

  aX  + bY  + cZ  = 0
  a1X + b1Y + c1Z = 0
  a2X + b2Y + c2Z = 0

  into:

  b1Y + c1Z = -a1 *(X=1)
  b2Y + c2Z = -a2 *(X=1)
  */
  guessed_mat[0][0] = matrix[1][1];
  guessed_mat[0][1] = matrix[1][2];
  guessed_mat[0][2] = -matrix[1][0];

  guessed_mat[1][0] = matrix[2][1];
  guessed_mat[1][1] = matrix[2][2];
  guessed_mat[1][2] = -matrix[2][0];

  const auto solution = gaussian_elimination(guessed_mat);

  if (solution.has_value()) [[likely]]
  {
    matrix[0][0] = matrix[0][0] + eigen_value;
    matrix[1][1] = matrix[1][1] + eigen_value;
    matrix[2][2] = matrix[2][2] + eigen_value;
    return std::array<f64, 3>{1, solution.value()[0], solution.value()[1]};
  }

  /*
  this means eigenvector.x != 1 and we assume y = 1
  so we turn:

  aX  + bY  + cZ  = 0
  a1X + b1Y + c1Z = 0
  a2X + b2Y + c2Z = 0
  into:

  aX  + 0  + cZ = -bY
  a2X + 0 + c2Z = -b2Y
  */
  gssslv_type guessed_mat2{};

  guessed_mat[0][0] = matrix[0][0];
  guessed_mat[0][1] = matrix[0][2];
  guessed_mat[0][2] = -matrix[0][1];

  guessed_mat[1][0] = matrix[2][0];
  guessed_mat[1][1] = matrix[2][2];
  guessed_mat[1][2] = -matrix[2][1];

  const auto solution2 = gaussian_elimination(guessed_mat);
  if (solution2.has_value()) [[likely]]
  {
    matrix[0][0] = matrix[0][0] + eigen_value;
    matrix[1][1] = matrix[1][1] + eigen_value;
    matrix[2][2] = matrix[2][2] + eigen_value;
    return std::array<f64, 3>{solution2.value()[0], 1, solution2.value()[1]};
  }

  /*
  this means eigenvector.y != 1 and we assume z = 1
  so we turn:

  aX  + bY  + cZ  = 0
  a1X + b1Y + c1Z = 0
  a2X + b2Y + c2Z = 0

  into:

  aX  + bY  + 0 = -cZ
  a1X + b1Y + 0 = -c1Z
  */
  gssslv_type guessed_mat3{};

  guessed_mat[0][0] = matrix[0][0];
  guessed_mat[0][1] = matrix[0][1];
  guessed_mat[0][2] = -matrix[0][2];

  guessed_mat[1][0] = matrix[1][0];
  guessed_mat[1][1] = matrix[1][1];
  guessed_mat[1][2] = -matrix[1][2];

  const auto solution3 = gaussian_elimination(guessed_mat);

  if (solution3.has_value()) [[likely]]
  {
    matrix[0][0] = matrix[0][0] + eigen_value;
    matrix[1][1] = matrix[1][1] + eigen_value;
    matrix[2][2] = matrix[2][2] + eigen_value;
    return std::array<f64, 3>{solution3.value()[0], solution3.value()[1], 1};
  }

  puts("no real eigenvector for this matrix");

  matrix[0][0] = matrix[0][0] + eigen_value;
  matrix[1][1] = matrix[1][1] + eigen_value;
  matrix[2][2] = matrix[2][2] + eigen_value;

  return std::nullopt;
}

inline std::optional<std::array<std::array<f64, 3>, 3>>
solve_eigenvectors(mat_type& matrix)
{
  const auto coeffs = ccoeff(matrix);
  auto roots        = solve_cubic(coeffs[0], coeffs[1], coeffs[2], coeffs[3]);

  const auto v0 = get_eigenvectors(matrix, roots[0]);
  const auto v1 = get_eigenvectors(matrix, roots[1]);
  const auto v2 = get_eigenvectors(matrix, roots[2]);

  if (v0.has_value() && v1.has_value() && v2.has_value())
  {
    return std::array{v0.value(), v1.value(), v2.value()};
  }
  return std::nullopt;
}
} // namespace detail

inline std::optional<std::array<glm::vec3, 3>>
eigen(const glm::mat3& _matrix)
{
  detail::mat_type mat{};
  mat[0][0] = _matrix[0][0];
  mat[0][1] = _matrix[0][1];
  mat[0][2] = _matrix[0][2];
  mat[1][0] = _matrix[1][0];
  mat[1][1] = _matrix[1][1];
  mat[1][2] = _matrix[1][2];
  mat[2][0] = _matrix[2][0];
  mat[2][1] = _matrix[2][1];
  mat[2][2] = _matrix[2][2];

  auto solution = detail::solve_eigenvectors(mat);

  if (solution.has_value())
  {
    return std::array{
        glm::vec3{solution.value()[0][0], solution.value()[0][1], solution.value()[0][2]},
        glm::vec3{solution.value()[1][0], solution.value()[1][1], solution.value()[1][2]},
        glm::vec3{solution.value()[2][0], solution.value()[2][1], solution.value()[2][2]},
    };
  }
  return std::nullopt;
}

} // namespace renderer::core::math