#! usr/bin/python
import serial
import random
# TODO: make AI more efficient, to allow for more depth (4,5,or even 6 would be the best!!)
#		-use different algorithm?
#		-what is alpha-beta pruning?
#		-use memoization!!!
def checkwin(grid, pos):
	xyz = (pos%4, (pos/4)%4, pos/16)
	win = [0 for x in xrange(12)]
	for i in xrange(4):
		win[0] += grid[xyz[0]+xyz[1]*4+i*16]
		win[1] += grid[xyz[0]+i*4+xyz[2]*16]
		win[2] += grid[i+xyz[1]*4+xyz[2]*16]
		if xyz[0]==xyz[1]:
			win[3] += grid[i+i*4+xyz[2]*16]
		if xyz[0]==3-xyz[1]:
			win[4] += grid[i+(3-i)*4+xyz[2]*16]
		if xyz[0]==xyz[2]:
			win[5] += grid[i+xyz[1]*4+i*16]
		if xyz[0]==3-xyz[2]:
			win[6] += grid[i+xyz[1]*4+(3-i)*16]
		if xyz[1]==xyz[2]:
			win[7] += grid[xyz[0]+i*4+i*16]
		if xyz[1]==3-xyz[2]:
			win[8] += grid[xyz[0]+i*4+(3-i)*16]
		if xyz[0]==xyz[1] and xyz[1]==xyz[2]:
			win[9] += grid[i+i*4+i*16]
		if xyz[0]==xyz[1] and xyz[1]==3-xyz[2]:
			win[10] += grid[i+i*4+(3-i)*16]
		if xyz[0]==3-xyz[1] and xyz[1]==xyz[2]:
			win[11] += grid[i+(3-i)*4+(3-i)*16]
	if 4 in win:
		return True
	return False
	

def move(grid, depth):
	'''use minimax to determine what move to use'''
	score = 0
	bestscore = float('inf')
	pos = -1	#best move pos
	if depth <= 0:
		return 0,0
	for i in xrange(64):
		if grid[i]: continue
		grid[i] = 1
		if checkwin(grid,i): 
			grid[i] = 0
			return i, 10
		newmove, newscore = move(map(lambda x: -x, grid),depth-1)	#run move for next player
		score -= newscore
		if newscore < bestscore:	#find maximum score: newscore should be less because the opposite player makes the next move.
			bestscore = newscore
			pos = i
		grid[i] = 0
	if depth == 3: pos, score
	return pos, score

serPort = '/dev/tty' + raw_input('serial port? (default is /dev/ttyUSB0)\n>| /dev/tty')
ser = serial.Serial(serPort);
ser.baudrate = 115200;
print 'using', ser.name
ser.flushInput()

while True:
	print 'waiting for connection from cube'
	in1 = ser.readline();
	if in1 == 'END\r\n':	#game is over
		print 'game is over!'
		continue
	elif in1 == 'hi:)\r\n':
		print 'handshake recieved!'
		ser.write(b'G')
		continue
	
	print 'making my move!'
	g1 = list(str(bin(int(in1)))[2:].zfill(64))
	g2 = list(str(bin(int(ser.readline())))[2:].zfill(64))
	grid = map(lambda x,y:int(x)-int(y), g2,g1)[::-1]
	pos, score = move(grid, 3);
	if score == 0:
		pos = random.randrange(0,64)
		
	print bytes(pos%4) + bytes((pos/4)%4) + bytes(pos/16), score, '\n\n'
	ser.write(bytes(pos%4) + bytes((pos/4)%4) + bytes(pos/16))
