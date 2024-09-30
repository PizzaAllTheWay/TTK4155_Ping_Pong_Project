import random
import time
import sys
import termios
import tty
import struct
from SerialCommunicationDriver import SerialCommunicationDriver

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

def main():
    serial_port = '/dev/ttyS4'
    baud_rate = 9600
    bytes_to_read = 7

    try:
        # Initialize the serial communication driver
        driver = SerialCommunicationDriver(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.\n")

        while(True):
            retrieved_data = driver.read_raw() #leses som hex
            print(f"Received: \t{retrieved_data}\r")
            converted_data = filter_and_convert_to_int(retrieved_data, bytes_to_read)
            #print("========================================\r")
                        
    except Exception as e:
        print(f"Error: {e}")
    finally:
        driver.close()
        print("Serial connection closed.")

if __name__ == "__main__":
    main()