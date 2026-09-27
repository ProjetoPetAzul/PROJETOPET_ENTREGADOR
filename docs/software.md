# Software, Controle e Navegação

## 1. Visão geral

O software do **ROBÔ ENTREGADOR** é responsável por integrar as informações dos sensores, controlar os atuadores e coordenar todo o fluxo de operação do robô.

O firmware principal foi desenvolvido para Arduino utilizando **C++**, enquanto um segundo Arduino é utilizado especificamente para a leitura do teclado matricial.

A lógica do robô é organizada através de uma **máquina de estados**, permitindo separar as diferentes etapas da operação:

```text
Seleção do destino
        ↓
Preparação da carga
        ↓
Geração da senha
        ↓
Preparação da navegação
        ↓
Deslocamento
        ↓
Chegada ao destino
        ↓
Autenticação
        ↓
Entrega da carga
        ↓
Nova viagem
```

---

# 2. Organização do software

O firmware principal está localizado em:

```text
arduino/
└── mestre/
    └── mestre.ino
```

O firmware responsável pelo teclado está localizado em:

```text
arduino/
└── escravo/
    └── escravo.ino
```

O `mestre.ino` concentra a lógica principal do robô.

O `escravo.ino` realiza a leitura do teclado e disponibiliza as teclas através da comunicação I²C.

---

# 3. Máquina de estados

O comportamento do robô é controlado por estados que representam cada etapa da operação.

Os estados implementados são:

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

A utilização dessa estrutura evita que todas as operações sejam executadas simultaneamente e permite que cada etapa tenha suas próprias condições de entrada, execução e saída.

---

# 4. Estado `ESCOLHENDO_DESTINO`

É o estado inicial de operação do robô.

O sistema permanece parado e aguarda que o usuário informe a estação de destino através do teclado.

As estações disponíveis são:

```text
A
B
C
D
```

O destino não pode ser igual à estação atualmente ocupada pelo robô.

Após a seleção, o usuário utiliza:

```text
# → confirmar
* → cancelar
```

Quando o destino é confirmado, o sistema passa para:

```text
AGUARDANDO_CARGA
```

---

# 5. Estado `AGUARDANDO_CARGA`

Nesse estado, o compartimento de carga é aberto.

O sistema aguarda que o usuário coloque a carga no compartimento.

O usuário pode:

```text
# → confirmar que a carga foi colocada
* → cancelar
```

Caso a operação seja confirmada, o sistema fecha o compartimento e prepara a viagem.

Caso o usuário cancele, o compartimento é fechado e o sistema retorna à seleção de destino.

---

# 6. Geração da senha da viagem

Após a confirmação da carga, o firmware gera uma senha de quatro dígitos para a viagem.

A geração utiliza:

```cpp
random(0, 10)
```

O gerador é inicializado através de:

```cpp
randomSeed(micros());
```

A senha é apresentada no LCD antes do início do deslocamento.

O usuário precisa confirmar o início da viagem através da tecla:

```text
#
```

---

# 7. Estado `MOSTRANDO_SENHA`

Nesse estado, a senha da viagem permanece disponível para o usuário.

A senha funciona como uma credencial para retirar a carga na estação de destino.

O usuário pode iniciar a viagem pressionando:

```text
#
```

Também é possível cancelar a viagem através de:

```text
*
```

Nesse caso, o sistema passa para:

```text
DEVOLVENDO_CARGA
```

---

# 8. Estado `DEVOLVENDO_CARGA`

Esse estado trata o cancelamento de uma viagem depois que a carga já foi colocada no compartimento.

O fluxo é:

```text
Viagem cancelada
       ↓
Abrir compartimento
       ↓
Usuário retira a carga
       ↓
Fechar compartimento
       ↓
Retornar à seleção de destino
```

Dessa forma, uma viagem pode ser cancelada antes do deslocamento sem deixar a carga presa dentro do compartimento.

---

# 9. Representação da posição

O sistema representa a posição do robô através da estação atual.

