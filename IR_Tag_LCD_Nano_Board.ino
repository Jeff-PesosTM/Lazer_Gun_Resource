//https://hackaday.io/project/160804-lzrtag-flexible-diy-lasertag

#include <IRremote.h>

IRsend irsend;

const int ledPin = 6;  // pins numbers
const int RECV_PIN = 2;
const int buzzerPin = 5;
const int triggpin = 13;
const int amoresetpin = 12;

IRrecv irrecv(RECV_PIN);

int triggbuttonState = 0;  // = LOW;
int amobuttonState = 0;    // = LOW;
int damagelevel = 10;
int maxamo = 1000;  // maximum amount of ammo the player can have
int amolevel = maxamo;
int timer = 10;
int reloading = 30;       //  ~ 1s / 10 increments
int reload_time = 30;

decode_results results;

void setup() {

  pinMode(triggpin, INPUT);     // to shoot, minus amo
  pinMode(amoresetpin, INPUT);  // resets amo
  pinMode(buzzerPin, OUTPUT);   // output for shooting, gettingshot and game over
  pinMode(ledPin, OUTPUT);      //output for shooting with LED muzzle
  pinMode(RECV_PIN, INPUT);     //getting shot, taking damage

  Serial.begin(9600);
  irrecv.enableIRIn();  // Start the receiver
}

void loop() { // constantly executes all the functions in this loop
  triggbuttonState = digitalRead(triggpin);
  amobuttonState = digitalRead(amoresetpin);
  //debug(); uncomment if you need to see the debug messages in the serial monitor
  reload(); // checks for reload
  shoot(); // checks for shooting
  gotShot(); // checks for if you got shot
  timer++;
}

void reload() {
  reloading++;
  if (amolevel < maxamo && damagelevel > 0) {
    if (amobuttonState == HIGH) {
      reloading = 0;
      amolevel = maxamo;
    }
  }
}

void debug() {
  delay(100);
  Serial.print(timer);
  Serial.print(" amobuttonState ");
  Serial.print(amobuttonState);
  Serial.print(" amocount ");
  Serial.print(amolevel);
  Serial.print(" damage level ");
  Serial.println(damagelevel);
  Serial.print(irrecv.decode());
}

void shoot() {
  //if you are pressing the trigger, have ammo, arent dead and are not mid reload
  if ((triggbuttonState == HIGH) && (amolevel > 0) && (damagelevel > 0) && (reloading > reload_time)) {
    timer = 11;
    amolevel--;
    IRsend irsend;
    irsend.sendSony(0xa90, 20);
    delay(1);

    for (int i = 500; i < 600; i++) {  // buzzer noise
      digitalWrite(buzzerPin, HIGH);
      delayMicroseconds(i);
      digitalWrite(buzzerPin, LOW);
      delayMicroseconds(i);
    }

    for (int i = 500; i < 600; i++) {  // turns the led on
      digitalWrite(ledPin, HIGH);
      delayMicroseconds(i);
      digitalWrite(ledPin, LOW);
      delayMicroseconds(i);
    }

    irrecv.enableIRIn();
    delay(100);
    Serial.println("triggbuttonState HIGH");
  }
}

void gotShot() {
  if (irrecv.decode(&results)) {
    //Serial.println(results.value, HEX);
    damagelevel = damagelevel-1;
    if (damagelevel < 1) {
      damagelevel = 0; 
      amolevel = 0;
    }
    
    for(int i = 200; i < 400; i++){ // got shot sound
      digitalWrite(buzzerPin, HIGH);
      delayMicroseconds(i);
      digitalWrite(buzzerPin, LOW);
      delayMicroseconds(i);
    }
    irrecv.resume();
  }
}
