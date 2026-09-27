#include <Wire.h> //Comunicação I2C, utilizado no LCD(imbutido) e na ponte H(motores)
#include <Servo.h> //Motor responsavel pela tranca do compartimento
#include <LiquidCrystal_I2C.h> //LCD com integração I2C
#include <Keypad.h>//Teclado 
// ===== CONFIGURAÇÕES =====

const byte VELOCIDADE = 128; //velocidade dos motores DC

const int LIMIAR_LDR = 300; // Parametro de medida para o LDR seguidor de Linha, os três LDR da frente
const int LIMIAR_LDR_ESTACAO = 100;//Parametro de medida para o LDR de indentificar uma estação, os dois LDR de trás

const int DISTANCIA_OBSTACULO = 30; // Distancia que o Sensor de proximidade considera como perigosa, parando o carrinho

// Servo (ângulos reais, em graus)
const int POSICAO_FECHADA = 0;
const int POSICAO_ABERTA = 90;

// Manobras
const unsigned long TEMPO_CURVA = 700; //tempo definido para uma curva completa
const unsigned long TEMPO_U = 1400; // tempo definido para uma curva em U ou seja meia volta, que é uma curva a esqueda com o dobro de tempo

// ===== CONTROLE DA VIAGEM =====

unsigned long inicioManobra = 0; //Variavel que armazena o tempo decorrido desde o inicio da manobra
bool saiuDaOrigem = false; //afirma se o carrinho saiu ou não da estação atual

// Evita contar, no tempo da manobra, o tempo em que o robô ficou
// parado esperando um obstáculo sair da frente.
bool pausadoPorObstaculo = false;
unsigned long inicioPausa = 0;

// ===== DIREÇÕES =====

enum Direcao {
  NORTE,
  LESTE,
  SUL,
  OESTE
};

enum Manobra {
  RETO,
  DIREITA,
  ESQUERDA,
  U
};


// ===== ESTADOS =====

enum Estado {
  ESCOLHENDO_DESTINO,
  PREPARACAO,
  MANOBRA_INICIAL,
  SAINDO_DA_ORIGEM,
  EM_VIAGEM,
  AGUARDANDO_SENHA,
  PORTA_ABERTA,
  AGUARDANDO_RETIRAR
};

Estado estadoAtual = ESCOLHENDO_DESTINO; //é o primeiro

// ===== NAVEGAÇÃO =====

char estacaoAtual = 'A'; // ao inicializar o arduino o carrinho se encontra em A
Direcao direcaoAtual = NORTE; //apontando pra norte
char origemViagem = 'A'; //da onde ele veio
char destino = 'A';// para onde ele vai
Direcao direcaoDesejada = NORTE;//direcao que ele deve apontar para ir ao seu destino
Manobra manobraAtual = RETO;//qual manobra ele deve fazer para ter a mesma direção do que a direção desejada, é uma curva? é uma reta?

// ===== SEGUIDOR =====

int ultimaDirecao = 0; //Utilizada como memoria 


// =====================================================
// LCD
// =====================================================
LiquidCrystal_I2C lcd(0x27, 16, 2); //Endereço do PCF(Acoplado no LCD), 16 colunas e 2 linhas de espaço do LCD
//Printa na tela sobre a viagem
void mostrarViagem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(origemViagem);
  lcd.print(" -> ");
  lcd.print(destino);
  lcd.setCursor(0, 1);
  lcd.print("EM VIAGEM");
}
// =====================================================
// Keypad
// =====================================================
//Usado para uma eficiente e fácil interação visual com o usuário

const byte LINHAS = 4; // O Keypad tem 4 linhas
const byte COLUNAS = 4; // O Keypad tem 4 colunas

