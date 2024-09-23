import numpy as np
import matplotlib.pyplot as plt

# Given values for the RC low-pass filter
R = 2000  # Resistance in ohms
C = 100e-9  # Capacitance in farads

# Calculate the cutoff frequency (fc) in Hz
fc = 1 / (2 * np.pi * R * C)

# Define the frequency range for the Bode plot (logarithmic scale)
frequencies = np.logspace(1, 6, 500)  # From 10 Hz to 1 MHz

# Calculate the transfer function H(jw) = 1 / (1 + jwRC)
omega = 2 * np.pi * frequencies  # Angular frequency
H = 1 / (1 + 1j * omega * R * C)

# Get magnitude and phase
magnitude = 20 * np.log10(np.abs(H))  # Convert to dB
phase = np.angle(H, deg=True)  # Convert to degrees

# Plot the magnitude and phase response
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6))

# Magnitude plot
ax1.semilogx(frequencies, magnitude)
ax1.set_title('Bode Plot of RC Low-Pass Filter')
ax1.set_ylabel('Magnitude (dB)')
ax1.grid(True)

# Phase plot
ax2.semilogx(frequencies, phase)
ax2.set_xlabel('Frequency (Hz)')
ax2.set_ylabel('Phase (degrees)')
ax2.grid(True)

plt.tight_layout()
plt.show()

# Print cutoff frequency
print("Cutoff frequency: ", fc)
