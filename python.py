import numpy as np
import cv2
import heapq

def extract_pixels(image_path):
    image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    if image is None:
        raise FileNotFoundError(f"Image at path {image_path} not found.")
    np.savetxt('pixels.txt', image, fmt='%d')
    return image.shape

def generate_adjacency_list(image):
    rows, cols = image.shape
    num_vertices = rows * cols + 2
    adjacency_list = [[] for _ in range(num_vertices)]

    for r in range(rows):
        for c in range(cols):
            u = r * cols + c

            # Connect each pixel to three pixels in the upper row
            if r > 0:
                v1 = (r - 1) * cols + max(c - 1, 0)  # Pixel to the left
                v2 = (r - 1) * cols + c              # Pixel directly above
                v3 = (r - 1) * cols + min(c + 1, cols - 1)  # Pixel to the right

                weight1 = abs(image[r, c] - image[r - 1, max(c - 1, 0)])
                weight2 = abs(image[r, c] - image[r - 1, c])
                weight3 = abs(image[r, c] - image[r - 1, min(c + 1, cols - 1)])

                adjacency_list[u].append((v1, weight1))
                adjacency_list[u].append((v2, weight2))
                adjacency_list[u].append((v3, weight3))

            # Connect source to the first row of pixels
            if r == 0:
                adjacency_list[num_vertices - 2].append((u, 0))

            # Connect the last row of pixels to the target
            if r == rows - 1:
                adjacency_list[u].append((num_vertices - 1, 0))

    return adjacency_list

def dijkstra(adjacency_list, source, target):
    num_vertices = len(adjacency_list)
    distances = [float('inf')] * num_vertices
    predecessors = [-1] * num_vertices
    pq = []

    distances[source] = 0
    heapq.heappush(pq, (0, source))

    while pq:
        dist_u, u = heapq.heappop(pq)

        if u == target:
            break

        for v, weight in adjacency_list[u]:
            new_dist = dist_u + weight
            if new_dist < distances[v]:
                distances[v] = new_dist
                predecessors[v] = u
                heapq.heappush(pq, (new_dist, v))

    # Trace back the shortest path to reconstruct the seam
    path = []
    at = target
    while at != -1:
        path.append(at)
        at = predecessors[at]
    path.reverse()

    return path[1:-1]  # Exclude source and target

def find_minimum_seam(image):
    adjacency_list = generate_adjacency_list(image)
    source = len(adjacency_list) - 2
    target = len(adjacency_list) - 1
    seam = dijkstra(adjacency_list, source, target)

    rows, cols = image.shape
    seam = [p % cols for p in seam]

    if len(seam) != rows - 2:
        raise ValueError("Seam length does not match the number of rows in the image.")

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

        # Read pixels from file
        image = np.loadtxt('pixels.txt', dtype=int)

        # Find minimum seam
        seam = find_minimum_seam(image)

        # Highlight seam in the image
        highlight_seam('input.png', seam)

        print("Seam highlighted and output saved to output.png")
    except Exception as e:
        print(f"Error: {e}")
