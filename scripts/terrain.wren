import "noiselib" for Noise
import "dbglib" for Debug

class terrain {

	static on_module_init() {
		__noise = Noise.new()
		__noise.setNoiseType("OpenSimplex2")
		__noise.setNoiseFreq(0.1)
		__noise.setFractalType("None")
	}
	
	static terrain_value(x,y,z) {		
		return __noise.get3d(x,y,z)
	}
}


















