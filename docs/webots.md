# Simulação no Webots

## 1. Visão geral

A simulação do **ROBÔ ENTREGADOR** foi desenvolvida utilizando o **Webots**, com o objetivo de representar virtualmente o comportamento do robô e o ambiente no qual ele deverá operar.

A utilização da simulação complementa as outras duas frentes do protótipo:

```text
Eletrônica
     +
Modelagem mecânica
     +
Simulação
```

Enquanto a eletrônica representa o sistema de controle e seus componentes, e o OpenSCAD representa a estrutura mecânica, o Webots permite representar o comportamento do robô em um ambiente virtual.

---

## 2. Objetivo da simulação

A simulação foi desenvolvida para representar o conceito de um robô móvel realizando deslocamentos entre diferentes pontos.

No contexto do projeto, o ambiente utilizado representa o **Centro de Tecnologia (CT) da Universidade Federal do Ceará (UFC)**.

A simulação permite visualizar uma representação virtual do ambiente e desenvolver o comportamento de navegação antes da construção de uma versão física completa.

---

## 3. Organização dos arquivos

Os arquivos relacionados ao Webots estão organizados no repositório da seguinte forma:

```text
webots/
├── controllers/
│   └── rota_3_pontos/
│       └── rota_3_pontos.py
│
└── worlds/
    ├── textures/
    │   └── centro_de_tecnologia.jpg
    ├── .criar.jpg
    ├── .criar.wbproj
    └── criar.wbt
```

Essa organização mantém os arquivos da simulação separados das demais partes do projeto.

---

## 4. Mundo da simulação

O mundo principal utilizado na simulação está localizado em:

```text
webots/worlds/criar.wbt
```

O arquivo `.wbt` contém a definição do ambiente utilizado pelo Webots.

A estrutura do mundo está associada à representação do ambiente do Centro de Tecnologia utilizada no projeto.

---

## 5. Representação do Centro de Tecnologia

A simulação utiliza uma textura referente ao Centro de Tecnologia:

```text
webots/worlds/textures/centro_de_tecnologia.jpg
```

Essa textura é utilizada como parte da representação visual do ambiente.

A utilização de uma representação do CT aproxima a simulação do cenário proposto para a aplicação real do robô.

---

## 6. Controlador

O projeto possui um controlador desenvolvido em Python:

```text
webots/controllers/rota_3_pontos/rota_3_pontos.py
```

O controlador é responsável pela lógica executada pelo robô dentro do ambiente de simulação.

O nome `rota_3_pontos` está relacionado à rota representada nessa etapa do desenvolvimento da simulação.

---

## 7. Relação entre simulação e projeto

A simulação representa uma etapa intermediária entre o conceito do robô e uma implementação física completa.

O desenvolvimento pode ser representado por:

```text
CONCEITO
   ↓
Definição do robô entregador
   ↓
PROTÓTIPO
   ├── Eletrônica
   ├── Mecânica
   └── Software
   ↓
SIMULAÇÃO
   ↓
Webots
   ↓
Validação e desenvolvimento do comportamento
   ↓
Futura implementação física
```

Essa abordagem permite desenvolver diferentes partes do projeto de forma independente antes da construção de um robô físico completo.

---

## 8. Ambiente virtual

O ambiente virtual foi desenvolvido para representar o cenário de operação do projeto.

A utilização do Webots permite trabalhar com uma representação tridimensional do ambiente e associar um controlador ao robô simulado.

O mundo utilizado pelo projeto é:

```text
criar.wbt
```

e o controlador associado está localizado em:

```text
controllers/rota_3_pontos/rota_3_pontos.py
```

---

## 9. Relação com a navegação

A navegação é uma das principais funções do ROBÔ ENTREGADOR.

No protótipo em Arduino, a navegação está relacionada ao seguimento da linha, identificação das estações, orientação do robô e detecção de obstáculos.

No Webots, o objetivo é representar o comportamento de deslocamento do robô dentro de um ambiente virtual.

Assim, as duas abordagens fazem parte do mesmo conceito geral:

```text
Protótipo Arduino
        ↓
Controle e navegação
        ↓
Robô físico

Webots
        ↓
Simulação do comportamento
        ↓
Robô virtual
```

---

## 10. Evolução do protótipo

A utilização do Webots faz parte da estratégia adotada pelo projeto para lidar com a limitação de tempo da etapa de desenvolvimento.

Construir e testar um robô físico completo capaz de navegar pelo Centro de Tecnologia em aproximadamente duas semanas representaria uma tarefa de grande complexidade.

