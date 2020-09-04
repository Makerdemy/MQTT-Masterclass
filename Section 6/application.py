import wiotp.sdk.application
import json
import base64

options = wiotp.sdk.application.parseConfigFile("app.yaml")
client = wiotp.sdk.application.ApplicationClient(options)



def myEventCallback(event):
    str = "%s event '%s' received from device [%s]: %s"
    print(str % (event.format, event.eventId, event.device, json.dumps(event.data)))


client.connect()
device = {"typeId": "gpsclient", "deviceId": "gpsesp"}
lastEvents = client.lec.getAll(device)


for event in lastEvents:
    print("- Cached at: %s" % (event.timestamp.isoformat()))
    # Depending on the content of the message this may not be a good idea (e.g. if it was originally binary data)
    print("- Payload (decoded): %s" % (base64.b64decode(event.payload).decode('utf-8')))
    
    
client.deviceEventCallback = myEventCallback

client.subscribeToDeviceEvents(eventId="status1") #latitude
client.subscribeToDeviceEvents(eventId="status2") #longitude