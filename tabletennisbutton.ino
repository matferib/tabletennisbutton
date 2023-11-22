// constants won't change. They're used here to set pin numbers:
constexpr int buttonOnePin = 10;//13;
constexpr int buttonTwoPin = 12;
constexpr int pausePin = 8;
constexpr int relayOnePin = 2;
constexpr int relayTwoPin = 3;
constexpr bool debug = true;
constexpr unsigned long kPauseButtontoleranceMs = 300;
constexpr unsigned long kBlinkIntervalMs = 300;
constexpr unsigned long kDelayMs = 30000;

// variables will change:
unsigned long counter_one_ms = 0;
unsigned long counter_two_ms = 0;
unsigned long counter_pause_ms = 0;
unsigned long left_pause_ms = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayOnePin, OUTPUT);
  pinMode(relayTwoPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonOnePin, INPUT);
  pinMode(buttonTwoPin, INPUT);
  if (debug) {
    Serial.begin(9600);
  }
}

bool ReadButton(const int buttonPin) {
  const int buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    return true;
  } else {
    // turn LED off:
    return false;
  }
}

void ControlRelay(const int relayPin, const unsigned long now_ms, unsigned long& counter_ms) {
  const unsigned long future_ms = counter_ms + kDelayMs;
  if (debug && (now_ms % 1000) == 0) { 
    Serial.print(" f:");
    Serial.println(future_ms);
  }
  if (counter_ms > 0 && future_ms >= now_ms) {
    digitalWrite(relayPin, LOW);
  } else {
    counter_ms = 0;
    digitalWrite(relayPin, HIGH);
  }
}

bool PlayerButton(const int buttonPin, const unsigned long now_ms, unsigned long& counter_ms) {
  bool button_state = false;
  if (ReadButton(buttonPin)) {
    button_state = true;
    if (counter_ms == 0) { counter_ms = now_ms; }
  }
  return button_state;
}

void HandlePause(unsigned long now_ms) {
  if (counter_pause_ms == 0) {
    if ((counter_one_ms > 0 || counter_two_ms > 0) && (now_ms >= left_pause_ms + kPauseButtontoleranceMs) && ReadButton(pausePin)) {
      counter_pause_ms = now_ms;
      Serial.println("entering pause");
    }
  } else {
    if (now_ms >= (counter_pause_ms + kPauseButtontoleranceMs) && ReadButton(pausePin)) {
      unsigned long delta_ms = now_ms - counter_pause_ms;
      counter_pause_ms = 0;
      left_pause_ms = now_ms;
      if (counter_one_ms > 0) { counter_one_ms += delta_ms; }
      if (counter_two_ms > 0) { counter_two_ms += delta_ms; }
      Serial.println("leaving pause");
    }
  }
}

// Blink the lights that are active.
void BlinkLights(unsigned long now_ms) {
  int state = ((((now_ms - counter_pause_ms) / kBlinkIntervalMs) % 2) == 0) ? HIGH : LOW;
  if (counter_one_ms > 0) {
    digitalWrite(relayOnePin, state);
  }
  if (counter_two_ms > 0) {
    digitalWrite(relayTwoPin, state);
  }
}

void loop() {
  const unsigned long now_ms = millis();

  HandlePause(now_ms);
  if (debug && (now_ms % 1000) == 0) {
    Serial.print("now:"); Serial.print(now_ms);
    Serial.print(" -- one:"); Serial.print(counter_one_ms);
    Serial.print(" -- two:"); Serial.println(counter_two_ms);
    if (counter_pause_ms > 0) { Serial.println(" -- paused --"); }
  }

  if (counter_pause_ms > 0) {
    BlinkLights(now_ms);
    return;
  }

  const bool b1_pressed = PlayerButton(buttonOnePin, now_ms, counter_one_ms);
  ControlRelay(relayOnePin, now_ms, counter_one_ms);
  const bool b2_pressed = PlayerButton(buttonTwoPin, now_ms, counter_two_ms);
  ControlRelay(relayTwoPin, now_ms, counter_two_ms);
  // Arduino led: indicates button pushed.
  digitalWrite(LED_BUILTIN, (b1_pressed || b2_pressed) ? HIGH : LOW);
}