Por isso, o projeto foi dividido em diferentes níveis de prototipagem:

### Eletrônica

Simulação dos componentes e da lógica de controle utilizando Arduino e Tinkercad.

### Mecânica

Modelagem do chassi e do compartimento de carga utilizando OpenSCAD.

### Simulação

Representação do ambiente e do comportamento do robô utilizando Webots.

Essa divisão permite desenvolver e demonstrar diferentes aspectos do projeto sem depender da construção imediata de uma versão física completa.

---

## 11. Arquivos da simulação

Os principais arquivos relacionados ao Webots são:

| Arquivo                                             | Função                        |
| --------------------------------------------------- | ----------------------------- |
| `webots/worlds/criar.wbt`                           | Mundo principal da simulação  |
| `webots/worlds/textures/centro_de_tecnologia.jpg`   | Textura utilizada no ambiente |
| `webots/controllers/rota_3_pontos/rota_3_pontos.py` | Controlador da simulação      |
| `webots/worlds/.criar.wbproj`                       | Arquivo de projeto do Webots  |

Também estão presentes arquivos auxiliares utilizados pelo projeto do mundo.

---

## 12. Organização do repositório

A separação dos arquivos do Webots dentro da pasta `webots/` facilita a organização do repositório:

```text
robo-entregador/
│
├── arduino/
│   ├── mestre/
│   └── escravo/
│
├── openscad/
│   └── chassi.scad
│
├── webots/
│   ├── controllers/
│   │   └── rota_3_pontos/
│   │       └── rota_3_pontos.py
│   │
│   └── worlds/
│       ├── textures/
│       │   └── centro_de_tecnologia.jpg
│       └── criar.wbt
│
└── docs/
    ├── eletronica.md
    ├── mecanica.md
    ├── software.md
    └── webots.md
```

Essa estrutura separa claramente as três principais frentes técnicas do protótipo.

---

## 13. Relação entre as ferramentas

Cada ferramenta utilizada no projeto possui uma finalidade específica:

| Ferramenta         | Aplicação                             |
| ------------------ | ------------------------------------- |
| Tinkercad Circuits | Simulação da eletrônica               |
| Arduino / C++      | Controle do protótipo eletrônico      |
| OpenSCAD           | Modelagem mecânica                    |
| Webots             | Simulação do robô e do ambiente       |
| Python             | Controlador da simulação do Webots    |
| GitHub             | Organização e documentação do projeto |

Essa divisão permite que diferentes aspectos do sistema sejam desenvolvidos e documentados separadamente.

---

## 14. Contribuição do Webots para o projeto

A simulação contribui para o desenvolvimento do projeto ao fornecer uma representação virtual do ambiente de operação.

Ela também permite que o comportamento de navegação seja desenvolvido em paralelo à eletrônica e à modelagem mecânica.

Dessa forma, o Webots funciona como uma etapa de prototipagem e experimentação dentro do desenvolvimento do ROBÔ ENTREGADOR.

---

## 15. Limitações da simulação

A simulação não deve ser interpretada como uma reprodução completa de todos os aspectos físicos do robô real.

Ela representa uma etapa de desenvolvimento destinada a estudar e demonstrar o comportamento do sistema em um ambiente virtual.

Diferenças entre a simulação e uma implementação física podem surgir em aspectos como:

* sensores reais;
* atrito;
* motores;
* alimentação elétrica;
* tolerâncias mecânicas;
* ruído de sensores;
* comportamento físico dos componentes.

Por isso, os resultados da simulação devem ser considerados parte do processo de prototipagem e não uma substituição completa dos testes físicos.

---

## 16. Próximas etapas

A partir das três frentes desenvolvidas, uma evolução natural do projeto seria integrar:

```text
Modelo mecânico
       +
Eletrônica
       +
Firmware
       +
Navegação
       +
Simulação
       ↓
Protótipo físico
```

Uma futura versão poderia utilizar o modelo mecânico desenvolvido no OpenSCAD como base para a construção física do robô, mantendo a arquitetura eletrônica e a lógica de controle desenvolvidas durante o projeto.

---

## 17. Arquivos relacionados

Os arquivos da simulação estão disponíveis em:

```text
webots/
```

Controlador:

```text
webots/controllers/rota_3_pontos/rota_3_pontos.py
```

Mundo:

```text
webots/worlds/criar.wbt
```

Textura:

```text
webots/worlds/textures/centro_de_tecnologia.jpg
```

