import binascii
import socket
import struct
import sys
import urllib
import urllib.request


def create_packet(request):
	data = struct.pack('!c', request)

	return data

def fetch_page(url):
  with urllib.request.urlopen(url) as response:
    html = response.read()
  return html

if __name__=='__main__':
	loadBalancerIP = sys.argv[1] 	#get the loadBalancerIP in a variable
	port = int(sys.argv[2]) 		#get the port in a variable
	logfile = sys.argv[3] 			#get the logfile in a variable

	fd = open(logfile, 'w')		#open the file in which to log

	try : 
			sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			print('Socket created')
	except (socket.error, msg) :
			print('Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
			sys.exit()

	addy = (loadBalancerIP, port)


	send_data = create_packet(b'Y')
	sock.sendto(send_data, addy) #ask load balancer which server to connect to
	d, a = sock.recvfrom(1024) #recieve the best possible server

	data = d[0]

	print(d.decode())

	addy = (a, 6969)
	sock.sendto(send_data, addy)

	fd.close()
	sock.close()