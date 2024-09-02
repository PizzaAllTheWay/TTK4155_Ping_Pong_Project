import serial
import sys
import termios
import tty
import threading

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

def listen_serial(ser):
    """Listens to the serial port and prints messages when received."""
    try:
        while True:
            message = ser.readline().decode('utf-8').strip()
            if message:
                print(f"Received: {message} \r") #'\r' shifts the cursor to the beginning of the curent line

    except serial.SerialException as e:
        print(f"Error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")

def main():
    # Update these variables with your serial port and baud rate.
    serial_port = '/dev/ttyS4'
    baud_rate = 9600

    special_message = b'Coolbeans\n' #needs to end with \n

    try:
        # Initialize the serial connection
        ser = serial.Serial(serial_port, baud_rate, timeout=1)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.")

        # Start the serial listener in a separate thread.
        listener_thread = threading.Thread(target=listen_serial, args=(ser,), daemon=True)
        listener_thread.start()

        print("Press ESC to send 'Hello World :)' and exit.")
        print("Press ENTER to send 'ENTER' to the serial line.")
        
        while True:
            key = getch()
            if key == '\x1b':  # ESC key
                print("ESC pressed. Sending 'Hello World :)' and exiting...")
                ser.write(b'Hello World :)\n')
                break
            elif key == '\r':  # Enter key
                print(f"ENTER pressed. Sending '{special_message}'")
                ser.write(special_message)
    except serial.SerialException as e:
        print(f"Error: {e}")
    finally:
        ser.close()
        print("Serial connection closed.")

if __name__ == "__main__":
    main()