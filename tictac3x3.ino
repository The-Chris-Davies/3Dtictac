#include <Rainbowduino.h>

void start();		//initializes all vars to whatever they should be, and asks if two player / one player and cpu.
void move();		//moves the 'cursor' and quits when a cell is selected.
byte checkWin();	//checks if a player has won. Returns the player's number (1,-1) or 0 if no one has won.
void redraw();		//redraws the board.
void winner(char);	//ends the game and eventually will have fancy fade/design thing for the winner's color.

void setup(){
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	//wait for serial port to connect, only done for usb serial
	while (!Serial);
	
	Rb.init();
	
	Serial.println("v1.0");
}

char grid[3][3][3];
byte pos[3];
int player = 1;

void loop(){
	start();
	do{
		move();
		if(!grid[pos[0]][pos[1]][pos[2]]){
			grid[pos[0]][pos[1]][pos[2]] = player;
			player = -player;
		}
		redraw();
	}while(!checkWin());
	winner(player);
	Rb.blankDisplay();
}

//TODO: diagonal checking
byte checkWin(){
	char winstates[12] = {0,0,0,0,0,0,0,0,0,0,0,0};	//check for win in x,y,z,xy,-xy,xz,-xz,yz,-yz,xyz,-xyz,-x-yz directions
	for(byte i = 0; i < 3; i++){
		for(byte j = 0; j < 12; j++);
		winstates[0] += grid[i][pos[1]][pos[2]];	//x
		winstates[1] += grid[pos[0]][i][pos[2]];	//y
		winstates[2] += grid[pos[0]][pos[1]][i];	//z
		if(pos[0]==pos[1])
			winstates[3] += grid[i][i][pos[2]];		//x,y
		if(pos[0]==2-pos[1])
			winstates[4] += grid[i][2-i][pos[2]];		//x,-y
		if(pos[0]==pos[2])
			winstates[5] += grid[i][pos[1]][i];		//x,z
		if(pos[0]==2-pos[2])
			winstates[6] += grid[i][pos[1]][2-i];		//x,-z
		if(pos[1]==pos[2])
			winstates[7] += grid[pos[0]][i][i];		//y,z
		if(pos[1]==2-pos[2])
			winstates[8] += grid[pos[0]][i][2-i];		//y,-z
		if(pos[0]==pos[1] && pos[1]==pos[2])
			winstates[9] += grid[i][i][i];			//x,y,z
		if(pos[0]==pos[1] && pos[1]==2-pos[2])
			winstates[10] += grid[i][i][2-i];		//x,y,-z
		if(pos[0]==2-pos[1] && pos[1]==pos[2])
			winstates[11] += grid[i][2-i][2-i];		//x,-y,-z
	}
	for(byte i = 0; i < 12; i++){
		if(winstates[i] == 3)
			return 1;
		else if(winstates[i] == -3)
			return -1;
	}
	return 0;
}

void move(){
	//built for easy conversion to nunchuck serial
	char moveDir[3] = {0,0,0};
	byte sign, dir;
	while(true){
		redraw();
		
		Serial.println("Which direction to move?");
		Serial.print("Pos: "); Serial.print(pos[0]); Serial.print(' '); Serial.print(pos[1]); Serial.print(' '); Serial.println(pos[2]);
		
		while (!Serial.available());
		sign = Serial.read();
		
		//if selected the spot:
		if(sign == 'c'){
			return;
		}
		
		while (!Serial.available());
		dir = Serial.read();
		
		//120 121 122 = x y z		43 44 45 = + , -
		moveDir[dir-120] = 44-sign;
		
		for(byte i = 0; i < 3; i++){
			pos[i] +=  moveDir[i];
			pos[i] = ((pos[i] == 255)?(pos[i]-1):(pos[i])) % 3;
			moveDir[i] = 0;
		}
	}
}

void redraw(){
	//clear display
	Rb.blankDisplay();
	//set tic tac toe grid
	for(byte x = 0; x < 3; x++)
		for(byte y = 0; y < 3; y++)
			for(byte z = 0; z < 3; z++)
				Rb.setPixelZXY(z+1,x,y, grid[x][y][z] > 0 ? 0xFF0000 : grid[x][y][z] < 0 ? 0x0000FF : 0x202020);	//set red if 1, set blue if -1, otherwise set blank
	//set cursor pixels
	Rb.setPixelZXY(pos[2]+1,3,pos[1],0xFF7000);
	Rb.setPixelZXY(pos[2]+1,pos[0],3,0xFF7000);
	
	//set player color boundary
	for(byte x = 0; x < 4; x++)
		for(byte y = 0; y < 4; y++)
			Rb.setPixelZXY(0,x,y, player>0 ? 0x400000 : 0x000040);
}

void winner(char player){
	Serial.print("Player ");
	Serial.print(-player, DEC);
	Serial.println(" Wins!");
	
}

void start(){
	Serial.println("Restarting");

	for(byte i=0;i<27;i++)
		*(&(grid[0][0][0])+i) = 0;
	for(byte i=0;i<3;i++)
		pos[i] = 0;

	Rb.blankDisplay();
	for(byte x=0;x<50;x++){
		Rb.setPixelZXY(random(4),random(4),random(4),random(2)*0xFEFF01 + 0x0000FF); //uses 24-bit RGB color
		delay(100); //delay to illustrate the postion change. can be removed
	}
	for(byte x=0;x<4;x++)
		for(byte z=0;z<4;z++){
			Rb.setPixelZXY(z,x,0,0xFF0000);
			Rb.setPixelZXY(z,x,1,0xFF0000);
			Rb.setPixelZXY(z,x,2,0x0000FF);
			Rb.setPixelZXY(z,x,3,0x0000FF);
		}
	delay(2000);
}
