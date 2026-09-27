# Eletrônica e Firmware

## 1. Visão geral

O **ROBÔ ENTREGADOR** é um protótipo de robô móvel autônomo desenvolvido para transportar uma carga entre quatro estações identificadas pelas letras **A, B, C e D**.

A implementação eletrônica combina:

* Arduino;
* dois motores DC;
* driver L293D;
* três sensores LDR para seguimento de linha;
* sensores para identificação das estações;
* dois sensores ultrassônicos;
* micro servo para acionamento do compartimento de carga;
* display LCD 16×2 com comunicação I²C;
* expansor de I/O PCF8574;
* teclado matricial 4×4;
* comunicação I²C entre dois Arduinos.

O sistema de controle é organizado por uma **máquina de estados**, permitindo separar as etapas de interação com o usuário, preparação da carga, navegação e entrega.

---

## 2. Arquitetura eletrônica

O sistema utiliza dois Arduinos.

### Arduino principal

O Arduino principal concentra a lógica de controle e é responsável por:

* controle dos motores;
* leitura dos sensores de linha;
* detecção das estações;
* leitura dos sensores ultrassônicos;
* controle do servo;
* controle do LCD;
* controle dos LEDs através do PCF8574;
* comunicação com o Arduino do teclado;
* geração das senhas;
* validação das senhas;
* controle das tentativas;
* lógica de navegação;
* máquina de estados.

### Arduino 2

O segundo Arduino é dedicado ao teclado matricial 4×4.

Ele:

1. realiza a varredura das linhas e colunas;
2. identifica a tecla pressionada;
3. armazena temporariamente o caractere;
4. disponibiliza a tecla através do barramento I²C.

O Arduino 2 funciona como **escravo I²C no endereço `0x10`**.

Essa divisão permite separar a leitura do teclado da lógica principal do robô.

---

# 3. Componentes principais

| Componente          | Função                                 |
| ------------------- | -------------------------------------- |
| Arduino principal   | Controle central do robô               |
| Arduino 2           | Leitura do teclado                     |
| L293D               | Acionamento dos motores DC             |
| 2 motores DC        | Deslocamento do robô                   |
| 3 LDRs              | Seguimento da linha                    |
| Sensores de estação | Identificação das estações             |
| 2 HC-SR04           | Detecção de obstáculos                 |
| Micro servo         | Abertura e fechamento do compartimento |
| LCD 16×2 I²C        | Interface de usuário                   |
| PCF8574             | Expansão de saídas para LEDs           |
| Teclado 4×4         | Entrada de comandos e senhas           |

---

# 4. Pinagem do Arduino principal

## 4.1 Motores

| Pino | Função                |
| ---- | --------------------- |
| D5   | PWM do motor esquerdo |
| D6   | PWM do motor direito  |
| D7   | IN1 do motor esquerdo |
| D8   | IN2 do motor esquerdo |
| D9   | IN1 do motor direito  |
| D10  | IN2 do motor direito  |

O controle dos motores é realizado através do **L293D**.

A velocidade padrão configurada no firmware é:

```text
128 / 255
```

---

## 4.2 Sensores do seguidor de linha

| Pino | Sensor       |
| ---- | ------------ |
| A0   | LDR esquerdo |
| A1   | LDR central  |
| A2   | LDR direito  |

O firmware utiliza:

```text
LIMIAR_LDR = 300
```

A condição utilizada é:

```text
valor < 300 → linha detectada
```

---

## 4.3 Sensores de estação

| Pino | Função                            |
| ---- | --------------------------------- |
| A3   | LDR esquerdo da estação           |
| D4   | Sensor digital direito da estação |

Uma estação somente é considerada detectada quando as duas condições são satisfeitas:

```text
LDR esquerdo < 100
E
sensor digital = LOW
```

A utilização simultânea dos dois sensores reduz a possibilidade de uma leitura isolada ser interpretada como uma estação.

---

## 4.4 Sensores ultrassônicos

