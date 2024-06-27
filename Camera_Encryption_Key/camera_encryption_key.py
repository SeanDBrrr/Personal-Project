import cv2
import numpy as np
import hashlib

def capture_image(camera_index=1):
    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return None

    ret, frame = cap.read()
    cap.release()
    if not ret:
        print("Error: Could not read frame.")
        return None
    return frame

def preprocess_image(frame):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    _, binary = cv2.threshold(gray, 128, 255, cv2.THRESH_BINARY)
    return binary

def detect_led_block(binary_image):
    contours, _ = cv2.findContours(binary_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    largest_contour = max(contours, key=cv2.contourArea)
    x, y, w, h = cv2.boundingRect(largest_contour)
    return binary_image[y:y+h, x:x+w]

def extract_8x8_grid(led_block_image):
    h, w = led_block_image.shape
    cell_size = h // 8
    grid = np.zeros((8, 8), dtype=int)

    for i in range(8):
        for j in range(8):
            cell = led_block_image[i*cell_size:(i+1)*cell_size, j*cell_size:(j+1)*cell_size]
            if np.mean(cell) > 128:
                grid[i, j] = 1
            else:
                grid[i, j] = 0
    return grid

def generate_encryption_key(grid):
    grid_str = ''.join(map(str, grid.flatten()))
    key = hashlib.sha256(grid_str.encode()).hexdigest()
    return key

def main():
    frame = capture_image()
    if frame is None:
        return

    binary_image = preprocess_image(frame)
    led_block_image = detect_led_block(binary_image)
    grid = extract_8x8_grid(led_block_image)
    key = generate_encryption_key(grid)
    print(f"Generated Encryption Key: {key}")

    # Display the result (for debugging purposes)
    cv2.imshow('Captured Image', frame)
    cv2.imshow('Binary Image', binary_image)
    cv2.imshow('LED Block Image', led_block_image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
