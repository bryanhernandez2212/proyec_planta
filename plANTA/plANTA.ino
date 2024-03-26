#include <LiquidCrystal_I2C.h>

const int pinLM35 = A0;
const int pinSensorHumedad = A1;
const int pinRelay = 2;
const int pinLuminosidad = A3;
int valor;
const long A = 1000;
const byte B = 15;
const byte Rc = 10;

int valorLM35;
float temperatura;
int valorHumedad;
float humedad;
float valorLuminosidad;
float temperaturaActual;
float luminosidadActual;
float humedadActual;
bool temperaturaInicial = true;
bool luminosidadInicial = true;
bool humedadInicial = true;

LiquidCrystal_I2C lcd(0x27, 16, 2);

boolean bombaEncendida = false;
boolean bombaApagada = true;

unsigned long previousMillis = 0;
const long interval = 10000; // intervalo de 10 segundos

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(pinRelay, OUTPUT);
}

void EnviarDatos(float valor) {
  Serial.println(valor);
}

void estado(String estadoBomba) {
  Serial.println(estadoBomba);
}

void mostrarDatosSerial(int comando) {
  switch (comando) {
    case 2:
      if (temperaturaInicial) {
        EnviarDatos(temperatura);
        temperaturaInicial = false;
      } else {
        valorLM35 = analogRead(pinLM35);
        temperaturaActual = (valorLM35 * 0.48828125);
        if (temperatura == temperaturaActual) {
          estado("igual a " + String(temperaturaActual));
        } else {
          EnviarDatos(temperaturaActual);
        }
      }
      break;

    case 3:
      if (luminosidadInicial) {
        EnviarDatos(valorLuminosidad);
        luminosidadInicial = false;
      } else {
        valor = analogRead(pinLuminosidad);
        luminosidadActual = ((long)valor * A * 10) / ((long)B * Rc * (1024 - valor));
        if (valorLuminosidad == luminosidadActual) {
          estado("igual a" + String(luminosidadActual));
        } else {
          EnviarDatos(luminosidadActual);
        }
      }
      break;

    case 4:
      if (humedadInicial) {
        EnviarDatos(humedad);
        humedadInicial = false;
      } else {
        valorHumedad = analogRead(pinSensorHumedad);
        humedadActual = 100 - map(valorHumedad, 0, 1023, 0, 100);
        if (humedad == humedadActual) {
          estado("igual a" + String(humedadActual));
        } else {
          EnviarDatos(humedadActual);
        }
      }
      break;

    case 5:
      estado(bombaEncendida ? "Encendido" : "Apagado");
      break;
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    valorLM35 = analogRead(pinLM35);
    temperatura = (valorLM35 * 0.48828125);

    valorHumedad = analogRead(pinSensorHumedad);
    humedad = 100 - map(valorHumedad, 0, 1023, 0, 100);

    valor = analogRead(pinLuminosidad);
    valorLuminosidad = ((long)valor * A * 10) / ((long)B * Rc * (1024 - valor));
  }

  if (humedad <= 40) {
    digitalWrite(pinRelay, LOW);
    bombaEncendida = true;
    bombaApagada = false;
  } else {
    digitalWrite(pinRelay, HIGH);
    bombaEncendida = false;
    bombaApagada = true;
  }

  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
  lcd.setCursor(12, 0);
  lcd.print(temperatura);

  lcd.setCursor(0, 1);
  lcd.print("Humedad:");
  lcd.setCursor(9, 1);
  lcd.print(humedad);
  lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("Luminosidad:");
  lcd.setCursor(13, 2);
  lcd.print(luminosidadActual);

  if (Serial.available() > 0) {
    int comando = Serial.read() - '0';
    mostrarDatosSerial(comando);
  }
}
