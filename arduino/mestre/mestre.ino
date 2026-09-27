#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ===== PINOS =====

// Motores
const byte MOTOR_ESQ_IN1 = 7;
const byte MOTOR_ESQ_IN2 = 8;
const byte MOTOR_ESQ_PWM = 5;

const byte MOTOR_DIR_IN1 = 9;
const byte MOTOR_DIR_IN2 = 10;
const byte MOTOR_DIR_PWM = 6;

// Seguidor de linha
const byte LDR_ESQ = A0;
const byte LDR_CEN = A1;
const byte LDR_DIR = A2;

// Estação
const byte LDR_EST_ESQ = A3;
const byte LDR_EST_DIR = 4;

// Ultrassônicos
const byte TRIG_ESQ = 11;
const byte ECHO_ESQ = 12;

const byte TRIG_DIR = 3;
const byte ECHO_DIR = 13;

// Servo
const byte PINO_SERVO = 2;

// I2C
const byte LCD_ENDERECO = 0x27;
const byte PCF_LED_ENDERECO = 0x20;
const byte TECLADO_ENDERECO = 0x10;

LiquidCrystal_I2C lcd(LCD_ENDERECO, 16, 2);
Servo servoPorta;


// ===== CONFIGURAÇÕES =====

const byte VELOCIDADE = 128;

const int LIMIAR_LDR = 300;
const int LIMIAR_LDR_ESTACAO = 100;

const int DISTANCIA_OBSTACULO = 30;

// Servo (ângulos reais, em graus)
const int ANGULO_SERVO_FECHADO = 0;
const int ANGULO_SERVO_ABERTO = 90;
const unsigned long TEMPO_MOVIMENTO_SERVO = 500;

// Manobras
const unsigned long TEMPO_CURVA = 700;
const unsigned long TEMPO_U = 1400;


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
  AGUARDANDO_CARGA,   
  MOSTRANDO_SENHA,
  DEVOLVENDO_CARGA,   
  MANOBRA_INICIAL,
  SAINDO_DA_ORIGEM,
  EM_VIAGEM,
  AGUARDANDO_SENHA,
  PORTA_ABERTA,
  SISTEMA_BLOQUEADO
};

Estado estadoAtual = ESCOLHENDO_DESTINO;


// ===== NAVEGAÇÃO =====

char estacaoAtual = 'A';
Direcao direcaoAtual = NORTE;
char origemViagem = 'A';
char destino = 'A';
bool destinoSelecionado = false;
Direcao direcaoDesejada = NORTE;
Manobra manobraAtual = RETO;


// ===== CONTROLE DA VIAGEM =====

unsigned long inicioManobra = 0;
bool saiuDaOrigem = false;

bool pausadoPorObstaculo = false;
unsigned long inicioPausa = 0;


// ===== SEGUIDOR =====

int ultimaDirecao = 0;


// ===== SENHA =====

char senhaViagem[5] = { '0', '0', '0', '0', '\0' };
char senhaDigitada[5] = { '\0', '\0', '\0', '\0', '\0' };
byte quantidadeSenha = 0;

// Controle de tentativas da senha da viagem
byte tentativasSenha = 0;
const byte MAX_TENTATIVAS_SENHA = 3;

// Código de manutenção para desbloqueio
char senhaManutencao[5] = { '1', '4', '0', '8', '\0' };
char codigoManutencao[5] = { '\0', '\0', '\0', '\0', '\0' };
byte quantidadeManutencao = 0;


// ===== CONTROLE DO TECLADO =====

unsigned long ultimaLeituraTeclado = 0;
const unsigned long INTERVALO_TECLADO = 50;


// ===== TELAS =====

void mostrarOrigemDestino() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ORIGEM: ");
  lcd.print(estacaoAtual);
  lcd.setCursor(0, 1);
  lcd.print("DESTINO? (A-D)");
}

void mostrarDestinoEscolhido() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DESTINO: ");
  lcd.print(destino);
  lcd.setCursor(0, 1);
  lcd.print("#=OK  *=VOLTA");
}

void mostrarAguardandoCarga() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COLOQUE A CARGA");
  lcd.setCursor(0, 1);
  lcd.print("#=OK  *=CANCELA");
}

void mostrarDevolvendoCarga() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RETIRE A CARGA");
  lcd.setCursor(0, 1);
  lcd.print("#=FECHAR");
}

