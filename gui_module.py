import tkinter as tk
import serial
import time
from threading import Thread

# Import your modules
# Assuming your other modules (helpers, states, client) are set up correctly
from helpers import calc_spin, update_tkinter_input_box
from states import motor_state

# --- Serial Connection Setup ---
try:
    # Adjust port name if necessary, it should be the same as the one used for uploading
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    time.sleep(2) # Wait for serial port to initialize
    print("Serial connection established.")
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    ser = None

# --- Serial Command Functions ---
def send_command(command, value=None):
    if ser:
        if value is not None:
            message = f"{command}{value}\n".encode()
            ser.write(message)
        else:
            message = f"{command}\n".encode()
            ser.write(message)
        
        try:
            response = ser.readline().decode().strip()
            print(f"Arduino response: {response}")
        except:
            print("No response from Arduino within timeout.")
    else:
        print("Serial port not connected.")

# --- Motor Control Functions (Sending Commands) ---
def adjust_speed(direction, result_label, inc_val, freq_tk, total_revs_tk):
    
    steps_per_rev = motor_state['spr']
    steps_per_sec = 1.0 / ( 2.0 * motor_state['delay'])
    freq_rpm = (steps_per_sec * 60.0) / steps_per_rev
    
    if(direction == 'u'):
        freq_rpm += float(inc_val.get())
    elif(direction == 'd'):
        freq_rpm -= float(inc_val.get())
    
    calc_spin(freq_rpm, motor_state['revs'], motor_state, result_label, freq_tk, total_revs_tk)
    
    new_speed_sps = (freq_rpm / 60) * steps_per_rev
    send_command('S', int(new_speed_sps))

def start_motor(checkbox_val):
    if checkbox_val.get():
        # This part requires more advanced logic on Arduino for continuous running
        # For a simple start, we can just send the current speed
        # Or you could send a specific command for continuous operation, e.g., 'C'
        new_speed_sps = (motor_state['delay'] * 60) * motor_state['spr'] 
        send_command('S', int(new_speed_sps))
    else:
        new_speed_sps = (motor_state['delay'] * 60) * motor_state['spr']
        send_command('S', int(new_speed_sps))
        # Logic for fixed steps is more complex via serial; it's better to send 'steps' to the Arduino.
        # For now, we'll keep it simple and just send the speed.
        
def stop_motor():
    send_command('X')

def reverse_direction():
    send_command('R')

def handle_enter(event=None):
    calc_spin(freq.get(), total_revs.get(), motor_state, result_label, freq, total_revs)

# --- GUI Creation ---
root = tk.Tk()
root.title("Motor Control & Angle Tracker")
root.lift()
root.attributes('-topmost', True)
root.after(100, lambda: root.attributes('-topmost', False))

# GUI state vars must be defined after root
inc_val = tk.StringVar(value="0.1")
checkbox_val = tk.IntVar()

# GUI widgets
result_label = tk.Label(root, text="")
start_button = tk.Button(root, text="Start Motor", command=lambda: start_motor(checkbox_val))
stop_button = tk.Button(root, text="Stop Motor", command=stop_motor)
reverse_button = tk.Button(root, text="Reverse", command=reverse_direction)
speed_up_button = tk.Button(root, text="Speed Up", command=lambda: adjust_speed('u', result_label, inc_val, freq, total_revs))
slow_down_button = tk.Button(root, text="Slow Down", command=lambda: adjust_speed('d', result_label, inc_val, freq, total_revs))
motor_info_button = tk.Button(root, text="Send Motor Info", command=lambda: send_motor_info(motor_state))
inc_label = tk.Label(root, text="Incremental value for speed adjustments (in revs per minute)")
inc = tk.Spinbox(root, from_=0, to=10, increment=0.1, textvariable=inc_val)
freq_label = tk.Label(root, text="Enter frequency (in revolutions per minute):")
freq = tk.Entry(root, width=30)
revs_label = tk.Label(root, text="Enter total revolutions:")
total_revs = tk.Entry(root, width=30)
checkbox = tk.Checkbutton(root, text="Run motor until stopped", variable=checkbox_val, onvalue=1, offvalue=0)
input_button = tk.Button(root, text="Get Input", command=handle_enter)

update_tkinter_input_box(freq, 30)
update_tkinter_input_box(total_revs, motor_state['revs'])

for w in [start_button, stop_button, reverse_button, speed_up_button, slow_down_button, motor_info_button,
          inc_label, inc, freq_label, freq, revs_label, total_revs,
          checkbox, input_button, result_label]:
    w.pack(pady=5)

root.bind("<Return>", handle_enter)
root.bind("<KP_Enter>", handle_enter)

def run_gui():
    print('running run_gui')
    root.mainloop()

if __name__ == '__main__':
    run_gui()