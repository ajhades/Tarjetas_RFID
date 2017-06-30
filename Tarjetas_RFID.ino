/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>

#include <Keyboard.h>
#include <Mouse.h>


#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

#define ledPinSubj  1
#define ledPinVerb  2
#define ledPinComp  3

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

int count = 1;
int cardsReads[3] = {0,0,0};
int stepcards; // Contador de secuencia
// const char *to_find = "96 A2 8E 8D 00 00 00 00 00 00 08 0A";
  const char *subject[] = { // strings to search through
    "86 16 8C 8D 00 00 00 00 00 00 08 0A", // Hen 1 - llavero
    "96 A2 8E 8D 00 00 00 00 00 00 08 0A", // Cow 2 - llavero
    "50 60 EB 87 00 00 00 00 00 00 08 0A", // Pig 3 - llavero
  };
    const char *verb[] = { // strings to search through
      "D4 0F E5 D5 00 00 00 00 00 00 08 0A", // Eat 1
      "DC AA 8C 65 00 00 00 00 00 00 08 0A", // Play 2
      "4E 29 5D 51 00 00 00 00 00 00 08 0A", // Sleep (7)
    };
    const char *complement[] = { // strings to search through
      "8E D4 59 51 00 00 00 00 00 00 08 0A", // Corn 4
      "4E 29 5D 51 00 00 00 00 00 00 08 0A", // Sand 5
      "DE FD 7F 51 00 00 00 00 00 00 08 0A", // Gallinero 6
      "B0 00 DC 87 00 00 00 00 00 00 08 0A", // Pasto 4 - llavero
      "1E D5 59 51 00 00 00 00 00 00 08 0A", // Ball 3
      "B0 00 DC 87 00 00 00 00 00 00 08 0A", // Establo (4)
      //"1E D5 59 51 00 00 00 00 00 00 08 0A", // Food
      //"1E D5 59 51 00 00 00 00 00 00 08 0A", // Lodo
      //"1E D5 59 51 00 00 00 00 00 00 08 0A", // Garaje
    };
    int len_sub = (sizeof (subject) / sizeof (*subject)); // Cantidad de sujetos
    int len_verb = (sizeof (verb) / sizeof (*verb)); // Cantidad de verbos
    int len_comp = (sizeof (complement) / sizeof (*complement)); // Cantidad de complementos
    int x; // generic loop counter

    void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  pinMode(ledPinVerb  , OUTPUT);   
  pinMode(ledPinComp, OUTPUT);  
  pinMode(ledPinSubj, OUTPUT);  
  //cardsReads[1] =  4294937741;
  //ncardsReads[2] =4294938253;
  stepcards=0;
  

  //digitalWrite(ledPinComp, true);
  //digitalWrite(ledPinVerb, true);
  //digitalWrite(ledPinSubj, true);

  Keyboard.begin();
}
/**
 * Obtiene el valor UID Hexadecimal de la tarjeta RFID
 * @return hex_num [String]
 */
String getHEXA(){
  String hex_num;
  char charBuf[50];

  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return ;
  }
  for (byte i = 0; i < sizeof(mfrc522.uid); i++) {
    if(mfrc522.uid.uidByte[i] < 0x10){
      String stringHexa = String(mfrc522.uid.uidByte[i], HEX);
      hex_num = hex_num + " 0";
      hex_num = hex_num + stringHexa;
    }else{
      hex_num = hex_num + " ";
      String stringHexa = String(mfrc522.uid.uidByte[i], HEX);
      hex_num = hex_num + stringHexa;
    }
  } 
  hex_num.toCharArray(charBuf, 200); 
  return hex_num;
}
/**
 * Busca si la tarjeta actual esta en el arreglo de Subjects
 * @param  to_find  [UID hexadecimal a buscar]
 * @return is_there [devuelve 1 si encuentra el UID]
 */
int finder_sub(String to_find){
  to_find.trim();
  int is_there = 0;
  Serial.println(to_find+ " buscar");
  for (x = 0; x < len_sub; x++) {
    if (to_find.equalsIgnoreCase(subject[x])){
      Serial.println(to_find+ " encontrado");
      is_there=1;
      cardsReads[0] = x;
    }
  }
  return is_there;
}
/**
 * Busca si la tarjeta actual esta en el arreglo de Verbs
 * @param  to_find  [UID hexadecimal a buscar]
 * @return is_there [devuelve 1 si encuentra el UID]
 */
int finder_verb(String to_find){
  to_find.trim();
  Serial.println(to_find+ " buscar");

  int is_there = 0;
  for (x = 0; x < len_verb; x++) {
    if (to_find.equalsIgnoreCase(verb[x])){
      Serial.println(to_find+ " encontrado");
      is_there=1;
      cardsReads[1] = x;
    }
  }
  return is_there;
}
/**
 * Busca si la tarjeta actual esta en el arreglo de Complements
 * @param  to_find  [UID hexadecimal a buscar]
 * @return is_there [devuelve 1 si encuentra el UID]
 */