void mostrarSenhaViagem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SENHA:");
  lcd.print(senhaViagem);
  lcd.setCursor(0, 1);
  lcd.print("#=INICIA *=VOLTA");
}

void mostrarViagem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(origemViagem);
  lcd.print(" -> ");
  lcd.print(destino);
  lcd.setCursor(0, 1);
  lcd.print("EM VIAGEM");
}

void mostrarChegada() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CHEGOU: ");
  lcd.print(destino);
  lcd.setCursor(0, 1);
  lcd.print("AGUARDE...");
}

void mostrarSenhaDestino() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DIGITE A SENHA");
  lcd.setCursor(0, 1);
  for (byte i = 0; i < 4; i++) {
    lcd.print(i < quantidadeSenha ? '*' : '_');
  }
}

void mostrarSenhaCorreta() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACESSO LIBERADO");
  lcd.setCursor(0, 1);
  lcd.print("RETIRE A CARGA");
}

void mostrarSenhaErrada(byte tentativasRestantes) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SENHA INCORRETA");
  lcd.setCursor(0, 1);

  if (tentativasRestantes > 0) {
    lcd.print("RESTAM ");
    lcd.print(tentativasRestantes);
    lcd.print(" TENT.");
  }
  else {
    lcd.print("TENTE NOVAMENTE");
  }

  delay(1200);

  mostrarSenhaDestino();
}

void mostrarSistemaBloqueado() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA BLOQ.");
  lcd.setCursor(0, 1);
  lcd.print("COD. MANUTENCAO");
}

void mostrarCodigoManutencao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COD. MANUTENCAO");
  lcd.setCursor(0, 1);

  for (byte i = 0; i < quantidadeManutencao; i++) {
    lcd.print('*');
  }
}


// ===== MOTORES =====

void motoresFrente(byte velocidade) {
  digitalWrite(MOTOR_ESQ_IN1, HIGH);
  digitalWrite(MOTOR_ESQ_IN2, LOW);

  digitalWrite(MOTOR_DIR_IN1, HIGH);
  digitalWrite(MOTOR_DIR_IN2, LOW);

  analogWrite(MOTOR_ESQ_PWM, velocidade);
  analogWrite(MOTOR_DIR_PWM, velocidade);
}

void virarDireita(byte velocidade) {
  digitalWrite(MOTOR_ESQ_IN1, HIGH);
  digitalWrite(MOTOR_ESQ_IN2, LOW);

  digitalWrite(MOTOR_DIR_IN1, LOW);
  digitalWrite(MOTOR_DIR_IN2, HIGH);

  analogWrite(MOTOR_ESQ_PWM, velocidade);
  analogWrite(MOTOR_DIR_PWM, velocidade);
}

void virarEsquerda(byte velocidade) {
  digitalWrite(MOTOR_ESQ_IN1, LOW);
  digitalWrite(MOTOR_ESQ_IN2, HIGH);

  digitalWrite(MOTOR_DIR_IN1, HIGH);
  digitalWrite(MOTOR_DIR_IN2, LOW);

  analogWrite(MOTOR_ESQ_PWM, velocidade);
  analogWrite(MOTOR_DIR_PWM, velocidade);
}

void manobraU(byte velocidade) {
  virarEsquerda(velocidade);
}

void pararMotores() {
  analogWrite(MOTOR_ESQ_PWM, 0);
  analogWrite(MOTOR_DIR_PWM, 0);
}


// ===== SEGUIDOR DE LINHA =====

void followTrack() {
  int esquerda = analogRead(LDR_ESQ);
  int centro = analogRead(LDR_CEN);
  int direita = analogRead(LDR_DIR);

  bool pistaEsquerda = esquerda < LIMIAR_LDR;
  bool pistaCentro = centro < LIMIAR_LDR;
  bool pistaDireita = direita < LIMIAR_LDR;

  if (pistaEsquerda && !pistaCentro && !pistaDireita) {
    virarEsquerda(VELOCIDADE);
    ultimaDirecao = -1;
  }
  else if (pistaEsquerda && pistaCentro && !pistaDireita) {
    virarEsquerda(VELOCIDADE);
    ultimaDirecao = -1;
  }
  else if (!pistaEsquerda && pistaCentro && !pistaDireita) {
    motoresFrente(VELOCIDADE);
    ultimaDirecao = 0;
  }
  else if (!pistaEsquerda && pistaCentro && pistaDireita) {
    virarDireita(VELOCIDADE);
    ultimaDirecao = 1;
  }
  else if (!pistaEsquerda && !pistaCentro && pistaDireita) {
    virarDireita(VELOCIDADE);
    ultimaDirecao = 1;
  }
  else if (!pistaEsquerda && !pistaCentro && !pistaDireita) {
    if (ultimaDirecao == -1) {
      virarEsquerda(VELOCIDADE);
    }
    else if (ultimaDirecao == 1) {
      virarDireita(VELOCIDADE);
    }
    else {
      pararMotores();
    }
  }
  else {
    motoresFrente(VELOCIDADE);
    ultimaDirecao = 0;
  }
}


