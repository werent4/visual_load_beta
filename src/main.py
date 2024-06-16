import time, json, psutil
import paho.mqtt.client as mqtt

TOPIC = "show_load"
PORT = 1883
mqtt_server = "10.0.59.200"
USERNAME = "werent4_test"
PASSWORD = "werent4test"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    print(f"Received message '{str(msg.payload)}' on topic '{msg.topic}' with QoS {str(msg.qos)}")

def reconnect(client):
    while not client.is_connected():
        print("Trying to connect to MQTT broker...")
        try:
            client.connect(mqtt_server, PORT, 60)
            print("Succesfully connected")
            break
        except Exception as e:
            print("Connection failed, trying again in 5 seconds...")
            time.sleep(5)


def send_memory_usage():
    memory = psutil.virtual_memory()
    used_memory = memory.used / (1024 ** 3)  
    available_memory = memory.total / (1024 ** 3)  
    message_memory = f"{used_memory:.2f}/{available_memory:.1f}GB"  # 

    cpu_usage = psutil.cpu_percent(interval=1)
    message_cpu = f"{cpu_usage}%"

    full_msg = {
        "RAM":message_memory,
        "CPU":message_cpu,
        "GPU":"--/--GB"
                }
    return json.dumps(full_msg)

def main():
    client = mqtt.Client()
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message

    reconnect(client) 

    while True:
        if not client.is_connected():
            reconnect(client)
        client.loop()

        msg = send_memory_usage()
        client.publish(TOPIC, msg)

        print(msg)
        time.sleep(2)

if __name__ == "__main__":
    main()