import serial
import time
import array
import sys

def chrWithEscape(value):
    var = chr(value)
    if(value == 0x7E or value == 0x7D or value == 0x11 or value == 0x13):
        var = chr(0x7D) + chr(value ^ 0x20)
    return var

def makeZigBeeTransmitRequestPacket(dst64addrH, dst64addrL, dst16addr, payLoad): #Frame type = 0x10 packet sender
    length = 1 + 1 + 8 + 2 + 1 + 1 + len(payLoad)

    frameData = chr(0x10) #frameType
    frameData += chr(0x01) #frameID

    sp8 = chr(dst64addrH % 256) #dst64addr High
    dst64addrH = dst64addrH / 256
    sp7 = chr(dst64addrH % 256)
    dst64addrH = dst64addrH / 256
    sp6 = chr(dst64addrH % 256)
    dst64addrH = dst64addrH / 256
    sp5 = chr(dst64addrH % 256)

    sp12 = chr(dst64addrL % 256) #dst64addr Low
    dst64addrL = dst64addrL / 256
    sp11 = chr(dst64addrL % 256)
    dst64addrL = dst64addrL / 256
    sp10 = chr(dst64addrL % 256)
    dst64addrL = dst64addrL / 256
    sp9 = chr(dst64addrL % 256)

    frameData += sp5 + sp6 + sp7 + sp8 + sp9 + sp10 + sp11 + sp12

    frameData += chr((dst16addr / 256) % 256) #dst16addr
    frameData += chr(dst16addr % 256)

    frameData += chr(0x00) #Broadcast radius
    frameData += chr(0x00) #options

    frameData += payLoad #data payload

    checksum = 0 #checksum calculation
    for i in frameData:
        checksum += ord(i)
    checksum = 0xFF - ord(chr(checksum % 256))
    # print "checksum:", checksum

    tempPacket = chr((length / 256) % 256) + chr(length % 256) + frameData + chr(checksum)

    sendPacket = chr(0x7E)
    for i in tempPacket:
        sendPacket += chrWithEscape(ord(i))

    print "===> send", length, "bytes data. Data: ", payLoad
    print "     hex:",
    for i in sendPacket:
        print hex(ord(i)),
    print ""

    return sendPacket

if __name__ == "__main__":
    #serial port initialization <===
    print "serial port initialization start"

    port = '/dev/ttyAMA0' #that is for raspberry pi
    serialPort = serial.Serial(port, 9600, timeout = 1)
    print port + " is opend"

    time.sleep(0.5) #wait for establishing stable serial connection


    print "serial port initialization end"
    #===> serial port 

    # broadcast
    sendStr = "Yahho!!!"
    argvs = sys.argv
    argc = len(argvs)
    print argc
    if(argc != 1):
        sendStr = argvs[1]
    print "broadcast send data: " + sendStr
    temp = makeZigBeeTransmitRequestPacket(0x00000000, 0x0000FFFF, 0xFFFE, sendStr)
    serialPort.write(temp)

    serialPort.close()
    print port + " is closed"