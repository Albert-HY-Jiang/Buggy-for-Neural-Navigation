import serial, time
import sys

if __name__ == '__main__':
    if len(sys.argv) != 3:
        #using the following command to run the script, baudrate is 115200 with bluetooth
        print("python script serial_port baudrate")
        #using the following command to list the available ports
        print("python -m serial.tools.list_ports")
        exit()
        
    port_name = sys.argv[1]
    baud = sys.argv[2]
    
    #Create a Serial object, configure the parameters, then open the connection
    #This is especially important when using wired connection with ESP32 cam
    #because it will try to use dsrdtr regardless of the setting, which reset the board
    com = serial.Serial()
    com.port = port_name
    com.baudrate = baud
    com.timeout = 10
    com.dsrdtr = False
    com.open()
    print("serial connected")
    num = 1 #number of pictures received
    while True:
        print(num)
        value = com.readline()
        line = str(value, encoding="UTF-8")
        print(line)
        '''
        structure of bytes stream transferred:
        transmission started\n
        time stamp, distance\n
        file size(bytes)\n
        file bytes
        '''
        if(value == b'transmission started\n'):
            
            name = com.readline()
            file_name = str(name, encoding="UTF-8")[:-1]            
            print(file_name)
            
            value = com.readline()
            line = str(value, encoding="UTF-8")
            file_len = int(line)
            print(f'file size: {file_len} bytes')
            
            start = time.time() #calculate tranmission time
            file_buf = com.read(file_len)
            print(len(file_buf))            
            with open(file_name+".jpg", "wb") as f: #save the image with time stamp, distance as file name
                f.write(file_buf)
                
            end = time.time()
            print("picture captured")
            print(end - start)
            num +=1
           
            