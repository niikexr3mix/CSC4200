import binascii
import socket
import struct
import binascii
import socket
import struct
import sys
import urllib
import urllib.request


# Create a TCP/IP socket
#def create_packet(s_n, a_n, ack, syn, fin, data_size):
#	data = struct.pack('!IIcccI', s_n, a_n, ack, syn, fin, data_size)

#	return data



#if __name__=='__main__':
#    webpage = get_webpage(webpage="http://www.python.org")
#    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#    ip = sys.argv[1]
#    port = int(sys.argv[2])
#    server_address = (ip, port)
#    buf = 512
#    r = open('test', 'rb')
#    total_read = 0
#    data_size = len(webpage)
#    print(data_size)
#    data = r.read(buf)
#    #print(data)
#    total_read += 512
#    send_data = create_packet(12345, 0, b'Y', b'N', b'N', data_size)
#    sock.sendto(data, server_address)
    #print("\n")
    #print(send_data)
#    while (total_read < 805):
#        if (sock.sendto(data, server_address)):
#            send_data = create_packet(12345, 0, b'Y', b'N', b'N', data_size)
#            data = r.read(buf)
#            print(data_size)
#            total_read += len(send_data)-12
#            print(total_read)

#    sock.close()
#    r.close()


def create_packet(s_n, a_n, ack, syn, fin, data_size):
	data = struct.pack('!IIcccI', s_n, a_n, ack, syn, fin, data_size)

	return data


def handshake(sock, server_address):

	send_data = create_packet(12345, 0, b'Y', b'Y', b'N', 0)
	sock.sendto(send_data, server_address)
	#print(struct.unpack('!IIccc', data))

	data, recv_addr = sock.recvfrom(512)
	recv_seqn, recv_ackn, ack, syn, fin, size = struct.unpack('!IIcccI', data)


	dataReturn = struct.pack('!IIcccI', 100, recv_ackn+1, b'Y', b'N', b'N', 0)
	print(dataReturn)

	return data, recv_addr

if __name__=='__main__':
	ip = sys.argv[1]
	port = int(sys.argv[2])
	filedump = sys.argv[3]
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	server_address = (ip, port)
	data, recv_addr = handshake(sock, server_address)

	number = 1
	number2 = 1
	recv_buf = []
	FIN_SET = 0
	fd = open(filedump, 'w')

	with open('recv_file', 'wb') as w:
		while True:
			data, address = sock.recvfrom(512)
			if number == number2:
				fd.write("Received connection from (IP, PORT): ")
				fd.write(str(address))
				fd.write("\n")
				number = number + 1
			try:
				w.write(data)
				print('Writing data')
			except socket.timeout:
				sock.close()
				print("File downloaded")
			if FIN_SET:
				break
	fd.close()
	sock.close()
	r.close()
