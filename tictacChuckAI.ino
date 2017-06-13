//4x4x4 tic tac toe on the rainbowduino

#include <Wire.h>
#include <Rainbowduino.h>
#include <WiiChuck.h>

void start();		//initializes all vars to whatever they should be, and asks if two player / one player and cpu.
void move();		//moves the 'cursor' and quits when a cell is selected.
void AImove();		//queries the attached computer (if it exists) for the AI's move.
byte checkWin();	//checks if a player has won. Returns the player's number (1,-1) or 0 if no one has won.
void redraw();		//redraws the board.
void winner(char);	//ends the game and eventually will have fancy fade/design thing for the winner's color.

short signum(short value) {
 return (value>0)-(value<0);
}

WiiChuck chuck = WiiChuck();
char grid[4][4][4];
byte pos[3];
byte player;
byte vsAI;

void setup(){
	// Open serial communications and wait for port to open:
	Serial.begin(115200);
	chuck.begin();
	chuck.update();
	//wait for serial port to connect, only done for usb serial
	while (!Serial);
	
	Rb.init();
}

void loop(){
	start();
	do{
		move();
		grid[pos[0]][pos[1]][pos[2]] = player;
		player = -player;
		redraw();
	}while(!checkWin());
	winner(player);
	Rb.blankDisplay();
}

//TODO: make pretty
byte checkWin(){
	char winstates[12] = {0,0,0,0,0,0,0,0,0,0,0,0};	//check for win in x,y,z,xy,-xy,xz,-xz,yz,-yz,xyz,-xyz,-x-yz directions
	for(byte i = 0; i < 4; i++){
		for(byte j = 0; j < 12; j++);
		winstates[0] += grid[i][pos[1]][pos[2]];	//x
		winstates[1] += grid[pos[0]][i][pos[2]];	//y
		winstates[2] += grid[pos[0]][pos[1]][i];	//z
		if(pos[0]==pos[1])
			winstates[3] += grid[i][i][pos[2]];		//x,y
		if(pos[0]==3-pos[1])
			winstates[4] += grid[i][3-i][pos[2]];		//x,-y
		if(pos[0]==pos[2])
			winstates[5] += grid[i][pos[1]][i];		//x,z
		if(pos[0]==3-pos[2])
			winstates[6] += grid[i][pos[1]][3-i];		//x,-z
		if(pos[1]==pos[2])
			winstates[7] += grid[pos[0]][i][i];		//y,z
		if(pos[1]==3-pos[2])
			winstates[8] += grid[pos[0]][i][3-i];		//y,-z
		if(pos[0]==pos[1] && pos[1]==pos[2])
			winstates[9] += grid[i][i][i];			//x,y,z
		if(pos[0]==pos[1] && pos[1]==3-pos[2])
			winstates[10] += grid[i][i][3-i];		//x,y,-z
		if(pos[0]==3-pos[1] && pos[1]==pos[2])
			winstates[11] += grid[i][3-i][3-i];		//x,-y,-z
	}
	for(byte i = 0; i < 12; i++){
		if(winstates[i] == 4)
			return i+1;
		else if(winstates[i] == -4)
			return -(i+1);
	}
	return 0;
}