### Sensor esquerdo

| Função | Pino |
| ------ | ---- |
| TRIG   | D11  |
| ECHO   | D12  |

### Sensor direito

| Função | Pino |
| ------ | ---- |
| TRIG   | D3   |
| ECHO   | D13  |

O limite configurado para detecção de obstáculos é:

```text
30 cm
```

---

## 4.5 Servo

| Pino | Função                 |
| ---- | ---------------------- |
| D2   | Servo do compartimento |

Posições utilizadas pelo firmware:

```text
0°  → fechado
90° → aberto
```

O tempo configurado para movimentação é de:

```text
500 ms
```

---

# 5. Seguidor de linha

O robô utiliza três LDRs para acompanhar a pista.

As leituras são convertidas em estados booleanos de acordo com o limiar configurado.

### Principais combinações

| Esquerdo | Centro | Direito | Ação                    |
| -------- | ------ | ------- | ----------------------- |
| 1        | 0      | 0       | Virar à esquerda        |
| 1        | 1      | 0       | Virar à esquerda        |
| 0        | 1      | 0       | Seguir em frente        |
| 0        | 1      | 1       | Virar à direita         |
| 0        | 0      | 1       | Virar à direita         |
| 0        | 0      | 0       | Utilizar última direção |
| Outros   |        |         | Seguir em frente        |

O firmware mantém a variável:

```cpp
ultimaDirecao
```

Ela registra a última tendência de movimento:

```text
-1 → esquerda
 0 → frente
+1 → direita
```

Quando nenhum sensor detecta a linha momentaneamente, o robô utiliza essa informação para tentar recuperar a trajetória.

---

# 6. Controle dos motores

O firmware possui quatro movimentos básicos.

### Frente

Os dois motores giram para frente.

### Direita

O motor esquerdo gira para frente e o direito para trás.

### Esquerda

O motor esquerdo gira para trás e o direito para frente.

### Parada

Os dois sinais PWM são zerados.

Essa configuração também permite realizar giros aproximadamente sobre o próprio eixo.

---

# 7. Detecção de obstáculos

A detecção de obstáculos é realizada por dois sensores ultrassônicos.

O firmware mede a distância dos dois sensores e considera que existe obstáculo quando qualquer um deles retorna uma distância inferior a:

```text
30 cm
```

Quando um obstáculo é identificado:

```text
motores → parada
```

O robô permanece parado enquanto a condição de obstáculo persistir.

---

## 7.1 Timeout dos sensores

A leitura dos HC-SR04 utiliza:

```cpp
pulseIn(echo, HIGH, 30000)
```

Portanto, o tempo máximo de espera pelo eco é de aproximadamente:

```text
30 ms
```

Quando não há retorno dentro desse período, o firmware considera que não existe obstáculo dentro do alcance utilizado.

---

## 7.2 Cache das leituras

As leituras dos sensores ultrassônicos são atualizadas em intervalos de aproximadamente:

```text
100 ms
```

Isso evita executar continuamente os dois `pulseIn()` em todas as iterações do `loop()`.

Dessa forma, o sistema mantém uma resposta adequada aos obstáculos sem bloquear excessivamente o restante da lógica.

---

# 8. Detecção das estações

As estações são identificadas através da combinação de dois sensores.

O firmware utiliza:

```cpp
LDR_EST_ESQ = A3
LDR_EST_DIR = 4
```

A estação é considerada detectada somente quando:

```text
LDR da estação < 100
```

e:

```text
sensor digital = LOW
```

Essa condição é importante durante a navegação porque o robô utiliza a detecção da estação para determinar quando chegou ao destino.

---

# 9. Sistema de navegação

O sistema possui quatro estações:

```text
A
B
C
D
```

Além da estação atual, o firmware mantém a orientação física do robô.

As orientações possíveis são:

```text
NORTE
LESTE
SUL
OESTE
```

Assim, o estado de navegação não é representado somente por:

```text
estação atual = B
```

mas também por:

