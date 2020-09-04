import wiotp.sdk.application
import json
options = wiotp.sdk.application.parseConfigFile("app.yaml")

client = wiotp.sdk.application.ApplicationClient(options)

def myEventCallback(event):
    str = "%s event '%s' received from device [%s]: %s"
    print(str % (event.format, event.eventId, event.device, json.dumps(event.data)))


client.connect()
client.deviceEventCallback = myEventCallback

client.subscribeToDeviceEvents(eventId="status1")
client.subscribeToDeviceEvents(eventId="status2")