// ===== DETECÇÃO DE ESTAÇÃO =====

bool estacaoDetectada() {
  int esquerda = analogRead(LDR_EST_ESQ);
  int direita = digitalRead(LDR_EST_DIR);

  bool estacaoEsquerda = esquerda < LIMIAR_LDR_ESTACAO;
  bool estacaoDireita = direita == LOW;

  return estacaoEsquerda && estacaoDireita;
}


// ===== ULTRASSÔNICOS =====

long medirDistancia(byte trig, byte echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  unsigned long duracao = pulseIn(echo, HIGH, 30000);

  if (duracao == 0) {
    return DISTANCIA_OBSTACULO + 1;
  }

  return duracao / 58;
}

unsigned long ultimaChecagemObstaculo = 0;
const unsigned long INTERVALO_OBSTACULO = 100;
bool obstaculoFoiDetectado = false;

bool obstaculoDetectado() {
  if (millis() - ultimaChecagemObstaculo < INTERVALO_OBSTACULO) {
    return obstaculoFoiDetectado;
  }

  ultimaChecagemObstaculo = millis();

  long esquerda = medirDistancia(TRIG_ESQ, ECHO_ESQ);

  if (esquerda < DISTANCIA_OBSTACULO) {
    obstaculoFoiDetectado = true;
    return true;
  }

  long direita = medirDistancia(TRIG_DIR, ECHO_DIR);

  obstaculoFoiDetectado = (direita < DISTANCIA_OBSTACULO);
  return obstaculoFoiDetectado;
}


// ===== SERVO =====

void servoAbrir() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ABRINDO...");

  servoPorta.attach(PINO_SERVO);
  servoPorta.write(ANGULO_SERVO_ABERTO);
  delay(TEMPO_MOVIMENTO_SERVO);
  servoPorta.detach();
}

void servoFechar() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FECHANDO...");

  servoPorta.attach(PINO_SERVO);
  servoPorta.write(ANGULO_SERVO_FECHADO);
  delay(TEMPO_MOVIMENTO_SERVO);
  servoPorta.detach();
}


// ===== LEDS (PCF8574) =====

void pcf8574Enviar(byte valor) {
  Wire.beginTransmission(PCF_LED_ENDERECO);
  Wire.write(valor);
  Wire.endTransmission();
}

void desligarTodosLeds() {
  pcf8574Enviar(0xFF);
}

void ledDestino(char destinoEscolhido) {
  byte valor = 0xFF;

  if (destinoEscolhido == 'A') {
    valor &= ~(1 << 7); // branco 
  }
  else if (destinoEscolhido == 'B') {
    valor &= ~(1 << 4); // vermelho
  }
  else if (destinoEscolhido == 'C') {
    valor &= ~(1 << 5); // amarelo
  }
  else if (destinoEscolhido == 'D') {
    valor &= ~(1 << 6); // verde
  }

  pcf8574Enviar(valor);
}


// ===== NAVEGAÇÃO =====

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


// ===== TECLADO =====

char lerTecla() {
  if (millis() - ultimaLeituraTeclado < INTERVALO_TECLADO) {
    return 0;
  }

  ultimaLeituraTeclado = millis();

  Wire.requestFrom(TECLADO_ENDERECO, (byte)1);

  if (Wire.available()) {
    char tecla = Wire.read();

    if (tecla != 0) {
      return tecla;
    }
  }

  return 0;
}


// ===== SENHA =====

void gerarSenha() {
  for (byte i = 0; i < 4; i++) {
    senhaViagem[i] = '0' + random(0, 10);
  }

  senhaViagem[4] = '\0';
}

