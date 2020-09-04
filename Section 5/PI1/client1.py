import wiotp.sdk.application
import json
options = wiotp.sdk.application.parseConfigFile("app.yaml")
client = wiotp.sdk.application.ApplicationClient(options)


def myEventCallback(event):
    str = "Message Received from [%s]: %s"
    print(str % (event.device, json.dumps(event.data)))
    
def eventPublishCallback():
    print("Message Sent")

    
while True:
    client.connect()
    client.deviceEventCallback = myEventCallback
    client.subscribeToDeviceEvents(typeId="Raspberrypi1", deviceId="client1", eventId="status1")
    msg = input(" ")
    myData={'name' : 'Naveen', 'MSG' : msg}
    client.publishEvent(typeId="Raspberrypi2", deviceId="client2", eventId="status2", msgFormat="json", data=myData, qos=2, onPublish=eventPublishCallback)

