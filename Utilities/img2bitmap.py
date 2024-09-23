import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
from PIL import Image
from datetime import datetime

# Variables to store the current threshold, width, and height values
current_threshold = 128  # Default threshold value
current_width = 64  # Default width
current_height = 64  # Default height
# source image
input_image_path = 'Utilities/Bitmap_files/Pinky_stock_art_sprite_pacman_website.webp'  # Replace with your input image path

# Function to update and threshold the image
def update_image(threshold=None, width=None, height=None):
    global current_threshold, current_width, current_height
    
    if threshold is not None:
        current_threshold = threshold  # Store the current threshold value
    
    if width is not None:
        current_width = int(width)  # Update the width
    
    if height is not None:
        current_height = int(height)  # Update the height
    
    # Resize the image based on the updated dimensions
    resized_image = image.resize((current_width, current_height))
    
    # Apply thresholding: pixels below the threshold are set to 0 (black), others to 255 (white)
    image_array = np.array(resized_image)
    bw_image_array = np.where(image_array < current_threshold, 0, 255)
    
    # Update the image displayed in the plot
    ax_img.set_data(bw_image_array)
    ax_img.set_extent((0, current_width, current_height, 0))  # Update axis size to match image
    fig.canvas.draw_idle()  # Redraw the figure to show the updated image

# Function to save the thresholded image and output to text file when the window is closed
def save_image(event):
    # Resize the image to the current dimensions and apply the final threshold before saving
    resized_image = image.resize((current_width, current_height))
    rotated_image = resized_image.rotate(270, expand=True)  # Rotate the image by 90 degrees
    bw_image_array = np.where(np.array(rotated_image) < current_threshold, 0, 255)
    
    

    # Convert the numpy array back to a binary array for output
    binary_output = np.where(bw_image_array == 0, 1, 0).astype(np.uint8)  # 1 for black, 0 for white
    output_lines = []
    
    # Generate output in the specified format with swapped dimensions
    output_lines.append(f"// Sprite {current_width}x{current_height}")
    output_lines.append(f"const unsigned char PROGMEM sprite[{current_width // 8}][{current_height}] = {{")
    
    # Process each column of the binary image instead of rows
    num_bytes = current_width // 8
    for row in range(num_bytes - 1, -1, -1):  # Now processing columns
        
        row_data = []
        for col in range(current_height):
            byte = 0
            for bit_pos in range(8):  # Pack bits into bytes
                # Invert the pixel value: 1 becomes 0 and 0 becomes 1
                pixel_value = 1 - binary_output[col, row * 8 + bit_pos]
                byte = (byte << 1) | pixel_value
            row_data.append(f"0b{byte:08b}")
        
        output_lines.append("\t{" + ",".join(row_data) + "}, // Row " + str(row))
    
    output_lines.append("};")

    # Format the datetime without invalid characters
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    
    # Save the output to a text file and use the formatted timestamp in the filename
    output_file_path = f'Utilities/Bitmap_files/{timestamp}_sprite_output.txt'

    with open(output_file_path, 'w') as f:
        f.write("\n".join(output_lines))
    
    print(f"Image data saved as text file at {output_file_path}")


    # Convert the numpy array back to a PIL Image
    bw_image = Image.fromarray(bw_image_array.astype(np.uint8))
    
    # Save the image to the specified output path
    output_image_path = f'Utilities/Bitmap_files/{timestamp}_sprite_image.bmp'
    bw_image.save(output_image_path)
    print(f"Image saved with threshold {current_threshold} and size {current_width}x{current_height} at {output_image_path}")



# Load and prepare the image
image = Image.open(input_image_path).convert('L')  # Convert image to grayscale

# Set up the matplotlib figure and axis
fig, ax = plt.subplots()
plt.subplots_adjust(left=0.1, bottom=0.35)  # Adjust for more space for sliders

# Display the initial image in grayscale
image_array = np.array(image.resize((current_width, current_height)))
ax_img = ax.imshow(image_array, cmap='gray', vmin=0, vmax=255)
ax.axis('off')  # Hide axes

# Create a slider for threshold adjustment
ax_threshold = plt.axes([0.1, 0.1, 0.8, 0.05], facecolor='lightgoldenrodyellow')
slider_threshold = Slider(ax_threshold, 'Threshold', 0, 255, valinit=current_threshold, valstep=1)

# Create sliders for width and height adjustment
ax_width = plt.axes([0.1, 0.2, 0.35, 0.05], facecolor='lightgoldenrodyellow')
slider_width = Slider(ax_width, 'Width', 8, 128, valinit=current_width, valstep=8)

ax_height = plt.axes([0.58, 0.2, 0.35, 0.05], facecolor='lightgoldenrodyellow')
slider_height = Slider(ax_height, 'Height', 8, 128, valinit=current_height, valstep=8)

# Update the image when any slider value is changed
slider_threshold.on_changed(lambda val: update_image(threshold=val))
slider_width.on_changed(lambda val: update_image(width=val))
slider_height.on_changed(lambda val: update_image(height=val))

# Register the function to save the image and text file when the window is closed
fig.canvas.mpl_connect('close_event', save_image)

# Initial call to set the threshold at the default value
update_image(128, current_width, current_height)

# Show the matplotlib window
plt.show()
