

import asyncio\
from bleak import BleakClient, BleakScanner\
import RPi.GPIO as GPIO\
\
# === Configuration ===\
LED_PIN = 18  # Use GPIO18 (supports PWM)\
SERVICE_UUID = "180C"\
CHAR_UUID = "2A56"\
\
# === GPIO Setup ===\
GPIO.setmode(GPIO.BCM)\
GPIO.setwarnings(False)              # Suppress warnings if pin is reused\
GPIO.setup(LED_PIN, GPIO.OUT)\
pwm = GPIO.PWM(LED_PIN, 1000)        # 1kHz PWM frequency\
pwm.start(0)\
\
# === Brightness Logic Based on Distance ===\
def set_led_brightness(distance_cm):\
    if distance_cm > 100:\
        brightness = 0\
    elif distance_cm > 50:\
        brightness = 40\
    elif distance_cm > 20:\
        brightness = 80\
    else:\
        brightness = 100\
\
    pwm.ChangeDutyCycle(brightness)\
    print(f"? \{distance_cm\} cm ? LED Brightness: \{brightness\}%")\
\
# === Main BLE Connection Logic ===\
async def run():\
    print("? Scanning for BLE devices...")\
    devices = await BleakScanner.discover()\
    target = None\
    for d in devices:\
        if d.name == "ParkingSensor":\
            target = d\
            break\
\
    if not target:\
        print("? 'ParkingSensor' device not found.")\
        return\
\
    print(f"? Connecting to \{target.address\}...")\
    try:\
        async with BleakClient(target.address, services_resolved=True, timeout=20.0) as client:\
            print("? Connected.")\
\
            def handle_notify(sender, data):\
                distance = int.from_bytes(data, byteorder='little')\
                set_led_brightness(distance)\
\
            await client.start_notify(CHAR_UUID, handle_notify)\
            print("? Receiving distance data... Press Ctrl+C to stop.")\
            await asyncio.sleep(90)  # Run for 90 seconds\
            await client.stop_notify(CHAR_UUID)\
\
    except Exception as e:\
        print(f"? Connection failed: \{e\}")\
\
# === Run BLE with Cleanup ===\
try:\
    asyncio.run(run())\
finally:\
    pwm.stop()\
    GPIO.cleanup()}
