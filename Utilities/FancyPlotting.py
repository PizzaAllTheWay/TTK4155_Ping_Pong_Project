import matplotlib.pyplot as plt
import numpy as np

class FancyPlottingControls:
    def __init__(self, data_range=100, fps=30):
        self.data_range = data_range
        self.fps = fps
        
        # Initialize data buffers
        self.pad_left_data = [0] * self.data_range
        self.pad_right_data = [0] * self.data_range
        self.joystic_btn_data = [0] * self.data_range
        self.left_btn_data = [0] * self.data_range
        self.right_btn_data = [0] * self.data_range
        self.joystic_data_y = [0] * self.data_range
        self.joystic_data_x = [0] * self.data_range

        # Set up the grid layout
        self.fig, self.ax1, self.ax2, self.ax3 = self.setup_plot()

        # Initialize plot lines with custom RGB colors
        self.line1_pad_left, = self.ax1.plot(self.pad_left_data, color=(0.75, 0.6, 0.85), label='Left Pad Data')  # Soft Lilac
        self.line1_pad_right, = self.ax1.plot(self.pad_right_data, color=(1.0, 0.7, 0.8), label='Right Pad Data')  # Soft Pink

        # Initialize binary button plot with softer colors
        self.line_btn_joy, = self.ax3.plot(self.joystic_btn_data, 'o-', color=(0.5, 0.5, 1.0), label='Joystick Button')  # Soft Blue
        self.line_btn_left, = self.ax3.plot(self.left_btn_data, 'o-', color=(0.5, 1.0, 0.5), label='Left Button')  # Soft Green
        self.line_btn_right, = self.ax3.plot(self.right_btn_data, 'o-', color=(1.0, 0.5, 0.5), label='Right Button')  # Soft Red

        # Initialize scatter plot for joystick position (empty at start)
        self.scatter = self.ax2.scatter([], [], c='r', label='Joystick Position', s=200)

        # Enable blitting for better performance
        self.background = None
        self.ax1.set_xlim(0, self.data_range)
        self.ax1.set_ylim(-110, 110)
        self.ax3.set_xlim(0, self.data_range)
        self.ax3.set_ylim(-2, 2)
        self.ax2.set_xlim(-110, 110)
        self.ax2.set_ylim(-110, 110)
        plt.pause(0.1)  # Small pause to set things up
        self.background = self.fig.canvas.copy_from_bbox(self.fig.bbox)

    def setup_plot(self):
        # Set up the grid layout with GridSpec
        from matplotlib.gridspec import GridSpec
        fig = plt.figure(constrained_layout=True)
        gs = GridSpec(2, 2, width_ratios=[1, 2], figure=fig)

        # Create subplots: Left-top (pads), Left-bottom (buttons), and Right (joystick)
        ax1 = fig.add_subplot(gs[0, 0])  # Top-left: Pad data
        ax3 = fig.add_subplot(gs[1, 0])  # Bottom-left: Button data
        ax2 = fig.add_subplot(gs[:, 1])  # Right side: Joystick (spanning both rows)

        # Set titles for each subplot
        ax1.set_title("Left Pad vs Right Pad")
        ax2.set_title("Joystick X vs Y")
        ax3.set_title("Button States")

        return fig, ax1, ax2, ax3

    def get_rgb_color(self, t):
        """ Generate RGB color dynamically based on time `t`."""
        r = (np.sin(t) + 1) / 2  # Sine wave between 0 and 1 for red
        g = (np.sin(t + 2*np.pi/3) + 1) / 2  # Shifted sine wave for green
        b = (np.sin(t + 4*np.pi/3) + 1) / 2  # Shifted sine wave for blue
        return (r, g, b)

    def update_plot(self, joystic_position_y, joystic_position_x, pad_left, pad_right, joy_btn, left_btn, right_btn, t):
        # Restore the background (fast redraw)
        self.fig.canvas.restore_region(self.background)

        # Update data buffers
        self.pad_left_data.append(pad_left)
        self.pad_left_data = self.pad_left_data[-self.data_range:]  # Keep data within range
        self.pad_right_data.append(pad_right)
        self.pad_right_data = self.pad_right_data[-self.data_range:]  # Keep data within range
        self.joystic_btn_data.append(joy_btn)
        self.joystic_btn_data = self.joystic_btn_data[-self.data_range:]  # Keep data within range
        self.left_btn_data.append(left_btn)
        self.left_btn_data = self.left_btn_data[-self.data_range:]  # Keep data within range
        self.right_btn_data.append(right_btn)
        self.right_btn_data = self.right_btn_data[-self.data_range:]  # Keep data within range
        self.joystic_data_y.append(joystic_position_y)
        self.joystic_data_y = self.joystic_data_y[-self.data_range:]  # Keep data within range
        self.joystic_data_x.append(joystic_position_x)
        self.joystic_data_x = self.joystic_data_x[-self.data_range:]  # Keep data within range

        # Update plot data
        self.line1_pad_left.set_ydata(self.pad_left_data)
        self.line1_pad_right.set_ydata(self.pad_right_data)
        self.line_btn_joy.set_ydata(self.joystic_btn_data)
        self.line_btn_left.set_ydata(self.left_btn_data)
        self.line_btn_right.set_ydata(self.right_btn_data)

        # Update scatter plot with new position and color
        color = self.get_rgb_color(t)
        self.scatter.set_offsets(np.c_[[joystic_position_x], [joystic_position_y]])  # Update position
        self.scatter.set_color([color])  # Update color

        # Redraw the areas that changed
        self.ax1.draw_artist(self.line1_pad_left)
        self.ax1.draw_artist(self.line1_pad_right)
        self.ax3.draw_artist(self.line_btn_joy)
        self.ax3.draw_artist(self.line_btn_left)
        self.ax3.draw_artist(self.line_btn_right)
        self.ax2.draw_artist(self.scatter)

        # Add legends after the first update
        self.ax1.legend()
        self.ax3.legend()
        self.ax2.legend()

        # Blit the updated area to the canvas
        self.fig.canvas.blit(self.fig.bbox)
        self.fig.canvas.flush_events()
        plt.pause(1/self.fps)