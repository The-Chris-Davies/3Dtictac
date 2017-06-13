/* Simple Serial ECHO script : Written by ScottC 03/07/2012 */

/* Use a variable called byteRead to temporarily store
   the data coming from the computer */
byte byteRead, byteRead2;

void setup() {                
// Turn the Serial Protocol ON
  Serial.begin(9600);
}

void loop() {
   /*  check if data has been sent from the computer: */
  while (!Serial.available());
    /* read the most recent byte */
  byteRead = Serial.read();
  while (!Serial.available());
  byteRead2 = Serial.read();
  /*ECHO the values that were read, back to the serial port. */
  Serial.println(byteRead, DEC);
  Serial.println(byteRead2, DEC);
  Serial.println(44-byteRead, DEC);
  Serial.println(byteRead2-120, DEC);
}