//Mapeamento dos botões
char teclas[LINHAS][COLUNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinosLinhas[LINHAS] = {4, 5, 6, 7}; //Pinos do arduino para cada linha
byte pinosColunas[COLUNAS] = {8, 10, 11, 12}; // para cada coluna

//Função que gera o objeto keypad com todas as carcteristicas fornecidas
Keypad keypad = Keypad(makeKeymap(teclas), pinosLinhas, pinosColunas, LINHAS, COLUNAS);

// =====================================================
// INTERACAO USUARIO
// =====================================================
//Exige uma resposta do usario positiva ou negativa.
bool confirmar(){
  while(true){
    char c = keypad.getKey();
    if(c == '#') return true;
    if(c == '*') return false;
  }
}
//Alguns Pinos definidos
// Motores
#define MOTOR_EN  3 // pino de Habilitação(Enable), responsavel por controlar a velocidade pelo sinal PWM

// Seguidor de linha
#define LDR_ESQ A0 
#define LDR_CEN A1
#define LDR_DIR A2

// Estação
#define LDR_EST_ESQ A3
#define LDR_EST_DIR 0

// =====================================================
// PCF8574
// =====================================================
//Extensor de pinos com 8 disponiveis, se baseia no conceito de byte.
#define PCF8574 0x20 //Endereço de rede do PCF
// Bits dos motores
#define BIT_MOTOR_E_IN1 0 // 0 e o 1 são os pinos de direção sua combinação é responsavel por dizer 
#define BIT_MOTOR_E_IN2 1 //se o motor esquerdo  vai pra frente, ou pra tras.
#define BIT_MOTOR_D_IN1 2 //Mesma explicação para o motor direito
#define BIT_MOTOR_D_IN2 3

// Bits dos LEDs (novos)
#define BIT_LED_VERDE    4 // indo para a estação D
#define BIT_LED_AMARELO  5 // C
#define BIT_LED_VERMELHO 6 // B
#define BIT_LED_BRANCO   7 // A

// motores em LOW (bits 0-3) e LEDs apagados (bits 4,5,6,7 em HIGH, já que são ativos em LOW)
byte estadoPCF = 0b00000000;
byte estadoAtualPCF = 0xFF; // Guarda o último byte enviado ao PCF

void atualizarPCF() {
  // Se o estado não mudou, economiza processamento e não envia nada
  if (estadoPCF == estadoAtualPCF) return;

  Wire.beginTransmission(PCF8574);
  Wire.write(estadoPCF);
  Wire.endTransmission();

  estadoAtualPCF = estadoPCF; // Atualiza o estado gravado
}

//Altera o valor de um pino
void definirPinoPCF(byte pino, bool nivel) {
  if (nivel) estadoPCF |= (1 << pino);
  else       estadoPCF &= ~(1 << pino);
}
//Le o valor de um pino
bool lerPinoPCF(byte pino) {
  Wire.requestFrom((uint8_t)PCF8574, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read() & (1 << pino);
  }
  return false;
}

// ===== LEDS =====
// LOW = desligado, HIGH = ligado
//
//Para garantir que quando acender um led os outros estejam apagados
void desligarTodosLeds() {
  definirPinoPCF(BIT_LED_VERMELHO, LOW);
  definirPinoPCF(BIT_LED_AMARELO, LOW);
  definirPinoPCF(BIT_LED_VERDE, LOW);
  definirPinoPCF(BIT_LED_BRANCO, LOW);
  atualizarPCF();
}
//Acende um unico led baseado para onde o carrinho vai
void ledDestino(char destinoEscolhido) {
  desligarTodosLeds(); // garante só uma cor acesa por vez
  if (destinoEscolhido == 'A') {
    definirPinoPCF(BIT_LED_BRANCO, HIGH); 
  }
  else if (destinoEscolhido == 'B') {
    definirPinoPCF(BIT_LED_VERMELHO, HIGH);
  }
  else if (destinoEscolhido == 'C') {
    definirPinoPCF(BIT_LED_AMARELO, HIGH);
  }
  else if (destinoEscolhido == 'D') {
    definirPinoPCF(BIT_LED_VERDE, HIGH);
  }
  atualizarPCF();
}
// =====================================================
// PING 
// =====================================================
//São os sensores ultrassônicos responsaveis por identificar se há obstaculos no caminho
//Nesse Sensor Ultrassonico o ping substiui ao mesmo tempo o Trigger e o Echo, em um pino só
//Mapeamento dos Pinos do arduino
#define PING_ESQ 9
#define PING_DIR 2

unsigned long ultimaChecagemObstaculo = 0; //armazena o tempo que foi a ultima checagem
const unsigned long INTERVALO_OBSTACULO = 150; // checa a cada 150ms, não a cada iteração
bool obstaculoFoiDetectado = false;
//mede a distancia de um objeto pro sensor
long medirDistancia(int pino) {
  // Pulso de Trigger (Saída)
  pinMode(pino, OUTPUT);
  digitalWrite(pino, LOW);
  delayMicroseconds(2);
  digitalWrite(pino, HIGH);
  delayMicroseconds(5); 
  digitalWrite(pino, LOW);

  // Leitura do Echo (Entrada)
  pinMode(pino, INPUT);
  long tempo = pulseIn(pino, HIGH, 15000UL);
  return tempo / 29 / 2; // Converte o tempo de viagem do som em centímetros
}
//Consegue detectar um objeto a 30 cm do sensor esquerdo ou direito
bool obstaculoDetectado(){
  if (millis() - ultimaChecagemObstaculo < INTERVALO_OBSTACULO) {
    return obstaculoFoiDetectado; // reaproveita o último resultado
  }
  ultimaChecagemObstaculo = millis();

  long distEsq = medirDistancia(PING_ESQ);
  if (distEsq <= DISTANCIA_OBSTACULO && distEsq > 2) { obstaculoFoiDetectado = true; return true; }
  long distDir = medirDistancia(PING_DIR);
  obstaculoFoiDetectado = (distDir <= DISTANCIA_OBSTACULO && distDir > 2);
  return obstaculoFoiDetectado;
}

// =====================================================
// COMPARTIMENTO(Servo Tampa e Objetos)
// =====================================================
#define SERVO_TAMPA 13 //pino arduino
Servo servoTampa;
void abrirTampa(){
  int graus = POSICAO_FECHADA;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Abrindo Tranca");
  for(graus; graus <= POSICAO_ABERTA; graus += 5){
    servoTampa.write(graus);
    delay(15);
  }
  lcd.clear();
  delay(50);
}

void fecharTampa(){
  int graus = POSICAO_ABERTA;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fechando Tranca");
  for(graus; graus >= POSICAO_FECHADA; graus -= 5){
    servoTampa.write(graus);
    delay(15);
  }
  lcd.clear();
  delay(50);
}
//Conferir que o usuario colocou o objeto no compartimento, essa função abre a tampa, coloca o objeto, gera a senha, fecha a tampa 
void colocarObjeto(){
  abrirTampa();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Coloque o objeto");
  lcd.setCursor(0,1);
  lcd.print("Confirme #");
  confirmar();
  gerarSenha();
  fecharTampa();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Deseja continuar");
  lcd.setCursor(0,1);
  lcd.print("Sim(#) Nao(*)");
  if(confirmar()){
    estadoAtual = PREPARACAO;
  }else{
    estadoAtual = ESCOLHENDO_DESTINO;
  }
}
//Conferir que o usuario retirou o objeto no compartimento
void retirarObjeto(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Retire o objeto");
  lcd.setCursor(0,1);
  lcd.print("Confirme #");
  confirmar();
  fecharTampa();
  estadoAtual = ESCOLHENDO_DESTINO;
}

// ===== MOTORES =====
//Se a optar pelo caminho C, a primeira manobra é seguir em frente
int estadoMotorAtual = -99; // Controle para evitar I2C Flooding
void moveForward(int velocidade) {
  if (estadoMotorAtual == 0) return; // Se já está indo reto, não repete o comando I2C
  estadoMotorAtual = 0;

  definirPinoPCF(BIT_MOTOR_E_IN1, HIGH);
  definirPinoPCF(BIT_MOTOR_E_IN2, LOW);
  definirPinoPCF(BIT_MOTOR_D_IN1, HIGH);
  definirPinoPCF(BIT_MOTOR_D_IN2, LOW);
  atualizarPCF();
  analogWrite(MOTOR_EN, velocidade); // pino único, PWM nativo
  lcd.setCursor(14, 0); // Apenas estetico
  lcd.print("^");
  lcd.setCursor(13, 1);
  lcd.print(" | ");
}
//Se optar pelo caminho B, a primeira manobra é dobrar à esquerda
void turnLeft(int velocidade) {
  if (estadoMotorAtual == -1) return; // Se já está virando à esquerda, ignora
  estadoMotorAtual = -1;
  definirPinoPCF(BIT_MOTOR_E_IN1, LOW);
  definirPinoPCF(BIT_MOTOR_E_IN2, HIGH);
  definirPinoPCF(BIT_MOTOR_D_IN1, HIGH);
  definirPinoPCF(BIT_MOTOR_D_IN2, LOW);
  atualizarPCF();
  analogWrite(MOTOR_EN, velocidade);
  lcd.setCursor(14, 0);
  lcd.print(" ");
  lcd.setCursor(13, 1);
  lcd.print("<- ");
}
//Se optar pelo caminho D, a primeira manobra é dobrar à direita
void turnRight(int velocidade) {
  if (estadoMotorAtual == 1) return; // Se já está virando à direita, ignora
  estadoMotorAtual = 1;
  definirPinoPCF(BIT_MOTOR_E_IN1, HIGH);
  definirPinoPCF(BIT_MOTOR_E_IN2, LOW);
  definirPinoPCF(BIT_MOTOR_D_IN1, LOW);
  definirPinoPCF(BIT_MOTOR_D_IN2, HIGH);
  atualizarPCF();
  analogWrite(MOTOR_EN, velocidade);
  lcd.setCursor(14, 0);
  lcd.print(" ");
  lcd.setCursor(13, 1);
  lcd.print(" ->");
}
//Para os motores
void stopMotors() {
  if (estadoMotorAtual == 2) return; // Se já está parado, ignora
  estadoMotorAtual = 2;
  analogWrite(MOTOR_EN, 0);
  definirPinoPCF(BIT_MOTOR_E_IN1, LOW);
  definirPinoPCF(BIT_MOTOR_E_IN2, LOW);
  definirPinoPCF(BIT_MOTOR_D_IN1, LOW);
  definirPinoPCF(BIT_MOTOR_D_IN2, LOW);
  atualizarPCF();
}
void manobraU(int velocidade) {
  // Mesmo giro da curva à esquerda, só que por mais tempo (TEMPO_U).
  turnLeft(velocidade);
}

// ===== SEGUIDOR DE LINHA =====
void followTrack() {
  //Leitura dos 3 LDR's
  int esquerda = analogRead(LDR_ESQ);
  int centro = analogRead(LDR_CEN);
  int direita = analogRead(LDR_DIR);

  bool pistaEsquerda = esquerda < LIMIAR_LDR; //Sensor Visualiza a linha(escura) na esquerda
  bool pistaCentro = centro < LIMIAR_LDR; //no centro
  bool pistaDireita = direita < LIMIAR_LDR;//na direita

  if (pistaEsquerda && !pistaCentro && !pistaDireita) { //Se só o sensor esquerdo visualiza a linha logo deve curvar à esquerda
 
    turnLeft(VELOCIDADE);
    
    ultimaDirecao = -1; //esquerda
  }

  else if (pistaEsquerda && pistaCentro && !pistaDireita) {

    turnLeft(VELOCIDADE);
    ultimaDirecao = -1;
  }

  else if (!pistaEsquerda && pistaCentro && !pistaDireita) {

    moveForward(VELOCIDADE);
    ultimaDirecao = 0; // centro
  }

  else if (!pistaEsquerda && pistaCentro && pistaDireita) {

    turnRight(VELOCIDADE);
    ultimaDirecao = 1; //direita
  }

  else if (!pistaEsquerda && !pistaCentro && pistaDireita) {

    turnRight(VELOCIDADE);
    ultimaDirecao = 1;
  }

  else if (!pistaEsquerda && !pistaCentro && !pistaDireita) {

    if (ultimaDirecao == -1) {
      turnLeft(VELOCIDADE);
    }
    else if (ultimaDirecao == 1) {
      turnRight(VELOCIDADE);
    }
    else {
      stopMotors();
    }
  }

  else {

    moveForward(VELOCIDADE);
    ultimaDirecao = 0;
  }
}
// ===== DETECÇÃO DE ESTAÇÃO =====
//detecta atraves dos dois sensores LDR se a linha abaixo de cada um é preta, logo estação encontrada 
bool estacaoDetectada() {
  int esquerda = analogRead(LDR_EST_ESQ);
  int direita = digitalRead(LDR_EST_DIR);

  bool estacaoEsquerda = esquerda < LIMIAR_LDR_ESTACAO;
  bool estacaoDireita = direita == LOW;

  return estacaoEsquerda && estacaoDireita;
}

// ===== NAVEGAÇÃO =====
//Baseado em qual estação o carrinho esta e para qual ele quer ir obtem a direção que tem que apontar
byte obterDirecaoDestino(char origem, char destinoEscolhido) {
  if (origem == 'A') {
    if (destinoEscolhido == 'B') return LESTE;
    if (destinoEscolhido == 'C') return NORTE;
    if (destinoEscolhido == 'D') return OESTE;
  }

  if (origem == 'B') {
    if (destinoEscolhido == 'A') return SUL;
    if (destinoEscolhido == 'C') return NORTE;
    if (destinoEscolhido == 'D') return LESTE;
  }

  if (origem == 'C') {
    if (destinoEscolhido == 'A') return SUL;
    if (destinoEscolhido == 'B') return LESTE;
    if (destinoEscolhido == 'D') return OESTE;
  }

  if (origem == 'D') {
    if (destinoEscolhido == 'A') return LESTE;
    if (destinoEscolhido == 'B') return OESTE;
    if (destinoEscolhido == 'C') return NORTE;
  }

  return direcaoAtual;
}
//Quando o carrinho chegar no destino identifica para qual direção esta apontando
byte obterOrientacaoChegada(char origem, char destinoEscolhido) {
  if (origem == 'A' && destinoEscolhido == 'B') return NORTE;
  if (origem == 'A' && destinoEscolhido == 'C') return NORTE;
  if (origem == 'A' && destinoEscolhido == 'D') return OESTE;

  if (origem == 'B' && destinoEscolhido == 'A') return OESTE;
  if (origem == 'B' && destinoEscolhido == 'C') return OESTE;
  if (origem == 'B' && destinoEscolhido == 'D') return LESTE;

  if (origem == 'C' && destinoEscolhido == 'A') return SUL;
  if (origem == 'C' && destinoEscolhido == 'B') return SUL;
  if (origem == 'C' && destinoEscolhido == 'D') return SUL;

  if (origem == 'D' && destinoEscolhido == 'A') return LESTE;
  if (origem == 'D' && destinoEscolhido == 'B') return OESTE;
  if (origem == 'D' && destinoEscolhido == 'C') return LESTE;

  return direcaoAtual;
}
//Baseado na direção em que o carrinho se encontra e a direção que ele precisa apontar, executa uma manobra
byte calcularManobra(byte atual, byte desejada) {
  if (atual == desejada) {
    return RETO;
  }

  if (
    (atual == NORTE && desejada == LESTE) ||
    (atual == LESTE && desejada == SUL) ||
    (atual == SUL && desejada == OESTE) ||
    (atual == OESTE && desejada == NORTE)
  ) {
    return DIREITA;
  }

  if (
    (atual == NORTE && desejada == OESTE) ||
    (atual == OESTE && desejada == SUL) ||
    (atual == SUL && desejada == LESTE) ||
    (atual == LESTE && desejada == NORTE)
  ) {
    return ESQUERDA;
  }

  return U;
}

// ===== SENHA e CÓDIGO DE MANUTENÇÃO=====
String senhaGerada; //senha gerada a cada viagem aleatoriamente, como segurança
String senhaDigitada = "";//armazena os digitos

// Controle de tentativas da senha da viagem
byte tentativasSenha = 0;  
const byte MAX_TENTATIVAS_SENHA = 3;

// Código de manutenção para desbloqueio
String senhaManutencao = "1408";//Em caso de ultrapassar o limite de tentativas da senha.
String codigoDigitado = "";
//Gera a senah randomicamente
void gerarSenha(){
  randomSeed(micros()); // Gera a semente randomica para a futura senha
  senhaGerada = "";
  for(int i = 0; i < 4; i++){ //4 digitos
    senhaGerada += random(0, 10);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Senha: " + senhaGerada);
  lcd.setCursor(0, 1);
  lcd.print("Clique #");
  confirmar();
}
//processa a escrita da senha
bool DigitarSenha(){
  senhaDigitada = "";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite a senha:");
  while(true){
    char c = keypad.getKey();
    if(c != '\0' && c != 'A' && c != 'B' && c != 'C' && c != 'D'){
      if(c == '#'){
        lcd.clear();
        lcd.setCursor(0,0);
        if(senhaDigitada == senhaGerada){
          lcd.print("Senha Correta");
          lcd.setCursor(0,1);
          lcd.print("Acesso Liberado");
          delay(100);
          return true;
        }
        else{
          lcd.print("Senha Incorreta"); //Se esta incorreta
          lcd.setCursor(0,1);
          lcd.print("Acesso Negado");
          senhaDigitada = "";
          tentativasSenha++;
          delay(100);
          return false;
        }
      }
      else if(c == '*'){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Senha Limpa");
        senhaDigitada = "";
      }
      else{
        senhaDigitada += c;
        lcd.setCursor(0,1);
        lcd.print(senhaDigitada);
      }
    }
  }
}
//processa a escrita do codigo de manutenção
bool DigitarCodigo(){
  codigoDigitado = "";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Cod. Manutencao:");
  while(true){
    char c = keypad.getKey();
    if(c != '\0' && c != 'A' && c != 'B' && c != 'C' && c != 'D'){
      if(c == '#'){
        lcd.clear();
        lcd.setCursor(0,0);
        if(codigoDigitado == senhaManutencao){
          lcd.print("Cod. Manutencao");
          lcd.setCursor(0,1);
          lcd.print("Valido");
          delay(100);
          return true;
        }
        else{
          lcd.print("Cod. Manutencao"); //Se esta incorreta
          lcd.setCursor(0,1);
          lcd.print("Invalido");
          codigoDigitado = "";
          delay(100);
          return false;
        }
      }
      else if(c == '*'){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Codigo Limpo");
        codigoDigitado = "";
      }
      else{
        codigoDigitado += c;
        lcd.setCursor(0,1);
        lcd.print(codigoDigitado);
      }
    }
  }
}
//Valida ou não a senha
void pedirSenha(){
  tentativasSenha = 0;
  while(true){
    if(tentativasSenha < MAX_TENTATIVAS_SENHA){
      if(DigitarSenha()){
        estadoAtual = AGUARDANDO_RETIRAR;
        return; //Só para quando senha for correta
      } 
      else{
        if(tentativasSenha < MAX_TENTATIVAS_SENHA - 1){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Tente Novamente");
          delay(300);
        } 
      }
    }
    else{
      if(DigitarCodigo()) tentativasSenha = 0;
      else{
        lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Tente Novamente");
          delay(300);
      }
    }
  }
}

// ===== PREPARAR VIAGEM =====
//Obtem as informações necessarias para começar a viagem como origem e destino, direção, manobra e etc.
void prepararViagem() {
  origemViagem = estacaoAtual;

  direcaoDesejada = (Direcao)obterDirecaoDestino(origemViagem, destino);
  manobraAtual = (Manobra)calcularManobra(direcaoAtual, direcaoDesejada);

  saiuDaOrigem = false;
  pausadoPorObstaculo = false;

  ledDestino(destino);
  mostrarViagem();

  inicioManobra = millis();

  if (manobraAtual == RETO) {
    estadoAtual = SAINDO_DA_ORIGEM;
  }
  else {
    estadoAtual = MANOBRA_INICIAL;
  }
}


// ===== MANOBRA INICIAL =====
//Faz alguma manobra e verifica se há obstaculo há 30 cm 
void executarManobraInicial() {
  if (obstaculoDetectado()) {
    stopMotors();

    if (!pausadoPorObstaculo) {
      pausadoPorObstaculo = true;
      inicioPausa = millis();
    }
    return;
  }

  // Desconta do cronômetro da manobra o tempo em que ficou parado.
  if (pausadoPorObstaculo) {
    inicioManobra += (millis() - inicioPausa);
    pausadoPorObstaculo = false;
  }

  unsigned long tempo = millis() - inicioManobra;

  if (manobraAtual == DIREITA) {
    turnRight(VELOCIDADE);

    if (tempo >= TEMPO_CURVA) {
      stopMotors();
      estadoAtual = SAINDO_DA_ORIGEM;
    }
  }
  else if (manobraAtual == ESQUERDA) {
    turnLeft(VELOCIDADE);

    if (tempo >= TEMPO_CURVA) {
      stopMotors();
      estadoAtual = SAINDO_DA_ORIGEM;
    }
  }
  else if (manobraAtual == U) {
    manobraU(VELOCIDADE);

    if (tempo >= TEMPO_U) {
      stopMotors();
      estadoAtual = SAINDO_DA_ORIGEM;
    }
  }
  else {
    estadoAtual = SAINDO_DA_ORIGEM;
  }
}


// ===== SAIR DA ORIGEM =====
//segue a linha até sair da origem
void sairDaOrigem() {
  if (obstaculoDetectado()) {
    stopMotors();
    return;
  }

  followTrack();

  // Enquanto estiver sobre a marca da origem, não pode considerar
  // que já chegou ao destino.
  if (!estacaoDetectada()) {
    saiuDaOrigem = true;
    estadoAtual = EM_VIAGEM;
    mostrarViagem();
  }
}


// ===== VIAGEM =====
//Segue a linha, verifica obstaculos e encerra quando achar uma estacao
void executarViagem() {
  if (obstaculoDetectado()) {
    stopMotors();
    return;
  }

  followTrack();

  // Só procura estação depois de ter saído da origem.
  if (saiuDaOrigem && estacaoDetectada()) {
    stopMotors();

    estacaoAtual = destino;
    direcaoAtual = (Direcao)obterOrientacaoChegada(origemViagem, destino);

    desligarTodosLeds();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Chegou na");
    lcd.setCursor(0,1);
    lcd.print("Estacao ");
    lcd.print(destino);
    delay(200);
    estadoAtual = AGUARDANDO_SENHA;
  }
}
//Pede um dos 4 lugares possiveis
char selecionarIda(){
  while(true){
    char c = keypad.getKey();
    if(c == 'A' || c == 'B' || c == 'C' || c == 'D'){
      return c;
    }
  }
}
//Primeira interação com o usario, nela decide para onde o carrinho vai
void iniciar(){
  while(true){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Origem ");
    lcd.print(estacaoAtual);
    lcd.setCursor(0,1);
    lcd.print("Destino? (A-D)");
    char local = selecionarIda();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Quer ir para ");
    lcd.print(local);
    lcd.print("?");
    lcd.setCursor(0,1);
    lcd.print("Sim(#) ou Nao(*)");
    if(confirmar()){
      if(local != estacaoAtual){
        destino = local;
        return;
      }
      else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Local");
        lcd.setCursor(0,1);
        lcd.print("Invalido");
        delay(50);
      }
    }
  }
}

void setup() {
  // Motores
  pinMode(MOTOR_EN, OUTPUT); //Pino do Motor Enable usado para os dois motores DC
  // I2C
  Wire.begin();
  Wire.setClock(400000L); // Aumenta a velocidade do I2C de 100kHz para 400kHz
  // LCD
  lcd.init();
  lcd.backlight();
  //LRD
  pinMode(LDR_EST_DIR, INPUT);
  // Servo
  servoTampa.attach(SERVO_TAMPA);
  servoTampa.write(POSICAO_FECHADA);
  // Motores parados
  stopMotors();
  desligarTodosLeds();
  estacaoAtual = 'A';
  direcaoAtual = NORTE;
  estadoAtual = ESCOLHENDO_DESTINO;
}


// ===== LOOP =====

void loop() {
  //Fluxo principal do carrinho em ordem cronologica
  switch (estadoAtual) {
    case ESCOLHENDO_DESTINO:
      iniciar();
      colocarObjeto();
      break;
    case PREPARACAO:
      prepararViagem();
      break;
    case MANOBRA_INICIAL:
      executarManobraInicial();
      break;

    case SAINDO_DA_ORIGEM:
      sairDaOrigem();
      break;

    case EM_VIAGEM:
      executarViagem();
      break;

    case AGUARDANDO_SENHA:
      pedirSenha();
      break;

    case AGUARDANDO_RETIRAR:
      abrirTampa();
      retirarObjeto();
      break;
  }
}

