typedef unsigned char byte;
const int kNumOfSevenSegments = 6;

#define OE_PIN A1 // output enable
#define LE_PIN A2 // latch enable


void setLED(byte val)
{
  digitalWrite(LE_PIN, LOW);
  SPI.transfer(0); // send command byte
  SPI.transfer(val); // send value (0~255)
  digitalWrite(LE_PIN, HIGH);
}



void setup()
{
    pinMode(OE_PIN, OUTPUT);
    pinMode(LE_PIN, OUTPUT);
    digitalWrite(OE_PIN, LOW);
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    Spark.subscribe("numPhotonsSold", numPhotonsSoldHandler);
    Serial.begin(9600);
    setLED(0);
}





byte digit2SevenSeg(char c)
{
    
    byte digit2SevenSegArr[17];
    
    // if it's a space then don't show any digit
    if (c == ' ')
        return 0;

        // COMMON ANODE   HGFEDCBA
    digit2SevenSegArr[0] = 0b00111111; // 0
    digit2SevenSegArr[1] = 0b00000110; // 1
    digit2SevenSegArr[2] = 0b01011011; // 2
    digit2SevenSegArr[3] = 0b01001111; // 3
    digit2SevenSegArr[4] = 0b01100110; // 4
    digit2SevenSegArr[5] = 0b01101101; // 5
    digit2SevenSegArr[6] = 0b01111101; // 6
    digit2SevenSegArr[7] = 0b00000111; // 7
    digit2SevenSegArr[8] = 0b01111111; // 8
    digit2SevenSegArr[9] = 0b01101111; // 9
    
    // digit2SevenSegArr[10] = 0b10000001; // FIGURE 8 SECTION 'a'
    // digit2SevenSegArr[11] = 0b10100000; // 'f'
    // digit2SevenSegArr[12] = 0b11000000; // 'g'
    // digit2SevenSegArr[13] = 0b10000100; // 'c'
    // digit2SevenSegArr[14] = 0b10001000; // 'd'
    // digit2SevenSegArr[15] = 0b10010000; // 'e'
    // digit2SevenSegArr[16] = 0b11000000; // 'g'
    // digit2SevenSegArr[17] = 0b10000010; // 'b'

    int ai = c - '0';
    if ((ai>=0) && (ai<=9))
    {
        return digit2SevenSegArr[ai];
    }
    else
    {
        return -1;
    }
}
 


void numPhotonsSoldHandler(const char *event, const char *data)
{
    Serial.print(event);
    Serial.print(", data: ");
    if (!data)
    {
        Serial.println("NULL");
        return;
    }

    int numOfPhotonsSold;
    char numOfPhotonsSoldStr[kNumOfSevenSegments+1];
   
    numOfPhotonsSold = atoi(data);
    sprintf(numOfPhotonsSoldStr, "%6d", numOfPhotonsSold);
    Serial.println(numOfPhotonsSoldStr);

    byte sendByte, i;
    char debugStr[80]; 

    for (i=0; i<strlen(numOfPhotonsSoldStr); i++)
    {
        sendByte = digit2SevenSeg(numOfPhotonsSoldStr[i]);
        setLED(sendByte); 
        // sprintf(debugStr,"%c is %d, strlen is %d",numOfPhotonsSoldStr[i], sendByte,strlen(numOfPhotonsSoldStr));
        // Serial.println(debugStr);
        delay(1000);
    }
    
    Serial.println("turn off");
    setLED(0);
    
    
}



void loop() 
{
  
}
