const int led = 10;

String string = "";
String binString = "";
String aux = "";
char modoTrans = "";

void setup()
{
    pinMode(led, OUTPUT);
    Serial.begin(9600);
}

/**
 * @brief Funcao para conversão de string para binario 8 bits de acordo com a tabela ASCII
 *
 * @param letra
 * @return String
 */
String converterTabelaAscii(char letra)
{
    switch (letra)
    {
    case 'a':
        return "01100001";
    case 'b':
        return "01100010";
    case 'c':
        return "01100011";
    case 'd':
        return "01100100";
    case 'e':
        return "01100101";
    case 'f':
        return "01100110";
    case 'g':
        return "01100111";
    case 'h':
        return "01101000";
    case 'i':
        return "01101001";
    case 'j':
        return "01101010";
    case 'k':
        return "01101011";
    case 'l':
        return "01101100";
    case 'm':
        return "01101101";
    case 'n':
        return "01101110";
    case 'o':
        return "01101111";
    case 'p':
        return "01110000";
    case 'q':
        return "01110001";
    case 'r':
        return "01110010";
    case 's':
        return "01110011";
    case 't':
        return "01110100";
    case 'u':
        return "01110101";
    case 'v':
        return "01110110";
    case 'w':
        return "01110111";
    case 'x':
        return "01111000";
    case 'y':
        return "01111001";
    case 'z':
        return "01111010";
    case ' ':
        return "00100000";
    default:
        Serial.println("Fora dos parâmetros!");
        break;
    }
}

/**
 * @brief Função para formar a string final passada para binario
 *
 * @param uni
 * @return String
 */
String converterBinario(String uni)
{
    String stringFinal;
    uni.toLowerCase();

    for (int i = 0; i < uni.length(); i++)
    {
        stringFinal = stringFinal + converterTabelaAscii(uni[i]);
    }
    return stringFinal;
}

/**
 * @brief Função para a transmissão dos sinais com forma de codificação NRZ-L
 *
 * @param uni
 */
void emitirNRZ_L(String uni)
{ // uni = Palavra em binário
    int x = 7;
    Serial.println("Enviando NRZ-L: ");

    for (int i = 0; i < uni.length(); i++)
    {

        Serial.print(uni[i]);

        if (uni[i] == '0')
        { // 0 é positivo = Led aceso
            digitalWrite(led, HIGH);
            delay(500);
        }
        else
        { // 1 é negativo = Led apagado
            digitalWrite(led, LOW);
            delay(500);
        }
        if (i / x == 1)
        {
            x += 8;
            Serial.println();
        }
    }
    Serial.println();
    digitalWrite(led, LOW);
}

/**
 * @brief Função para a transmissão dos sinais com forma de codificação NRZ-I
 *
 * @param uni
 */
void emitirNRZ_I(String uni)
{
    int aux = 0;
    int cont = 1;
    Serial.println("Enviando NRZ-I: ");

    Serial.print(uni[0]);
    digitalWrite(led, 0);
    delay(500);

    // Transmissão dos bits
    for (int i = 1; i < uni.length(); i++)
    {
        cont++;
        Serial.print(uni[i]);

        if (uni[i] == '0') { // 0 = mantem o sinal
            digitalWrite(led, aux);
            delay(500);
        } else { // 1 = inverte o sinal
            aux = 1 - aux; // inverte o valor de aux (entre 0 e 1)
            digitalWrite(led, aux);
            delay(500);
        } if(cont == 8){
          cont = 0;
          Serial.println();
        }
    }
    Serial.println();
    digitalWrite(led, LOW);
}

#define POLYNOMIAL 0x07
#define WIDTH  (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

/**
 * @brief Função para calcular o crc 8 bits
 *
 * @param data
 * @param length
 * @return uint8_t
 */
uint8_t crc8(uint8_t *data, size_t length) {
  uint8_t remainder = 0;

  for (size_t byte = 0; byte < length; ++byte) {
    remainder ^= data[byte]; // Operação de XOR entre valor atual do remainder e o valor atual dos bytes em data ( ela faz operação de XOR bit a bit entre os operandos)
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      if (remainder & TOPBIT)
      { // Se bit mais significativo for 1
        remainder = (remainder << 1) ^ POLYNOMIAL; // Desloca bit a esquerda e faz XOR (divisão novamente com o polinomio e o bit deslocado)
      }
      else
      { // Se não for apenas desloca
        remainder = (remainder << 1);
      }
    }
    return remainder;
}

void loop()
{
    if (Serial.available() > 0)
    {
        string = Serial.readString();

        modoTrans = string[0];
        for (int i = 0; i < (string.length() - 1); i++)
        {
            string[i] = string[i + 1];
        }

        string.trim(); // Remove espaços
        binString = converterBinario(string);

        // Converte a string para bytes (uint8_t)
        uint8_t binaryMessage[binString.length()/8];
        for (int i = 0; i < binString.length(); i += 8) {
            binaryMessage[i/8] = strtol(binString.substring(i, i+8).c_str(), NULL, 2);
        }

        uint8_t crc = crc8(binaryMessage, sizeof(binaryMessage));
        String crcString = "";
        crcString = crcString + String(crc, BIN);
        // Concate bits no crc caso necessário
        for(int i=0; i<8; i++){
          if(crcString.length()<8){
            crcString = crcString + "0";
          }
        }

        Serial.println();
        Serial.println("Mensagem para ser enviada");
        Serial.println(string);
        Serial.print("CRC: ");
        Serial.println(crcString);

        binString = binString + crcString;

        if (modoTrans == 'a')
        {
            digitalWrite(led, HIGH);
            delay(2000);
            emitirNRZ_L(binString);
        }
        else
        {
            if (modoTrans == 'b')
            {
                digitalWrite(led, HIGH);
                delay(1000);
                digitalWrite(led, LOW);
                delay(1000);
                emitirNRZ_I(binString);
            }
            else
            {
                Serial.println("Caractere para seleção inválido");
            }
        }
    }
}