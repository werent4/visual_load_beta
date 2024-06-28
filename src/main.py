import time, json, psutil, requests, argparse
import paho.mqtt.client as mqtt

def get_memory_usage():
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
    return full_msg

def send_memory_usage(ip: str):
    url = f'http://{ip}:80/data'
    headers = {'Content-Type': 'application/json'}
    full_msg = get_memory_usage()
    response = requests.post(url, headers=headers, json=full_msg)
    print(f"Status Code: {response.status_code}, Response: {response.text}")


def main(args):
    while True:
        send_memory_usage(args.IP)
        time.sleep(2)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Enter IP addr")
    parser.add_argument("--IP", default= "192.168.20.100" ,type=str, help="Name of the dataset to use")
    args = parser.parse_args()
    main(args)