import socket
ADDRESS = ("192.168.1.255", 8266)
BEGIN = b"3"
END = b"4"
udpClientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udpClientSocket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
udpClientSocket.sendto(BEGIN, ADDRESS)
udpClientSocket.close()
print('begin OTA')