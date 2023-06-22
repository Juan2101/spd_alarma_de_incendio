// C++ code
//
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <Servo.h>

#define LED_ROJO 10
#define LED_VERDE 9
#define SENSOR A0
#define SERVO 8
#define IR 11
#define onOff 0xFF00BF00
#define stop 0xFA05BF00
#define more 0xFE01BF00
#define less 0xF609BF00

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
Servo servoMotor;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    IrReceiver.begin(IR, DISABLE_LED_FEEDBACK);
    lcd.begin(16, 2);
    servoMotor.attach(SERVO);
    Serial.begin(9600);
}

void mostrarEstacion(String estacion, int temperatura, int &temActual)
{
    lcd.setCursor(0, 0);
    lcd.print(estacion);
    lcd.setCursor(0, 1);
    lcd.print(temperatura);
    lcd.print("C");
    temActual = temperatura;
}

void estaciones(int temperatura, int &temActual)
{
    if (temperatura != temActual)
    {
        lcd.clear();
        if (temperatura >= -40 && temperatura <= 5)
        {
            mostrarEstacion("Winter", temperatura, temActual);
        }
        else if (temperatura > 5 && temperatura <= 15)
        {
            mostrarEstacion("Autumn", temperatura, temActual);
        }
        else if (temperatura > 15 && temperatura <= 25)
        {
            mostrarEstacion("Spring", temperatura, temActual);
        }
        else if (temperatura > 25 && temperatura < 50)
        {
            mostrarEstacion("Summer", temperatura, temActual);
        }
    }
}

void alarma(bool &incendio, int &ms)
{
    lcd.setCursor(0, 0);
    lcd.print("Incendio");
    int i = 0;
    bool flag = true;

    while (incendio)
    {
        if (IrReceiver.decode())
        {
            if (IrReceiver.decodedIRData.decodedRawData == stop)
            {
                incendio = false;
            }
            if (IrReceiver.decodedIRData.decodedRawData == more && ms >= 5)
            {
                ms--;
            }
            if (IrReceiver.decodedIRData.decodedRawData == less && ms <= 30)
            {
                ms++;
            }
            IrReceiver.resume();
        }
        
        servoMotor.write(i);
        delay(ms);

        if (flag)
            i++;
        else
            i--;

        if (i > 180)
            flag = false;
        else if (i < 0)
            flag = true;
    }
}

int temActual = 0;
bool encendido = false;
bool incendio = false;
int ms = 15;

void loop()
{
    if (IrReceiver.decode())
    {
        if (IrReceiver.decodedIRData.decodedRawData == onOff)
        {
            lcd.clear();
            temActual = 0;
            encendido = !encendido;
        }
        IrReceiver.resume();
    }

    if (encendido)
    {
        int lecturaSensor = analogRead(SENSOR);
        int temperatura = map(lecturaSensor, 20, 358, -40, 125);

        digitalWrite(LED_ROJO, LOW);
        digitalWrite(LED_VERDE, HIGH);

        estaciones(temperatura, temActual);

        if (temperatura >= 50)
        {
            incendio = true;
            digitalWrite(LED_VERDE, HIGH);
            digitalWrite(LED_ROJO, HIGH);
            alarma(incendio, ms);
        }
    }
    else
    {
        digitalWrite(LED_ROJO, HIGH);
        digitalWrite(LED_VERDE, LOW);
    }
}