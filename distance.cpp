// This is free-ware
// Example code for using the Cytron Technologies HC-SR4 Ultrasonic distance sensor with Spark Core

volatile int StartMeasurement = D1;
volatile int EchoPin = D2;

volatile unsigned long startPulse = 0;
volatile unsigned long endPulse = 0;

int attemptDistanceMeasurementOnce(void);
int attemptDistanceUntilSuccess(void);
void ultrasonicISR(void);

void setup() {
    pinMode(StartMeasurement, OUTPUT);
    pinMode(EchoPin, INPUT);
    Serial.begin(9600);
}

void loop() {
    int duration = 0;
    float distanceInches = 0.0;
    
    // get a distance measurement (might have to retry a few times -- hardware has been inconsistent)
    duration = attemptDistanceUntilSuccess();

    // use a USB terminal app to view the distance measurements 
    // (I downloaded "serailTools" for Mac for free on appStore)
    Serial.print("Duration in microseconds: ");
    Serial.println(duration); 
    
    // totally empirical conversion 
    distanceInches = duration / 148.0;
    
    Serial.print("Distance in inches: ");
    Serial.println(distanceInches); 
    Serial.println(" "); 

    // make a new measurement about 4 times per second
    delay(250);
}

// mine doesn't work totally reliably, so I built in retrying
int attemptDistanceUntilSuccess()
{
    int duration;
    int attempts = 1;
    
    while(attempts < 10)
    {
        duration = attemptDistanceMeasurementOnce();
        if(duration > 0 && duration < 60000)
        {
            break;
        }
        
        // wait a short time before attempting again -- the primary failure mode is very slow - 187 ms echo pulse
        delay(200);
    }
    
    return duration;
    
}

int attemptDistanceMeasurementOnce(void)
{
    int duration;
    
    endPulse = startPulse = 0;
    attachInterrupt(EchoPin, ultrasonicISR, CHANGE);
    
    // pulse the sensor to make it get a distance reading
    digitalWrite(StartMeasurement, HIGH);
    delay(5);
    digitalWrite(StartMeasurement, LOW);
    
    // wait while we ostensibly get both up and down edges of pulse (and interrupts)
    // the interrupt service routine sets our start and end "times" in microseconds
    delay(20);
    duration = endPulse - startPulse;
    
    detachInterrupt(EchoPin);

    return duration;
}

void ultrasonicISR(void)
{
    if(digitalRead(EchoPin) == HIGH)
        startPulse = micros();
    else
        endPulse = micros();
    
    
}