```text
estação atual = B
orientação atual = OESTE
```

Essa informação permite calcular a manobra necessária para a próxima viagem.

---

# 10. Direção do destino

A função:

```cpp
obterDirecaoDestino()
```

determina a direção física necessária para cada combinação de origem e destino.

As rotas implementadas são:

| Origem | Destino | Direção |
| ------ | ------- | ------- |
| A      | B       | Leste   |
| A      | C       | Norte   |
| A      | D       | Oeste   |
| B      | A       | Sul     |
| B      | C       | Norte   |
| B      | D       | Leste   |
| C      | A       | Sul     |
| C      | B       | Leste   |
| C      | D       | Oeste   |
| D      | A       | Leste   |
| D      | B       | Oeste   |
| D      | C       | Norte   |

---

# 11. Orientação após a chegada

A função:

```cpp
obterOrientacaoChegada()
```

atualiza a orientação física do robô após cada deslocamento.

Isso permite que uma viagem seguinte utilize a orientação obtida na viagem anterior.

A posição e a orientação são, portanto, informações independentes.

---

# 12. Cálculo da manobra

A função:

```cpp
calcularManobra()
```

compara:

```text
direcaoAtual
```

com:

```text
direcaoDesejada
```

e seleciona uma das seguintes manobras:

```text
RETO
DIREITA
ESQUERDA
U
```

### Exemplo

Se:

```text
direção atual = NORTE
direção desejada = LESTE
```

a manobra é:

```text
DIREITA
```

Se:

```text
direção atual = NORTE
direção desejada = OESTE
```

a manobra é:

```text
ESQUERDA
```

Quando as direções são opostas, o firmware utiliza:

```text
U
```

---

# 13. Manobra inicial

Antes de iniciar o seguimento da linha, o robô executa a manobra necessária para alinhar sua orientação com a direção do deslocamento.

Os tempos configurados são:

```text
Curva: 700 ms
Retorno U: 1400 ms
```

Essa etapa é controlada pelo estado:

```text
MANOBRA_INICIAL
```

Se o robô estiver na direção correta, nenhuma curva adicional é realizada.

---

# 14. Saída da estação de origem

O estado:

```text
SAINDO_DA_ORIGEM
```

evita que a própria estação de origem seja imediatamente interpretada como destino.

O fluxo é:

```text
Manobra inicial
      ↓
Começa a seguir a linha
      ↓
Ainda está sobre a origem?
      ↓
Sim → continua
      ↓
Não
      ↓
EM_VIAGEM
```

Somente depois que a condição da estação de origem deixa de ser detectada o sistema passa a procurar a estação de destino.

---

# 15. Viagem

Durante o estado:

```text
EM_VIAGEM
```

o firmware executa continuamente:

1. verificação de obstáculos;
2. parada caso exista obstáculo;
3. seguimento da linha;
4. detecção de estação.

Quando uma estação é detectada depois que o robô deixou a origem:

```text
motores → parada
```

O firmware então atualiza:

```text
estacaoAtual
direcaoAtual
```

e inicia o processo de autenticação no destino.

---

# 16. Comunicação I²C

O Arduino principal utiliza o barramento I²C através dos pinos:

```text
A4 → SDA
A5 → SCL
```

Os dispositivos conectados ao barramento são:

| Dispositivo | Endereço |
| ----------- | -------- |
| LCD 16×2    | `0x27`   |
| PCF8574     | `0x20`   |
| Arduino 2   | `0x10`   |

A velocidade configurada no firmware é:

```text
400 kHz
```

---

# 17. LCD

O display utilizado é um:

```text
LCD 16×2 I²C
```

com endereço:

```text
0x27
```

Ele apresenta as informações necessárias para a operação do robô.

Exemplos de telas:

```text
ORIGEM: A
DESTINO? (A-D)
```

```text
DESTINO: B
#=OK *=VOLTA
```

```text
COLOQUE A CARGA
#=OK *=CANCELA
```

