import binascii
import socket
import struct
import sys
import urllib
import urllib.request

#def create_packet(**kwargs):
#    print(kwargs)
#    m_v = kwargs['message_version']
#    m_t = kwargs['message_type']
#    m_s = kwargs['message_string']
#    data = struct.pack('!I', m_v) #pack the version
#    data += struct.pack('!I', m_t) #pack the version
#    data += struct.pack("!I", len(m_s)) #pack the length of string
#    data += m_s.encode() #pack the data
#    return data


#def run_command(command):
#    return 0

#if __name__=='__main__':
#    version =17
#    hello_message = "Hello"
#    hello_packet = create_packet(message_version=version, message_type = 1,message_string=hello_message)
    #ip = sys.argv[1]
 #   port = int(sys.argv[1])
 #   filedump = sys.argv[2]

#    fd = open(filedump, 'w')
 #   number = 1
 #   number2 = 1

    #create a TCP/IP socket
 #   sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#    sock.bind(('10.128.0.2', port))
    #sock.listen(5) #backlog
#    recv_buf = []
#    FIN_SET = 0
 #   with open('recv_file', 'wb') as w:
 #       while True:
 #           data, address = sock.recvfrom(512)
 #           if number == number2:
 #               fd.write("Received connection from (IP, PORT): ")
  #              fd.write(str(address))
  #              fd.write("\n")
  #              number = number + 1
  #          try:
  #              w.write(data)
  #              print('Writing data')
  #          except socket.timeout:
   #             sock.close()
  #              print("File downloaded")
  #          if FIN_SET:
  #              break
  #  fd.close()


# Create a TCP/IP socket
def create_packet(s_n, a_n, ack, syn, fin, data_size):
    data = struct.pack('!IIcccI', s_n, a_n, ack, syn, fin, data_size)

    return data

def get_webpage(**kwargs):
    page = kwargs['webpage']
    with urllib.request.urlopen(page) as response, open("test", 'w') as w:
       html = response.read()
       w.write(html.decode())
    return html

def handshake(sock):
    data, recv_addr = sock.recvfrom(512)
    #print(struct.unpack('!IIccc', data))

    recv_seqn, recv_ackn, ack, syn, fin, size = struct.unpack('!IIcccI', data)

    if syn == b'Y':
        print('sync recieved from', recv_addr, 'sending ack + syn back')

    dataReturn = struct.pack('!IIcccI', 100, recv_ackn+1, b'Y', b'Y', b'N', 0)
    print(dataReturn)
    sock.sendto(dataReturn, recv_addr)

    return data, recv_addr

if __name__=='__main__':
    website = sys.argv[3]
    webpage = get_webpage(webpage=website)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    port = int(sys.argv[1])
    filedump = sys.argv[2]
    sock.bind(('10.128.0.2', port))
    print(port)

    fd = open(filedump, 'w')
    number = 1
    number2 = 1
    while True:
        buf = 512
        r = open('test', 'rb')
        total_read = 0
        data_size = len(webpage)
        data1 = r.read(buf)
        total_read += 512
        data, recv_addr = handshake(sock)
        send_data = create_packet(12345, 0, b'Y', b'N', b'N', data_size)
        sock.sendto(data1, recv_addr)
        while(total_read < data_size):
            if(sock.sendto(data1, recv_addr)):
                send_data = create_packet(12345, 0, b'Y', b'N', b'N', data_size)
                print(data_size)
                print(total_read)
                data = r.read(buf)
                total_read += len(send_data)-12
            elif(data_size - total_read < 512):
                send_data = create_packet(12345, 0, b'Y', b'N', b'Y', data_size-total_read)
                break
    fd.close()