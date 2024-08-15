class NoiseImpl {	
	foreign static setNoiseType(h, type)
	foreign static setNoiseSeed(h, seed)
	foreign static setNoiseFreq(h, freq)

	foreign static getNoise2d(h, x, y)
	foreign static getNoise3d(h, x, y, z)
}


class Noise {

	foreign getNewNoiseHandle()
	foreign freeNoiseHandle(h)

	construct new() {
		_handle = getNewNoiseHandle()
	}

	setType(type) {
		NoiseImpl.setNoiseType(_handle, type)
	}

	setSeed(seed) {
		NoiseImpl.setNoiseSeed(_handle, seed)
	}

	setFreq(freq) {
		NoiseImpl.setNoiseFreq(_handle, freq)
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