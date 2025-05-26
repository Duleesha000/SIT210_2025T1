from flask import Flask, jsonify, render_template, request
import json
import ssl
import threading
from gpiozero import OutputDevice, Button, LED
import paho.mqtt.client as mqtt
import smtplib
from email.mime.text import MIMEText

app = Flask(__name__)

# GPIO Setup
fan = OutputDevice(17)
button = Button(27)
humidity_led = LED(22)
temperature_led = LED(23)
fan_led = LED(24)

# State
sensor_data = {
    'temperature': 0,
    'humidity': 0,
    'eco2': 0,
    'tvoc': 0,
    'fan_status': 'OFF',
    'mode': 'AUTO',
    'threshold': 1000
}
manual_override = False

@app.route('/')
def index():
    return render_template('dashboard.html')

@app.route('/status')
def status():
    return jsonify(sensor_data)

@app.route('/toggle', methods=['POST'])
def toggle_fan():
    global manual_override
    manual_override = True
    sensor_data['mode'] = 'MANUAL'
    if fan.is_active:
        fan.off()
        fan_led.off()
        sensor_data['fan_status'] = 'OFF (Manual)'
    else:
        fan.on()
        fan_led.on()
        sensor_data['fan_status'] = 'ON (Manual)'
    return jsonify(sensor_data)

@app.route('/reset', methods=['POST'])
def reset_auto():
    global manual_override
    manual_override = False
    sensor_data['mode'] = 'AUTO'
    return jsonify(sensor_data)

@app.route('/threshold', methods=['POST'])
def set_threshold():
    try:
        sensor_data['threshold'] = int(request.form['value'])
        return jsonify({'success': True, 'threshold': sensor_data['threshold']})
    except:
        return jsonify({'success': False}), 400

def send_email_alert():
    subject = "⚠️ Fan Activated - Smart Room Alert"
    body = f"""
    Hello,

    The fan in your smart room system was turned ON automatically due to high eCO₂ levels.

    Current Sensor Readings:
    - Temperature: {sensor_data['temperature']} °C
    - Humidity: {sensor_data['humidity']} %
    - eCO₂: {sensor_data['eco2']} ppm
    - TVOC: {sensor_data['tvoc']} ppb
    - Mode: {sensor_data['mode']}

    Regards,
    Smart Room Air Quality Optimizer Monitor
    """

    msg = MIMEText(body)
    msg['Subject'] = subject
    msg['From'] = "pereraduleesha6@gmail.com"
    msg['To'] = "pereraduleesha6@gmail.com"

    try:
        with smtplib.SMTP_SSL('smtp.gmail.com', 465) as server:
            server.login("pereraduleesha6@gmail.com", "ydzanpowfmqwgzao")
            server.send_message(msg)
            print("✅ Alert email sent.")
    except Exception as e:
        print("❌ Failed to send email:", e)

def handle_button_press():
    global manual_override
    manual_override = True
    sensor_data['mode'] = 'MANUAL'
    if fan.is_active:
        fan.off()
        fan_led.off()
        sensor_data['fan_status'] = 'OFF (Manual - Physical Button)'
    else:
        fan.on()
        fan_led.on()
        sensor_data['fan_status'] = 'ON (Manual - Physical Button)'
    print("🟢 Fan toggled by physical button.")

# Attach physical button event
button.when_pressed = handle_button_press

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ MQTT connected")
        client.subscribe("smartroom/airquality")
    else:
        print("❌ MQTT connect failed")

def on_message(client, userdata, msg):
    global manual_override
    data = json.loads(msg.payload.decode())

    sensor_data['temperature'] = data.get('temperature', 0)
    sensor_data['humidity'] = data.get('humidity', 0)
    sensor_data['eco2'] = data.get('eco2', 0)
    sensor_data['tvoc'] = data.get('tvoc', 0)

    temperature_led.on() if sensor_data['temperature'] > 15 else temperature_led.off()
    humidity_led.on() if sensor_data['humidity'] > 70 else humidity_led.off()

    if not manual_override:
        if sensor_data['eco2'] > sensor_data['threshold']:
            if not fan.is_active:
                fan.on()
                fan_led.on()
                sensor_data['fan_status'] = f"ON (Auto > {sensor_data['threshold']})"
                send_email_alert()
        else:
            fan.off()
            fan_led.off()
            sensor_data['fan_status'] = f"OFF (Auto ≤ {sensor_data['threshold']})"

client = mqtt.Client()
client.username_pw_set("user", "1234")
client.tls_set(cert_reqs=ssl.CERT_NONE)
client.tls_insecure_set(True)
client.on_connect = on_connect
client.on_message = on_message
client.connect("o321250b.ala.eu-central-1.emqxsl.com", 8883)

threading.Thread(target=client.loop_forever, daemon=True).start()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
