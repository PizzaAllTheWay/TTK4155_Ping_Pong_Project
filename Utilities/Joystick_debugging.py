import matplotlib.pyplot as plt
import numpy as np
import time
from SerialCommunicationDriver import SerialCommunicationDriver

enum direction {up, down, left, right, neutral}


def voltage_to_position(v):
    #get voltages, return position
    #assumes linear relationship between voltage and position
    volt = np.linspace(0, 5, 200)
    pos = np.linspace(-1, 1, 200)
    for i in range(200):
        if v <= volt[i]:
            return pos[i] #starts with low, returns once larger than v
    return 1

def position_to_direction(x, y, neutral):
    #the limits are defined by neutral

    if np.abs(x) < np.abs(y):
        #y is the dominant direction
        if y > neutral:
            return direction.up
        elif y < -neutral:
            return direction.down
        else:
            return direction.neutral
    else:
        #x is the dominant direction
        if x > neutral:
            return direction.right
        elif x < -neutral:
            return direction.left
        else:
            return direction.neutral
    

#i would prefer a float though
def filter_and_convert_to_int(data):
    filtered_data = ord(str(data)[2]) #get the third character

    print(f"Raw data: \t\t\t {data} \nFiltered data: \t\t\t {filtered_data}")
    
    if filtered_data:
        return filtered_data
    else:
        return 0 

def main():
    baud_rate = 9600
    serial_port = '/dev/ttyS4'

    neutral_zone = 0.2 #neutral zone limit [0,1]

    #joystick y, joystick x, button, touch
    bytes_to_read = 4
    raw_data = [0] * bytes_to_read
    converted_data = [0] * bytes_to_read


    try:
        # Initialize the serial communication driver
        driver = SerialCommunicationDriver(serial_port, baud_rate)
        print(f"Connected to {serial_port} at {baud_rate} baud rate.")
        print("Press ESC to exit.")
        print("Press ENTER to start")

        #initialize the plot
        plt.ion()
        fig, ((ax1, ax2), (ax3, ax4), (ax5, _)) = plt.subplots(3, 2)
        y_data, x_data, btn_data, touch = [], [], [], []
        line1, = ax1.plot(y_data, 'r-')
        line2, = ax2.plot(x_data, 'b-')
        line3, = ax3.plot(btn_data, 'g-')
        line4, = ax4.plot(touch, 'y-')
        scatter = ax5.plot([], [], 'ro')

        #titles
        ax1.set_title("Y-axis")
        ax2.set_title("X-axis")
        ax3.set_title("Button")
        ax4.set_title("Touch")
        ax5.set_title("Joystick position")

        while(True):
            #recieve and convert data, one byte at a time
            #don't know if it works if the input is an int
            for i in range (bytes_to_read):
                raw_data[i] = driver.read_raw()
                converted_data[i] = filter_and_convert_to_int(raw_data[i])

            #calculate position from data
            y_position = voltage_to_position(converted_data[0])
            x_position = voltage_to_position(converted_data[1])

            #direction of joystick
            direction = position_to_direction(x_position, y_position, neutral_zone)
            print(f"Direction: {direction}")

            #update data
            y_data.append(y_position)
            x_data.append(x_position)
            btn_data.append(converted_data[2])
            touch.append(converted_data[3])

            #update plot
            line1.set_xdata(x_data)
            line2.set_ydata(y_data)
            line3.set_ydata(btn_data)
            line4.set_ydata(touch)
            ax1.relim()
            ax1.autoscale_view()
            ax2.relim()
            ax2.autoscale_view()
            ax3.relim()
            ax3.autoscale_view()
            ax4.relim()
            ax4.autoscale_view()
            scatter.set_xdata([x_position])
            scatter.set_ydata([y_position])
            ax5.relim()
            ax5.autoscale_view()

            plt.draw()
            plt.pause(0.1)
                        
    except Exception as e:
        print(f"Error: {e}")
    finally:
        driver.close()
        print("Serial connection closed.")
        plt.ioff()
        plt.show()

if __name__ == "__main__":
    main()