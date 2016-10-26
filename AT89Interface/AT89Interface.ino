#define dummyData 0xAA
#define RDY 75
#define NRDY 76

const int _MISO = 12; //7
const int _MOSI = 11; //6
const int _CLK = 13; //8
const int RST = 10;

uint8_t currentPage = 0;
uint8_t pageData[256];

void setup() {
  pinMode(_MISO, INPUT);
  pinMode(_MOSI, OUTPUT);
  pinMode(_CLK, OUTPUT);
  pinMode(RST, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  while (!Serial.available());
  switch (Serial.read())
  {
    //Store page
    case 'x':
      while (!Serial.available());
      currentPage = Serial.read();
      break;
    //Read page number
    case 'y':
      Serial.write(currentPage);
      break;
    case 'a':
      write_program_memory();
      Serial.print(currentPage, DEC);
      Serial.print("+");
      break;
    case 'b':
      read_program_memory();
      break;
    case 'o': digitalWrite(RST, 1); delay(1); break;
    case 'c': digitalWrite(RST, 0); break;
    case 'e': programming_enable(); delay(1); break;
    case 'w':
      {
        for (int idx = 0; idx < 256; idx++)
        {
          while (!Serial.available());
          int incoming = Serial.read();
          pageData[idx] = incoming;
        }
        break;
      }
    //Read data to PC
    case 'r':
      for (int i = 0; i < 256; i++)
        Serial.write(pageData[i]);
      break;
    case 'p':
      erase();
      Serial.write('.');
      break;
    case 'i':
      uint8_t sig;
      sendSPI(0x28);
      sendSPI(0x00);
      sendSPI(0x00);
      sig = sendSPI(dummyData);
      Serial.write(sig);

      sendSPI(0x28);
      sendSPI(0x01);
      sendSPI(0x00);
      sig = sendSPI(dummyData);
      Serial.write(sig);

      sendSPI(0x28);
      sendSPI(0x02);
      sendSPI(0x00);
      sig = sendSPI(dummyData);
      Serial.write(sig);
      break;
    default: break;
  }
}

uint8_t sendSPI(uint8_t data)
{
  uint8_t retval = 0;
  int t;

  for (int ctr = 0; ctr < 8; ctr++)
  {
    if (data & 0x80) digitalWrite(_MOSI, 1);
    else digitalWrite(_MOSI, 0);

    digitalWrite(_CLK, 1);
    delayMicroseconds(2);

    t = digitalRead(_MISO);
    digitalWrite(_CLK, 0);
    delayMicroseconds(2);

    if (t) retval |= 1;
    else retval &= 0xFE;

    if (ctr != 7)
    {
      retval <<= 1;
      data <<= 1;
    }
  }
  return retval;
}

void programming_enable()
{
  sendSPI(0xAC);
  sendSPI(0x53);
  sendSPI(dummyData);
  sendSPI(dummyData);
}

void read_program_memory()
{
  sendSPI(0x30);
  sendSPI(currentPage);
  for (int i = 0; i < 256; i++)
  {
    pageData[i] = sendSPI(dummyData);
  }
}

void write_program_memory()
{
  sendSPI(0x50);
  sendSPI(currentPage);
  for (int i = 0; i < 256; i++)
  {
    sendSPI(pageData[i]);
  }
}

void erase()
{
  sendSPI(0xAC);
  sendSPI(0x80);
  sendSPI(dummyData);
  sendSPI(dummyData);
  delay(500);
}

