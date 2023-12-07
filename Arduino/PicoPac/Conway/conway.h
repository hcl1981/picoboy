#include "stamps.h"
#include <vector>

#define I_MAX 128
#define J_MAX 64
#define I_CENTER I_MAX / 2
#define J_CENTER J_MAX / 2
#define STEMPEL_SIZE = 9
#define STEMPEL_NUMBER = 2
#define DELAY_TIME 50
#define FONT u8g2_font_5x7_tf
#define FONT_HEIGHT 6
#define INPUT_DELAY 250

class Conway {
	private:
    U8G2 u8g2;
	uint_fast8_t pixel[I_MAX][J_MAX];
	uint_fast8_t temp[I_MAX][J_MAX];
	boolean randomMode = false;
	int currentSelection = 0;
	
	public:
    Conway (U8G2 &u8g2P)
    {
		u8g2 = u8g2P;
	}
	
	String split(String data, char separator, int index)
	{
		int stringData = 0;   // variable to count data part nr
		String dataPart = ""; // variable to hole the return text
		
		for (unsigned int i = 0; i < data.length(); i++)
		{ // Walk through the text one letter at a time
			if (data[i] == separator)
			{
				// Count the number of times separator character appears in the text
				stringData++;
			}
			else if (stringData == index)
			{
				// get the text when separator is the rignt one
				dataPart.concat(data[i]);
			}
			else if (stringData > index)
			{
				// return text and stop if the next separator appears - to save CPU-time
				return dataPart;
				break;
			}
		}
		// return text if this is the last part
		return dataPart;
	}
	
	int countOccurances(String input, char symbol)
	{
		int count = 0;
		for (unsigned int i = 0; i < input.length(); i++)
		{
			if (input.charAt(i) == symbol)
			{
				count++;
			}
		}
		return count;
	}
	
	bool showPattern(String pattern, int xPos, int yPos, bool center)
	{
		unsigned int lineCount = countOccurances(pattern, '#') + 1;
		unsigned int lineLen = 0;
		bool valid = true;
		String lines[lineCount];
		for (unsigned int i = 0; i < lineCount; i++)
		{
			lines[i] = split(pattern, '#', i);
			if (i == 0)
			{
				lineLen = lines[0].length();
			}
			else if (lineLen != lines[i].length())
			{
				valid = false;
			}
		}
		if (valid)
		{
			for (unsigned int i = 0; i < lineCount; i++)
			{
				for (unsigned int j = 0; j < lineLen; j++)
				{
					if (lines[i].charAt(j) != '1' && lines[i].charAt(j) != '0')
					{
						valid = false;
					}
				}
			}
		}
		if (valid)
		{
			for (unsigned int i = 0; i < lineCount; i++)
			{
				for (unsigned int j = 0; j < lineLen; j++)
				{
					if (lines[i].charAt(j) == '1')
					{
						if (center)
						{
							u8g2.drawPixel(j + I_MAX / 2 - lineLen / 2, i + J_MAX / 2 - lineCount / 2);
						}
						else
						{
							u8g2.drawPixel((j + xPos + I_MAX) % I_MAX, (i + yPos + J_MAX) % J_MAX);
						}
					}
				}
			}
		}
		return valid;
	}
	
