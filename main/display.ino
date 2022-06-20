#ifndef INO_DISPLAY
#define INO_DISPLAY

#define DA 11
#define DB A0
#define DC  8
#define DD  3
#define DE  2
#define DF 10
#define DG  9
#define DISABLE1 0
#define DISABLE2 1

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

void activateDisplay(int display)
{
  if (display == 1)
  {
    digitalWrite(DISABLE1, LOW);
    digitalWrite(DISABLE2, HIGH);
  }
  else
  {
    digitalWrite(DISABLE1, HIGH);
    digitalWrite(DISABLE2, LOW);
  }
}

void writeNumber(int toDisplay)
{
  digitalWrite(DA, (toDisplay >> 6) & 1); // Shift bits to get segment
  digitalWrite(DB, (toDisplay >> 5) & 1);
  digitalWrite(DC, (toDisplay >> 4) & 1);
  digitalWrite(DD, (toDisplay >> 3) & 1);
  digitalWrite(DE, (toDisplay >> 2) & 1);
  digitalWrite(DF, (toDisplay >> 1) & 1);
  digitalWrite(DG, (toDisplay >> 0) & 1);
}

#endif