String palavra = "";
const int ldr = A0;
int valorLdr = 0;
int soma = 0;

void setup()
{
  pinMode(ldr, INPUT);
  Serial.begin(9600);
}

/**
 * @brief Funcao para conversão de binario 8 bits para letra de acordo com a tabela ASCII
 *
 * @param uni
 * @return char
 */
char converterTabelaAscii(String uni)
{
  if (uni == "01100001")
  {
    return 'a';
  }
  else if (uni == "01100010")
  {
    return 'b';
  }
  else if (uni == "01100011")
  {
    return 'c';
  }
  else if (uni == "01100100")
  {
    return 'd';
  }
  else if (uni == "01100101")
  {
    return 'e';
  }
  else if (uni == "01100110")
  {
    return 'f';
  }
  else if (uni == "01100111")
  {
    return 'g';
  }
  else if (uni == "01101000")
  {
    return 'h';
  }
  else if (uni == "01101001")
  {
    return 'i';
  }
  else if (uni == "01101010")
  {
    return 'j';
  }
  else if (uni == "01101011")
  {
    return 'k';
  }
  else if (uni == "01101100")
  {
    return 'l';
  }
  else if (uni == "01101101")
  {
    return 'm';
  }
  else if (uni == "01101110")
  {
    return 'n';
  }
  else if (uni == "01101111")
  {
    return 'o';
  }
  else if (uni == "01110000")
  {
    return 'p';
  }
  else if (uni == "01110001")
  {
    return 'q';
  }
  else if (uni == "01110010")
  {
    return 'r';
  }
  else if (uni == "01110011")
  {
    return 's';
  }
  else if (uni == "01110100")
  {
    return 't';
  }
  else if (uni == "01110101")
  {
    return 'u';
  }
  else if (uni == "01110110")
  {
    return 'v';
  }
  else if (uni == "01110111")
  {
    return 'w';
  }
  else if (uni == "01111000")
  {
    return 'x';
  }
  else if (uni == "01111001")
  {
    return 'y';
  }
  else if (uni == "01111010")
  {
    return 'z';
  }
  else if (uni == "00100000")
  {
    return ' ';
  }
  else
  {
    Serial.println("Valor invalido");
    return ' ';
  }
}

#define POLYNOMIAL 0x07
#define WIDTH (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

/**
 * @brief Função para calcular o crc 8 bits
 *
 * @param data
 * @param length
 * @return uint8_t
 */
uint8_t crc8(uint8_t *data, size_t length)
{
  uint8_t remainder = 0;

  for (size_t byte = 0; byte < length; ++byte)
  {
    remainder ^= data[byte]; // Operação de XOR entre valor atual do remainder e o valor atual dos bytes em data ( ela faz operação de XOR bit a bit entre os operandos)
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      if (remainder & TOPBIT)
      {                                            // Se bit mais significativo for 1
        remainder = (remainder << 1) ^ POLYNOMIAL; // Desloca bit a esquerda e faz XOR (divisão novamente com o polinomio e o bit deslocado)
      }
      else
      { // Se não for apenas desloca
        remainder = (remainder << 1);
      }
    }
    return remainder;
  }
}
/**
 * @brief Função para recebimento dos sinais com forma de codificação NRZ-L
 *
 */
String receptorNRZ_L()
{
  palavra = "";
  Serial.println("");
  Serial.println("------Leitor NRZ-L------");
  String letraBin = "";
  char letra = "";
  String binString = "";
  String receivedCRC = "";
  int lastchar = 0;

  while (letraBin != "11111111" && letraBin != "00000000")
  {

    letraBin = "";
    letra = "";

    // Faz leitura dos bits emitidos
    for (int i = 0; i < 8; i++)
    {
      valorLdr = analogRead(ldr);
      if (valorLdr < 500)
      {
        letraBin = letraBin + "1";
      }
      else
      {
        letraBin = letraBin + "0";
      }
      delay(500);
    }

    binString = binString + letraBin;

    if (letraBin != "11111111" && letraBin != "00000000")
    {
      letra = converterTabelaAscii(letraBin);
      Serial.print("Letra: ");
      Serial.println(letra);
      Serial.print("Binario: ");
      Serial.println(letraBin);
      receivedCRC = letraBin;
      palavra = palavra + letra;
    }
  }

  // Converte a string para bytes (uint8_t)
  uint8_t binaryMessage[binString.length() / 8];
  for (int i = 0; i < binString.length(); i += 8)
  {
    binaryMessage[i / 8] = strtol(binString.substring(i, i + 8).c_str(), NULL, 2);
  }

  uint8_t calculatedCRC = crc8(binaryMessage, sizeof(binaryMessage) - 2);
  String crcString = "";
  crcString = crcString + String(calculatedCRC, BIN);

  // Concate bits no crc caso necessário
  for (int i = 0; i < 8; i++)
  {
    if (crcString.length() < 8)
    {
      crcString = crcString + "0";
    }
  }

  Serial.println();
  Serial.print("CRC: ");
  Serial.println(receivedCRC);
  Serial.print("Calculo do CRC: ");
  Serial.println(crcString);

  if (crcString == receivedCRC)
  {
    Serial.println("Dados recebidos corretamente");
  }
  else
  {
    Serial.println("Erro na transmissão dos dados");
  }

  return palavra;
}