As estações disponíveis são:

```text
A
B
C
D
```

Além da estação, o firmware mantém a orientação física do robô.

As orientações possíveis são:

```text
NORTE
LESTE
SUL
OESTE
```

Portanto, a navegação utiliza duas informações:

```text
Estação atual
+
Orientação atual
```

Exemplo:

```text
Estação atual = B
Orientação = OESTE
```

---

# 10. Rotas

As rotas entre as estações são determinadas pela função:

```cpp
obterDirecaoDestino()
```

O firmware possui as seguintes relações:

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

Essa tabela permite transformar a escolha do usuário em uma direção física necessária para o deslocamento.

---

# 11. Cálculo da manobra

Depois de determinar a direção desejada, o firmware compara essa direção com a orientação atual do robô.

A função utilizada é:

```cpp
calcularManobra()
```

As possibilidades são:

```text
RETO
DIREITA
ESQUERDA
U
```

Exemplo:

```text
Orientação atual: NORTE
Direção desejada: LESTE
```

Resultado:

```text
DIREITA
```

Outro exemplo:

```text
Orientação atual: NORTE
Direção desejada: OESTE
```

Resultado:

```text
ESQUERDA
```

Quando a direção desejada é oposta à orientação atual, é utilizada uma manobra de retorno em U.

---

# 12. Estado `MANOBRA_INICIAL`

Antes de iniciar o deslocamento, o robô precisa alinhar sua orientação com a direção da viagem.

Esse procedimento ocorre no estado:

```text
MANOBRA_INICIAL
```

Os tempos configurados no firmware são:

```text
Curva: 700 ms
Manobra U: 1400 ms
```

Se a orientação atual já estiver alinhada com a direção desejada, o robô segue sem realizar uma curva adicional.

---

# 13. Estado `SAINDO_DA_ORIGEM`

Após realizar a manobra inicial, o robô precisa deixar a região correspondente à estação de origem.

Isso é necessário porque a estação de origem continua sendo detectável enquanto o robô ainda estiver sobre sua marca.

O estado:

```text
SAINDO_DA_ORIGEM
```

mantém o robô seguindo a linha até que a estação de origem deixe de ser detectada.

O fluxo é:

```text
MANOBRA_INICIAL
       ↓
Seguir linha
       ↓
Ainda detecta origem?
    ↙          ↘
  SIM          NÃO
   ↓             ↓
Continua      EM_VIAGEM
```

---

# 14. Seguidor de linha

Durante o deslocamento, o robô utiliza três LDRs para determinar a posição da linha.

As leituras correspondem a:

```text
LDR esquerdo
LDR central
LDR direito
```

O firmware utiliza o limiar:

```text
300
```

Uma leitura inferior a esse valor é interpretada como detecção da linha.

---

# 15. Estratégia de recuperação da linha

O software mantém a variável:

```cpp
ultimaDirecao
```

Ela registra a última tendência conhecida do movimento:

```text
-1 → esquerda
 0 → frente
+1 → direita
```

Quando nenhum dos três sensores detecta a linha momentaneamente, o firmware utiliza essa informação para tentar recuperar a trajetória.

Isso permite que uma perda momentânea da linha não resulte imediatamente em uma interrupção completa do deslocamento.

---

# 16. Controle do movimento

O firmware possui comandos para controlar o movimento dos motores.

Os principais comportamentos são:

```text
Frente
Direita
Esquerda
Parada
```

Os movimentos de curva são realizados através do acionamento diferenciado dos motores.

Para uma curva à direita:

```text
Motor esquerdo → frente
Motor direito  → trás
```

Para uma curva à esquerda:

```text
Motor esquerdo → trás
Motor direito  → frente
```

Para uma parada:

```text
PWM esquerdo → 0
PWM direito  → 0
```

---

# 17. Estado `EM_VIAGEM`

Esse é o principal estado de deslocamento do robô.

Durante a viagem, o software realiza continuamente:

```text
Verificação de obstáculos
        ↓
Seguimento da linha
        ↓
Detecção de estação
```