void move(){
	//built for easy conversion to nunchuck serial
	char moveDir[3] = {0,0,0};
	byte zPressed = chuck.buttonZ, cPressed = chuck.buttonC;
	byte tick;
	//unsigned long lastTime(0);
	short xAcc(0), yAcc(0), zAcc(0);
	while(true){
		
		chuck.update();
		/*
		Serial.print((int)chuck.readAccelX()); 
		Serial.print(", ");  
		Serial.print((int)chuck.readAccelY()); 
		Serial.print(", ");  
		Serial.println((int)chuck.readAccelZ()); 
		*/
		//if selected the spot:
		if(chuck.buttonZ && chuck.buttonC && !grid[pos[0]][pos[1]][pos[2]]){
			
			return;
		}
		if(!tick){
			/*
			if(abs(chuck.readAccelX()-xAcc)>75){
				moveDir[0] -= signum(chuck.readAccelX()-xAcc);
				tick = 50;
			}
			if(abs(chuck.readAccelY()-yAcc)>75){
				moveDir[1] -= signum(chuck.readAccelY()-yAcc);
				tick = 50;
			}
			if(abs(chuck.readAccelZ()-zAcc)>75){
				moveDir[2] -= signum(chuck.readAccelZ()-zAcc);
				tick = 50;
			}
			*/
			if(chuck.readJoyX()>120){
				moveDir[1] = 1;
				tick = 100;
			}
			if(chuck.readJoyX()<-120){
				moveDir[1] = -1;
				tick = 100;
			}
			if(chuck.readJoyY()>120){
				moveDir[0] = 1;
				tick = 100;
			}
			if(chuck.readJoyY()<-120){
				moveDir[0] = -1;
				tick = 100;
			}
			moveDir[2] = (chuck.buttonC!=cPressed&&!chuck.buttonC) - (chuck.buttonZ!=zPressed&&!chuck.buttonZ);
			if(moveDir[2]) tick=50;
			
			for(byte i = 0; i < 3; i++){
				pos[i] +=  moveDir[i];
				pos[i] = ((pos[i] == 255)?(3):(pos[i])) % 4;
				moveDir[i] = 0;
			}
		}
		else{
			if(tick == 50)
				redraw();
			--tick;
		}
		cPressed = chuck.buttonC;
		zPressed = chuck.buttonZ;
		/*
		xAcc = chuck.readAccelX();
		yAcc = chuck.readAccelY();
		zAcc = chuck.readAccelZ();
		*/
		//delay(10 + millis() - lastTime);
		//lastTime = millis();
	}
}

void AImove(){		//todo: finish aimove and implement

	while (!Serial.available());
		sign = Serial.read();


}


void redraw(){
	//clear display
	Rb.blankDisplay();
	//set tic tac toe grid
	for(byte x = 0; x < 4; x++)
		for(byte y = 0; y < 4; y++)
			for(byte z = 0; z < 4; z++)
				Rb.setPixelZXY(z,x,y, grid[x][y][z]>0?0xFF0000:grid[x][y][z]<0?0x0000FF:0x202020);	//set red if 1, set blue if -1, otherwise set blank
	//set cursor pixels
	Rb.setPixelZXY(pos[2],pos[0],pos[1],player>0?0xFF7000:0x00FF80);
}

void winner(char player){
	Serial.print("Player ");
	Serial.print(player>0?2:1);
	Serial.println(" Wins!");
	delay(2000);
	for(byte x=0;x<50;x++){
		Rb.setPixelZXY(random(4),random(4),random(4),player<0?0xFF0000:0x0000FF); //uses 24-bit RGB color
		delay(100);
	}
}

void start(){
		
	for(byte i=0;i<64;i++)
		*(&(grid[0][0][0])+i) = 0;
	for(byte i=0;i<3;i++)
		pos[i] = 0;

	Rb.blankDisplay();
	
	Serial.println("hi:)");		//notify cpu of an available game
	
	for(byte x=0;x<50;x++){		//just a time waster to give cpu time to respond
		Rb.setPixelZXY(random(4),random(4),random(4),random(2)*0xFEFF01 + 0x0000FF); //uses 24-bit RGB color
		delay(100);
	}
	
	if(!Serial.available() || Serial.read() != 'G')		//ai program not responding at all or 'good to go!'
		for(byte x=0;x<4;x++)
			for(byte z=0;z<4;z++){
				Rb.setPixelZXY(z,x,0,0xFF0000);
				Rb.setPixelZXY(z,x,1,0xFF0000);
				Rb.setPixelZXY(z,x,2,0x0000FF);
				Rb.setPixelZXY(z,x,3,0x0000FF);
			}
	else{
		byte zPressed, cPressed;
		chuck.update();
		while(!(chuck.buttonC&&chuck.buttonZ)){
			cPressed = chuck.buttonC;
			zPressed = chuck.buttonZ;
			chuck.update();
			vsAI = (vsAI + (chuck.buttonC!=cPressed&&!chuck.buttonC) + (chuck.buttonZ!=zPressed&&!chuck.buttonZ)) % 2;
			for(byte x=0;x<4;x++)
				for(byte z=0;z<4;z++){
					Rb.setPixelZXY(z,x,0,0xFF0000);
					Rb.setPixelZXY(z,x,1,0xFF0000);
					Rb.setPixelZXY(z,x,2,
							!vsAI ? (millis()%500)<250?0x0000FF:0x000088:
								(millis()%500)<250?0x202020:0x606060);
					Rb.setPixelZXY(z,x,3,
							!vsAI ? (millis()%500)<250?0x0000FF:0x000088:
								(millis()%500)<250?0x202020:0x606060);
				}
		}
	}
	player = 1;
	delay(1000);
}
