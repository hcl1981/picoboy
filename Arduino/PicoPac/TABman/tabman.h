#include <U8g2lib.h>

#include "tiles.h"
#include "levels.h"
#include "sprites.h"
#include "images.h"

class TABman {
	private:
    U8G2 u8g2;
	
    int life = 3;
    int levelNum = 0;
    int levelMax = 9;//9;
    int stepTime = 45;//45
    unsigned int level[12][21];
    int score =  0;
    bool ghostsEdible = false;
    int edibleDuration = 2500;
    unsigned long int edibleTime = 0;
    unsigned long int timerDie = 0;
	unsigned int lastTone = 0;
    const int xPosGorigin = 52;
    int yPosGorigin = 42;
	
    int xPos;
    int yPos;
    int direc;
	
    int xPosG1;
    int yPosG1;
    int direcG1;
	
    int xPosG2;
    int yPosG2;
    int direcG2;
	
    int xPosG3;
    int yPosG3;
    int direcG3;
    unsigned long int last;
	
	public:
    TABman (U8G2 &u8g2P)
    {
		u8g2 = u8g2P;
	}
    void writeScore(int score) {
		u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
		int ypos = 5;
		for (int i = 0; i < 4; i++)
		{
			int digit = score % 10;
			score /= 10;
			char cstr[16];
			itoa(digit, cstr, 10);
			u8g2.setCursor(ypos, 118 - u8g2.getStrWidth(cstr) / 2);
			u8g2.println(cstr);
			ypos += 11;//15
		}
	}
	
    void showLifes(int anz) {
		u8g2.setFont(u8g2_font_micro_mr);
		u8g2.setCursor(56, 109);
		u8g2.println("LIVES");
		for (int i = 0; i < anz; i++) {
			u8g2.drawBox(51, 112 + i * 5, 3, 3);
		}
	}
	
    void drawTile(int i, int j, int tile) {
		for (int k = 0; k < 5; k++) {
			for (int l = 0; l < 5; l++) {
				if (tiles[tile][k][l] > 0) {
					u8g2.drawPixel(63 - ((i * 5) + k), ((j * 5) + l) );
				}
			}
		}
	}
	
    void drawImage(const byte image[64][128]) {
		for (int k = 0; k < 64; k++) {
			for (int l = 0; l < 128; l++) {
				if (image[k][l] > 0) {
					u8g2.drawPixel(63 - k , l);
				}
			}
		}
	}
	
