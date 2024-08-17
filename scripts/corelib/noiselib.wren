class NoiseImpl {	
  foreign static setNoiseType(h, type)
  foreign static setNoiseSeed(h, seed)
  foreign static setNoiseFreq(h, freq)

  foreign static setFractalType(h, type)
  foreign static setFractalOctaves(h, octaves)
  foreign static setFractalLacunarity(h, lac)
  foreign static setFractalGain(h, gain)
  foreign static setFractalWStrength(h, strength)
  foreign static setFractalPPStrength(h, ppstrength)

  foreign static setCellularDistFunc(h, dist_func)
  foreign static setCellularRetType(h, type)
  foreign static setCellularJitter(h, jitter)
 
  foreign static setDomainWarpType(h, type)
  foreign static setDomainWarpRotationType3d(h, type)
  foreign static setDomainWarpAmplitude(h, amplitude)
  foreign static setDomainWarpSeed(h, seed)
  foreign static setDomainWarpFrequency(h, freq)

  foreign static setDomainWarpFractalType(h, type)
  foreign static setDomainWarpFractalOctaves(h, octaves)
  foreign static setDomainWarpFractalLacunarity(h, lac)
  foreign static setDomainWarpFractalGain(h, gain)

  foreign static getNoise2d(h, x, y)
  foreign static getNoise3d(h, x, y, z)
}

class Noise {

  foreign getNewNoiseHandle()
  foreign freeNoiseHandle(h)

  construct new() {
    _handle = getNewNoiseHandle()
  }
  
  setNoiseType(type) {
    NoiseImpl.setNoiseType(_handle, type)
  }
  setNoiseSeed(seed) {
    NoiseImpl.setNoiseSeed(_handle, seed)
  }
  setNoiseFreq(freq) {
    NoiseImpl.setNoiseFreq(_handle, freq)
  }

  setFractalType(type) {
    NoiseImpl.setFractalType(_handle, type)
  }
  setFractalOctaves(octaves) {
    NoiseImpl.setFractalOctaves(_handle, octaves)
  }
  setFractalLacunarity(lac){
    NoiseImpl.setFractalLacunarity(_handle, lac)
  }
  setFractalGain(gain){
    NoiseImpl.setFractalGain(_handle, gain)
  }
  setFractalWStrength(strength){
    NoiseImpl.setFractalWStrength(_handle, strength)
  }
  setFractalPPStrength(ppstrength){
    NoiseImpl.setFractalPPStrength(_handle, ppstrength)
  }
  
  setCellularDistFunc(dist_func){
    NoiseImpl.setCellularDistFunc(_handle, dist_func)
  }
  setCellularRetType(type){
    NoiseImpl.setCellularRetType(_handle, type)
  }
  setCellularJitter(jitter){
    NoiseImpl.setCellularJitter(_handle, jitter)
  }

  setDomainWarpType(type){
    NoiseImpl.setDomainWarpType(_handle, type)
  }
  setDomainWarpRotationType3d(type){
    NoiseImpl.setDomainWarpRotationType3d(_handle, type)
  }
  setDomainWarpAmplitude(amplitude){
    NoiseImpl.setDomainWarpAmplitude(_handle, amplitude)
  }
  setDomainWarpSeed(seed){
    NoiseImpl.setDomainWarpSeed(_handle, seed)
  }
  setDomainWarpFrequency(freq){
    NoiseImpl.setDomainWarpFrequency(_handle, freq)
  }

  setDomainWarpFractalType(type){
    NoiseImpl.setDomainWarpFractalType(_handle, type)
  }
  setDomainWarpFractalOctaves(octaves){
    NoiseImpl.setDomainWarpFractalOctaves(_handle, octaves)
  }
  setDomainWarpFractalLacunarity(lac){
    NoiseImpl.setDomainWarpFractalLacunarity(_handle, lac)
  }
  setDomainWarpFractalGain(gain){
    NoiseImpl.setDomainWarpFractalGain(_handle, gain) 
  }
  get2d(x, y) {
    return NoiseImpl.getNoise2d(_handle, x, y)
  }
  get3d(x, y, z) {
    return NoiseImpl.getNoise3d(_handle, x, y, z)
  }

  free() {
    freeNoiseHandle(_handle)
  }
}