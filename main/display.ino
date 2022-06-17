#define DA 11
#define DB A0
#define DC  8
#define DD  3
#define DE  2
#define DF 10
#define DG  9
#define DISABLE1 0
#define DISABLE2 1

int NUMBERS[] =
{
  0b1111110, // 0
  0b0110000, // 1
  0b1101101, // 2
  0b1111001, // 3
  0b0110011, // 4
  0b1011011, // 5
  0b1011111, // 6
  0b1110000, // 7
  0b1111111, // 8
  0b1111011  // 9
};

void display_setup()
{
  pinMode(DA, OUTPUT);
  pinMode(DB, OUTPUT);
  pinMode(DC, OUTPUT);
  pinMode(DD, OUTPUT);
  pinMode(DE, OUTPUT);
  pinMode(DF, OUTPUT);
  pinMode(DG, OUTPUT);
  pinMode(DISABLE1, OUTPUT);
  pinMode(DISABLE2, OUTPUT);
}

/** DISPLAY LOOP
void loop()
{
  for (int i = 0; i <= 9 ; i++)
  {
    unsigned long timeout_s = millis() + 500;
    while(timeout_s > millis())
    {
      writeNumber(i, state);
      if (timeout < millis())
      {
        timeout = millis() + 250;
        state = !state;
      }
    }
  }
}
*/

void writeNumber(int number, boolean firstDisplay)
{
  digitalWrite(DISABLE1, firstDisplay);
  digitalWrite(DISABLE2, !firstDisplay);

  int toDisplay = NUMBERS[number];
  digitalWrite(DA, (toDisplay >> 6) & 1);
  digitalWrite(DB, (toDisplay >> 5) & 1);
  digitalWrite(DC, (toDisplay >> 4) & 1);
  digitalWrite(DD, (toDisplay >> 3) & 1);
  digitalWrite(DE, (toDisplay >> 2) & 1);
  digitalWrite(DF, (toDisplay >> 1) & 1);
  digitalWrite(DG, (toDisplay >> 0) & 1);
}