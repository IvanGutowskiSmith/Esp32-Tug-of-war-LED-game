#include <Adafruit_NeoPixel.h>

#define PIN_WS2812B 0             // GPIO0 connected to WS2812B
#define NUM_PIXELS 53             // Number of LEDs (pixels)
#define BUTTON_PIN_PLAYER1 20     // Button pin for player 1
#define BUTTON_PIN_PLAYER2 21     // Button pin for player 2
#define DEBOUNCE_DELAY 50         // Debounce delay in milliseconds
#define WIN_ANIMATION_DELAY 2000  // Delay after winning animation in milliseconds

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

int Player1_Btn_State = HIGH;
int Player1_previous_Btn_State = HIGH;

int Player2_Btn_State = HIGH;
int Player2_previous_Btn_State = HIGH;

int ScorePixel_Position = NUM_PIXELS / 2;     // Initial position of the illuminated LED

void setup() {
  ws2812b.begin();                // Initialize WS2812B strip object
  ws2812b.clear();                // Clear all pixel colors
  pinMode(BUTTON_PIN_PLAYER1, INPUT_PULLUP);  // Set player 1 button pin as input with internal pull-up resistor
  pinMode(BUTTON_PIN_PLAYER2, INPUT_PULLUP);  // Set player 2 button pin as input with internal pull-up resistor
  iluminateLed(ScorePixel_Position); // Set initial LED state
}

void loop() {
  // Read button states with debouncing
  debounceButtons();

  // Handle Player 1 button press
  if (Player1_Btn_State == LOW && Player1_previous_Btn_State == HIGH) {
    moveScore(1); // Move LED towards Player 2's side
    checkWinCondition();
  }

  // Handle Player 2 button press
  if (Player2_Btn_State == LOW && Player2_previous_Btn_State == HIGH) {
    moveScore(-1); // Move LED towards Player 1's side
    checkWinCondition();
  }

  // Update button state history
  Player1_previous_Btn_State = Player1_Btn_State;
  Player2_previous_Btn_State = Player2_Btn_State;
}

void debounceButtons() {
  static unsigned long lastDebounceTime = 0;

  // Debounce Player 1 button
  if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
    int reading = digitalRead(BUTTON_PIN_PLAYER1);

    if (reading != Player1_Btn_State) {
      Player1_Btn_State = reading;
      lastDebounceTime = millis();
    }
  }

  // Debounce Player 2 button
  if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
    int reading = digitalRead(BUTTON_PIN_PLAYER2);

    if (reading != Player2_Btn_State) {
      Player2_Btn_State = reading;
      lastDebounceTime = millis();
    }
  }
}

void moveScore(int direction) {
  ScorePixel_Position += direction;
  if (ScorePixel_Position >= NUM_PIXELS) {
    winAnimation(1); // Player 1 wins
  }
  if (ScorePixel_Position < 0) {
    winAnimation(2); // Player 2 wins
  }
  if (ScorePixel_Position >= NUM_PIXELS) {
    ScorePixel_Position = NUM_PIXELS - 1;
  }
  if (ScorePixel_Position < 0) {
    ScorePixel_Position = 0;
  }
  iluminateLed(ScorePixel_Position);
}

void iluminateLed(int targetPixel) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    if (i != targetPixel) {
      // Set all pixels except target to dim white to ensure the powerbank draws enough power, to avoid it turning off
      ws2812b.setPixelColor(i, ws2812b.Color(30, 30, 30));
    }
  }
  // Set target LED color to red at full brightness
  ws2812b.setPixelColor(targetPixel, ws2812b.Color(255, 0, 0));
  ws2812b.show();                         // Update LED display
}

void winAnimation(int player) {
  // Smooth animation for winning player
  for (int i = 0; i < NUM_PIXELS; i++) {
    ws2812b.setPixelColor(i, ws2812b.Color(0, 255, 0)); // Flash green
    ws2812b.show();
    delay(50);
    ws2812b.setPixelColor(i, ws2812b.Color(0, 0, 0)); // Turn off
    ws2812b.show();
  }
  delay(WIN_ANIMATION_DELAY);
  
  // Reset game state after delay
  ScorePixel_Position = NUM_PIXELS / 2;
  iluminateLed(ScorePixel_Position);
}

void checkWinCondition() {
  if (ScorePixel_Position >= NUM_PIXELS) {
    winAnimation(1); // Player 1 wins
  }
  if (ScorePixel_Position < 0) {
    winAnimation(2); // Player 2 wins
  }
}


