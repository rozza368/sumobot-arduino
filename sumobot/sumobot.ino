int leftBack = 2;
int leftFwd = 3;
int rightBack = 5;
int rightFwd = 4;
int shieldRight = 6;
int shieldLeft = 7;
int shieldLstate = 0;
int shieldRstate = 0;

int started = 0;

int motors[4] = {leftBack, leftFwd, rightBack, rightFwd};
int shields[2] = {shieldRight, shieldLeft};

int ldrPin = A0;
int ldrVal;
int ldrThreshold = 300;
int reverseDuration = 800; // ms

int getLdr() {
  ldrVal = analogRead(ldrPin);
  Serial.print("LDR: ");
  Serial.println(ldrVal);
  return ldrVal;
}

void stopped() {
  // all motors off
  for (int i = 0; i < 4; i++) {
    digitalWrite(motors[i], LOW);
  }
}

void forward() {
  digitalWrite(leftFwd, HIGH);
  digitalWrite(rightFwd, HIGH);
  digitalWrite(leftBack, LOW);
  digitalWrite(rightBack, LOW);
}

void backward() {
  digitalWrite(leftFwd, LOW);
  digitalWrite(rightFwd, LOW);
  digitalWrite(leftBack, HIGH);
  digitalWrite(rightBack, HIGH);
}

void pivot() {
  digitalWrite(leftFwd, HIGH);
  digitalWrite(rightFwd, LOW);
  digitalWrite(leftBack, LOW);
  digitalWrite(rightBack, HIGH);
  // digitalWrite(rightBack, LOW);
}

void getShields() {
  shieldLstate = digitalRead(shieldLeft);
  shieldRstate = digitalRead(shieldRight);
  //Serial.println(shieldLstate);
  //Serial.println(shieldRstate);
}

void checkHit() {
  getShields();
  if (!shieldLstate || !shieldRstate) // shield hit
  {
    Serial.println("Shield hit, immobilised");
    pivot();
    delay(1000);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // set up all pins
  pinMode(leftBack, OUTPUT);
  pinMode(leftFwd, OUTPUT);
  pinMode(rightBack, OUTPUT);
  pinMode(rightFwd, OUTPUT);
  pinMode(shieldRight, INPUT);
  pinMode(shieldLeft, INPUT);

  stopped();

  // wait for both shields to be pressed
  Serial.println("Waiting to start...");
  do {
    getShields();
  } while (shieldLstate && shieldRstate);
  forward();
  delay(500); // ensure shield is not detected as hit
}

void loop() {
  // put your main code here, to run repeatedly:

  if (getLdr() > ldrThreshold) // not over black line
  {
    // normal operation
    forward();
    checkHit();
  }

  else  // black detected
  {
    Serial.println("Detected black");
    while (getLdr() < ldrThreshold) {
      backward();
      delay(reverseDuration);
      if (getLdr() < ldrThreshold) // still in black
      {
        // might be going backwards into black, get out by moving forward
        forward();
        delay(500);
      }
      else
      {
        pivot();
        // rotates about 90 degrees
        for (int i = 0; i < 30; i++) {
          checkHit(); // ensure shields can still be hit
          delay(10);
        }
        
        break;
      }
    }
    forward();
  }
}