void limparSenhaDigitada() {
  quantidadeSenha = 0;

  for (byte i = 0; i < 5; i++) {
    senhaDigitada[i] = '\0';
  }
}

bool senhaEstaCorreta() {
  if (quantidadeSenha != 4) {
    return false;
  }

  for (byte i = 0; i < 4; i++) {
    if (senhaDigitada[i] != senhaViagem[i]) {
      return false;
    }
  }

  return true;
}


// ===== CÓDIGO DE MANUTENÇÃO =====

void limparCodigoManutencao() {
  quantidadeManutencao = 0;

  for (byte i = 0; i < 5; i++) {
    codigoManutencao[i] = '\0';
  }
}

bool codigoManutencaoCorreto() {
  if (quantidadeManutencao != 4) {
    return false;
  }

  for (byte i = 0; i < 4; i++) {
    if (codigoManutencao[i] != senhaManutencao[i]) {
      return false;
    }
  }

  return true;
}


// ===== PREPARAR VIAGEM =====

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

void executarManobraInicial() {
  if (obstaculoDetectado()) {
    pararMotores();

    if (!pausadoPorObstaculo) {
      pausadoPorObstaculo = true;
      inicioPausa = millis();
    }

    tratarObstaculoNaViagem();
    return;
  }
  
  if (pausadoPorObstaculo) {
    inicioManobra += (millis() - inicioPausa);
    pausadoPorObstaculo = false;
  }

  limparAvisoObstaculo();

  unsigned long tempo = millis() - inicioManobra;

  if (manobraAtual == DIREITA) {
    virarDireita(VELOCIDADE);

    if (tempo >= TEMPO_CURVA) {
      pararMotores();
      estadoAtual = SAINDO_DA_ORIGEM;
    }
  }
  else if (manobraAtual == ESQUERDA) {
    virarEsquerda(VELOCIDADE);

    if (tempo >= TEMPO_CURVA) {
      pararMotores();
      estadoAtual = SAINDO_DA_ORIGEM;
    }
  }
  else if (manobraAtual == U) {
    manobraU(VELOCIDADE);

    if (tempo >= TEMPO_U) {
      pararMotores();
      estadoAtual = SAINDO_DA_ORIGEM;
    }
  }
  else {
    estadoAtual = SAINDO_DA_ORIGEM;
  }
}


// ===== AVISO DE OBSTÁCULO PERSISTENTE =====
// Puramente visual: se o robô ficar parado por obstáculo por tempo
// demais, avisa no LCD. Não interfere em nenhum timer de manobra nem
// na lógica de obstaculoDetectado()/movimento.

bool avisoObstaculoAtivo = false;
bool avisoObstaculoExibido = false;
unsigned long inicioObstaculoViagem = 0;
const unsigned long TEMPO_AVISO_OBSTACULO = 2000;

void tratarObstaculoNaViagem() {
  if (!avisoObstaculoAtivo) {
    avisoObstaculoAtivo = true;
    avisoObstaculoExibido = false;
    inicioObstaculoViagem = millis();
    return;
  }

  if (!avisoObstaculoExibido && millis() - inicioObstaculoViagem >= TEMPO_AVISO_OBSTACULO) {
    lcd.setCursor(0, 1);
    lcd.print("OBSTACULO NA VIA");
    avisoObstaculoExibido = true;
  }
}

void limparAvisoObstaculo() {
  if (avisoObstaculoAtivo) {
    avisoObstaculoAtivo = false;

    if (avisoObstaculoExibido) {
      mostrarViagem();
    }
  }
}


// ===== SAIR DA ORIGEM =====