A detecção de obstáculos possui prioridade sobre o seguimento da linha.

Quando uma estação é detectada depois que o robô deixou a origem, ela é considerada a estação de destino.

---

# 18. Detecção do destino

Durante a viagem, o sistema verifica continuamente os sensores responsáveis pela identificação das estações.

Quando uma estação é detectada:

```text
Motores → parada
```

O firmware atualiza:

```text
estacaoAtual
direcaoAtual
```

A orientação obtida é calculada através de:

```cpp
obterOrientacaoChegada()
```

Em seguida, o sistema passa para a etapa de autenticação.

---

# 19. Atualização da orientação

A orientação física do robô é atualizada após cada viagem.

Isso permite que a próxima viagem considere a posição final deixada pela viagem anterior.

Assim, o planejamento de uma nova rota utiliza:

```text
estação atual
+
orientação atual
+
estação de destino
```

Essa abordagem permite calcular a próxima manobra sem precisar assumir que o robô sempre começa voltado para a mesma direção.

---

# 20. Detecção de obstáculos

A navegação possui dois sensores ultrassônicos.

O software verifica a distância medida pelos sensores e considera que existe um obstáculo quando qualquer um deles apresenta distância inferior a:

```text
30 cm
```

Quando um obstáculo é detectado:

```text
Parar motores
```

O robô permanece parado enquanto a condição de obstáculo persistir.

---

# 21. Atualização das leituras ultrassônicas

As leituras dos sensores ultrassônicos são atualizadas aproximadamente a cada:

```text
100 ms
```

A leitura utiliza:

```cpp
pulseIn(echo, HIGH, 30000)
```

O timeout utilizado é de aproximadamente:

```text
30 ms
```

Quando não há retorno do sinal dentro desse período, o firmware considera que não existe obstáculo detectado dentro da condição utilizada.

---

# 22. Prioridade dos obstáculos

Durante a viagem, a lógica pode ser representada por:

```text
              EM_VIAGEM
                  ↓
        Verificar obstáculo
                  ↓
          Existe obstáculo?
             ↙         ↘
           SIM         NÃO
            ↓            ↓
      Parar motores   Seguir linha
            ↓            ↓
       Aguardar       Verificar
                      estação
```

Caso o obstáculo permaneça por aproximadamente dois segundos, o LCD apresenta:

```text
OBSTACULO NA VIA
```

Quando a condição deixa de existir, o sistema retoma o fluxo normal da viagem.

---

# 23. Estado `AGUARDANDO_SENHA`

Após identificar a estação de destino, o robô permanece parado e solicita a senha da viagem.

O usuário deve informar os quatro dígitos através do teclado.

Por questões de segurança visual, os valores digitados são apresentados no LCD como:

```text
*
```

A tecla:

```text
*
```

pode ser utilizada para limpar a entrada atual.

A tecla:

```text
#
```

confirma a senha.

---

# 24. Validação da senha

O firmware compara a senha digitada com a senha gerada no início da viagem.

Caso os valores sejam iguais:

```text
Senha correta
       ↓
Acesso liberado
       ↓
Abrir compartimento
       ↓
Retirar carga
```

O sistema então passa para:

```text
PORTA_ABERTA
```

---

# 25. Tentativas de autenticação

O sistema permite até:

```text
3 tentativas
```

de autenticação para cada viagem.

Quando a senha está incorreta, o número de tentativas restantes é atualizado.

Depois de três tentativas incorretas, o sistema entra em:

```text
SISTEMA_BLOQUEADO
```

O código de manutenção utilizado para desbloqueio é definido diretamente no firmware e não é apresentado nesta documentação.

---

# 26. Estado `SISTEMA_BLOQUEADO`

Nesse estado, o sistema deixa de aceitar a senha normal da viagem.

O usuário precisa realizar o procedimento de manutenção definido pelo firmware.

Após a autenticação de manutenção, o sistema pode retornar ao fluxo de autenticação.

