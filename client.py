import socket
import random
import time

# UDP configuration
UDP_IP = "192.168.1.196"  # Replace with your ESP-01's IP address
UDP_PORT = 1234
NUM_LEDS = 300

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

it_c = 0

def send_random_colors():
    global it_c
    # Generate 300 random RGB values
    buffer = bytearray(NUM_LEDS * 3)
    for i in range(NUM_LEDS):
        if i == it_c % 300:
            w = 0
            r = 255
        else:
            w = 255
            r = 0
        buffer[i * 3] = random.randint(w, 255)  # R
        buffer[i * 3 + 1] = random.randint(0, r)  # G
        buffer[i * 3 + 2] = random.randint(0, r)  # B
    sock.sendto(buffer, (UDP_IP, UDP_PORT))
    it_c += 1

def main():
    print(f"Sending random colors to {UDP_IP}:{UDP_PORT}")
    try:
        while True:
            send_random_colors()
            # Minimal delay to prevent overwhelming the ESP-01
            time.sleep(0.03)  # ~33 FPS
    except KeyboardInterrupt:
        print("Stopping client")
        sock.close()

if __name__ == "__main__":
    main()
