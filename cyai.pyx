#usr/bin/python
import serial

def checkwin(grid, int pos):
	cdef int x, y, z, i, win
	x = pos%4
	y = (pos/4)%4
	z = pos/16
	#check z
	win = 0
	for i in range(4):
		win += grid[x+y*4+i*16]
	#check y
	win = 0
	for i in range(4):
		win += grid[x+i*4+z*16]
	if win == 4: return True
	#check x
	win = 0
	for i in range(4):
		win += grid[i+y*4+z*16]
	if win == 4: return True
	#check x y
	win = 0
	if x==y:
		for i in range(4):
			win += grid[i+i*4+z*16]
	if win == 4: return True
	#check x -y
	win = 0
	if x==3-y:
		for i in range(4):
			win += grid[i+(3-i)*4+z*16]
	if win == 4: return True
	#check x z
	win = 0
	if x==z:
		for i in range(4):
			win += grid[i+y*4+i*16]
	if win == 4: return True
	#check x -z
	win = 0
	if x==3-z:
		for i in range(4):
			win += grid[i+y*4+(3-i)*16]
	if win == 4: return True
	#check y z
	win = 0
	if y==z:
		for i in range(4):
			win += grid[x+i*4+i*16]
	if win == 4: return True
	#check y -z
	win = 0
	if y==3-z:
		for i in range(4):
			win += grid[x+i*4+(3-i)*16]
	if win == 4: return True
	#check x y z
	win = 0
	if x==y and y==z:
		for i in range(4):
			win += grid[i+i*4+i*16]
	if win == 4: return True
	#check x y -z
	if x==y and y==3-z:
		for i in range(4):
			win += grid[i+i*4+(3-i)*16]
	if win == 4: return True
	#check x -y -z
	if x==3-y and y==z:
		for i in range(4):
			win += grid[i+(3-i)*4+(3-i)*16]
	if win == 4: return True
	return False
	

def move(grid, int depth):
	'''use minimax to determine what move to use'''
	cdef int score, bestscore, pos, newmove, newscore
	score = 0
	bestscore = 2147483647	#max int
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

cdef int depth
depth = int(raw_input('depth of ai? default is 4\n>| '))
serPort = '/dev/tty' + raw_input('serial port? (if unsure, use /dev/ttyUSB0)\n>| /dev/tty')
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
