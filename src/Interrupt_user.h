
void Interrupt_init (void)
{
  // Timer0 is already using millis() - we create an interrupt somewhere in the middle and call the “Compare A” function below it
  // The interrupt is called once per millisecond
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
  sei(); // Enable interrupts globally
}



// The interrupt is called once per millisecond
SIGNAL(TIMER0_COMPA_vect)
{
  Count_read_Temperature++;
  IND_Update();
}
