import sys
import termios
import tty
import threading
from SerialCommunicationDriver import SerialCommunicationDriver  # Import the custom SerialCommunicationDriver

def filter_and_convert_to_int(data):
    # data is a bytes object
    if data:
        # Convert data to a list of integers
        data_list = list(data)
        # Return the first byte's integer value
        value = data_list[0]
        return value
    else:
        return 0

def getch():
    """Captures keyboard input without pressing enter."""
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch

def listen_serial(driver):
    """Listens to the serial port and prints messages when received."""
    try:
        while True:
            message = driver.read_raw()
            if message:
                print(f"Received: {message}\r")
                print(f"int: {filter_and_convert_to_int(message)}\r")

    except Exception as e:
        print(f"Unexpected error: {e}")

def main():
    # Update these variables with your serial port and baud rate.
    serial_port = '/dev/ttyS4'
    baud_rate = 9600 

    special_message = "Coolbeans"

    try:
        # Initialize the serial communication driver
        driver = SerialCommunicationDriver(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.")

        # Start the serial listener in a separate thread.
        listener_thread = threading.Thread(target=listen_serial, args=(driver,), daemon=True)
        listener_thread.start()

        print("Press ESC to send 'Hello World :)' and exit.")

        print("Press ENTER to send 'Coolbeans' to the serial line.")
        
        while True:
            key = getch()
            if key == '\x1b':  # ESC key
                print("ESC pressed. Sending 'Hello World :)' and exiting...")
                driver.write("Hello World :)")
                break
            elif key == '\r':  # Enter key
                print(f"ENTER pressed. Sending '{special_message}'")
                driver.write(special_message)
    except Exception as e:
        print(f"Error: {e}")
    finally:
        driver.close()
        print("Serial connection closed.")

if __name__ == "__main__":
    main()
