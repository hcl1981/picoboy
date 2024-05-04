#include "tiles.h"
#include "memor_logo.h"

#define X_SIZE  8
#define Y_SIZE 6

class Memor {
	private:
    U8G2 u8g2;
	int xpos = 0;
	int ypos = 0;
	int opencard = -1;
	int xop = -1;
	int yop = -1;
	int found;
	
	bool repaint = false;
	unsigned long int  repainttime;
	unsigned long int starttime;
	boolean lastbox = true;
	unsigned long int lastmillis = millis();
	
	
	int board [X_SIZE][Y_SIZE];
	public:
    Memor (U8G2 &u8g2P)
    {
		u8g2 = u8g2P;
	}
	
	
	void initCards() {
		for (int i = 0; i < X_SIZE * Y_SIZE; i++) {
			board[i % 8][i / 8] = -1;
		}
		
		for (int i = 0; i < 24; i++) {
			
			int rx = random(0, X_SIZE);
			int ry = random(0, Y_SIZE);
			
			while (board[rx][ry] != -1)
			{
				rx = random(0, X_SIZE);
				ry = random(0, Y_SIZE);
			}
			board[rx][ry] = i;
			
			while (board[rx][ry] != -1)
			{
				rx = random(0, X_SIZE);
				ry = random(0, Y_SIZE);
			}
			board[rx][ry] = i;
			
			
		}
	}
	
	
	void drawCard(int card, int x, int y) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (tilesMem[card][i][j] == 1) {
					u8g2.drawPixel(x + i, y + j);
				}
			}
		}
		u8g2.sendBuffer();
	}
	
	void drawVisible() {
		for (int i = 0; i < X_SIZE; i++) {
			for (int j = 0; j < Y_SIZE; j++) {
				drawCard(board[i][j], i * 10 + 2, j * 10 + 2);
			}
		}
	}
	
	void drawBack() {
		for (int i = 0; i < X_SIZE; i++) {
			for (int j = 0; j < Y_SIZE; j++) {
				if (board[i][j] != -1) {
					u8g2.drawFrame(i * 10 + 2, j * 10 + 2, 9, 9);
				}
			}
		}
		if (opencard != -1) {
			drawCard(opencard, xop * 10 + 2, yop * 10 + 2);
		}
	}
	
	void controls() {
		if (digitalRead(KEY_UP) == LOW && ypos > 0 && millis() > lastmillis + 150) {
			lastmillis = millis();
			
			if (board[xpos][ypos] != -1) {
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				} else {
				u8g2.setDrawColor(0);
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				u8g2.setDrawColor(1);
			}
			
			ypos--;
		}
		
		if (digitalRead(KEY_DOWN) == LOW && ypos < 5 && millis() > lastmillis + 150) {
			lastmillis = millis();
			if (board[xpos][ypos] != -1) {
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				} else {
				u8g2.setDrawColor(0);
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				u8g2.setDrawColor(1);
			}
			ypos++;
		}
		
		if (digitalRead(KEY_LEFT) == LOW && xpos > 0 && millis() > lastmillis + 150) {
			lastmillis = millis();
			if (board[xpos][ypos] != -1) {
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				} else {
				u8g2.setDrawColor(0);
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				u8g2.setDrawColor(1);
			}
			xpos--;
		}
		
		if (digitalRead(KEY_RIGHT) == LOW && xpos < 7 && millis() > lastmillis + 150) {
			lastmillis = millis();
			if (board[xpos][ypos] != -1) {
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				} else {
				u8g2.setDrawColor(0);
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				u8g2.setDrawColor(1);
			}
			xpos++;
		}
		
		if (digitalRead(KEY_CENTER) == LOW && millis() > lastmillis + 150 && board[xpos][ypos] != -1 & !(xop == xpos && yop == ypos) && !repaint) { //darf nicht leer sein..
			lastmillis = millis();
			if (opencard == -1) {
				opencard = board[xpos][ypos];
				xop = xpos;
				yop = ypos;
				
				drawCard(board[xpos][ypos], xpos * 10 + 2, ypos * 10 + 2);
				} else {
				drawCard(board[xpos][ypos], xpos * 10 + 2, ypos * 10 + 2);
				if (opencard != board[xpos][ypos]) {
					opencard = -1;
					u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
					u8g2.sendBuffer();
					repaint = true;
					repainttime = millis() + 1000;
					opencard = -1;
					
					} else {
					found++;
					opencard = -1;
					board[xpos][ypos] = -1;
					board[xop][yop] = -1;
					u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
					u8g2.sendBuffer();
					repaint = true;
					repainttime = millis() + 500;
					opencard = -1;
				}
				xop = -1;
				yop = -1;
			}
			//printboard();
		}
	}
	
	void repaintF() {
		if (repaint == true && millis() > repainttime) {
			u8g2.clearBuffer();
			drawBack();
			repaint = false;
		}
	}
	
	void printboard() {
		for (int i = 0; i < X_SIZE; i++) {
			for (int j = 0; j < Y_SIZE; j++) {
				Serial.print(board[i][j]);
				Serial.print(" ");
			}
			Serial.println();
		}
	}
	
	void writetime() {
		unsigned long int ttime = (millis() - starttime) / 100;
		int mins = ttime / 600;
		int secs = (ttime / 10) % 60;
		int tens = ttime % 10;
		Serial.println();
		
		char timeString[20];
		sprintf(timeString, "%02d:%02d.%01d", mins, secs, tens);
		
		u8g2.setFont(u8g2_font_profont17_mn);
		u8g2.setFontDirection(3);
		u8g2.setCursor(100, 62);
		u8g2.print(timeString);
		u8g2.setFont(u8g2_font_michaelmouse_tu);
		u8g2.setCursor(120, 59);
		u8g2.print("MEMOR");
	}
	
	void ende() {
		//u8g2.clearBuffer();
		unsigned long int ttime = (millis() - starttime) / 100;
		int mins = ttime / 600;
		int secs = (ttime / 10) % 60;
		int tens = ttime % 10;
		Serial.println();
		
		char timeString[20];
		sprintf(timeString, "%02d:%02d.%01d", mins, secs, tens);
		
		u8g2.setDrawColor(0);
				u8g2.drawRBox(13, 7, 99, 51, 4);
			//	u8g2.drawBox(110, 0, 127, 63);
				u8g2.setDrawColor(1);
				u8g2.drawRFrame(13, 7, 99, 51, 4);
				u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
			u8g2.setFontDirection(0);
			
			if (ttime < eepromReadInt(8) || eepromReadInt(8) == 0)
			{
				eepromWriteInt(8, ttime);
				u8g2.setCursor(64 - u8g2.getStrWidth("BEST") / 2,22);
				u8g2.println("BEST");
				u8g2.setCursor(64 - u8g2.getStrWidth("TIME") / 2,37);
				u8g2.println("TIME");
				char cstr[16];
				//itoa(progress - 1, cstr, 10);
				u8g2.setCursor(64 - u8g2.getStrWidth(timeString) / 2, 52);
				u8g2.println(timeString);
				u8g2.sendBuffer();
				delay(10);
			}
			else
			{
				u8g2.setCursor(64 - u8g2.getStrWidth("YOUR") / 2, 22);
				u8g2.println("YOUR");
				u8g2.setCursor(64 - u8g2.getStrWidth("TIME") / 2, 37);
				u8g2.println("TIME");
				char cstr[16];
				//itoa(progress - 1, cstr, 10);
				u8g2.setCursor( 64 - u8g2.getStrWidth(timeString) / 2,52);
				u8g2.println(timeString);
				u8g2.sendBuffer();
				delay(10);
			}
		////////////
		
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
	
	void intro() {
		u8g2.setFont(u8g2_font_michaelmouse_tu);
		u8g2.setFontDirection(1);
		while (digitalRead(KEY_CENTER) == HIGH) {
			u8g2.clearBuffer();
			//u8g2.setCursor(35, 23);
			//u8g2.print("MEMOR");
			//u8g2.drawRFrame(10, 5, 107, 53, 8);
			//u8g2.drawRFrame(7, 2, 113, 59, 11);
			
			drawImage(memor_logo);
			
			if (millis() % 400 > 200) {
				//u8g2.setFontDirection(0);
				u8g2.setCursor(15, 40);
				u8g2.print("PRESS!");
			}
			u8g2.sendBuffer();
		}
		u8g2.clearBuffer();
		delay(10);
		while (digitalRead(KEY_CENTER) == LOW) {}
	}
	
	
	void run(void) {
	intro();
		
		while (digitalRead(KEY_CENTER) == LOW) {}
		u8g2.setDisplayRotation(U8G2_R0);
		
		u8g2.sendBuffer();
		randomSeed(millis());
		
		while(true){
		
		delay(500);
		initCards();
		
		drawBack();
		u8g2.sendBuffer();
		
		starttime = millis();
		found = 0;
		while (found < 24 && millis() - starttime < 3000000) {//24
			controls();
			if (millis() % 70 < 35) {
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				} else {
				u8g2.setDrawColor(0);
				u8g2.drawFrame(xpos * 10 + 2, ypos * 10 + 2, 9, 9);
				u8g2.setDrawColor(1);
			}
			repaintF();
			writetime();
			
			u8g2.sendBuffer();
		}
		
		ende();
		delay(300);
		while (digitalRead(KEY_CENTER) == HIGH) {}
		u8g2.clearBuffer();
		u8g2.sendBuffer();
		
		delay(500);
		}
	}
};
