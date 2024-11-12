#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_VERDE 5
#define LED_VERMELHO 4
#define BUZZER 2
#define SERVO_PIN 3

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Criando instância do MFRC522
Servo meuServo; // Cria um objeto da classe Servo

// IDs autorizados (substitua pelos IDs dos seus cartões RFID)
String autorizados[] = {"13F58A0F", "ID2", "ID3", "E30E980F"};

void setup() {
  Serial.begin(9600);   // Inicializa comunicação serial
  SPI.begin();          // Inicializa comunicação SPI
  mfrc522.PCD_Init();   // Inicializa o leitor RFID
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  meuServo.attach(SERVO_PIN); // Anexa o servo ao pino definido
  meuServo.write(90); // Garante que o servo começa na posição inicial (90 graus)
  Serial.println("Aproxime o cartão do leitor...");
}

void loop() {
  // Verifica se há uma nova tag presente
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Verifica se é possível ler o cartão
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Lê o UID do cartão
  String conteudo = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  conteudo.toUpperCase();
  Serial.println("UID do cartão: " + conteudo);

  // Verifica se o UID está na lista de autorizados
  boolean autorizado = false;
  for (int i = 0; i < sizeof(autorizados) / sizeof(autorizados[0]); i++) {
    if (conteudo == autorizados[i]) {
      autorizado = true;
      break;
    }
  }

  if (autorizado) {
    Serial.println("Acesso autorizado");
    digitalWrite(LED_VERDE, HIGH);
    tone(BUZZER, 1000); // Emite som no buzzer
    delay(200); // Duração do som
    noTone(BUZZER); // Para o som
    meuServo.write(180); // Move o servo para 90 graus
    delay(10000); // Aguarda 10 segundos
    meuServo.write(90); // Retorna o servo para a posição inicial
    delay(2000);
    digitalWrite(LED_VERDE, LOW);
    meuServo.read();
  } else {
    Serial.println("Acesso negado");
    digitalWrite(LED_VERMELHO, HIGH);
    for (int i = 0; i < 3; i++) {
      tone(BUZZER, 1000); // Emite som no buzzer
      delay(200); // Duração do som
      noTone(BUZZER); // Para o som
      delay(200); // Pausa entre os toques
    }
    delay(1400);
    digitalWrite(LED_VERMELHO, LOW);
  }

  // Para de ler o cartão
  mfrc522.PICC_HaltA();
}