```text
A -> B
EM VIAGEM
```

```text
DIGITE A SENHA
____
```

---

# 18. PCF8574 e LEDs

O PCF8574 é utilizado como expansor de entradas e saídas.

Endereço:

```text
0x20
```

Os LEDs utilizam lógica ativa em nível baixo:

```text
LOW  → ligado
HIGH → desligado
```

Mapeamento utilizado:

| Pino | Função                    |
| ---- | ------------------------- |
| P4   | LED da estação B          |
| P5   | LED da estação C          |
| P6   | LED da estação D          |
| P7   | LED associado à estação A |

Durante uma viagem, o LED correspondente ao destino é ativado.

Após a chegada, os LEDs são desligados.

---

# 19. Arduino 2 — teclado

O Arduino 2 funciona como escravo I²C:

```text
endereço = 0x10
```

O teclado utilizado é uma matriz 4×4.

### Linhas

| Linha | Pino |
| ----- | ---- |
| R1    | D2   |
| R2    | D3   |
| R3    | D4   |
| R4    | D5   |

### Colunas

| Coluna | Pino |
| ------ | ---- |
| C1     | D6   |
| C2     | D7   |
| C3     | D8   |
| C4     | D9   |

O mapeamento das teclas é:

```text
1  2  3  A
4  5  6  B
7  8  9  C
*  0  #  D
```

O Arduino 2 realiza a varredura da matriz e armazena a tecla pressionada.

Quando recebe uma requisição I²C do Arduino principal, envia o caractere armazenado.

---

# 20. Controle do teclado

O Arduino principal consulta o teclado somente nos estados em que uma entrada do usuário é necessária.

Durante:

```text
MANOBRA_INICIAL
SAINDO_DA_ORIGEM
EM_VIAGEM
```

não são realizadas requisições de teclado.

O intervalo mínimo entre leituras é:

```text
50 ms
```

No Arduino 2 também existe uma pequena espera para evitar múltiplas leituras da mesma tecla.

---

# 21. Compartimento de carga

O compartimento é acionado por um micro servo.

Posições:

```text
0°  → fechado
90° → aberto
```

O servo é acionado somente quando necessário.

### Abertura

Pode ocorrer:

* antes da colocação da carga;
* quando a viagem é cancelada após a carga ter sido colocada;
* quando a senha do destino é validada.

### Fechamento

Pode ocorrer:

* após a colocação da carga;
* durante o cancelamento;
* após a retirada da carga;
* durante a inicialização do sistema.

---

# 22. Sistema de senha

Cada viagem possui uma senha de quatro dígitos.

A senha é gerada através do gerador pseudoaleatório do Arduino:

```cpp
random(0, 10)
```

O gerador é inicializado utilizando:

```cpp
randomSeed(micros());
```

A senha é apresentada ao usuário antes do início da viagem.

O usuário precisa pressionar `#` para iniciar o deslocamento.

---

# 23. Cancelamento antes da viagem

Depois que a carga é colocada e a senha é apresentada, o usuário pode cancelar a viagem através da tecla:

```text
*
```

Nesse caso:

1. o compartimento é reaberto;
2. o usuário retira a carga;
3. o compartimento é fechado;
4. o sistema retorna à seleção de destino.

Essa etapa é representada pelo estado:

```text
DEVOLVENDO_CARGA
```

---

# 24. Autenticação no destino

Quando o robô chega ao destino, solicita a senha da viagem.

Os dígitos digitados são apresentados como:

```text
*
```

em vez dos valores reais.

A tecla:

```text
*
```

limpa a senha atual.

A tecla:

```text
#
```

confirma a entrada.

---

# 25. Senha correta

Quando a senha está correta:

```text
ACESSO LIBERADO
RETIRE A CARGA
```

O servo abre o compartimento.

O usuário retira a carga.

Depois, `#` ou `*` fecha o compartimento e o sistema retorna à seleção de uma nova viagem.

---

# 26. Tentativas de autenticação

