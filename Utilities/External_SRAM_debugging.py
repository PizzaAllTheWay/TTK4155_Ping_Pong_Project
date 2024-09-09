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
    reps = 100

    try:
        # Initialize the serial communication driver
        driver = SerialCommunicationDriver(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.")
        print("Press ESC to exit.")
        print("Press ENTER to start, and to send data to the SRAM.")

        for _ in range(reps):
            some_value = random.randint(1, 255)
            print(f"\nSending:\t\t\t {some_value}")
            driver.write(some_value.to_bytes(1, byteorder = 'big'))
            driver.write('\n')
            retrieved_value = driver.read()
            if retrieved_value: 
                if compare(some_value, retrieved_value):
                    print(f"SRAM test completed with no errors.\r")
                else:
                    print(f"Value recieved from SRAM: \t{retrieved_value}\r")
                    print(f"Value should be: \t\t {some_value - 1}\r")
                    fail += 1
            else:
                print("No bitches?")            
    except Exception as e:
        print(f"Error: {e}")
    finally:
        driver.close()
        print("Serial connection closed.")
    
    fail_rate = 100*fail/reps
    print(f"fail percentage: \t {fail_rate:.2f}")

if __name__ == "__main__":
    main()