import matplotlib.pyplot as plt
import time
import struct
import threading
from queue import Empty  # For handling an empty queue
from SerialCommunicationDriver import SerialCommunicationDriver
from FancyPlotting import FancyPlottingControls

# Filter and convert to integers based on the received data
def filter_and_convert_to_int(data, bytes_to_read):
    """Converts raw data to a list of integers."""
    try:
        if len(data) >= bytes_to_read:  # Ensure we have enough bytes
            # Convert data to a list of int8
            data_list = list(struct.unpack(f'{bytes_to_read}b', data[:bytes_to_read]))
            return data_list[:bytes_to_read]
        else:
            print(f"Warning: Expected {bytes_to_read} bytes, received {len(data)} bytes.")
            return [0] * bytes_to_read
    except Exception as e:
        print(f"Error during conversion: {e}")
        return [0] * bytes_to_read

# Data acquisition from the serial port
def data_acquisition(bytes_to_read, driver, data_buffer, data_lock):
    """Thread to acquire data from the serial driver and store the latest in a buffer."""
    while True:
        try:
            data_raw = driver.read_raw()  # This will block until valid data is received

            if data_raw:
                data_converted = filter_and_convert_to_int(data_raw, bytes_to_read)

                if data_converted:
                    # Update the data buffer with the latest data
                    with data_lock:
                        data_buffer["latest_data"] = data_converted
                    print(f"Latest data stored: {data_converted}")
        except Exception as e:
            print(f"Serial read error: {e}")
        time.sleep(0.1)  # Adjust as per data rate

# Plotting logic, updated with the latest data from the buffer
def update_plotting(fpc, data_buffer, data_lock):
    """Main thread updates the plotting with the latest data from the buffer."""
    while True:
        try:
            # Read the latest data from the buffer
            with data_lock:
                data_converted = data_buffer.get("latest_data", None)

            if data_converted:
                # Unpack the data
                y_position = data_converted[0]
                x_position = data_converted[1]
                pad_left = data_converted[2]
                pad_right = data_converted[3]
                joy_btn = data_converted[4]
                left_btn = data_converted[5]
                right_btn = data_converted[6]

                # Update FancyPlottingControls with real data
                fpc.update_plot(
                    joystic_position_y=y_position,
                    joystic_position_x=x_position,
                    pad_left=pad_left,
                    pad_right=pad_right,
                    joy_btn=joy_btn,
                    left_btn=left_btn,
                    right_btn=right_btn,
                    t=time.time()  # Use current time for color generation
                )

        except Exception as e:
            print(f"Plot update error: {e}")

        plt.pause(0.1)  # Keep the plot updating smoothly

def main():
    baud_rate = 9600
    serial_port = '/dev/ttyS4'
    bytes_to_read = 7  # joystick y, joystick x, pad left, pad right, joy_btn, left_btn, right_btn

    # Shared data buffer and lock
    data_buffer = {}
    data_lock = threading.Lock()

    try:
        # Initialize the serial communication driver
        driver = SerialCommunicationDriver(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.")
        print("Press ESC to exit.")

        # Create the FancyPlottingControls instance
        fpc = FancyPlottingControls(fps=10)

        # Start the data acquisition thread (only this thread reads serial data)
        data_thread = threading.Thread(target=data_acquisition, args=(bytes_to_read, driver, data_buffer, data_lock), daemon=True)
        data_thread.start()

        # Start the plotting updates (on the main thread)
        update_plotting(fpc, data_buffer, data_lock)

    except Exception as e:
        print(f"Error: {e}")
    finally:
        driver.close()
        print("Serial connection closed.")
        plt.ioff()
        plt.show()

if __name__ == "__main__":
    main()