/**
 * @brief Função para recebimento dos sinais com forma de codificação NRZ-I
 *
 */
String receptorNRZ_I()
{
  palavra = "";
  Serial.println("");
  Serial.println("------Leitor NRZ-I------");
  String letraBin = "";
  String palavraBin = "";
  String sinalBin = "";
  String receivedCRC = "";
  String auxString = "";
  char letra = "";

  Serial.println();

  // While irá ler os bits em binário recebido e concatená-los em uma
  // string de tamanho 8, o que representa o sinal nrzi recebido de cada letra
  while (letraBin != "01010101" && letraBin != "00000000")
  {

    letraBin = "";
    letra = "";

    for (int i = 0; i < 8; i++)
    {
      valorLdr = analogRead(ldr);
      if (valorLdr > 500)
      { // Led desligado
        // Serial.print("0");
        letraBin = letraBin + "0";
      }
      else
      {
        // Serial.print("1");  // Led ligado
        letraBin = letraBin + "1";
      }
      delay(500);
    }

    if (letraBin != "01010101" && letraBin != "00000000")
    {
      palavraBin = palavraBin + letraBin;
    }

    if (letraBin == "11111111" || letraBin == "00000000")
    {
      break;
    }
    else
    {
      Serial.print("Letra em Binario: ");
      Serial.println(letraBin);
    }
  }

  // Agora que temos a palavra inteira como um sinal binário nrzi,
  // este for irá selecionando letra por letra (cada iteração),
  // e irá decodificá-la

  for (int i = 0; i < (((palavraBin.length() - 8) / 8)); i++)
  {
    sinalBin = palavraBin.substring(i * 8, i * 8 + 8); // Seleciona letra por letra do sinal em binário, recortando espaços de tamanho 8
    // Este for irá transformar o sinal recebido de uma letra no valor ascii binário da mesma
    for (int j = 0; j < 8; j++)
    {
      if (j == 0)
      {
        letraBin = "0";
      }
      else if (sinalBin[j] != sinalBin[j - 1])
      {
        letraBin = letraBin + "1";
      }
      else
      {
        letraBin = letraBin + "0";
      }
    }
    if (i != ((palavraBin.length() - 8) / 8))
    {
      auxString = auxString + letraBin;
      receivedCRC = letraBin;
      letra = converterTabelaAscii(letraBin);
      palavra = palavra + letra;
    }
  }
  uint8_t binaryMessage[auxString.length() / 8];
  for (int i = 0; i < auxString.length(); i += 8)
  {
    binaryMessage[i / 8] = strtol(auxString.substring(i, i + 8).c_str(), NULL, 2);
  }

  uint8_t calculatedCRC = crc8(binaryMessage, sizeof(binaryMessage) - 1);
  String crcString = "";
  crcString = crcString + String(calculatedCRC, BIN);
  for (int i = 0; i < 8; i++)
  {
    if (crcString.length() < 8)
    {
      crcString = crcString + "0";
    }
  }

  receivedCRC[0] = '1';
  Serial.println();
  Serial.print("CRC: ");
  Serial.println(receivedCRC);
  Serial.print("Calculo do CRC: ");
  Serial.println(crcString);

  if (crcString == receivedCRC)
  {
    Serial.println("Dados recebidos corretamente");
  }
  else
  {
    Serial.println("Erro na transmissão dos dados");
  }

  return palavra;
}

void loop()
{
  valorLdr = analogRead(ldr);
  Serial.print("Valor lido pelo LDR = ");
  Serial.println(valorLdr);
  delay(500);

  if (valorLdr > 500)
  {
    for (int i = 0; i < 3; i++)
    {
      soma += valorLdr;
      valorLdr = analogRead(ldr);
      delay(500);
    }

    if (soma > 1500)
    {
      palavra = receptorNRZ_L();
      Serial.print("Palavra recebida pelo NRZ-L: ");
      Serial.println(palavra);
      Serial.println();
    }
    else
    {
      palavra = receptorNRZ_I();
      Serial.print("Palavra recebida pelo NRZ-I: ");
      Serial.println(palavra);
      Serial.println();
    }
    soma = 0;
  }
}