Essa estrutura impede que a mesma senha da viagem continue sendo testada indefinidamente.

---

# 27. Estado `PORTA_ABERTA`

Quando a senha correta é informada, o compartimento é aberto.

O LCD informa que a carga pode ser retirada.

Após a retirada, o usuário pode utilizar:

```text
#
```

ou:

```text
*
```

para finalizar a operação.

O servo então fecha o compartimento e o sistema retorna para:

```text
ESCOLHENDO_DESTINO
```

---

# 28. Comunicação com o teclado

O Arduino principal não realiza diretamente a varredura da matriz do teclado.

Essa função é executada pelo segundo Arduino.

A comunicação entre os dois dispositivos ocorre através de I²C.

O Arduino do teclado possui o endereço:

```text
0x10
```

O Arduino principal realiza uma requisição e o Arduino 2 retorna o caractere armazenado.

---

# 29. Estados sem leitura do teclado

Durante o deslocamento, não é necessário consultar o teclado.

Por isso, as requisições de teclado não são realizadas durante:

```text
MANOBRA_INICIAL
SAINDO_DA_ORIGEM
EM_VIAGEM
```

O teclado é consultado principalmente nos estados em que existe uma interação direta com o usuário.

Essa organização evita que a leitura do teclado interfira desnecessariamente na execução da navegação.

---

# 30. Controle do compartimento

O compartimento é controlado através de um micro servo.

As posições utilizadas são:

```text
0°  → fechado
90° → aberto
```

O software realiza a abertura e o fechamento conforme o estado atual do sistema.

A abertura ocorre principalmente durante:

```text
AGUARDANDO_CARGA
PORTA_ABERTA
```

Também pode ocorrer durante o processo de cancelamento para permitir a retirada da carga.

---

# 31. Fluxo completo de operação

O funcionamento completo pode ser representado por:

```text
INÍCIO
  ↓
ESCOLHENDO_DESTINO
  ↓
Selecionar destino
  ↓
Confirmar
  ↓
AGUARDANDO_CARGA
  ↓
Colocar carga
  ↓
Confirmar
  ↓
MOSTRANDO_SENHA
  ↓
Gerar senha
  ↓
Confirmar início
  ↓
MANOBRA_INICIAL
  ↓
SAINDO_DA_ORIGEM
  ↓
EM_VIAGEM
  ↓
Seguir linha
  ↓
Verificar obstáculos
  ↓
Detectar destino
  ↓
AGUARDANDO_SENHA
  ↓
Informar senha
  ↓
Senha correta?
 ↙          ↘
SIM         NÃO
 ↓           ↓
PORTA_       Tentativa
ABERTA       novamente
 ↓
Retirar carga
 ↓
Fechar compartimento
 ↓
ESCOLHENDO_DESTINO
```

---

# 32. Fluxo de cancelamento

O cancelamento antes do deslocamento segue:

```text
AGUARDANDO_CARGA
        ↓
Carga colocada
        ↓
MOSTRANDO_SENHA
        ↓
Usuário pressiona *
        ↓
DEVOLVENDO_CARGA
        ↓
Abrir compartimento
        ↓
Retirar carga
        ↓
Fechar compartimento
        ↓
ESCOLHENDO_DESTINO
```

---

# 33. Fluxo de bloqueio

Quando a autenticação falha três vezes:

```text
AGUARDANDO_SENHA
        ↓
Tentativa 1
        ↓
Tentativa 2
        ↓
Tentativa 3
        ↓
SISTEMA_BLOQUEADO
        ↓
Procedimento de manutenção
        ↓
Retorno à autenticação
```

---

# 34. Inicialização do software

Durante o `setup()`, o firmware realiza as configurações iniciais do sistema.

Entre elas:

1. configuração dos pinos;
2. inicialização da comunicação I²C;
3. configuração do LCD;
4. configuração dos sensores;
5. configuração do controle dos motores;
6. desligamento dos LEDs;
7. parada dos motores;
8. fechamento do compartimento;
9. inicialização do gerador aleatório;
10. definição da estação inicial;
11. definição da orientação inicial;
12. apresentação da tela inicial.