void sairDaOrigem() {
  if (obstaculoDetectado()) {
    pararMotores();
    tratarObstaculoNaViagem();
    return;
  }

  limparAvisoObstaculo();
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

void executarViagem() {
  if (obstaculoDetectado()) {
    pararMotores();
    tratarObstaculoNaViagem();
    return;
  }

  limparAvisoObstaculo();
  followTrack();

  // Só procura estação depois de ter saído da origem.
  if (saiuDaOrigem && estacaoDetectada()) {
    pararMotores();

    estacaoAtual = destino;
    direcaoAtual = (Direcao)obterOrientacaoChegada(origemViagem, destino);

    desligarTodosLeds();
    limparSenhaDigitada();

    // Começa uma nova contagem de tentativas ao chegar ao destino.
    tentativasSenha = 0;

    mostrarChegada();
    delay(500);

    estadoAtual = AGUARDANDO_SENHA;
    mostrarSenhaDestino();
  }
}


// ===== ESCOLHA DO DESTINO =====

void processarEscolhaDestino(char tecla) {
  if (tecla == 0) {
    return;
  }

  if (tecla == '*') {
    destinoSelecionado = false;
    mostrarOrigemDestino();
    return;
  }

  if (tecla >= 'A' && tecla <= 'D') {
    char novoDestino = tecla;

    if (novoDestino == estacaoAtual) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DESTINO INVALIDO");
      lcd.setCursor(0, 1);
      lcd.print("ESCOLHA OUTRO");

      delay(1000);

      mostrarOrigemDestino();
      return;
    }

    destino = novoDestino;
    destinoSelecionado = true;
    mostrarDestinoEscolhido();
    return;
  }

  if (tecla == '#' && destinoSelecionado) {
    // Abre o compartimento para o usuário colocar a carga antes de
    // gerar a senha e iniciar a viagem.
    servoAbrir();

    estadoAtual = AGUARDANDO_CARGA;
    mostrarAguardandoCarga();
  }
}


// ===== AGUARDANDO CARGA (recebimento do item na origem) =====

void processarAguardandoCarga(char tecla) {
  if (tecla == 0) {
    return;
  }

  if (tecla == '#') {
    // Carga colocada: fecha o compartimento e só então gera a senha
    // que vai liberar o item no destino.
    servoFechar();

    gerarSenha();
    mostrarSenhaViagem();

    estadoAtual = MOSTRANDO_SENHA;
    return;
  }

  if (tecla == '*') {
    // Cancelou antes de confirmar a carga: fecha o compartimento
    // (ainda vazio) e volta para a escolha de destino.
    servoFechar();

    destinoSelecionado = false;
    mostrarOrigemDestino();

    estadoAtual = ESCOLHENDO_DESTINO;
  }
}


// ===== APÓS MOSTRAR A SENHA (espera # para começar) =====

void processarSenhaMostrada(char tecla) {
  if (tecla == 0) {
    return;
  }

  if (tecla == '#') {
    prepararViagem();
    return;
  }

  if (tecla == '*') {
    // A carga já está dentro do compartimento nesta etapa: reabre a
    // porta para o usuário poder retirá-la antes de cancelar de vez.
    servoAbrir();
    mostrarDevolvendoCarga();

    estadoAtual = DEVOLVENDO_CARGA;
  }
}


// ===== DEVOLVENDO CARGA (viagem cancelada após colocar o item) =====

void processarDevolvendoCarga(char tecla) {
  if (tecla == 0) {
    return;
  }

  if (tecla == '#' || tecla == '*') {
    servoFechar();

    destinoSelecionado = false;
    mostrarOrigemDestino();

    estadoAtual = ESCOLHENDO_DESTINO;
  }
}


// ===== SENHA NA ESTAÇÃO DE DESTINO =====

void processarSenhaDestino(char tecla) {
  if (tecla == 0) {
    return;
  }

  if (tecla == '*') {
    limparSenhaDigitada();
    mostrarSenhaDestino();
    return;
  }

  if (tecla >= '0' && tecla <= '9') {
    if (quantidadeSenha < 4) {
      senhaDigitada[quantidadeSenha] = tecla;
      quantidadeSenha++;
      mostrarSenhaDestino();
    }
    return;
  }

  if (tecla == '#') {
    if (senhaEstaCorreta()) {
      servoAbrir();
      mostrarSenhaCorreta();

      // Senha correta: zera as tentativas.
      tentativasSenha = 0;

      estadoAtual = PORTA_ABERTA;
    }
    else {
      tentativasSenha++;

      if (tentativasSenha >= MAX_TENTATIVAS_SENHA) {
        limparSenhaDigitada();
        limparCodigoManutencao();

        estadoAtual = SISTEMA_BLOQUEADO;

        mostrarSistemaBloqueado();

        delay(1500);

        mostrarCodigoManutencao();
      }
      else {
        mostrarSenhaErrada(MAX_TENTATIVAS_SENHA - tentativasSenha);
        limparSenhaDigitada();
      }
    }
  }
}


