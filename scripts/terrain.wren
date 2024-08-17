import "noiselib" for Noise
import "dbglib" for Debug

class terrain {

	static on_module_init() {
		__noise = Noise.new()
		__noise.setNoiseType("OpenSimplex2")
		__noise.setNoiseFreq(0.1)
		__noise.setFractalType("None")
		__noise.setDomainWarpType("OpenSimplex2")
		__noise.setDomainWarpAmplitude(300)
	}
	
	static terrain_value(x,y,z) {		
		var val = __noise.get3d(x,y,z)
		
		if (y > 20) val = 1
		if (y < 1)  val = -1
		
		return val
	}
}













