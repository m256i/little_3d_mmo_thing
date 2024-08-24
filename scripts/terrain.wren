import "noiselib" for Noise
import "dbglib" for Debug

class terrain {

	static on_module_init() {
		__noise = Noise.new()
		__noise.setNoiseType("OpenSimplex2")
		__noise.setNoiseFreq(0.24)
		__noise.setFractalType("None")
	}
	
	static terrain_value(x,y,z) {		
		var val = __noise.get3d(x,y,z)
		if (y > 10) val = 0
		if (y < 2) val = 1
		return 1-val
	}
}

