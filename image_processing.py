#Run the python file to generate the pixel.txt file
#run the c++ file to generate the seam.txt file
#again run the same python file to highlight the seam in the image
import numpy as np
import cv2

def extract_pixels(image_path):
    image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    if image is None:
        raise FileNotFoundError(f"Image at path {image_path} not found.")
    np.savetxt('pixels.txt', image, fmt='%d')
    return image.shape

def read_seam():
    try:
        with open('seam.txt', 'r') as file:
            seam = list(map(int, file.readline().strip().split()))
    except FileNotFoundError:
        raise FileNotFoundError("seam.txt file not found. Ensure the C++ program has run successfully.")
    return seam

def highlight_seam(image_path, seam):
    image = cv2.imread(image_path)
    if image is None:
        raise FileNotFoundError(f"Image at path {image_path} not found.")
    
    for r, c in enumerate(seam):
        if r < image.shape[0] and c < image.shape[1]:
            image[r, c] = [0, 0, 255]  # Highlight seam in red
    
    cv2.imwrite('output.png', image)

if __name__ == "__main__":
    try:
        # Extract pixels and save to text file
        rows, cols = extract_pixels('input.png')
        
        # Assuming the seam file is created by the C++ code
        seam = read_seam()
        
        
        
        # Highlight seam in the image
        highlight_seam('input.png', seam)

        print("Seam highlighted and output saved to output.png")
    except Exception as e:
        print(f"Error: {e}")