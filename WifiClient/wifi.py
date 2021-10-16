import subprocess

def get_wifi_networks_names():
    print("Listing all availables Wifi networks...")
    output = subprocess.run("nmcli dev wifi", shell=True, capture_output=True, text=True)
    string_list = output.stdout.splitlines()
    network_names = []
    for string in string_list:
        sub_list = string.split(" ")
        while "" in sub_list:
            sub_list.remove("")
        while "*" in sub_list:
            sub_list.remove("*")
        network_names.append(sub_list[1])
    print(network_names)
    return network_names

def connect_to_wifi_network(network_name):
    print("Connecting to " + network_name + " network")
    #output = subprocess.run(["nmcli", "dev", "wifi", "connect", network_name], capture_output=True, text=True)
    output = subprocess.run(["nmcli", "con", "up", network_name], capture_output=True)
    if output.returncode == 0:
        print("Successfully connected to " + network_name + " network")
        return True
    else:
        print("Can't connect to " + network_name + " network")
        return False