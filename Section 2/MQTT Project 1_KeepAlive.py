import paho.mqtt.client as mqtt
from gpiozero import OutputDevice
import Adafruit_DHT
import sys
import time

broker="192.168.137.1"
port =1883
keepalive=5
relay = OutputDevice(17)

def on_message(client, userdata, message): #forprocessingsubscribedmessages
    msg = str(message.payload.decode("utf-8"))
    print("message received " ,msg)
    automation(msg)
    
def automation(msg):
    if msg == "on":
        relay.on()
    elif msg == "off":
        relay.off()
    else:
        relay.off()
        print("Invalid Message")
    
client = mqtt.Client()
client.on_message = on_message
client.connect(broker, port, keepalive)
client.loop_start()

try:
    while True:
        humidity, temperature = Adafruit_DHT.read_retry(11, 4)  #sensor, gpio
        humidity = round(humidity, 2)
        temperature = round(temperature, 2)
        print('Temp={0:0.1f}*C  Humidity={1:0.1f}%'.format(temperature, humidity))
        # Sending humidity and temperature data to Broker
        client.publish('sensor/temp',temperature, 0)
        client.publish('sensor/hum',humidity, 0)
        client.subscribe("automation/bulb1", 0)
        time.sleep(10)
except KeyboardInterrupt:
    pass

client.loop_stop()
client.disconnect()