O firmware permite até:

```text
3 tentativas
```

de senha para a viagem.

Após três tentativas incorretas, o sistema entra no estado:

```text
SISTEMA_BLOQUEADO
```

Nesse estado, a senha normal da viagem deixa de ser processada e o sistema solicita o código de manutenção definido no firmware.

O código de manutenção é armazenado diretamente no programa e deve ser tratado como uma informação de configuração do protótipo.

---

# 27. Máquina de estados

O funcionamento do sistema é organizado através dos seguintes estados:

```text
ESCOLHENDO_DESTINO
AGUARDANDO_CARGA
MOSTRANDO_SENHA
DEVOLVENDO_CARGA
MANOBRA_INICIAL
SAINDO_DA_ORIGEM
EM_VIAGEM
AGUARDANDO_SENHA
PORTA_ABERTA
SISTEMA_BLOQUEADO
```

Fluxo principal:

```text
ESCOLHENDO_DESTINO
        ↓
AGUARDANDO_CARGA
        ↓
MOSTRANDO_SENHA
        ↓
MANOBRA_INICIAL
        ↓
SAINDO_DA_ORIGEM
        ↓
EM_VIAGEM
        ↓
AGUARDANDO_SENHA
        ↓
PORTA_ABERTA
        ↓
ESCOLHENDO_DESTINO
```

Fluxo de cancelamento:

```text
MOSTRANDO_SENHA
        ↓
DEVOLVENDO_CARGA
        ↓
ESCOLHENDO_DESTINO
```

Fluxo de bloqueio:

```text
AGUARDANDO_SENHA
        ↓
3 tentativas incorretas
        ↓
SISTEMA_BLOQUEADO
        ↓
código de manutenção
        ↓
AGUARDANDO_SENHA
```

---

# 28. Estados do sistema

### `ESCOLHENDO_DESTINO`

O robô está parado aguardando a seleção da estação de destino.

### `AGUARDANDO_CARGA`

O compartimento está aberto aguardando a colocação da carga.

### `MOSTRANDO_SENHA`

A senha da viagem foi gerada e está sendo apresentada ao usuário.

### `DEVOLVENDO_CARGA`

A viagem foi cancelada depois que a carga foi colocada.

### `MANOBRA_INICIAL`

O robô executa a curva necessária antes de iniciar o deslocamento.

### `SAINDO_DA_ORIGEM`

O robô deixa a marca da estação inicial.

### `EM_VIAGEM`

O robô realiza o seguimento da linha e monitora obstáculos e estações.

### `AGUARDANDO_SENHA`

O robô chegou ao destino e aguarda a autenticação.

### `PORTA_ABERTA`

A senha foi validada e o usuário pode retirar a carga.

### `SISTEMA_BLOQUEADO`

O limite de tentativas foi atingido e o sistema aguarda o procedimento de manutenção.

---

# 29. Proteção contra obstáculos durante a navegação

A verificação de obstáculos possui prioridade sobre o seguimento de linha.

A lógica durante o deslocamento é:

```text
Verificar obstáculo
       ↓
Existe obstáculo?
   ↙          ↘
 SIM          NÃO
 ↓             ↓
Parar       Seguir linha
 ↓             ↓
Aguardar     Verificar estação
```

Se o obstáculo permanecer por aproximadamente:

```text
2 segundos
```

o LCD apresenta:

```text
OBSTACULO NA VIA
```

Esse aviso possui função visual e não altera a lógica de navegação.

Quando o caminho é liberado, o sistema retorna à indicação normal da viagem.

---

# 30. Inicialização

Durante o `setup()`, o firmware:

1. configura os pinos dos motores;
2. configura os sensores ultrassônicos;
3. configura o sensor digital de estação;
4. inicia o barramento I²C;
5. configura o I²C em 400 kHz;
6. inicializa o LCD;
7. desliga os LEDs;
8. para os motores;
9. fecha o compartimento;
10. define a estação inicial como A;
11. define a orientação inicial como Norte;
12. inicializa o gerador aleatório;
13. apresenta a tela inicial.

