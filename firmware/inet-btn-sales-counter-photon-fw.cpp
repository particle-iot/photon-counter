// You know the routine by now, just going to comment on the new things!

#include "InternetButton/InternetButton.h"

InternetButton b = InternetButton();

void setup() {
    b.begin();
    b.playSong("C4,8,E4,8,G4,8,C5,8,G5,4");
}

void loop() {
    if(b.buttonOn(1) || b.buttonOn(2) || b.buttonOn(3) ||b.buttonOn(4)) {
        b.rainbow(3);
        Particle.publish("increase-counter",PRIVATE);
        b.playSong("E5,8,G5,8,E6,12,C6,8,D6,8,G6,12,E5,8,G5,8,E6,16,C6,8,D6,8,G6,32");
        b.allLedsOff();
    }

}
