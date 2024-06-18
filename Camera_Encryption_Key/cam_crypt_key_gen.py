# pylint: disable=no-member,invalid-name

import cv2
import numpy as np
import hashlib
import os

def capture_image(cap):
    ret, frame = cap.read()
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
    # Draw a rectangle around the detected block for debugging purposes
    cv2.rectangle(binary_image, (x, y), (x + w, y + h), (255, 255, 255), 2)
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
            # Draw rectangles around the cells for debugging purposes
            cv2.rectangle(led_block_image, (j*cell_size, i*cell_size), ((j+1)*cell_size, (i+1)*cell_size), (128, 128, 128), 1)
    return grid

def generate_encryption_key(grid):
    grid_str = ''.join(map(str, grid.flatten()))
    key = hashlib.sha256(grid_str.encode()).hexdigest()
    return key

def save_images(folder, frame, binary_image, led_block_image, key):
    if not os.path.exists(folder):
        os.makedirs(folder)
    
    frame_path = os.path.join(folder, "captured_image.png")
    binary_image_path = os.path.join(folder, "binary_image.png")
    led_block_image_path = os.path.join(folder, "led_block_image.png")
    key_path = os.path.join(folder, "encryption_key.txt")

    cv2.imwrite(frame_path, frame)
    cv2.imwrite(binary_image_path, binary_image)
    cv2.imwrite(led_block_image_path, led_block_image)

    with open(key_path, "w") as file:
        file.write(key)

def main():
    cam = cv2.VideoCapture(1)
    if cam.isOpened():
        folder_counter = 1
        while True:
            frame = capture_image(cam)
            if frame is None:
                continue

            binary_image = preprocess_image(frame)
            led_block_image = detect_led_block(binary_image)
            grid = extract_8x8_grid(led_block_image)
            key = generate_encryption_key(grid)
            print(f"Generated Encryption Key: {key}")

            # Display the result (for debugging purposes)
            cv2.imshow('Captured Image', frame)
            cv2.imshow('Binary Image', binary_image)
            cv2.imshow('LED Block Image', led_block_image)

            key_pressed = cv2.waitKey(1) & 0xFF
            if key_pressed == ord(' '):  # Spacebar to capture and save images
                folder_name = f"encryption_data_{folder_counter}"
                save_images(folder_name, frame, binary_image, led_block_image, key)
                print(f"Images saved to '{folder_name}' folder.")
                folder_counter += 1
            elif key_pressed == ord('q'):  # 'q' to quit
                break
    else:
         print("Error: Could not open camera.")

    cam.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
