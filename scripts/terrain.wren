import "noiselib" for Noise

class terrain {
	static terrain_value(x,y,z) {
		var noise = Noise.new()
		noise.setType("OpenSimplex2")
		noise.setFreq(0.09)
		noise.setSeed(2384354)
		var val = noise.get3d(x,y,z)
		
		noise.setFreq(0.3)
		val = val - (noise.get3d(x,y,z) * 0.4)
		
		noise.free()
		
		if (y > 20) val = 1
		if (y < 1) val = -1
		
		return val
	}
}




