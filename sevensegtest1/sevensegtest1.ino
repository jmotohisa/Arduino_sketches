// 7seg test

void setup() {
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(A5,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A3,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(11,OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(A5,HIGH);
  digitalWrite(A4,LOW);
  digitalWrite(A3,LOW);
  digitalWrite(A2,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A0,LOW);
  digitalWrite(9,LOW);
  digitalWrite(11,LOW);
 for(int i=2;i<9;i++)
 {
    digitalWrite(i,HIGH);
    delay(600);
 }
  digitalWrite(A5,LOW);
  digitalWrite(A4,HIGH);
  delay(600);
  digitalWrite(A4,LOW);
  digitalWrite(A3,HIGH);
  delay(600);
  digitalWrite(A3,LOW);
  digitalWrite(A2,HIGH);
  delay(600);
  digitalWrite(A2,LOW);
  digitalWrite(A1,HIGH);
  delay(600);
  digitalWrite(A1,LOW);
  digitalWrite(A0,HIGH);
  delay(600);
  digitalWrite(A0,LOW);
  digitalWrite(9,HIGH);
  delay(600);
  digitalWrite(9,LOW);
  digitalWrite(11,HIGH);
  delay(600);
 for(int i=2;i<9;i++)
 {
    digitalWrite(i,LOW);
    delay(600);
 }
 digitalWrite(11,LOW);

// delay(1000);
}