A condição inicial é:

```text
Estação = A
Orientação = NORTE
```

---

# 35. Parâmetros de controle

Os principais parâmetros utilizados pelo software são:

| Parâmetro                      |   Valor |
| ------------------------------ | ------: |
| Velocidade dos motores         |     128 |
| Limiar dos LDRs de linha       |     300 |
| Limiar dos sensores de estação |     100 |
| Distância para obstáculo       |   30 cm |
| Atualização ultrassônica       |  100 ms |
| Timeout ultrassônico           |   30 ms |
| Tempo de curva                 |  700 ms |
| Tempo da manobra U             | 1400 ms |
| Tempo do servo                 |  500 ms |
| Tentativas de senha            |       3 |

Esses valores estão definidos diretamente no firmware e podem ser ajustados durante o desenvolvimento e os testes do protótipo.

---

# 36. Estrutura lógica do firmware

A organização geral do programa pode ser representada como:

```text
Configuração
     ↓
Definição de estados
     ↓
Definição de direções
     ↓
Definição de manobras
     ↓
Controle de motores
     ↓
Leitura dos sensores
     ↓
Seguidor de linha
     ↓
Detecção de estações
     ↓
Detecção de obstáculos
     ↓
Controle do servo
     ↓
Comunicação I²C
     ↓
Interface com usuário
     ↓
Senhas
     ↓
Máquina de estados
```

Essa estrutura permite que cada subsistema tenha uma responsabilidade definida dentro do funcionamento do robô.

---

# 37. Integração entre software e hardware

O software funciona como a camada responsável por interpretar os sensores e controlar os atuadores.

A relação pode ser representada por:

```text
                 SENSORES
                    ↓
          ┌──────────────────┐
          │ Arduino Principal│
          │                  │
          │ Máquina de       │
          │ estados          │
          │                  │
          │ Navegação        │
          └────────┬─────────┘
                   ↓
        ┌──────────┼──────────┐
        ↓          ↓          ↓
     MOTORES     SERVO      LEDs
        ↓          ↓          ↓
    Movimento  Compart.   Indicação
                  

                   ↕
                  I²C
                   ↕
          ┌────────────────┐
          │   Arduino 2    │
          └───────┬────────┘
                  ↓
             TECLADO 4×4
```

O Arduino principal funciona como o núcleo de decisão do sistema, enquanto o segundo Arduino atua como uma interface dedicada para entrada de dados.

---

# 38. Arquivos relacionados

Os principais arquivos relacionados ao software são:

```text
arduino/
├── mestre/
│   └── mestre.ino
└── escravo/
    └── escravo.ino
```

O arquivo `mestre.ino` contém:

* máquina de estados;
* navegação;
* controle dos motores;
* leitura dos sensores;
* controle do servo;
* controle do LCD;
* comunicação I²C;
* gerenciamento das senhas.

O arquivo `escravo.ino` contém:

* varredura do teclado;
* identificação das teclas;
* armazenamento temporário da tecla;
* resposta às requisições I²C.

---

# 39. Relação com as outras etapas do projeto

O software integra as demais partes desenvolvidas no projeto:

```text
Eletrônica
    ↓
Sensores e atuadores
    ↓
Software
    ↓
Controle e navegação
    ↓
Mecânica
    ↓
Estrutura física
    ↓
Webots
    ↓
Simulação do comportamento
```

Dessa forma, o firmware representa a camada de controle que conecta os componentes eletrônicos à lógica de operação do robô.

---

## 40. Arquivos do projeto

| Área                | Arquivo                       |
| ------------------- | ----------------------------- |
| Firmware principal  | `arduino/mestre/mestre.ino`   |
| Firmware do teclado | `arduino/escravo/escravo.ino` |
| Modelo mecânico     | `openscad/chassi.scad`        |
| Simulação           | `webots/`                     |