// ===== SISTEMA BLOQUEADO =====

void processarSistemaBloqueado(char tecla) {
  if (tecla == 0) {
    return;
  }

  if (tecla == '*') {
    limparCodigoManutencao();
    mostrarCodigoManutencao();
    return;
  }

  if (tecla >= '0' && tecla <= '9') {
    if (quantidadeManutencao < 4) {
      codigoManutencao[quantidadeManutencao] = tecla;
      quantidadeManutencao++;
      mostrarCodigoManutencao();
    }

    return;
  }

  if (tecla == '#') {
    if (codigoManutencaoCorreto()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MANUTENCAO OK");
      lcd.setCursor(0, 1);
      lcd.print("DESBLOQUEADO");

      delay(1500);

      limparCodigoManutencao();
      limparSenhaDigitada();

      tentativasSenha = 0;

      estadoAtual = AGUARDANDO_SENHA;
      mostrarSenhaDestino();
    }
    else {
      limparCodigoManutencao();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CODIGO ERRADO");
      lcd.setCursor(0, 1);
      lcd.print("TENTE NOVAMENTE");

      delay(1200);

      mostrarCodigoManutencao();
    }
  }
}


// ===== PORTA ABERTA =====

void processarPortaAberta(char tecla) {
  if (tecla == 0) {
    return;
  }

  if (tecla == '#' || tecla == '*') {
    servoFechar();

    limparSenhaDigitada();
    destinoSelecionado = false;
    mostrarOrigemDestino();

    estadoAtual = ESCOLHENDO_DESTINO;
  }
}


// ===== SETUP =====

void setup() {
  pinMode(MOTOR_ESQ_IN1, OUTPUT);
  pinMode(MOTOR_ESQ_IN2, OUTPUT);
  pinMode(MOTOR_ESQ_PWM, OUTPUT);

  pinMode(MOTOR_DIR_IN1, OUTPUT);
  pinMode(MOTOR_DIR_IN2, OUTPUT);
  pinMode(MOTOR_DIR_PWM, OUTPUT);

  pinMode(TRIG_ESQ, OUTPUT);
  pinMode(ECHO_ESQ, INPUT);

  pinMode(TRIG_DIR, OUTPUT);
  pinMode(ECHO_DIR, INPUT);

  pinMode(LDR_EST_DIR, INPUT);

  Wire.begin();
  Wire.setClock(400000L); // I2C a 400kHz: LCD, teclado e PCF respondem mais rápido

  lcd.init();
  lcd.backlight();

  desligarTodosLeds();
  pararMotores();

  // Garante que o compartimento comece trancado, independente da
  // posição em que o servo ficou antes de desligar o Arduino.
  servoFechar();

  estacaoAtual = 'A';
  direcaoAtual = NORTE;
  destinoSelecionado = false;
  estadoAtual = ESCOLHENDO_DESTINO;

  randomSeed(micros());

  mostrarOrigemDestino();
}


// ===== LOOP =====

void loop() {
  char tecla = 0;

  // O teclado só é acessado nos estados em que ele é realmente
  // necessário; durante manobra e viagem não há requisição I2C.
  if (
    estadoAtual == ESCOLHENDO_DESTINO ||
    estadoAtual == AGUARDANDO_CARGA ||
    estadoAtual == MOSTRANDO_SENHA ||
    estadoAtual == DEVOLVENDO_CARGA ||
    estadoAtual == AGUARDANDO_SENHA ||
    estadoAtual == PORTA_ABERTA ||
    estadoAtual == SISTEMA_BLOQUEADO
  ) {
    tecla = lerTecla();
  }

  switch (estadoAtual) {
    case ESCOLHENDO_DESTINO:
      processarEscolhaDestino(tecla);
      break;

    case AGUARDANDO_CARGA:
      processarAguardandoCarga(tecla);
      break;

    case MOSTRANDO_SENHA:
      processarSenhaMostrada(tecla);
      break;

    case DEVOLVENDO_CARGA:
      processarDevolvendoCarga(tecla);
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
      processarSenhaDestino(tecla);
      break;

    case PORTA_ABERTA:
      processarPortaAberta(tecla);
      break;

    case SISTEMA_BLOQUEADO:
      processarSistemaBloqueado(tecla);
      break;
  }
}

