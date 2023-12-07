class StarIntro {
	private:
    U8G2 u8g2;
	int gameID = 1;
	public:
    StarIntro (U8G2 &u8g2P)
    {
		u8g2 = u8g2P;
	}
	
	
	int starIntro()
	{
		u8g2.setFont(u8g2_font_t0_11_tr);
		const byte numstars = 150;
		unsigned long lastSelection = millis();
		float stars[numstars][4];
		
		int eescoreS = eepromReadInt(2);
		int eescoreT = eepromReadInt(0);
		int eescoreTM = eepromReadInt(4);
		
		for (int i = 0; i < numstars; i++)
		{
			stars[i][0] = 33;
			stars[i][1] = 32;
			float angle = (random(3600) / 1800.0) * PI;
			float speed = random(100) + 100;
			stars[i][2] = speed * sin(angle);
			stars[i][3] = speed * cos(angle);
		}
		
		while (digitalRead(KEY_CENTER) == HIGH)
		{
			u8g2.clearBuffer();
			u8g2.setCursor(32 - u8g2.getStrWidth("PicoPac!") / 2, 15);
			u8g2.println("PicoPac!");
			if (millis() % 6000 < 2000)
			{
				u8g2.setCursor(32 - u8g2.getStrWidth("PICTRIS") / 2, 75);
				u8g2.println("PICTRIS");
				u8g2.setCursor(32 - u8g2.getStrWidth("HI-SCORE") / 2, 60);
				u8g2.println("HI-SCORE");
				char cstr[16];
				itoa(eescoreT, cstr, 10);
				u8g2.setCursor(32 - u8g2.getStrWidth(cstr) / 2, 90);
				u8g2.println(cstr);
			}
			else if (millis() % 6000 < 4000)
			{
				u8g2.setCursor(32 - u8g2.getStrWidth("SCLANGE") / 2, 75);
				u8g2.println("SCLANGE");
				u8g2.setCursor(32 - u8g2.getStrWidth("HI-SCORE") / 2, 60);
			    u8g2.println("HI-SCORE");
				char cstr[16];
				itoa(eescoreS, cstr, 10);
				u8g2.setCursor(32 - u8g2.getStrWidth(cstr) / 2, 90);
				u8g2.println(cstr);
			} else {
				u8g2.setCursor(32 - u8g2.getStrWidth("TABMAN") / 2, 75);
				u8g2.println("TABMAN");
				u8g2.setCursor(32 - u8g2.getStrWidth("HI-SCORE") / 2, 60);
			    u8g2.println("HI-SCORE");
				char cstr[16];
				itoa(eescoreTM, cstr, 10);
				u8g2.setCursor(32 - u8g2.getStrWidth(cstr) / 2, 90);
				u8g2.println(cstr);
			}
			if ( millis() % 600 < 150) {
			analogWrite(LEDR, 65);
			analogWrite(LEDY, 0);
			analogWrite(LEDG, 0);
			} else if (millis() % 600 < 300) {
			analogWrite(LEDR, 0);
			analogWrite(LEDY, 15);
			analogWrite(LEDG, 0);
			} else if (millis() % 600 < 450) {
			analogWrite(LEDR, 0);
			analogWrite(LEDY, 0);
			analogWrite(LEDG, 185);
			} else {
			analogWrite(LEDR, 0);
			analogWrite(LEDG, 30);
			analogWrite(LEDG, 0);
			}
			
			if (millis() % 500 < 250)
			{
			u8g2.setCursor(32 - u8g2.getStrWidth("Select") / 2, 108);
			u8g2.println("Select");
			if (gameID == 1)
			{
			u8g2.setCursor(32 - u8g2.getStrWidth("<        >") / 2, 123);
			u8g2.println("<        >");
			u8g2.setCursor(32 - u8g2.getStrWidth("PICTRIS") / 2, 123);
			u8g2.println("PICTRIS");
			}
			else if (gameID == 2)
			{
			u8g2.setCursor(32 - u8g2.getStrWidth("<        >") / 2, 123);
			u8g2.println("<        >");
			u8g2.setCursor(32 - u8g2.getStrWidth("SCLANGE") / 2, 123);
			u8g2.println("SCLANGE");
			}
			else if (gameID == 3)
			{
			u8g2.setCursor(32 - u8g2.getStrWidth("<        >") / 2, 123);
			u8g2.println("<        >");
			u8g2.setCursor(32 - u8g2.getStrWidth("TABMAN") / 2, 123);
			u8g2.println("TABMAN");
			}
			else if (gameID == 4)
			{
			u8g2.setCursor(32 - u8g2.getStrWidth("<        >") / 2, 123);
			u8g2.println("<        >");
			u8g2.setCursor(32 - u8g2.getStrWidth("DEMO") / 2, 123);
			u8g2.println("DEMO");
			}
			else if (gameID == 5)
			{
			u8g2.setCursor(32 - u8g2.getStrWidth("<        >") / 2, 123);
			u8g2.println("<        >");
			u8g2.setCursor(32 - u8g2.getStrWidth("CONWAY") / 2, 123);
			u8g2.println("CONWAY");
			}
			else if (gameID == 6)
			{
			u8g2.setCursor(32 - u8g2.getStrWidth("<        >") / 2, 123);
			u8g2.println("<        >");
			u8g2.setCursor(32 - u8g2.getStrWidth("HWTest") / 2, 123);
			u8g2.println("HWTest");
			}
			}    for (int i = 0; i < numstars; i++)
			{
			
			u8g2.drawPixel(stars[i][1], stars[i][0]);
			stars[i][0] += stars[i][2] / 1000;
			stars[i][1] += stars[i][3] / 1000;
			
			stars[i][2] *= 1.1;
			stars[i][3] *= 1.1;
			
			if (stars[i][0] < 0 || stars[i][0] > 128 || stars[i][1] < 0 || stars[i][1] > 64)
			{
			stars[i][0] = 33;
			stars[i][1] = 32;
			float angle = (random(3600) / 1800.0) * PI;
			float speed = random(100) + 100;
			stars[i][2] = speed * sin(angle);
			stars[i][3] = speed * cos(angle);
			}
			}
			u8g2.sendBuffer();
			if (digitalRead(KEY_DOWN) == LOW && millis() > lastSelection + 200)
			{
			gameID = gameID - 1;
			if (gameID == 0)
			gameID = 6;//2
			lastSelection = millis();
			// break;
			}
			else if (digitalRead(KEY_UP) == LOW && millis() > lastSelection + 200)
			{
			gameID = gameID + 1;
			if (gameID == 7)//3
			gameID = 1;
			lastSelection = millis();
			// break;
			}
			}
			delay(10);
			analogWrite(LEDR, 0);
			analogWrite(LEDG, 0);
			analogWrite(LEDY, 0);
			while (digitalRead(KEY_CENTER) == LOW) {}
			delay(90);
			return gameID;
			}
			
			
			};			