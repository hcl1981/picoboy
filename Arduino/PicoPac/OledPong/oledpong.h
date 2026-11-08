#define MIN_INPUT -2
#define MAX_INPUT 2

#define BAT_HEIGHT 4
#define BAT_WIDTH 32
#define BAT_DELTA 10

#define BALL_RAD 4

#define ALPHA 0.5

#define PAGE_DELAY 50
#define SPEED 3
#define SPEEDINC 5

#define HS_ADDR 10

#define HIGH_TONE 440
#define LOW_TONE 220
#define TONE_DUR 25

class OledPong
{
private:
    U8G2 u8g2;
    Sensor sensor;

    int score = 0;
    float val = 0;
    int dpWidth, dpHeight, vx, vy, ballx, bally, xBat;
    bool improvedHighscore = false;

    float map_float(float x, float in_min, float in_max, float out_min, float out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    void newHighscore()
    {
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, HIGH);
        digitalWrite(LEDY, HIGH);
        delay(30);
        digitalWrite(LEDR, LOW);
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDY, LOW);
        delay(100);
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, HIGH);
        digitalWrite(LEDY, HIGH);
        delay(30);
        digitalWrite(LEDR, LOW);
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDY, LOW);
    }

    void nextLevel()
    {
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, HIGH);
        digitalWrite(LEDY, HIGH);
        delay(30);
        digitalWrite(LEDR, LOW);
        digitalWrite(LEDG, LOW);
        digitalWrite(LEDY, LOW);
    }

    void bounce(void)
    {
        if (bally >= dpHeight - BALL_RAD - BAT_HEIGHT - 1)
        {
            if ((ballx + BALL_RAD >= xBat) && (ballx - BALL_RAD <= xBat + BAT_WIDTH))
            {
                bally = dpHeight - BALL_RAD - BAT_HEIGHT - 1;
                vy *= -1;
                score += 1;
                tone(SPEAKER, LOW_TONE, TONE_DUR);
                if (score > eepromReadInt(HS_ADDR))
                {
                    eepromWriteInt(HS_ADDR, score);
                    if (!improvedHighscore)
                    {
                        improvedHighscore = true;
                        newHighscore();
                    }
                }
                if (score % SPEEDINC == 0)
                {
                    nextLevel();
                    if (vy > 0)
                        vy++;
                    else
                        vy--;
                    if (vx > 0)
                        vx++;
                    else
                        vx--;
                }
            }
            else
            {
                score = 0;
                init_ball();
                return;
            }
        }
        if (bally <= BALL_RAD)
        {
            bally = BALL_RAD;
            vy *= -1;
            tone(SPEAKER, HIGH_TONE, TONE_DUR);
        }
        if (ballx >= dpWidth - BALL_RAD - 1)
        {
            ballx = dpWidth - BALL_RAD - 1;
            vx *= -1;
            tone(SPEAKER, HIGH_TONE, TONE_DUR);
        }
        if (ballx <= BALL_RAD)
        {
            ballx = BALL_RAD;
            vx *= -1;
            tone(SPEAKER, HIGH_TONE, TONE_DUR);
        }
    }

    void init_ball(void)
    {
        if (random(2))
            vx = SPEED;
        else
            vx = -SPEED;
        vy = SPEED;
        ballx = random(BALL_RAD, dpWidth - BALL_RAD);
        bally = BALL_RAD;
    }

    void title_screen(void)
    {
        u8g2.firstPage();
        do
        {
            u8g2.setFont(u8g2_font_helvB08_tf);
            u8g2.setCursor(0, 14);
            u8g2.print("OLED Pong");
            u8g2.setCursor(0, 28);
            u8g2.print("© 2023 by");
            u8g2.setCursor(0, 42);
            u8g2.print("Christoph Scholz");
            u8g2.setCursor(0, 56);
            u8g2.print("Push center to start");
        } while (u8g2.nextPage());
        while (digitalRead(KEY_CENTER) == HIGH)
        {
        }
    }

public:
    OledPong(U8G2 &u8g2P)
    {
        u8g2 = u8g2P;
    }

    void run(void)
    {
        u8g2.setDisplayRotation(U8G2_R0);
        u8g2.enableUTF8Print();
        u8g2.setFont(u8g2_font_ncenB14_tr);
        u8g2.clearBuffer();
        u8g2.sendBuffer();
        dpWidth = u8g2.getDisplayWidth();
        dpHeight = u8g2.getDisplayHeight();

        title_screen();
        sensor.init();
        init_ball();

        unsigned long previousMillis = 0;
        while (true)
        {
            unsigned long currentMillis = millis();
            if ((unsigned long)(currentMillis - previousMillis) > PAGE_DELAY)
            {
                previousMillis = currentMillis;

                u8g2.firstPage();

                val = (1 - ALPHA) * val + ALPHA * -1 * sensor.yAcc();
                xBat = int(map_float(constrain(val, MIN_INPUT, MAX_INPUT), MIN_INPUT, MAX_INPUT, 0, dpWidth - BAT_WIDTH));

                do
                {
                    u8g2.setCursor(0, 15);
                    u8g2.print(score);

                    u8g2.drawDisc(ballx, bally, BALL_RAD);

                    u8g2.drawBox(xBat, dpHeight - BAT_HEIGHT, BAT_WIDTH, BAT_HEIGHT);
                } while (u8g2.nextPage());

                ballx += vx;
                bally += vy;
                bounce();
            }
        }
    }
};
