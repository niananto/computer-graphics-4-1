import cv2
import os

# Path to the directory containing your BMP images
image_dir = 'images'

# Get a list of all BMP files in the directory
image_files = [os.path.join(image_dir, f) for f in os.listdir(image_dir) if f.endswith('.bmp')]

# Sort the image files in alphabetical order (you can customize the sorting)
image_files.sort()

# Get the first image to determine the video size
first_image = cv2.imread(image_files[0])
height, width, layers = first_image.shape

# Define the output video file and codec
output_video = 'demo.mp4'
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
video = cv2.VideoWriter(output_video, fourcc, 30, (width, height))

# Loop through the image files and write each frame to the video
for image_file in image_files:
    frame = cv2.imread(image_file)
    video.write(frame)

# Release the video object
video.release()

# Optional: Display a message when the video is created
print("Video created successfully: " + output_video)
