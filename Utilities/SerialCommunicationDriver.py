import serial

class SerialCommunicationDriver:
    def __init__(self, port, baud_rate):
        self.ser = serial.Serial(port, baud_rate, timeout=1)
        self.ser.flushInput()  # Flush input buffer
        self.ser.flushOutput()  # Flush output buffer

    def write(self, message):
        # Ensure the message is a string, then encode it to bytes, and add a newline at the end
        if isinstance(message, str):
            message = message.encode('utf-8')
        self.ser.write(message + b'\n')  # Append newline character and send as bytes

    def read(self):
        data = self.ser.readline().decode('utf-8', errors='ignore').strip()
        if data:  # Only add carriage return if data is not empty
            data += "\r"
        return data

    def read_raw(self):
        raw_data = self.ser.readline()
        #if raw_data:  # Only add carriage return if data is not empty
            #raw_data += "\r"
        return raw_data

    def close(self):
        self.ser.close()