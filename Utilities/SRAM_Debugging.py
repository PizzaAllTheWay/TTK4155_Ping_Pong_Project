import random
import time
import sys
import termios
import tty
from SerialCommunicationDriver import SerialCommunicationDriver

"""
rand numer inr 8
1-255

sender du det til mc
vet til data tilbake (data du ga meg - 1)
samenlight  TRUE/FALSE på om det var korrekt før og etter
"""

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


def main():
    """dont care about address right now"""
    #ext_ram = 0x1800; # Start address for the SRAM
    #max = 0x800; # Size of the SRAM in bytes 
    #aka we can write to any address in the range 0x1800-0x1FFF

    seed = int(time.time())
    random.seed(seed)

    serial_port = '/dev/ttyS4'
    baud_rate = 9600

    try:
        # Initialize the serial communication driver
        driver = SerialCommunicationDriver(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.")
        print("Press ESC to exit.")
        print("Press ENTER to start, and to send data to the SRAM.")
        
        while True:
            key = getch()
            if key == '\x1b':  # ESC key
                print("ESC pressed. Exiting...")
                break
            elif key == '\r':  # Enter key
                some_value = random.randint(1, 255)
                some_value &= 0xFF # Ensure the value is within 8 bits
                print(f"ENTER pressed. Sending:\t {some_value}")
                driver.write(some_value)

                time.sleep(0.5)
                retrieved_value = driver.read()

                if retrieved_value: 
                    if retrieved_value == (some_value - 1):
                        print(f"SRAM test completed with no errors")
                    else:
                        print(f"Value recieved from SRAM: {retrieved_value} \t(should be {some_value - 1})")
                else:
                    print("No data recieved :C")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        driver.close()
        print("Serial connection closed.")

if __name__ == "__main__":
    main()