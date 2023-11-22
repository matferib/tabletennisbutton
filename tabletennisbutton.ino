// constants won't change. They're used here to set pin numbers:
constexpr int buttonOnePin = 10;//13;
constexpr int buttonTwoPin = 12;
constexpr int ledPin = LED_BUILTIN;    // the number of the LED pin
constexpr int relayOnePin = 2;
constexpr int relayTwoPin = 3;

constexpr int kDelay = 30;

constexpr bool debug = true;

// variables will change:
int buttonOneState = 0;
int buttonTwoState = 0;

int counterOne = 0;
int counterTwo = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
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

void ControlRelay(const int relayPin, const int now, int& counter) {
  const int future = counter + kDelay;
  if (debug && (millis() % 1000) == 0) { 
    Serial.print(" f:"); Serial.println(future);
  }
  if (counter > 0 && future >= now) {
    digitalWrite(relayPin, LOW);
  } else {
    counter = 0;
    digitalWrite(relayPin, HIGH);
  }
}

bool PlayerButton(const int buttonPin, const int relayPin, const int now, int& counter) {
  bool button_state = false;
  if (ReadButton(buttonPin)) {
    button_state = true;
    if (counter <= 0) { counter = now; }
  }
  ControlRelay(relayPin, now, counter);
  return button_state;
}

void loop() {
  const int now = millis() / 1000;
  if (debug && (millis() % 1000) == 0) {
    Serial.print("now:"); Serial.print(now);
    Serial.print(" -- one:"); Serial.print(counterOne);
    Serial.print(" -- two:"); Serial.println(counterTwo);
  }

  bool b1 = PlayerButton(buttonOnePin, relayOnePin, now, counterOne);
  bool b2 = PlayerButton(buttonTwoPin, relayTwoPin, now, counterTwo);
  digitalWrite(ledPin, (b1 || b2) ? HIGH : LOW);
}
