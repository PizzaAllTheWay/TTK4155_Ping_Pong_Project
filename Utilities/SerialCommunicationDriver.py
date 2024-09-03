import serial

class SerialCommunicationDriver:
    def __init__(self, port, baud_rate):
        self.ser = serial.Serial(port, baud_rate, timeout=1)

    def write(self, message):
        # Ensure the message is a string, then encode it to bytes, and add a newline at the end
        if isinstance(message, str):
            message = message.encode('utf-8')
        self.ser.write(message + b'\n')  # Append newline character and send as bytes, \n signifies to microcontroller its end of the message

    def read(self):
        return (self.ser.readline().decode('utf-8').strip()) + "\r"  # Read, decode, strip, and add carriage return

    def close(self):
        self.ser.close()
