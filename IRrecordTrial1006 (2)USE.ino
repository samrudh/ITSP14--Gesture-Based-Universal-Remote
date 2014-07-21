/*
 * IRrecord: record and play back IR signals as a minimal 
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * An IR LED must be connected to the output PWM pin 3.
 * A button must be connected to the input BUTTON_PIN; this is the
 * send button.
 * A visible LED can be connected to STATUS_PIN to provide status.
 *
 * The logic is:
 * If the button is pressed, send the IR code.
 * If an IR code is received, record it.
 *
 * I have changed the logic
 * I will initially take the code in the array as raw and assign 
 * buttons to them.And then afterwards when those buttons are pressed
 * the same code will be sent back in void loop.AVP
 *
 * Version 0.11 September, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 11;
int BUTTON_PIN = 12;
int STATUS_PIN = 13;

IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results[6];//to store the info of the result here 6 is merely a number

int k=0;
int & kref=k;//this variable is used as reference to change it as i want in the void loop


void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(BUTTON_PIN, INPUT);
  pinMode(STATUS_PIN, OUTPUT);
}

// Storage for the recorded code
int codeType[6] = {-1}; // The type of code
unsigned long codeValue[6]; // The code value if not raw
unsigned int rawCodes[6][RAWBUF]; // The durations if raw
int codeLen[6]; // The length of the code
int toggle[6] = {0}; // The RC5/6 toggle state

// Stores the code for later playback
// Most of this code is just logging
//initially they had used those -> pointers but with array i doonnoo why it dint seem to work so i changed it to . ope. 


//i think i dont need to give the [k] for the results since m already passing the address of the k th result but it gibes error


void storeCode(decode_results *results) {
  codeType[k] = results[k].decode_type;
  int count = results[k].rawlen;
  if (codeType[k] == UNKNOWN) {
    Serial.println("Received unknown code, saving as raw");
    codeLen[k] = results[k].rawlen - 1;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
    for (int i = 1; i <= codeLen[k]; i++) {
      if (i % 2) {
        // Mark
        Serial.print(" m");
      } 
      else {
        // Space
       ////////////////////////////// note//////////i donno what exactly this function does                  rawCodes[k][i - 1] = results[k]->rawbuf[k][i]*USECPERTICK + MARK_EXCESS;
        Serial.print(" s");
      }
      Serial.print(rawCodes[k][i - 1], DEC);
    }
    Serial.println("");
  }
  else {
    if (codeType[k] == NEC) {
      Serial.print("Received NEC: ");
      if (results[k].value == REPEAT) {
        // Don't record a NEC repeat value as that's useless.
        Serial.println("repeat; ignoring.");
        return;
      }
    } 
    else if (codeType[k] == SONY) {
      Serial.print("Received SONY: ");
    } 
    else if (codeType[k] == RC5) {
      Serial.print("Received RC5: ");
    } 
    else if (codeType[k] == RC6) {
      Serial.print("Received RC6: ");
    } 
    else {
      Serial.print("Unexpected codeType ");
      Serial.print(codeType[k], DEC);
      Serial.println("");
    }
    Serial.println(results[k].value, HEX);
    codeValue[k] = results[k].value;
    codeLen[k] = results[k].bits;
  }
}

void sendCode(int repeat) {//repeat is only needed in NEC
  if (codeType[k] == NEC) {
    if (repeat) {
      irsend.sendNEC(REPEAT, codeLen[k]);
      Serial.println("Sent NEC repeat");
    } 
    else {
      irsend.sendNEC(codeValue[k], codeLen[k]);
      Serial.print("Sent NEC ");
      Serial.println(codeValue[k], HEX);
    }
  } 
  else if (codeType[k] == SONY) {
    irsend.sendSony(codeValue[k], codeLen[k]);
    Serial.print("Sent Sony ");
    Serial.println(codeValue[k], HEX);
  } 
  else if (codeType[k] == RC5 || codeType[k] == RC6) {
    if (!repeat) {
      // Flip the toggle bit for a new button press
      toggle[k] = 1 - toggle[k];
    }
    // Put the toggle bit into the code to send
    codeValue[k] = codeValue[k] & ~(1 << (codeLen[k] - 1));
   //////////////////////////////////////////////////////////////////note//////////////////// codeValue[k] = codeValue[k] | (toggle << (codeLen[k] - 1));
    if (codeType[k] == RC5) {
      Serial.print("Sent RC5 ");
      Serial.println(codeValue[k], HEX);
      irsend.sendRC5(codeValue[k], codeLen[k]);
    } 
    else {
      irsend.sendRC6(codeValue[k], codeLen[k]);
      Serial.print("Sent RC6 ");
      Serial.println(codeValue[k], HEX);
    }
  } 
  else if (codeType[k] == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    //rawCodesTmp=
    
    ////////////////////////////////////////////////////////////////////////////vv imp function finally//////////////////////////////////////////////////////////////////
    
    int codeLenTmp;
    codeLenTmp==codeLen[k];
    unsigned int rawCodesTmp[RAWBUF];
    rawCodesTmp[RAWBUF]=rawCodes[k][RAWBUF];    
    irsend.sendRaw(rawCodesTmp, codeLenTmp, 38);
    Serial.println("Sent raw");
  }
}
    // char firstButton,secondButton,thirdButton,fourthButton; //thought of something which m nt implementing right now
    char incomingByte;
    
     //Serial.read()=incomingByte;
     
     

int lastButtonState;

void loop() {
  
  
                                                    /////////////////////////receiving code ////////////////////////////
                                                    
                                                    
                     ///i wanted this out of the void loop but its giving some uncanny error :: IRrecord:185: error: expected unqualified-id before 'if'
                     //try to get it out of the loop
                    if(Serial.read()=='w')
                      {storeCode(&results[0]);//it will store in results ka array
                       Serial.println("Accepted as W ");//tp
                       irrecv.enableIRIn(); // Re-enable receiver
                      }
                       if(Serial.read()=='s')
                      {storeCode(&results[1]);
                       Serial.println("Accepted as S ");
                       irrecv.enableIRIn(); // Re-enable receiver
                      }
                       if(Serial.read()=='d')
                      {storeCode(&results[2]);
                       Serial.println("Accepted as D ");
                       irrecv.enableIRIn(); // Re-enable receiver
                      }
                       if(Serial.read()=='a')
                      {storeCode(&results[3]);
                       Serial.println("Accepted as A ");
                       irrecv.enableIRIn(); // Re-enable receiver
                      }
   //If button pressed, send the code.
   int buttonState = digitalRead(BUTTON_PIN);
  //if (lastButtonState == HIGH && buttonState == LOW) {
    Serial.println("Reached void loop");
    //irrecv.enableIRIn(); // Re-enable receiver
    
                                                              ///////////////////////////////sending code /////////////////
                      if (Serial.read()=='w')
                      {
                         Serial.println("Pressed W, sending");
                         digitalWrite(STATUS_PIN, HIGH);
                         ////////////////////////////////what to send in repeat?? ///////////////////////////////////////
                         sendCode(lastButtonState == buttonState);
                         kref=0;
                         digitalWrite(STATUS_PIN, LOW);
                         delay(50); // Wait a bit between retransmissions
                       }
                        else if (Serial.read()=='s')
                      {
                         Serial.println("Pressed s, sending");
                         digitalWrite(STATUS_PIN, HIGH);
                         ////////////////////////////////what to send in repeat?? ///////////////////////////////////////
                         sendCode(lastButtonState == buttonState);
                         kref=1;
                         digitalWrite(STATUS_PIN, LOW);
                         delay(50); // Wait a bit between retransmissions
                       }
                         else if (Serial.read()=='d')
                      {
                         Serial.println("Pressed d, sending");
                         digitalWrite(STATUS_PIN, HIGH);
                         ////////////////////////////////what to send in repeat?? ///////////////////////////////////////
                         sendCode(lastButtonState == buttonState);
                         kref=2;
                         digitalWrite(STATUS_PIN, LOW);
                         delay(50); // Wait a bit between retransmissions
                       }
                        else if (Serial.read()=='a')
                      {
                         Serial.println("Pressed a, sending");
                         digitalWrite(STATUS_PIN, HIGH);
                         ////////////////////////////////what to send in repeat?? ///////////////////////////////////////
                         sendCode(lastButtonState == buttonState);
                         kref=3;
                         digitalWrite(STATUS_PIN, LOW);
                         delay(50); // Wait a bit between retransmissions
                       }
                       else
                       {
                         Serial.println("NOT A VALID INPUT;PLEASE TRY AGAIN");
                       }
    
            
 /* else if (irrecv.decode(&results[k])) {
    digitalWrite(STATUS_PIN, HIGH);
    storeCode(&results[k]);
    irrecv.resume(); // resume receiver
    digitalWrite(STATUS_PIN, LOW);
  }*/
  lastButtonState = buttonState;
}