    void drawLevel() {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 21; j++) {
				if (level[i][j] > 0) {
					drawTile(i, j, level[i][j]);
				}
			}
		}
	}
	
    void loadLevel(int lev) {
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 21; j++) {
				level[i][j] = levels[lev][i][j];
			}
		}
	}
	
    void drawPacman(int x, int y, int dire) {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (millis() % 500 > 250) {
					if (pacman[0][i][j] > 0) {
						u8g2.drawPixel(65 - (y + i), (x + j) - 2 );
					}
					} else {
					if (pacman[dire][i][j] > 0) {
						u8g2.drawPixel(65 - (y + i), (x + j) - 2 );
					}
				}
				
			}
		}
	}
	
    void drawGhost(int x, int y) {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (!ghostsEdible) {
					if (millis() % 604 > 302) {
						if (ghost[0][i][j] > 0) {
							u8g2.drawPixel(65 - (y + i), (x + j) - 2 );
						}
						} else {
						if (ghost[1][i][j] > 0) {
							u8g2.drawPixel(65 - (y + i), (x + j) - 2 );
						}
					}
					} else {
					if (millis() % 604 > 302) {
						if (ghost[2][i][j] > 0) {
							u8g2.drawPixel(65 - (y + i), (x + j) - 2 );
						}
						} else {
						if (ghost[3][i][j] > 0) {
							u8g2.drawPixel(65 - (y + i), (x + j) - 2 );
						}
					}
				}
			}
		}
	}
	
	void drawEdible(){
		
		int pixels = (100*(edibleDuration-(millis()-edibleTime)))/edibleDuration;
		if(pixels < 101){
			u8g2.drawLine(0,2,0,pixels+2);
			u8g2.drawLine(1,2,1,pixels+2);
			u8g2.drawLine(2,2,2,pixels+2);
		}
	}
	
    void pacmanStep() {
		if (direc == UP && ((level[((yPos - 3) / 5)][(xPos - 2) / 5] < 1) ||  (level[(yPos - 3) / 5][(xPos - 2) / 5] > 16))) {
			yPos = yPos - 1;
		}
		if (direc == DOWN && ((level[((yPos + 3) / 5)][(xPos - 2) / 5] < 1) ||  (level[(yPos + 3) / 5][(xPos - 2) / 5] > 16))) {
			yPos = yPos + 1;
		}
		if (direc == LEFT && ((level[((yPos - 2) / 5)][(xPos - 3) / 5] < 1) ||  (level[(yPos - 2) / 5][(xPos - 3) / 5] > 16))) {
			xPos = xPos - 1;
		}
		if (direc == RIGHT && ((level[((yPos - 2) / 5)][(xPos + 3) / 5] < 1) ||  (level[(yPos - 2) / 5][(xPos + 3) / 5] > 16))) {
			xPos = xPos + 1;
		}
		if (xPos == 2) {
			xPos = 101;
			} else if (xPos == 102) {
			xPos = 3;
		}
	}
	
    bool canMove(int x, int y, int direc) {
		if ((x - 2) % 5 == 0 && (y - 2) % 5 == 0) {
			
			if (direc == DOWN && ((level[((y + 3) / 5)][(x - 2) / 5] < 1) ||  (level[(y + 3) / 5][(x - 2) / 5] > 16))) {
				return true; //DOWN
			}
			
			if (direc == UP && ((level[((y - 3) / 5)][(x - 2) / 5] < 1) ||  (level[(y - 3) / 5][(x - 2) / 5] > 16))) {
				//    ((level[((y - 3) / 5)][(x - 2) / 5] < 1) ||  (level[(y - 3) / 5][(x - 2) / 5] > 16))
				return true; //UP
			}
			
			if (direc == LEFT && ((level[((y - 2) / 5)][(x - 3) / 5] < 1) ||  (level[(y - 2) / 5][(x - 3) / 5] > 16))) {
				return true; //LEFT
			}
			
			if (direc == RIGHT && ((level[((y - 2) / 5)][(x + 3) / 5] < 1) ||  (level[(y - 2) / 5][(x + 3) / 5] > 16))) {
				return true; //RIGHT
			}
			
		}
		return false;
	}
	
    void ghostStep() {
		if (direcG1 == UP && ((level[((yPosG1 - 3) / 5)][(xPosG1 - 2) / 5] < 1) ||  (level[(yPosG1 - 3) / 5][(xPosG1 - 2) / 5] > 16))) {
			yPosG1 = yPosG1 - 1;
		}
		if (direcG1 == DOWN && ((level[((yPosG1 + 3) / 5)][(xPosG1 - 2) / 5] < 1) ||  (level[(yPosG1 + 3) / 5][(xPosG1 - 2) / 5] > 16))) {
			yPosG1 = yPosG1 + 1;
		}
		if (direcG1 == LEFT && ((level[((yPosG1 - 2) / 5)][(xPosG1 - 3) / 5] < 1) ||  (level[(yPosG1 - 2) / 5][(xPosG1 - 3) / 5] > 16))) {
			xPosG1 = xPosG1 - 1;
		}
		if (direcG1 == RIGHT && ((level[((yPosG1 - 2) / 5)][(xPosG1 + 3) / 5] < 1) ||  (level[(yPosG1 - 2) / 5][(xPosG1 + 3) / 5] > 16))) {
			xPosG1 = xPosG1 + 1;
		}
		
		if (direcG2 == UP && ((level[((yPosG2 - 3) / 5)][(xPosG2 - 2) / 5] < 1) ||  (level[(yPosG2 - 3) / 5][(xPosG2 - 2) / 5] > 16))) {
			yPosG2 = yPosG2 - 1;
		}
		if (direcG2 == DOWN && ((level[((yPosG2 + 3) / 5)][(xPosG2 - 2) / 5] < 1) ||  (level[(yPosG2 + 3) / 5][(xPosG2 - 2) / 5] > 16))) {
			yPosG2 = yPosG2 + 1;
		}
		if (direcG2 == LEFT && ((level[((yPosG2 - 2) / 5)][(xPosG2 - 3) / 5] < 1) ||  (level[(yPosG2 - 2) / 5][(xPosG2 - 3) / 5] > 16))) {
			xPosG2 = xPosG2 - 1;
		}
		if (direcG2 == RIGHT && ((level[((yPosG2 - 2) / 5)][(xPosG2 + 3) / 5] < 1) ||  (level[(yPosG2 - 2) / 5][(xPosG2 + 3) / 5] > 16))) {
			xPosG2 = xPosG2 + 1;
		}
		
		if (direcG3 == UP && ((level[((yPosG3 - 3) / 5)][(xPosG3 - 2) / 5] < 1) ||  (level[(yPosG3 - 3) / 5][(xPosG3 - 2) / 5] > 16))) {
			yPosG3 = yPosG3 - 1;
		}
		if (direcG3 == DOWN && ((level[((yPosG3 + 3) / 5)][(xPosG3 - 2) / 5] < 1) ||  (level[(yPosG3 + 3) / 5][(xPosG3 - 2) / 5] > 16))) {
			yPosG3 = yPosG3 + 1;
		}
		if (direcG3 == LEFT && ((level[((yPosG3 - 2) / 5)][(xPosG3 - 3) / 5] < 1) ||  (level[(yPosG3 - 2) / 5][(xPosG3 - 3) / 5] > 16))) {
			xPosG3 = xPosG3 - 1;
		}
		if (direcG3 == RIGHT && ((level[((yPosG3 - 2) / 5)][(xPosG3 + 3) / 5] < 1) ||  (level[(yPosG3 - 2) / 5][(xPosG3 + 3) / 5] > 16))) {
			xPosG3 = xPosG3 + 1;
		}
	}
	
    void pacmanSteer() {
		if (digitalRead(KEY_UP) == LOW && (canMove(xPos, yPos, UP) || direc == DOWN)) {
			direc = UP;
		}
		if (digitalRead(KEY_DOWN) == LOW && (canMove(xPos, yPos, DOWN) || direc == UP)) {
			direc = DOWN;
		}
		if (digitalRead(KEY_LEFT) == LOW && (canMove(xPos, yPos, LEFT) || direc == RIGHT)) {
			direc = LEFT;
		}
		if (digitalRead(KEY_RIGHT) == LOW && (canMove(xPos, yPos, RIGHT) || direc == LEFT)) {
			direc = RIGHT;
		}
	}
	
    bool inBox(int x, int y) {
		return ((x - 2) / 5 > 8 && (x - 2) / 5 < 12 && (y - 2) / 5 == 8);
	}
	
    void ghostSteer() {
		// Geist 1
		if (((xPosG1 - 2) % 5 == 0 && (yPosG1 - 2) % 5 == 0)) {
			bool setG1 = true;
			while (setG1) {
				if (canMove(xPosG1, yPosG1, DOWN)) {
					if (random(1000) > 500 && setG1 && (direcG1 != UP || inBox(xPosG1, yPosG1))) {
						direcG1 = DOWN;
						setG1 = false;
					}
				}
				if (canMove(xPosG1, yPosG1, UP)) {
					if (random(1000) > 500 && setG1 && (direcG1 != DOWN || inBox(xPosG1, yPosG1))) {
						direcG1 = UP;
						setG1 = false;
					}
				}
				if (canMove(xPosG1, yPosG1, LEFT)) {
					if (xPosG1 > 101 || random(1000) > 500 && setG1 && (direcG1 != RIGHT || inBox(xPosG1, yPosG1))) {
						direcG1 = LEFT;
						setG1 = false;
					}
				}
				if (canMove(xPosG1, yPosG1, RIGHT)) {
					if (xPosG1 < 3 || random(1000) > 500 && setG1 && (direcG1 != LEFT || inBox(xPosG1, yPosG1))) {
						direcG1 = RIGHT;
						setG1 = false;
					}
				}
			}
		}
		// Geist 2
		if (((xPosG2 - 2) % 5 == 0 && (yPosG2 - 2) % 5 == 0)) {
			bool setG2 = true;
			while (setG2) {
				if (canMove(xPosG2, yPosG2, DOWN)) {
					if (random(1000) > 500 && setG2 && (direcG2 != UP || inBox(xPosG2, yPosG2))) {
						direcG2 = DOWN;
						setG2 = false;
					}
				}
				if (canMove(xPosG2, yPosG2, UP)) {
					if (random(1000) > 500 && setG2 && (direcG2 != DOWN || inBox(xPosG2, yPosG2))) {
						direcG2 = UP;
						setG2 = false;
					}
				}
				if (canMove(xPosG2, yPosG2, LEFT)) {
					if (xPosG2 > 101 || random(1000) > 500 && setG2 && (direcG2 != RIGHT || inBox(xPosG2, yPosG2))) {
						direcG2 = LEFT;
						setG2 = false;
					}
				}
				if (canMove(xPosG2, yPosG2, RIGHT)) {
					if (xPosG2 < 3 || random(1000) > 500 && setG2 && (direcG2 != LEFT || inBox(xPosG2, yPosG2))) {
						direcG2 = RIGHT;
						setG2 = false;
					}
				}
			}
		}
		// Geist 3
		if (((xPosG3 - 2) % 5 == 0 && (yPosG3 - 2) % 5 == 0)) {
			bool setG3 = true;
			while (setG3) {
				if (canMove(xPosG3, yPosG3, DOWN)) {
					if (random(1000) > 500 && setG3 && (direcG3 != UP || inBox(xPosG3, yPosG3))) {
						direcG3 = DOWN;
						setG3 = false;
					}
				}
				if (canMove(xPosG3, yPosG3, UP)) {
					if (random(1000) > 500 && setG3 && (direcG3 != DOWN || inBox(xPosG3, yPosG3))) {
						direcG3 = UP;
						setG3 = false;
					}
				}
				if (canMove(xPosG3, yPosG3, LEFT)) {
					if (xPosG3 > 101 || random(1000) > 500 && setG3 && (direcG3 != RIGHT || inBox(xPosG3, yPosG3))) {
						direcG3 = LEFT;
						setG3 = false;
					}
				}
				if (canMove(xPosG3, yPosG3, RIGHT)) {
					if (xPosG3 < 3 || random(1000) > 500 && setG3 && (direcG3 != LEFT || inBox(xPosG3, yPosG3))) {
						direcG3 = RIGHT;
						setG3 = false;
					}
				}
			}
		}
	}
	
    void collect() {
		if ((xPos - 2) % 5 == 0 && (yPos - 2) % 5 == 0) {
			if (level[(yPos - 2) / 5][((xPos - 2) / 5)] == 17) {
				level[(yPos - 2) / 5][((xPos - 2) / 5)] = 0;
				score += 1;
				//tone(SPEAKER, 6000, 10);
			} //kleine Pille
			if (level[(yPos - 2) / 5][((xPos - 2) / 5)] == 18) {
				level[(yPos - 2) / 5][((xPos - 2) / 5)] = 0;
				score += 1;
				ghostsEdible = true;
				tone(SPEAKER,3500,10);
				edibleTime = millis();
				//tone(SPEAKER, 4000, 10);
			} //große Pille
		}
	}
	
    void die() {
	
		tone(SPEAKER,3000,10);
		boolean hiscore = false;
		direc = 0;
		life = life - 1;
		for (int h = 0; h < 18; h++) {
			while (millis() < last + stepTime) {}
			last = millis();
			ghostSteer();
			ghostStep();
			u8g2.clearBuffer();
			writeScore(score);
			drawLevel();
			drawEdible();
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					if (dieAnimation[h / 2][i][j] > 0) {
						u8g2.drawPixel(65 - (yPos + i), (xPos + j) - 2 );
					}
				}
			}
			drawGhost(xPosG1, yPosG1);
			drawGhost(xPosG2, yPosG2);
			drawGhost(xPosG3, yPosG3);
			showLifes(life);
			if (millis() > edibleTime + edibleDuration) {
				ghostsEdible = false;
				//digitalWrite(LEDR,LOW);
			}
			u8g2.sendBuffer();
		}
		timerDie = millis();
		while (millis() < timerDie + 4000) {
			while (millis() < last + stepTime) {}
			last = millis();
			ghostSteer();
			ghostStep();
			u8g2.clearBuffer();
			writeScore(score);
			drawLevel();
			drawEdible();
			drawGhost(xPosG1, yPosG1);
			drawGhost(xPosG2, yPosG2);
			drawGhost(xPosG3, yPosG3);
			showLifes(life);
			if (millis() > edibleTime + edibleDuration) {
				ghostsEdible = false;
				//digitalWrite(LEDR,LOW);
			}
			if ( millis() > timerDie + 1000) {
				u8g2.setDrawColor(0);
				u8g2.drawRBox(18, 14, 34, 77, 4);
				u8g2.setDrawColor(1);
				u8g2.drawRFrame(18, 14, 34, 77, 4);
				u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
				if (life > 0) {
					if (millis() > timerDie + 3000) {
						u8g2.setCursor(37, 22);
						u8g2.println("TABman");
						u8g2.setCursor(23, 22);
						u8g2.println("died!     1");
						} else if (millis() > timerDie + 2000) {
						u8g2.setCursor(37, 22);
						u8g2.println("TABman");
						u8g2.setCursor(23, 22);
						u8g2.println("died!    2");
						} else if (millis() > timerDie + 1000) {
						u8g2.setCursor(37, 22);
						u8g2.println("TABman");
						u8g2.setCursor(23, 22);
						u8g2.println("died!    3");
					}
					} else {
					
					////
				if (score > eepromReadInt(4) || hiscore)
				{
					eepromWriteInt(4, score);
					//u8g2.getStrWidth("YOUR")
					u8g2.setCursor(37, 19);
					u8g2.println("HI-SCORE");
					if(millis()%100 > 50){
					char cstr[16];
					itoa(score, cstr, 10);
					u8g2.setCursor(23,52 - u8g2.getStrWidth(cstr) / 2);
					u8g2.println(cstr);
					}
					hiscore = true;
				}
				else
				{
					u8g2.setCursor(37, 21);
					u8g2.println("No more");
					u8g2.setCursor(23, 21);
					u8g2.println("TABmen!");
				}
				}
			}
			
			u8g2.sendBuffer();
			
		}
		xPos = 52;
		yPos = 32;
		direc = UP;
		
		xPosG1 = 52;
		yPosG1 = 42;
		direcG1 = RIGHT;
		
		xPosG2 = 52;
		yPosG2 = 42;
		direcG2 = RIGHT;
		
		xPosG3 = 52;
		yPosG3 = 42;
		direcG3 = RIGHT;
		ghostsEdible = false;
		//digitalWrite(LEDR,LOW);
	}
	
    void handleCollision() {
		if (ghostsEdible) {
			if (abs(xPos - xPosG1) < 2 && abs(yPos - yPosG1) < 2) {
				xPosG1 = xPosGorigin;
				yPosG1 = yPosGorigin;
				
				tone(SPEAKER,3500,10);
			}
			if (abs(xPos - xPosG2) < 2 && abs(yPos - yPosG2) < 2) {
				xPosG2 = xPosGorigin;
				yPosG2 = yPosGorigin;
				
				tone(SPEAKER,3500,10);
			}
			if (abs(xPos - xPosG3) < 2 && abs(yPos - yPosG3) < 2) {
				xPosG3 = xPosGorigin;
				yPosG3 = yPosGorigin;
				
				tone(SPEAKER,3500,10);
			}
			} else {
			if (abs(xPos - xPosG1) < 2 && abs(yPos - yPosG1) < 2) {
				die();
			}
			if (abs(xPos - xPosG2) < 2 && abs(yPos - yPosG2) < 2) {
				die();
			}
			if (abs(xPos - xPosG3) < 2 && abs(yPos - yPosG3) < 2) {
				die();
			}
		}
	}
	
    boolean checkWon() {
		bool won = true;
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 21; j++) {
				if (level[i][j] == 17 || level[i][j] == 18) {
					won = false;
				}
			}
		}
		return won;
	}
	
    void run2(void) {
		last = millis();
		
		while (digitalRead(KEY_CENTER) == HIGH || millis() < last + 500) {
			u8g2.clearBuffer();
			drawImage(titlePic);
			if (millis() % 600 > 300) {
				u8g2.setFont(u8g2_font_micro_mr);
				u8g2.setCursor(7, 66);
				u8g2.println("PUSH TO PLAY");
			}
			u8g2.sendBuffer();
			//delay(50);
		}
		
		delay(500);
		while (life > 0 && stepTime >= 20) {
			while (levelNum <= levelMax && life > 0) {
				if (life > 0)loadLevel(levelNum);
				xPos = 52;
				yPos = 32;
				direc = UP;
				
				xPosG1 = 52;
				yPosG1 = 42;
				direcG1 = LEFT;
				
				xPosG2 = 52;
				yPosG2 = 42;
				int direcG2 = DOWN;
				
				xPosG3 = 52;
				yPosG3 = 42;
				direcG3 = RIGHT;
				
				while (!checkWon() && life > 0) {
					while (millis() < last + stepTime) {}
					last = millis();
					pacmanSteer();
					pacmanStep();
					ghostSteer();
					ghostStep();
					collect();
					handleCollision();
					u8g2.clearBuffer();
					writeScore(score);
					drawLevel();
					drawEdible();
					if (life > 0) {
						drawPacman(xPos, yPos, direc);
					}
					drawGhost(xPosG1, yPosG1);
					drawGhost(xPosG2, yPosG2);
					drawGhost(xPosG3, yPosG3);
					showLifes(life);
					u8g2.sendBuffer();
					
					if (millis() > edibleTime + edibleDuration) {
						ghostsEdible = false;
						//digitalWrite(LEDR,LOW);
						//digitalWrite(LEDG,LOW);
						//digitalWrite(LEDY,LOW);
						} else {
						/*
						if(millis() > (edibleTime + edibleDuration)-500){
							
							if(digitalRead(LEDR)==LOW){
								digitalWrite(LEDR,HIGH);
								//digitalWrite(LEDG,HIGH);
								//digitalWrite(LEDY,HIGH);
								} else {
								digitalWrite(LEDR,LOW);
								//digitalWrite(LEDG,LOW);
								//digitalWrite(LEDY,LOW);
							}
						}
						*/
						if((millis() > lastTone + 50) && millis() > (edibleTime + edibleDuration)-50){
							
							tone(SPEAKER,5000,10);
							lastTone = millis();
							
							}
					}
				}
				levelNum += 1;
			}
			stepTime -= 5;
			edibleDuration -= 200;
			levelNum = 0;
		}
		if (life == 0) {
			int last2 = millis();
			while (millis() < last2 + 2000) {
				while (millis() < last + stepTime) {}
				last = millis();
				ghostSteer();
				ghostStep();
				u8g2.clearBuffer();
				writeScore(score);
				drawLevel();
				drawEdible();
				drawGhost(xPosG1, yPosG1);
				drawGhost(xPosG2, yPosG2);
				drawGhost(xPosG3, yPosG3);
				showLifes(life);
				u8g2.sendBuffer();
				if (millis() > edibleTime + edibleDuration) {
					ghostsEdible = false;
					digitalWrite(LEDR,LOW);
					//digitalWrite(LEDG,LOW);
					//digitalWrite(LEDY,LOW);
					}/* else {
					if(millis() > (edibleTime + edibleDuration)-500){
						if(digitalRead(LEDR)==LOW){
							digitalWrite(LEDR,HIGH);
							//digitalWrite(LEDG,HIGH);
							//digitalWrite(LEDY,HIGH);
							} else {
							digitalWrite(LEDR,LOW);
							//digitalWrite(LEDG,LOW);
							//digitalWrite(LEDY,LOW);
						}
					} 
				} */
			}
			} else {
			u8g2.clearBuffer();
			drawImage(wonPic);
			u8g2.sendBuffer();
			delay(3000);
		}
		//vorbei..
	}
	void run(){
	//set_sys_clock_khz(125000, true);
	set_sys_clock_khz(50000, true);
		u8g2.setFontDirection(1);
		while (true)
		{
			run2();
			life = 3;
			levelNum = 0;
			stepTime = 45;
			score =  0;
			bool ghostsEdible = false;
		}
	}
};
