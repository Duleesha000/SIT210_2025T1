import tkinter as tk
import RPi.GPIO as GPIO

# GPIO pin setup
RED_PIN = 32   # GPIO 12
GREEN_PIN = 33 # GPIO 13
BLUE_PIN = 12  # GPIO 18

GPIO.setmode(GPIO.BOARD)
GPIO.setup(RED_PIN, GPIO.OUT)
GPIO.setup(GREEN_PIN, GPIO.OUT)
GPIO.setup(BLUE_PIN, GPIO.OUT)

# Start PWM on each pin at 0% duty cycle
red_pwm = GPIO.PWM(RED_PIN, 100)
green_pwm = GPIO.PWM(GREEN_PIN, 100)
blue_pwm = GPIO.PWM(BLUE_PIN, 100)

red_pwm.start(0)
green_pwm.start(0)
blue_pwm.start(0)

# GUI Setup
root = tk.Tk()
root.title("LED Brightness Control")

def update_red(value):
    red_pwm.ChangeDutyCycle(float(value))

def update_green(value):
    green_pwm.ChangeDutyCycle(float(value))

def update_blue(value):
    blue_pwm.ChangeDutyCycle(float(value))

def close_program():
    try:
        red_pwm.ChangeDutyCycle(0)
        green_pwm.ChangeDutyCycle(0)
        blue_pwm.ChangeDutyCycle(0)
        red_pwm.stop()
        green_pwm.stop()
        blue_pwm.stop()
        GPIO.cleanup()
    except:
        pass  # Prevent exceptions during cleanup
    root.destroy()

# Sliders and GUI widgets
tk.Label(root, text="Red LED Brightness").pack()
tk.Scale(root, from_=0, to=100, orient=tk.HORIZONTAL, command=update_red).pack()

tk.Label(root, text="Green LED Brightness").pack()
tk.Scale(root, from_=0, to=100, orient=tk.HORIZONTAL, command=update_green).pack()

tk.Label(root, text="Blue LED Brightness").pack()
tk.Scale(root, from_=0, to=100, orient=tk.HORIZONTAL, command=update_blue).pack()

tk.Button(root, text="Exit", command=close_program).pack(pady=10)

# Run GUI safely
try:
    root.mainloop()
finally:
    close_program()
