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

#ikke 100% sikker på om denne er i orden mtp datatyper
#val er int, recieved er str (me thinks)
def compare(sent, recieved):
    val = sent - 1
    if (val == recieved):
        return True
    else:
        return False

def filter_and_convert_to_int(data):
    filtered_data = ord(str(data)[2])

    print(f"Raw data: \t\t\t {data} \nFiltered data: \t\t\t {filtered_data}")
    
    if filtered_data:
        return filtered_data
    else:
        return 0  # Return 0 if no valid characters are found

def main():
    """dont care about address right now"""
    #ext_ram = 0x1800; # Start address for the SRAM
    #max = 0x800; # Size of the SRAM in bytes 
    #aka we can write to any address in the range 0x1800-0x1FFF

    seed = int(time.time())
    random.seed(seed)

    serial_port = '/dev/ttyS4'
    baud_rate = 9600
    fail = 0
    reps = 1000

    try:
        # Initialize the serial communication driver
        driver = SerialCommunicationDriver(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.")
        print("Press ESC to exit.")
        print("Press ENTER to start, and to send data to the SRAM.")

        for _ in range(reps):
            # generate random number
            some_value = random.randint(34, 126) #oppfattes som hex i mc

            # Send byte to SRAM
            print(f"\nSending:\t\t\t {some_value}")
            driver.write(some_value.to_bytes(1, byteorder = 'big'))

            # Get SRAM value
            retrieved_value = driver.read_raw() #leses som hex
            converted_value = filter_and_convert_to_int(retrieved_value)

            # Compare the values
            print(f"Value recieved from SRAM: \t {converted_value}\r")
            print(f"Value should be: \t\t {some_value - 1}\r")

            
            if compare(some_value, converted_value):
                print(f"PASSED\r")
            else:
                print(f"FAILED\r")
                fail += 1

            print("========================================\r")
                        
    except Exception as e:
        print(f"Error: {e}")
    finally:
        driver.close()
        print("Serial connection closed.")
    
    fail_rate = 100*fail/reps
    print(f"fail percentage: \t\t {fail_rate:.2f}")

if __name__ == "__main__":
    main()