int finder_comp(String to_find){
  to_find.trim();
  Serial.println(to_find+ " buscar");

  int is_there = 0;
  for (x = 0; x < len_comp; x++) {
    if (to_find.equalsIgnoreCase(complement[x])){
      Serial.println(to_find+ " encontrado");
      is_there=1;
      cardsReads[2] = x;
    }
  }
  return is_there;
}
void loop() {
  delay(2000);
  
  //mfrc522.PCD_DumpVersionToSerial();
  // Look for new cards
  /*if ( ! mfrc522.PICC_IsNewCardPresent()) {
  //Serial.println("nueva card");
    return;
  }*/
  if(mfrc522.PICC_IsNewCardPresent()) {
    String uid = getHEXA();
    if(uid != -1){

      if(stepcards == 0){
        stepcards++;
      }
      else if(stepcards > 3){
        stepcards = 0;
      }
      // Apagar LEDs para volver a empezar
      if(stepcards == 0){
        digitalWrite(ledPinComp, false);
        digitalWrite(ledPinVerb, false);
        digitalWrite(ledPinSubj, false);
      }else if(stepcards == 1){ // Paso 1: Sujeto
        int as = finder_sub(uid);
        if(as == 1){
          Serial.print("Card detected subject, UID: "); Serial.println(uid);
          digitalWrite(ledPinComp, true);
          stepcards++;
        }else{
          Serial.println("Card not detected");
        }
        
        Serial.println("1");
      }else if(stepcards == 2){ // Paso 2: Verbo
        int as = finder_verb(uid);
        if(as == 1){
          Serial.print("Card detected verb, UID: "); Serial.println(uid);
          digitalWrite(ledPinVerb, true);
          stepcards++;
        }else{
          Serial.println("Card not detected");
        }
        
        Serial.println("2");
      }else if(stepcards == 3){ // Paso 3: Complemento
        int as = finder_comp(uid);
        if(as == 1){
          Serial.print("Card detected comp, UID: "); Serial.println(uid);
          digitalWrite(ledPinSubj, true);
          stepcards++;
        }else{
          Serial.println("Card not detected");
        }
        Serial.println("3");
        if (cardsReads[0] == 0)
        {
          // Hen
          if (cardsReads[1] == 0)
          {
            // Eat
            if (cardsReads[2] == 0)
            {
              // Corn
              Keyboard.press('q');
              delay(100);
              Keyboard.releaseAll();
            }else{
              Keyboard.press('r');
              delay(100);
              Keyboard.releaseAll();
            }
          }else if (cardsReads[1] == 1)
          {
            // Play
            if (cardsReads[2] == 1)
            {
              //Sand
              Keyboard.press('w');
              delay(100);
              Keyboard.releaseAll();
            }else{
              Keyboard.press('r');
              delay(100);
              Keyboard.releaseAll();
            }
          }else if (cardsReads[1] == 2)
          {
            // Sleep
            if (cardsReads[2] == 2)
            {
              //Gallinero
              Keyboard.press('e');
              delay(100);
              Keyboard.releaseAll();
            }else{
              Keyboard.press('r');
              delay(100);
              Keyboard.releaseAll();
            }
          }
        }else if(cardsReads[0] == 1) // Cow
        {
          // Cow
          if (cardsReads[1] == 0) // Eat
          {
            if (cardsReads[2] == 3) // Pasto
            {
              Keyboard.press('t');
              delay(100);
              Keyboard.releaseAll();
            }else{
              Keyboard.press('i');
              delay(100);
              Keyboard.releaseAll();
            }
          }else if(cardsReads[1] == 1) // Play 
          {
            if (cardsReads[2] == 4)
            {
              Keyboard.press('y');
              delay(100);
              Keyboard.releaseAll();
            } else{
              Keyboard.press('i');
              delay(100);
              Keyboard.releaseAll();
            }
          } else if(cardsReads[1] == 2) // Sleep 
          {
            if (cardsReads[2] == 5)
            {
              Keyboard.press('u');
              delay(100);
              Keyboard.releaseAll();
            }else{
              Keyboard.press('i');
              delay(100);
              Keyboard.releaseAll();
            }
          }
        } else if(cardsReads[0] == 2) // Pig
        {
          if (cardsReads[1] == 0) // Eat
          {
            if (cardsReads[2] == 6) // Food
            {
              Keyboard.press('p');
              delay(100);
              Keyboard.releaseAll();
            }else{
              Keyboard.press('d');
              delay(100);
              Keyboard.releaseAll();
            }
          }else if (cardsReads[1] == 1) // Play
          {
            if (cardsReads[2] == 7) // Lodo
            {
              Keyboard.press('a');
              delay(100);
              Keyboard.releaseAll();
            }else{
              Keyboard.press('d');
              delay(100);
              Keyboard.releaseAll();
            }
          }else if (cardsReads[1] == 1) // Sleep
          {
            if (cardsReads[2] == 8) // Garage
            {
              Keyboard.press('s');
              delay(100);
              Keyboard.releaseAll();
            }else{
              Keyboard.press('d');
              delay(100);
              Keyboard.releaseAll();
            }
          }
        }
      }
      //Serial.println("arreglo "+sizeof(cardsReads));
      Serial.println(cardsReads[0]);Serial.println(cardsReads[1]);Serial.println(cardsReads[2]);
    }
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
  //Serial.println("PICC_ReadCardSerial");
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}
