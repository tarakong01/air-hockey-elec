import matplotlib.pyplot as plt
import matplotlib.lines as lines
import matplotlib.animation as animation
from matplotlib import style
import numpy as np
import random
import serial

#initialize serial port
ser = serial.Serial()
ser.port = 'COM3' #Arduino serial port
ser.baudrate = 250000
ser.timeout = 50 #specify timeout when using readline()
ser.open()
if ser.is_open==True:
	print("\nAll right, serial port now open. Configuration:\n")
	print(ser, "\n") #print serial parameters

# Create figure for plotting
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
count = [0]
xs = [] #store index here
ys = [] #store values here

# This function is called periodically from FuncAnimation
def animate(i, xs, ys, count):
    
    #Aquire and parse data from serial port
    line=ser.readline()
    line_as_list = line.split(b'\r\n')
    print(line_as_list)
    str = line_as_list[0].decode('ascii').strip()
    # print(str)
    # val = float(str)
	
	# # Add x and y to lists
    # xs.append(count[0])
    # ys.append(val)
    # count[0] += 1

    # Limit x and y lists to 20 items
    #xs = xs[-20:]
    #ys = ys[-20:]

    # Draw x and y lists
    # my_line.set_data(xs, ys)

    ax.clear()
    ax.plot(xs, ys, label="Voltage (amplified)") 
    # ^^ is plotting the whole thing each time - can we instead do one point at a time?

#     # Format plot
#     plt.xticks(rotation=45, ha='right')
#     plt.subplots_adjust(bottom=0.30)
#     plt.title('This is how I roll...')
#     plt.ylabel('Relative frequency')
#     plt.legend()
#     plt.axis([1, None, 0, 1.1]) #Use for arbitrary number of trials
#     #plt.axis([1, 100, 0, 1.1]) #Use for 100 trial demo

# # Set up plot to call animate() function periodically
ani = animation.FuncAnimation(fig, animate, frames=100, fargs=(xs, ys, count), interval=20)
plt.show()