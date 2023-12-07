#include "image.h" 

class SCLange {
	private:
    U8G2 u8g2;
	int board[28][15];
	int score, level, len, direction, px, py;
	public:
    SCLange (U8G2 &u8g2P)
    {
		u8g2 = u8g2P;
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
	
	void drawRect(int x, int y, int width, int height, bool filled)
	{
		u8g2.drawLine(x, y, x + (width - 1), y);
		u8g2.drawLine(x, y + (height - 1), x + (width - 1), y + (height - 1));
		u8g2.drawLine(x, y, x, y + (height - 1));
		u8g2.drawLine(x + (width - 1), y, x + (width - 1), y + (height - 1));
		if (filled)
		{
			u8g2.drawPixel(x + 2, y + 2);
			u8g2.drawPixel(x + 2, y + 3);
			u8g2.drawPixel(x + 3, y + 2);
			u8g2.drawPixel(x + 3, y + 3);
		}
	}
	
    void lostSound()
    {
		for (int i = 0; i < 40; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(2500);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(2500);
		}
		
		for (int i = 0; i < 70; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(3500);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(3500);
		}
	}
	
    void winSound()
    {
		
		for (int i = 0; i < 40; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(3500);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(3500);
		}
		
		for (int i = 0; i < 30; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(2500);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(2500);
		}
	}
	
    void putApple()
    {
		int rx = random(0, 28);
		int ry = random(0, 15);
		
		while (board[rx][ry] != 0)
		{
			rx = random(0, 28);
			ry = random(0, 15);
		}
		board[rx][ry] = -1;
	}
	
    void initializeS()
    {
		score = 0;
		level = 0;
		len = 3;
		px = 13;
		py = 8;
		
		for (int ix = 0; ix < 28; ix++)
		{
			for (int iy = 0; iy < 15; iy++)
			{
				board[ix][iy] = 0;
			}
		}
	}
	
    void writeScoreS(int score){
		
		u8g2.setFont( u8g2_font_4x6_mr );
		u8g2.setCursor(57, 119);
		u8g2.println("SC");
		u8g2.setCursor(51, 119);
		u8g2.println("OR");
		u8g2.setCursor(45, 121);
		u8g2.println("E");
		u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
		int ypos = 0;
		for (int i = 0; i < 4; i++)
		{
			int digit = score % 10;
			score /= 10;
			char cstr[16];
			itoa(digit, cstr, 10);
			u8g2.setCursor(ypos, 122 - u8g2.getStrWidth(cstr) / 2);
			u8g2.println(cstr);
			ypos += 11;
		}
	}
	
    void drawS()
    {
		u8g2.clearBuffer();
		drawRect(0, 0, 64, 116, false);
		
		for (int ix = 0; ix < 28; ix++)
		{
			for (int iy = 0; iy < 15; iy++)
			{
				if (board[ix][iy] > 0)
				{
					drawRect(iy * 4 + 2, ix * 4 + 2, 4, 4, false);
					if (board[ix][iy] == len)
					{
						drawRect(iy * 4 + 3, ix * 4 + 3, 2, 2, false);
					}
				}
				else if (board[ix][iy] == -1)
				{
					drawRect(iy * 4 + 2, ix * 4 + 3, 4, 2, false);
					drawRect(iy * 4 + 3, ix * 4 + 2, 2, 4, false);
				}
			}
		}
		writeScoreS(len - 3);
		u8g2.sendBuffer();
	}
	
    bool crash(int dir)
    {
		// return false;
		if (dir == UP && (py == 14 || board[px][py + 1] > 0))
		{
			return true;
		}
		if (dir == DOWN && (py == 0 || board[px][py - 1] > 0))
		{
			return true;
		}
		
		if (dir == LEFT && (px == 0 || board[px - 1][py] > 0))
		{
			return true;
		}
		
		if (dir == RIGHT && (px == 27 || board[px + 1][py] > 0))
		{
			return true;
		}
		
		return false;
	}
	
    void stepS()
    {
		for (int ix = 0; ix < 28; ix++)
		{
			for (int iy = 0; iy < 15; iy++)
			{
				if (board[ix][iy] > 0)
				{
					board[ix][iy] = board[ix][iy] - 1;
				}
			}
		}
	}
	
    void countdown()
    {
		
		for (int i = 3; i > 0; i--)
		{
			u8g2.clearBuffer();
			drawRect(0, 0, 64, 116, false);
			char cstr[16];
			itoa(i, cstr, 10);
			u8g2.setCursor(36, 58 - u8g2.getStrWidth("GET READY!") / 2);
			u8g2.println("GET READY!");
			u8g2.setCursor(20, 58 - u8g2.getStrWidth(cstr) / 2);
			u8g2.println(cstr);
			writeScoreS(0);
			u8g2.sendBuffer();
			
			switch (i)
			{
				
				case 1:
				digitalWrite(LEDG, HIGH);
				digitalWrite(LEDY, LOW);
				break;
				case 2:
				digitalWrite(LEDR, LOW);
				digitalWrite(LEDY, HIGH);
				break;
				case 3:
				digitalWrite(LEDR, HIGH);
				break;
				default:
				break;
			}
			delay(1000);
		}
		digitalWrite(LEDG, LOW);
		for (int i = 0; i < 8; i++)
		{
			u8g2.clearBuffer();
			drawRect(0, 0, 64, 116, false);
			writeScoreS(0);
			if (i % 2 == 1)
			{
				u8g2.setCursor(28, 58 - u8g2.getStrWidth("GO!") / 2);
				u8g2.println("GO!");
				u8g2.sendBuffer();
				
				digitalWrite(LEDR, HIGH);
				digitalWrite(LEDG, HIGH);
				digitalWrite(LEDY, HIGH);
				delay(30);
				digitalWrite(LEDR, LOW);
				digitalWrite(LEDG, LOW);
				digitalWrite(LEDY, LOW);
				delay(30);
			}
			else
			{
				u8g2.sendBuffer();
				delay(80);
				
			}
		}
		digitalWrite(LEDR, LOW);
		digitalWrite(LEDG, LOW);
		digitalWrite(LEDY, LOW);
	}
	
    void playS()
    {
		
		// countdown();
		
		uint32_t lastTime = 0;
		int lastDir = 0;
		direction = random(1, 5);
		putApple();
		
		while (true)
		{
			while (millis() < lastTime + 60)
			{
				if (digitalRead(KEY_RIGHT) == LOW && lastDir != LEFT)
				{
					direction = RIGHT;
				}
				if (digitalRead(KEY_LEFT) == LOW && lastDir != RIGHT)
				{
					direction = LEFT;
				}
				if (digitalRead(KEY_DOWN) == LOW && lastDir != UP)
				{
					direction = DOWN;
				}
				if (digitalRead(KEY_UP) == LOW && lastDir != DOWN)
				{
					direction = UP;
				}
			}
			
			lastDir = direction;
			
			if (!crash(direction))
			{
				if (direction == LEFT)
				px--;
				if (direction == RIGHT)
				px++;
				if (direction == DOWN)
				py--;
				if (direction == UP)
				py++;
			}
			else
			{
				u8g2.setDrawColor(0);
				u8g2.drawBox(1, 1, 62, 114);
				u8g2.setDrawColor(1);
				
				if (len - 3 > eepromReadInt(2))
				{
					eepromWriteInt(2, len - 3);
					u8g2.setCursor(40, 58 - u8g2.getStrWidth("HIGH") / 2);
					u8g2.println("HIGH");
					u8g2.setCursor(25, 58 - u8g2.getStrWidth("SCORE") / 2);
					u8g2.println("SCORE");
					char cstr[16];
					itoa(len - 3, cstr, 10);
					u8g2.setCursor(10, 58 - u8g2.getStrWidth(cstr) / 2);
					u8g2.println(cstr);
					u8g2.sendBuffer();
					delay(10);
					winSound();
				}
				else
				{
					u8g2.setCursor(40, 58 - u8g2.getStrWidth("YOUR") / 2);
					u8g2.println("YOUR");
					u8g2.setCursor(25, 58 - u8g2.getStrWidth("SCORE") / 2);
					u8g2.println("SCORE");
					char cstr[16];
					itoa(len - 3, cstr, 10);
					u8g2.setCursor(10, 58 - u8g2.getStrWidth(cstr) / 2);
					u8g2.println(cstr);
					u8g2.sendBuffer();
					delay(10);
					lostSound();
				}
				
				while (digitalRead(KEY_CENTER) == HIGH)
				{
				}
				break;
			}
			
			if (board[px][py] == -1)
			{
				tick();
				putApple();
				len++;
			}
			
			board[px][py] = len;
			
			drawS();
			stepS();
			
			lastTime = millis();
		}
	}
    /////ENDOOOP
	
    void tick()
    {
		digitalWrite(LEDR, HIGH);
		for (int i = 0; i < 50; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(100);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(100);
			if (i == 17) {
				digitalWrite(LEDR, LOW);
				digitalWrite(LEDY, HIGH);
				} else if (i == 35) {
				digitalWrite(LEDY, LOW);
				digitalWrite(LEDG, HIGH);
			}
		}
		digitalWrite(LEDG, LOW);
	}
	
	void run(){
		//u8g2.setFont(u8g2_font_t0_11_tr);
		u8g2.setFontDirection(1);
		
				unsigned long int last;
		last = millis();
			
			while (digitalRead(KEY_CENTER) == HIGH || millis() < last + 200) {
				u8g2.clearBuffer();
				drawImage(titlePicSC);
				if (millis() % 600 > 300) {
					u8g2.setFont(u8g2_font_micro_mr);
					u8g2.setCursor(16, 41);
					u8g2.println("PUSH TO PLAY");
				}
				u8g2.sendBuffer();
				delay(25);
			}
		
		//u8g2.setFont(u8g2_font_t0_11_tr);
		u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
		countdown();
		
		//u8g2.setFont(u8g2_font_smart_patrol_nbp_tr);
		while (true)
		{
			
			initializeS();
			playS();
		}
	}
    
};