	int selectPattern()
	{
		int selection = currentSelection;
		u8g2.clear();
		u8g2.drawStr(I_MAX/2-u8g2.getStrWidth("<- Select Pattern ->")/2, FONT_HEIGHT, "<- Select Pattern ->");
		showPattern(stamps[selection], 0, 10, true);
		u8g2.sendBuffer();
		unsigned long lastMillis = millis();
		while (digitalRead(KEY_CENTER) == HIGH || millis() < lastMillis + INPUT_DELAY)
		{
			if (digitalRead(KEY_RIGHT) == LOW && millis() > lastMillis + INPUT_DELAY)
			{
				selection = (selection + 1) % (numStamps + 1);
				u8g2.clearBuffer();
				u8g2.drawStr(I_MAX/2-u8g2.getStrWidth("<- Select Pattern ->")/2, FONT_HEIGHT, "<- Select Pattern ->");
				if (selection == numStamps)
				{
					u8g2.drawStr(I_MAX/2-u8g2.getStrWidth("Start!")/2, J_MAX / 2 + FONT_HEIGHT / 2, "Start!");
				}
				else
				{
					showPattern(stamps[selection], 0, 10, true);
				}
				u8g2.sendBuffer();
				lastMillis = millis();
			}
			else if (digitalRead(KEY_LEFT) == LOW && millis() > lastMillis + INPUT_DELAY)
			{
				selection = ((selection + (numStamps + 1)) - 1) % (numStamps + 1);
				u8g2.clearBuffer();
				u8g2.drawStr(I_MAX/2-u8g2.getStrWidth("<- Select Pattern ->")/2, FONT_HEIGHT, "<- Select Pattern ->");
				if (selection == numStamps)
				{
					u8g2.drawStr(I_MAX/2-u8g2.getStrWidth("Start!")/2, J_MAX / 2 + FONT_HEIGHT / 2, "Start!");
				}
				else
				{
					showPattern(stamps[selection], 0, 10, true);
				}
				u8g2.sendBuffer();
				lastMillis = millis();
			}
		}
		return selection;
	}
	
	void initialisiereZufaellig()
	{
		// initial array mit zufallswerten befuellen
		for (int i = 0; i < I_MAX; i++)
		{
			for (int j = 0; j < J_MAX; j++)
			{
				pixel[i][j] = random(0, 2);
			}
		}
	}
	
	void drawGame()
	{
		for (int i = 0; i < I_MAX; i++)
		{
			for (int j = 0; j < J_MAX; j++)
			{
				if (pixel[i][j] == 1)
				{
					u8g2.drawPixel(i, j);
				}
			}
		}
	}
	
	void putPattern(String pattern, int xPos, int yPos)
	{
		unsigned int lineCount = countOccurances(pattern, '#') + 1;
		unsigned int lineLen = 0;
		bool valid = true;
		String lines[lineCount];
		for (unsigned int i = 0; i < lineCount; i++)
		{
			lines[i] = split(pattern, '#', i);
			if (i == 0)
			{
				lineLen = lines[0].length();
			}
			else if (lineLen != lines[i].length())
			{
				valid = false;
			}
		}
		if (valid)
		{
			for (unsigned int i = 0; i < lineCount; i++)
			{
				for (unsigned int j = 0; j < lineLen; j++)
				{
					if (lines[i].charAt(j) != '1' && lines[i].charAt(j) != '0')
					{
						valid = false;
					}
				}
			}
		}
		if (valid)
		{
			for (unsigned int i = 0; i < lineCount; i++)
			{
				for (unsigned int j = 0; j < lineLen; j++)
				{
					if (lines[i].charAt(j) == '1')
					{
						pixel[(j + xPos + I_MAX) % I_MAX][(i + yPos + J_MAX) % J_MAX] = 1;
					}
				}
			}
		}
	}
	
	void placePattern(int pattern)
	{
		unsigned int lineCount = countOccurances(stamps[pattern], '#') + 1;
		unsigned int lineLen = split(stamps[pattern], '#', 0).length();
		int xPos = I_MAX/2 - lineLen/2;
		int yPos = J_MAX/2 - lineCount/2;
		int timediff = 0;
		unsigned long int lastClick = millis();
		while (digitalRead(KEY_CENTER) == HIGH || millis() < lastClick + INPUT_DELAY)
		{
			u8g2.clearBuffer();
			if (digitalRead(KEY_UP) == LOW)
			{
				yPos = (yPos + J_MAX - 1) % J_MAX;
			}
			if (digitalRead(KEY_DOWN) == LOW)
			{
				yPos = (yPos + 1) % J_MAX;
			}
			if (digitalRead(KEY_LEFT) == LOW)
			{
				xPos = (xPos + I_MAX - 1) % I_MAX;
			}
			if (digitalRead(KEY_RIGHT) == LOW)
			{
				xPos = (xPos + 1) % I_MAX;
			}
			if (millis() % 200 < 100)
			{
				timediff = millis();
				showPattern(stamps[pattern], xPos, yPos, false);
				timediff = millis() - timediff;
				drawGame();
				u8g2.sendBuffer();
			}
			else
			{
				delay(timediff);
				drawGame();
				u8g2.sendBuffer();
			}
			// lastClick = millis();
		}
		showPattern(stamps[pattern], xPos, yPos, false);
		putPattern(stamps[pattern], xPos, yPos);
		u8g2.sendBuffer();
		delay(INPUT_DELAY);
	}
	
