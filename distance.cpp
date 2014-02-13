// This is free-ware
// Example code for using the Cytron Technologies HC-SR4 Ultrasonic distance sensor with Spark Core

volatile int StartMeasurement = D1;
volatile int EchoPin = D2;


int measurePulse(int pin);
int attemptDistanceMeasurementOnce(void);
int attemptDistanceUntilSuccess(void);

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
    Serial.print("Duration: ");
    Serial.println(duration); 
    
    // totally empirical conversion (Note: dependent on processor speed of Spark Core!  Yuck!)
    distanceInches = (duration - 5.0) / 56.0;
    
    Serial.print("Distance in inches: ");
    Serial.println(distanceInches); 
    Serial.println(" "); 

    // make a new measurement about 4 times per second
    delay(250);
}

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
    
    // pulse the sensor to make it get a distance reading
    digitalWrite(StartMeasurement, HIGH);
    delay(5);
    digitalWrite(StartMeasurement, LOW);
    
    // see how long the echo pulse emitted by distance sensor is
    duration = measurePulse(EchoPin);

    return duration;
}


// NOTE: This is ugly -- spark team has not provided micro second timing yet and need sub-millisecond timing...
// at 1130 feet per second, 1 inch is only about 74 microseconds, so obviously millisecond resolution
// won't work for typical distances for which the sensor is used
// so we are using this skanky processor speed dependent counter loop to deterimine the length of the echo pulse
// this should be improved once Spark team provides a sub-millisecond timer
int measurePulse(int pin)
{
    int duration = 0;
    
        // wait for the initial rising of the echo pin
        // give up if no pulse arrives
    while(digitalRead(EchoPin) == LOW && duration < 100000)
    {
        duration += 1;
    }
    
    
    // count cycles until the pin goes low
    // give up if something goes wrong (duration keeps going forever)
    duration = 0;
    while(digitalRead(EchoPin) == HIGH && duration < 100000)
    {
        duration += 1;
    }

    return duration;
}
