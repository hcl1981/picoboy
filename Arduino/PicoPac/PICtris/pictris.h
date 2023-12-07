#include "blocks.h"
#include "image.h"

class PICtris {
	private:
    U8G2 u8g2;
	bool fix[10][20];
	int type, rotation;
	int x, y;
	int nextType;
	int numblocks; 
	int score, level;
	int leveltime;
	
	public:
    PICtris (U8G2 &u8g2P)
    {
		u8g2 = u8g2P;
	}
	
	void drawImage(const byte image[64][128]) {
		for (int k = 0; k < 64; k++) {
			for (int l = 0; l < 128; l++) {
				if (image[k][l] > 0 && ((random(1000) > 100) || l < 63)) {
					u8g2.drawPixel(63 - k , l);
				}
			}
		}
	}
	
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
	
	void drawBlock(int x, int y, bool filled)
	{
		int basex = 2 + x * 6;
		int basey = 30 + (y - 4) * 6;
		drawRect(basex, basey, 6, 6, filled);
	}
	
	void drawFix()
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 4; j < 20; j++)
			{
				if (fix[i][j] == true)
				{
					drawBlock(i, j, true);
				}
			}
		}
	}
	
	void drawTetromino()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (blocks[type][rotation][j][i] == true)
				{
					drawBlock(i + x, j + y, true);
				}
			}
		}
	}
	
	void drawNext()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (blocks[nextType][0][j][i] == true)
				{
					drawBlock(i + 6, j - 3, true);
				}
			}
		}
	}
	
	bool fits(int x, int y, int rotation)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (blocks[type][rotation][j][i] == true && (i + x < 0))
				{
					return false;
				}
				
				if (blocks[type][rotation][j][i] == true && (i + x > 9))
				{
					return false;
				}
				
				if (blocks[type][rotation][j][i] == true && (j + y > 19))
				{
					return false;
				}
				
				if (blocks[type][rotation][j][i] == true && fix[i + x][j + y] == true)
				{
					return false;
				}
			}
		}
		return true;
	}
	
	void rotateTetromino()
	{
		if (fits(x, y, (rotation + 1) % 4))
		{
			rotation = (rotation + 1) % 4;
		}
	}
	
	void newTetromino()
	{
		x = 3;
		y = 0;
		type = nextType;
		nextType = rand() % 7;
		rotation = 0;
	}
	
	bool step()
	{
		if (fits(x, y + 1, rotation))
		{
			y++;
			return true;
		}
		return false;
	}
	
	void moveLeft()
	{
		if (fits(x - 1, y, rotation))
		{
			x--;
		}
	}
	
	void moveRight()
	{
		if (fits(x + 1, y, rotation))
		{
			x++;
		}
	}
	
	void settle()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (blocks[type][rotation][j][i])
				{
					fix[i + x][j + y] = true;
				}
			}
		}
	}
	
	bool clean()
	{
		bool cleant = false;
		int n = 0;
		for (int line = 19; line > 0; line--)
		{
			bool complete = true;
			for (int column = 0; column < 10; column++)
			{
				if (fix[column][line] == false)
				{
					complete = false;
				}
			}
			if (complete)
			{
				cleant = true;
				for (int lineC = line; lineC > 1; lineC--)
				{
					for (int column = 0; column < 10; column++)
					{
						fix[column][lineC] = fix[column][lineC - 1];
					}
				}
				line++;
				n++;
			}
		}
		switch (n)
		{
			case 1:
			score += 40 * (level + 1);
			break;
			case 2:
			score += 100 * (level + 1);
			break;
			case 3:
			score += 300 * (level + 1);
			break;
			case 4:
			score += 1200 * (level + 1);
			break;
			default:
			break;
		}
		if (cleant)
		{
			tick();
		}
		return cleant;
	}
	
	void initialize()
	{
		score = 0;
		level = 0;
		numblocks = 0;
		leveltime = 500;
		nextType = rand() % 7;
		
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				fix[i][j] = false;
			}
		}
	}
	
	void drawAll()
	{
		u8g2.clearBuffer();
		drawFix();
		drawTetromino();
		u8g2.drawFrame(0, 14, 64, 114);
		u8g2.setDrawColor(0);
		u8g2.drawBox(0, 0, 64, 14);
		u8g2.setDrawColor(1);
		drawNext();
		u8g2.setCursor(0, 10);
		u8g2.print(score);
		u8g2.sendBuffer();
	}
	
	void playT(void)
	{
		unsigned long last;
		unsigned long lastCTRL;
		unsigned long lastROT;
		bool run;
		bool pressedBefore;
		int steerBefore;
		
		while (true)
		{
			last = millis();
			lastCTRL = millis();
			lastROT = millis();
			run = true;
			pressedBefore = false;
			steerBefore = false;
			
			newTetromino();
			numblocks++;
			while (run)
			{
				
				while (millis() < last + leveltime)
				{
					if (steerBefore > 1)
					{
						if (digitalRead(KEY_UP) == LOW && (millis() > (lastCTRL + leveltime / 4)))
						{
							moveRight();
							lastCTRL = millis();
							drawAll();
							steerBefore++;
						}
						
						if (digitalRead(KEY_DOWN) == LOW && (millis() > (lastCTRL + leveltime / 4)))
						{
							moveLeft();
							lastCTRL = millis();
							drawAll();
							steerBefore++;
						}
					}
					else
					{
						if (digitalRead(KEY_UP) == LOW && (millis() > (lastCTRL + leveltime / 2)))
						{
							moveRight();
							lastCTRL = millis();
							drawAll();
							steerBefore++;
						}
						
						if (digitalRead(KEY_DOWN) == LOW && (millis() > (lastCTRL + leveltime / 2)))
						{
							moveLeft();
							lastCTRL = millis();
							drawAll();
							steerBefore++;
						}
					}
					
					if (!(digitalRead(KEY_UP) == LOW || digitalRead(KEY_DOWN) == LOW))
					{
						steerBefore = LOW;
					}
					
					if (!pressedBefore && digitalRead(KEY_LEFT) == LOW && (millis() > (lastROT + leveltime / 4))) // 4
					{
						rotateTetromino();
						lastROT = millis();
						drawAll();
						pressedBefore = true;
					}
					
					if (digitalRead(KEY_LEFT) == HIGH)
					{
						pressedBefore = false;
						delay(8);
					}
					
					if (digitalRead(KEY_RIGHT) == LOW && (millis() > (lastCTRL + leveltime / 8)))
					{
						lastCTRL = millis();
						last -= 1000;
					}
				}
				if (step())
				{
				}
				else
				{
					settle();
					newTetromino();
					if (!fits(x, y, rotation))
					{
						u8g2.setDrawColor(0);
						u8g2.drawBox(9, 49, 48, 49);
						u8g2.setDrawColor(1);
						u8g2.drawFrame(9, 49, 48, 49);
						
						if (score > eepromReadInt(0))
						{
							eepromWriteInt(0, score);
							u8g2.setCursor(32 - u8g2.getStrWidth("HIGH") / 2, 62);
							u8g2.println("HIGH");
							u8g2.setCursor(32 - u8g2.getStrWidth("SCORE") / 2, 77);
							u8g2.println("SCORE");
							char cstr[16];
							itoa(score, cstr, 10);
							u8g2.setCursor(32 - u8g2.getStrWidth(cstr) / 2, 92);
							u8g2.println(cstr);
						}
						else
						{
							u8g2.setCursor(32 - u8g2.getStrWidth("YOUR") / 2, 62);
							u8g2.println("YOUR");
							u8g2.setCursor(32 - u8g2.getStrWidth("SCORE") / 2, 77);
							u8g2.println("SCORE");
							char cstr[16];
							itoa(score, cstr, 10);
							u8g2.setCursor(32 - u8g2.getStrWidth(cstr) / 2, 92);
							u8g2.println(cstr);
						}
						
						u8g2.sendBuffer();
						while (digitalRead(KEY_CENTER) == HIGH)
						{
						}
						initialize();
						u8g2.clearBuffer();
						u8g2.sendBuffer();
						return;
					}
					numblocks++;
					if (numblocks >= 15) // 15
					{
						level++;
						leveltime *= 0.8;
						numblocks = 0;
					}
				}
				
				drawAll();
				
				if (clean())
				{
					delay(leveltime / 8);
					drawAll();
				}
				
				last = millis();
			}
		}
	}
	void run(){
		
		unsigned long int last;
		last = millis();
			
			while (digitalRead(KEY_CENTER) == HIGH || millis() < last + 200) {
				u8g2.clearBuffer();
				drawImage(titlePicPT);
				if (millis() % 600 > 300) {
					u8g2.setFont(u8g2_font_micro_mr);
					u8g2.setCursor(7, 71);
					u8g2.println("PUSH TO PLAY");
				}
				u8g2.sendBuffer();
				delay(25);
			}
			u8g2.setFont(u8g2_font_t0_11_tr);
		while (true)
		{
			
			initialize();
			playT();
		}
	}
	
};