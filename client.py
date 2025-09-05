# intbus_client.py
import socket, json
SOCK_PATH = "/tmp/intbus.sock"

def send_motor_info(d: dict,
              sock_path: str = SOCK_PATH,
              timeout: float = 0.2):
    try:
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.settimeout(timeout)
            s.connect(sock_path)
            s.sendall(json.dumps(d).encode())
        return True
    except OSError:
        return False