A condição inicial é:

```text
Estação = A
Orientação = Norte
```

---

# 31. Pinagem completa

| Pino | Função                     |
| ---- | -------------------------- |
| D0   | Livre                      |
| D1   | Livre                      |
| D2   | Servo                      |
| D3   | TRIG ultrassônico direito  |
| D4   | Sensor digital de estação  |
| D5   | PWM motor esquerdo         |
| D6   | PWM motor direito          |
| D7   | Direção motor esquerdo     |
| D8   | Direção motor esquerdo     |
| D9   | Direção motor direito      |
| D10  | Direção motor direito      |
| D11  | TRIG ultrassônico esquerdo |
| D12  | ECHO ultrassônico esquerdo |
| D13  | ECHO ultrassônico direito  |
| A0   | LDR linha esquerdo         |
| A1   | LDR linha central          |
| A2   | LDR linha direito          |
| A3   | LDR estação esquerdo       |
| A4   | SDA                        |
| A5   | SCL                        |

---

# 32. Parâmetros principais

| Parâmetro                            |   Valor |
| ------------------------------------ | ------: |
| Velocidade dos motores               |     128 |
| Limiar LDR de linha                  |     300 |
| Limiar LDR de estação                |     100 |
| Distância de obstáculo               |   30 cm |
| Timeout ultrassônico                 |   30 ms |
| Intervalo das leituras ultrassônicas |  100 ms |
| Intervalo de leitura do teclado      |   50 ms |
| Tempo de curva                       |  700 ms |
| Tempo da manobra U                   | 1400 ms |
| Tempo de movimentação do servo       |  500 ms |
| Máximo de tentativas da senha        |       3 |
| Endereço LCD                         |  `0x27` |
| Endereço PCF8574                     |  `0x20` |
| Endereço Arduino 2                   |  `0x10` |
| Frequência I²C                       | 400 kHz |

---

# 33. Organização do firmware

O firmware principal está dividido logicamente em módulos:

```text
Configuração de pinos
        ↓
Configurações
        ↓
Direções e manobras
        ↓
Máquina de estados
        ↓
Interface LCD
        ↓
Controle dos motores
        ↓
Seguidor de linha
        ↓
Detecção de estação
        ↓
Sensores ultrassônicos
        ↓
Servo
        ↓
LEDs / PCF8574
        ↓
Navegação
        ↓
Teclado / I²C
        ↓
Senhas
        ↓
Máquina de estados
        ↓
setup()
        ↓
loop()
```

Essa organização facilita a manutenção do código e separa as responsabilidades de cada subsistema.

---

# 34. Relação entre eletrônica e software

O funcionamento do robô depende da integração entre sensores, atuadores e software.

```text
             ┌──────────────┐
             │   SENSORES   │
             └──────┬───────┘
                    ↓
             ┌──────────────┐
             │   ARDUINO    │
             │   PRINCIPAL  │
             └──────┬───────┘
                    ↓
       ┌────────────┼────────────┐
       ↓            ↓            ↓
    MOTORES       SERVO       PCF8574
       ↓            ↓            ↓
   Movimento    Compart.      LEDs
       
                    ↕
                 I²C
                    ↕
             ┌──────────────┐
             │  ARDUINO 2   │
             └──────┬───────┘
                    ↓
               TECLADO 4×4
```

O Arduino principal integra as informações dos sensores com a máquina de estados e determina as ações do robô.

---

# 35. Arquivos relacionados

O firmware do projeto está separado em dois arquivos:

```text
arduino/
├── mestre/
│   └── mestre.ino
└── escravo/
    └── escravo.ino
```

O arquivo `mestre.ino` contém a lógica principal do robô.

O arquivo `escravo.ino` contém a implementação do Arduino responsável pelo teclado.

A simulação eletrônica complementar foi desenvolvida no **Tinkercad Circuits**.

