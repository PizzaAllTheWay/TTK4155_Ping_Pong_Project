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

    def read_raw(self):
        """Blocking raw read until valid data is received."""
        while True:
            raw_data = self.ser.read(self.ser.in_waiting)  # Read all available bytes

            # Validate the received data
            if self.validate_data(raw_data):
                return raw_data  # Return valid data

    def validate_data(self, raw_data):
        """Validate the raw data to filter out invalid or corrupted data."""
        if not raw_data:
            return False  # Skip if no data was read

        if len(raw_data) < 7:  # Check if data is at least as long as expected
            return False  # Ignore incomplete data

        if raw_data.startswith(b'\x00'):  # You can add additional conditions here
            return False  # Skip if the data starts with null bytes

        # Additional validation logic can go here based on your protocol
        return True  # If all conditions are met, the data is considered valid

    def close(self):
        self.ser.close()
