#pragma once

#include <common.h>

namespace world_gen
{

namespace detail
{

/**
 * @brief A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D, 4D).
 */
class simplex_noise
{
public:
  // 1D Perlin simplex noise
  static f32 noise(f32 x);
  // 2D Perlin simplex noise
  static f32 noise(f32 x, f32 y);
  // 3D Perlin simplex noise
  static f32 noise(f32 x, f32 y, f32 z);

  // Fractal/Fractional Brownian Motion (fBm) noise summation
  f32 fractal(usize octaves, f32 x) const;
  f32 fractal(usize octaves, f32 x, f32 y) const;
  f32 fractal(usize octaves, f32 x, f32 y, f32 z) const;

  /**
   * Constructor of to initialize a fractal noise summation
   *
   * @param[in] frequency    Frequency ("width") of the first octave of noise (default to 1.0)
   * @param[in] amplitude    Amplitude ("height") of the first octave of noise (default to 1.0)
   * @param[in] lacunarity   Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
   * @param[in] persistence  Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
   */
  explicit simplex_noise(f32 frequency = 1.0f, f32 amplitude = 1.0f, f32 lacunarity = 2.0f, f32 persistence = 0.5f)
      : mFrequency(frequency), mAmplitude(amplitude), mLacunarity(lacunarity), mPersistence(persistence)
  {
  }

private:
  // Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
  f32 mFrequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
  f32 mAmplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
  f32 mLacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
  f32 mPersistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
};
} // namespace detail

inline f32
noise3d(f64 x, f64 y, f64 z) noexcept
{
}

}; // namespace world_gen