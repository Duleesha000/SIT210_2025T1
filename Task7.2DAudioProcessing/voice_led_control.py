import speech_recognition as sr
from gpiozero import LED
from time import sleep

# Setup
led = LED(17)
recognizer = sr.Recognizer()

def listen_command():
    with sr.Microphone() as source:
        print("Listening... Say 'switch on' or 'switch off'")
        audio = recognizer.listen(source)
        
    try:
        command = recognizer.recognize_google(audio)
        print(f"You said: {command}")
        return command.lower()
    except sr.UnknownValueError:
        print("Sorry, could not understand.")
        return None
    except sr.RequestError:
        print("Request error from Google Speech Recognition.")
        return None

while True:
    command = listen_command()
    
    if command:
        if "switch on" in command:
            led.on()
            print("LED Turned ON")
        elif "switch off" in command:
            led.off()
            print("LED Turned OFF")
        else:
            print("Command not recognized. Please say 'ON' or 'OFF'.")
    
    sleep(1)
