#%%
import numpy as np
from PIL import Image
from scipy.special import j0

def create_16bit_grayscale_image(width, height, func, filename="generated_image.png"):
    """
    Generates a 16-bit grayscale image from a 2D function f(x, y).

    Args:
        width (int): The width of the image in pixels.
        height (int): The height of the image in pixels.
        func (callable): A function that takes two numpy arrays (x, y) and
                         returns a numpy array of values.
        filename (str): The name of the file to save the image as.
                        Should support 16-bit depth (e.g., .png, .tiff).
    """
    print(f"Generating a {width}x{height} image...")

    # 1. Create a grid of (x, y) coordinates
    # np.linspace creates evenly spaced numbers over a specified interval.
    x = np.linspace(-5.0, 5.0, width)
    y = np.linspace(-5.0, 5.0, height)
    xx, yy = np.meshgrid(x, y)

    # 2. Apply the user-defined function to the grid
    print("Calculating function values...")
    z = func(xx, yy)

    # 3. Normalize the data to the range [0, 1]
    # This is essential to map the function's output to pixel brightness.
    z_min = z.min()
    z_max = z.max()
    if z_max == z_min:
        # Handle the case of a flat function (all values are the same)
        normalized_z = np.zeros_like(z)
    else:
        normalized_z = (z - z_min) / (z_max - z_min)

    # 4. Scale the normalized data to the 16-bit integer range [0, 65535]
    image_data = (normalized_z * 65535).astype(np.uint16)

    # 5. Create the image from the numpy array
    # The mode 'I;16' specifies a 16-bit integer grayscale image.
    # This is crucial for saving with the correct bit depth.
    img = Image.fromarray(image_data, mode='I;16')

    # 6. Save the image
    try:
        img.save(filename)
        print(f"Successfully saved image to '{filename}'")
    except Exception as e:
        print(f"Error saving image: {e}")

# --- Main execution ---
if __name__ == "__main__":
    IMAGE_WIDTH = 1024
    IMAGE_HEIGHT = 1024

    def wave_function(x, y):
        return np.sin(x * 2) * np.cos(y * 2) + np.sin(x * y * 0.1)

    def sinc_function(x, y):
        r = np.sqrt(20*x**2 + 20*y**2) + 1e-9 # Add a small epsilon to avoid division by zero
        return np.sin(r) / r

    def bessel_function(x, y):
        return j0(x**2 + y**2)

    def bessel_function_azym(x,y):
        theta = np.atan2(x,y)
        return j0(x**2 + y**2) * np.cos(2*theta)

    create_16bit_grayscale_image(IMAGE_WIDTH, IMAGE_HEIGHT, wave_function, "wave_pattern_16bit.png")
    create_16bit_grayscale_image(IMAGE_WIDTH, IMAGE_HEIGHT, sinc_function, "sinc_pattern_16bit.png")
    create_16bit_grayscale_image(IMAGE_WIDTH, IMAGE_HEIGHT, bessel_function, "bessel_pattern_16bit.png")
    create_16bit_grayscale_image(IMAGE_WIDTH, IMAGE_HEIGHT, bessel_function_azym, "bessel_pattern2_16bit.png")


# %%
