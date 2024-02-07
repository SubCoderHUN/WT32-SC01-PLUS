#include <SimpleTimer.h>

// Create a first timer and specify its interval in milliseconds
SimpleTimer firstTimer(5000);
// Create a second timer
SimpleTimer secondTimer;

// A flag indicates, that a first timer is ready
bool flag = false;

void setup()
{
    Serial.begin(9600);

    // Set an interval to 3 secs for the second timer
    secondTimer.setInterval(3000);
}

void loop()
{
    if (firstTimer.isReady() && !flag) {            // Check is ready a first timer
        Serial.println("5 seconds have passed");
        // Do something ...

        // Set the flag so as not to fall into this condition
        flag = true;
    }

    if (secondTimer.isReady()) {                    // Check is ready a second timer
        Serial.println("Called every 3 sec");
        // Do something ...
        secondTimer.reset();                        // Reset a second timer
    }
}
