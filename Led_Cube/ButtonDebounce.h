enum ButtonStates { ButtonIdle, ButtonWait, ButtonLow } ButtonState;
unsigned long ButtonTime = 0;

int ButtonInitialize() {
	pinMode(4, INPUT);
	ButtonState = ButtonIdle;
	pinMode(13, OUTPUT);
}

int ButtonNextState(int InputPin) {
	switch(ButtonState) {
		case ButtonIdle:
			if(InputPin == LOW) {
				ButtonTime = millis();
				ButtonState = ButtonWait;
				digitalWrite(13, HIGH);
			}
      break;
    case ButtonWait:
      if(InputPin == HIGH) {
        ButtonState = ButtonIdle;
        digitalWrite(13, LOW);
      } else if(millis() - ButtonTime >= 5) {
        ButtonState = ButtonLow;
        digitalWrite(13, LOW);
        return 1;
      }
      break;
    case ButtonLow:
      if(InputPin == HIGH) {
        ButtonState = ButtonIdle;
        return 2;
      }
      break;
	}
  return 0;
}
