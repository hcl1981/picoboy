#include "arrow.h"
#include "wnr_logo.h"
#define A_SIZE 19

class Sensy {
	private:
    U8G2 u8g2;
	
	const int buttonPins[4] = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}; // Pins, an denen die Taster angeschlossen sind
	int buttonStates[4] = {HIGH, HIGH, HIGH, HIGH}; // Aktuelle Zustände der Taster
	int lastButtonStates[4] = {HIGH, HIGH, HIGH, HIGH}; // Vorherige Zustände der Taster
	unsigned long lastDebounceTimes[4] = {0, 0, 0, 0}; // Zeitstempel der letzten Änderungen
	unsigned long debounceDelay = 5;
	
	byte history [10000];
	int progress = 0;
	
	bool rungame = true;
	bool repaint = false;
	unsigned long int  repainttime;
	unsigned long int starttime;
	unsigned long int lastmillis = millis();
	
	public:
    Sensy (U8G2 &u8g2P)
    {
		u8g2 = u8g2P;
	}
	
	
	void drawCircles(bool filled) {
		if (!filled) {
			u8g2.drawCircle(64, 32, 8);
			} else {
			u8g2.drawDisc(64, 32, 8);
		}
		u8g2.drawCircle(64, 32, 31);	
	}
	
	void drawLeft(int x, int y) {
		for (int i = 0; i < A_SIZE; i++) {
			for (int j = 0; j < A_SIZE; j++) {
				if (arrow[i][j] == 1) {
					u8g2.drawPixel(x + i, y + j);
				}
			}
		}
	}
	
	void drawRight(int x, int y) {
		for (int i = 0; i < A_SIZE; i++) {
			for (int j = 0; j < A_SIZE; j++) {
				if (arrow[i][j] == 1) {
					u8g2.drawPixel(x - i, y + j);
				}
			}
		}
	}
	
	void drawUp(int x, int y) {
		for (int i = 0; i < A_SIZE; i++) {
			for (int j = 0; j < A_SIZE; j++) {
				if (arrow[i][j] == 1) {
					u8g2.drawPixel(x + j, y + i);
				}
			}
		}
	}
	
	void drawDown(int x, int y) {
		for (int i = 0; i < A_SIZE; i++) {
			for (int j = 0; j < A_SIZE; j++) {
				if (arrow[i][j] == 1) {
					u8g2.drawPixel(x + j, y - i);
				}
			}
		}
	}
	
	int repeatAdd() {
		int dir = random(4);
		history[progress] = dir;
		for (int i = 0; i <= progress; i++) {
			u8g2.clearBuffer();
			u8g2.setCursor(15,17);
			u8g2.println("W");
			u8g2.setCursor(15,27);
			u8g2.println("A");
			u8g2.setCursor(15,37);
			u8g2.println("T");
			u8g2.setCursor(15,47);
			u8g2.println("C");
			u8g2.setCursor(15,57);
			u8g2.println("H");
			drawCircles(false);
			
			u8g2.sendBuffer();
			delay(300);
			if (history[i] == 0) {
				drawUp(64 - 9, 32 - 9 - 20);//-19
			}
			if (history[i] == 1) {
				drawDown(64 - 9, 32 + 19 - 9 + 19);
			}
			if (history[i] == 2) {
				drawLeft(64 - 9 - 20, 32 - 9);
			}
			if (history[i] == 3) {
				drawRight(64 + 19 - 9 + 19, 32 - 9);
			}
			u8g2.setCursor(15,17);
			u8g2.println("W");
			u8g2.setCursor(15,27);
			u8g2.println("A");
			u8g2.setCursor(15,37);
			u8g2.println("T");
			u8g2.setCursor(15,47);
			u8g2.println("C");
			u8g2.setCursor(15,57);
			u8g2.println("H");
			drawCircles(false);
			u8g2.sendBuffer();
			delay(500);
		}
		
		u8g2.clearBuffer();
		drawCircles(false);
		u8g2.setCursor(15,17);
		u8g2.println("W");
		u8g2.setCursor(15,27);
		u8g2.println("A");
		u8g2.setCursor(15,37);
		u8g2.println("T");
		u8g2.setCursor(15,47);
		u8g2.println("C");
		u8g2.setCursor(15,57);
		u8g2.println("H");
		u8g2.sendBuffer();
		delay(500);
		u8g2.clearBuffer();
		drawCircles(false);
		
		u8g2.sendBuffer();
		progress++;
		return dir;
	}
	
	void play() {
		while (rungame) {
			int current = repeatAdd();
			int richtung = - 1;
			
			for (int j = 0; j < progress && rungame;) {
				for (int i = 0; i < 4 && rungame; i++) {
					int reading = digitalRead(buttonPins[i]);
					
					u8g2.clearBuffer();
					if (digitalRead(KEY_UP) == LOW) drawUp(64 - 9, 32 - 9 - 20);
					if (digitalRead(KEY_DOWN) == LOW)drawDown(64 - 9, 32 + 19 - 9 + 19);
					if (digitalRead(KEY_LEFT) == LOW)drawLeft(64 - 9 - 20, 32 - 9);
					if (digitalRead(KEY_RIGHT) == LOW)drawRight(64 + 19 - 9 + 19, 32 - 9);
					//u8g2.setCursor(120, 63);
					u8g2.setCursor(107,10);
					u8g2.println("R");
					u8g2.setCursor(107,20);
					u8g2.println("E");
					u8g2.setCursor(107,30);
					u8g2.println("P");
					u8g2.setCursor(107,40);
					u8g2.println("E");
					u8g2.setCursor(107,50);
					u8g2.println("A");
					u8g2.setCursor(107,60);
					u8g2.println("T");
					
					drawCircles(true);
					
					u8g2.sendBuffer();
					
					if (reading != lastButtonStates[i]) {
						lastDebounceTimes[i] = millis();
					}
					if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
						if (reading != buttonStates[i]) {
							buttonStates[i] = reading;
							if (buttonStates[i] == LOW) {
								if (history[j] == i) {
									j++;
									} else {
									rungame = false;
								}
							}
						}
					}
					lastButtonStates[i] = reading;
				}
			}
			while (digitalRead(KEY_UP) == LOW || digitalRead(KEY_DOWN) == LOW || digitalRead(KEY_LEFT) == LOW || digitalRead(KEY_RIGHT) == LOW ) {}
			u8g2.clearBuffer();
			//u8g2.setCursor(120, 63);
			u8g2.setCursor(107,10);
			u8g2.println("R");
			u8g2.setCursor(107,20);
			u8g2.println("E");
			u8g2.setCursor(107,30);
			u8g2.println("P");
			u8g2.setCursor(107,40);
			u8g2.println("E");
			u8g2.setCursor(107,50);
			u8g2.println("A");
			u8g2.setCursor(107,60);
			u8g2.println("T");
			drawCircles(true);
			
			if (rungame) {
				digitalWrite(LEDG, HIGH);
				delay(50);
				digitalWrite(LEDG, LOW);
				} else {
				digitalWrite(LEDR, HIGH);
				delay(50);
				digitalWrite(LEDR, LOW);
				
			}
			u8g2.sendBuffer();
			
			delay(300);
			
			
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
	
	void intro() {
		u8g2.setFontDirection(1);
		u8g2.setFont(u8g2_font_inb16_mf );
		while (digitalRead(KEY_CENTER) == HIGH) {
		u8g2.clearBuffer();
			//u8g2.clearBuffer();
			//u8g2.setCursor(5, 23);
			//u8g2.println("watch");
			//u8g2.println("&");
			//u8g2.println("repeat");
			//u8g2.drawRFrame(0, 5, 128, 53, 8);
			
			drawImage(wnr_logo);
			if (millis() % 400 > 200) {
				//u8g2.setFontDirection(0);
				u8g2.setCursor(5, 4);
				u8g2.print("P U S H !");
			}
			u8g2.sendBuffer();
		}
		u8g2.clearBuffer();
		delay(10);
		while (digitalRead(KEY_CENTER) == LOW) {}
	}
	
	
	void run(void) {
		
		
		
		intro();
		
		u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
		u8g2.setFontDirection(0);
		u8g2.setDisplayRotation(U8G2_R0);
		
		while (true) {
			progress = 0;
			
			rungame = true;
			repaint = false;
			
			randomSeed(millis());
			
			play();
			
			u8g2.setDrawColor(0);
			u8g2.drawRBox(13, 7, 99, 51, 4);
			u8g2.drawBox(105, 0, 127, 63);
			u8g2.setDrawColor(1);
			u8g2.drawRFrame(13, 7, 99, 51, 4);
			u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
			
			
			if (progress-1 > eepromReadInt(6))
			{
				eepromWriteInt(6, progress - 1);
				u8g2.setCursor(64 - u8g2.getStrWidth("HIGH") / 2,22);
				u8g2.println("HIGH");
				u8g2.setCursor(64 - u8g2.getStrWidth("SCORE") / 2,37);
				u8g2.println("SCORE");
				char cstr[16];
				itoa(progress - 1, cstr, 10);
				u8g2.setCursor(64 - u8g2.getStrWidth(cstr) / 2,52);
				u8g2.println(cstr);
				u8g2.sendBuffer();
				delay(10);
			}
			else
			{
				u8g2.setCursor(64 - u8g2.getStrWidth("YOUR") / 2, 22);
				u8g2.println("YOUR");
				u8g2.setCursor(64 - u8g2.getStrWidth("SCORE") / 2, 37);
				u8g2.println("SCORE");
				char cstr[16];
				itoa(progress - 1, cstr, 10);
				u8g2.setCursor( 64 - u8g2.getStrWidth(cstr) / 2,52);
				u8g2.println(cstr);
				u8g2.sendBuffer();
				delay(10);
			}
			
			u8g2.sendBuffer();
			delay(500);
			while (digitalRead(KEY_CENTER) == HIGH) {}
			
			u8g2.clearBuffer();
			u8g2.sendBuffer();
			
			delay(100);
			while (digitalRead(KEY_CENTER) == LOW) {}
			
		}
		
	}
};