	void drawInitial()
	{
		u8g2.clearBuffer();
		u8g2.sendBuffer();
		int pattern = 0;
		while (pattern >= 0)
		{
			pattern = selectPattern();
			currentSelection = pattern;
			if (pattern < numStamps)
			{
				placePattern(pattern);
			}
			else
			{
				break;
			}
		}
	}
	
	void run()
	{
		for(int i = 0; i < I_MAX; i++){
			for(int j = 0; j < J_MAX; j++){
				pixel[i][j] = 0;
				temp[i][j] = 0;
				}
			}
		
		u8g2.setDisplayRotation(U8G2_R0);
		u8g2.clearBuffer(); // clear the internal memory
		u8g2.setFont(FONT);
		u8g2.drawStr(I_MAX/2-u8g2.getStrWidth("UP: Draw!")/2, J_MAX / 2 - FONT_HEIGHT, "UP: Draw!");
		u8g2.drawStr(I_MAX/2-u8g2.getStrWidth("DOWN: Let the chaos begin!")/2, J_MAX / 2 + FONT_HEIGHT, "DOWN: Let the chaos begin!");
		u8g2.sendBuffer();
		while (true)
		{
			if (digitalRead(KEY_DOWN) == LOW)
			{
				initialisiereZufaellig();
				randomMode = true;
				break;
			}
			if (digitalRead(KEY_UP) == LOW)
			{
				drawInitial();
				break;
			}
		}
		
		while(true){
			if (randomMode)
			{
				update_from_joystick();
			}
			else
			{
				delay(DELAY_TIME);
			}
			
			for (int i = 0; i < I_MAX; i++)
			{
				for (int j = 0; j < J_MAX; j++)
				{
					// fuer den naechsten frame
					temp[i][j] = update(i, j);
					// zeichne frame
					if (pixel[i][j] == 1)
					{
						u8g2.drawPixel(i, j);
					}
				}
			}
			// sende frame ans display und so
			u8g2.sendBuffer();
			u8g2.clearBuffer();
			
			for (int i = 0; i < I_MAX; i++)
			{
				for (int j = 0; j < J_MAX; j++)
				{
					pixel[i][j] = temp[i][j];
				}
			}
		}
	}
	
	void update_from_joystick()
	{
		// joy-stick nach unten druecken macht restart mit Zufallswerten
		if (digitalRead(KEY_CENTER) == LOW)
		{
			initialisiereZufaellig();
		}
	}
	
	char update(int i, int j)
	{
		// zaehle lebende nachbarn
		uint_fast8_t lebend = 0;
		lebend += pixel[(i + 1) % I_MAX][(j + 1) % J_MAX];
		lebend += pixel[(i + 1) % I_MAX][(j) % J_MAX];
		lebend += pixel[(i + 1) % I_MAX][(J_MAX + j - 1) % J_MAX];
		lebend += pixel[(i) % I_MAX][(j + 1) % J_MAX];
		lebend += pixel[(i) % I_MAX][(J_MAX + j - 1) % J_MAX];
		lebend += pixel[(I_MAX + i - 1) % I_MAX][(j + 1) % J_MAX];
		lebend += pixel[(I_MAX + i - 1) % I_MAX][(j) % J_MAX];
		lebend += pixel[(I_MAX + i - 1) % I_MAX][(J_MAX + j - 1) % J_MAX];
		
		// conrads bedingungen fuer leben und TOOOOT
		if (pixel[i][j] == 0 && lebend == 3)
		{
			return 1;
		}
		else if (pixel[i][j] == 1 && lebend < 2)
		{
			return 0;
		}
		else if (pixel[i][j] == 1 && lebend < 4)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
};