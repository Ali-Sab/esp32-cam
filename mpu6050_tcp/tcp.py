import socket
import time

HOST = "192.168.4.1"
PORT = 63000

avg = []

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    
    for i in range(1000):
        lastTime = time.time()
        s.sendall(b"p")
        data = s.recv(128)
        newTime = time.time()
        diff = (newTime - lastTime) * 1000
        avg.append(diff)
        #print(f"{diff} ms\n")
        #print(f"{data.decode('utf-8')}\n")
        time.sleep(0.001)
        
print(f"avg delay: {sum(avg) / 1000}")
print(sorted(avg))
    
