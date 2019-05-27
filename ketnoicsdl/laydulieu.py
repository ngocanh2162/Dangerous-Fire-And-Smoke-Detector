
import paho.mqtt.client as mqtt
from ghidulieu import ghigiatri

# MQTT Settings 
MQTT_Broker = "192.168.1.233"
MQTT_Port = 1883
MQTT_Topic = "sensor/#"

#Subscribe to all Sensors at Base Topic
def on_connect(client,user,msg,rc):
	mqttc.subscribe(MQTT_Topic, 0)

#Save Data into DB Table
def on_message(client, userdata, msg):
	print "MQTT Topic: " + msg.topic  
	print "Data: " + msg.payload
	ghigiatri(msg)

def on_subscribe(client, userdata, mid, granted_qos):
    pass

mqttc = mqtt.Client()

# Assign event callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe

# Connect
mqttc.connect(MQTT_Broker, int(MQTT_Port))

# Continue the network loop
mqttc.loop_forever()
