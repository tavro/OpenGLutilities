from PIL import Image
import numpy as np
from noise import pnoise2

def generate_perlin_noise_tga(size, scale=100, octaves=6, persistence=0.5, lacunarity=2.0):
    image_array = np.zeros((size, size), dtype=np.float32)

    for x in range(size):
        for y in range(size):
            noise_val = pnoise2(x / scale, 
                                y / scale, 
                                octaves=octaves, 
                                persistence=persistence, 
                                lacunarity=lacunarity, 
                                repeatx=size, 
                                repeaty=size, 
                                base=0)
            image_array[x, y] = (noise_val + 0.5) * 255

    image_array = np.clip(image_array, 0, 255)

    image = Image.fromarray(image_array.astype(np.uint8), 'L')

    image.save('perlin_noise_output.tga')

generate_perlin_noise_tga(256)
