# Robô Entregador

Projeto desenvolvido para a etapa **Core Project** do processo seletivo 2026.2 do **PET Engenharia de Computação — UFC**.

## Sobre o projeto

O **Robô Entregador** é uma proposta de robô autônomo desenvolvido para realizar o transporte de objetos entre diferentes pontos do **Centro de Tecnologia (CT) da Universidade Federal do Ceará (UFC)**.

A ideia consiste em selecionar um destino, realizar a coleta de um objeto e conduzi-lo autonomamente até o ponto escolhido, utilizando mecanismos de navegação, detecção de obstáculos e controle do compartimento de carga.

Durante o período de desenvolvimento do Core Project, a construção de um robô físico completamente funcional para navegação pelo CT não era viável. Por isso, a equipe desenvolveu um **protótipo integrado**, representando as principais partes necessárias para a construção do robô real:

* 🔌 **Eletrônica e controle**, desenvolvidos e simulados no Tinkercad com Arduino e C++;
* ⚙️ **Chassi e estrutura mecânica**, modelados em OpenSCAD;
* 🌐 **Simulação da navegação e do comportamento do robô**, desenvolvida no Webots.

O repositório reúne os códigos, modelos, documentação, simulações e materiais utilizados durante o desenvolvimento do projeto.

---

## Navegação pelo projeto

O repositório está organizado de acordo com as principais frentes de desenvolvimento do Robô Entregador.

| Seção                                       | Conteúdo                                                                                            |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| 📚 [`docs/`](docs/)                         | Documentação técnica do projeto, incluindo eletrônica, mecânica, software, Webots e testes.         |
| 📖 [`manual/`](manual/)                     | Manual do projeto, com orientações para utilização e compreensão do protótipo.                      |
| 🔌 [`arduino/`](arduino/)                   | Códigos dos Arduinos utilizados no sistema.                                                         |
| ⚙️ [`openscad/`](openscad/)                 | Modelo e arquivos relacionados ao chassi desenvolvido em OpenSCAD.                                  |
| 🌐 [`webots/`](webots/)                     | Arquivos da simulação desenvolvida no Webots, incluindo mundo, textura e controlador.               |
| 🖼️ [`media/`](media/)                      | Imagens, vídeos e materiais visuais do desenvolvimento e da apresentação.                           |
| 🔗 [`links/recursos.md`](links/recursos.md) | Links para recursos externos utilizados no projeto, incluindo o circuito desenvolvido no Tinkercad. |

---

## Visão geral do desenvolvimento

O projeto foi desenvolvido de forma modular, dividindo o protótipo em três frentes principais.

### Eletrônica e controle

Responsável pelo controle dos motores, sensores, teclado, display, iluminação, detecção de obstáculos e gerenciamento do compartimento de carga.

A implementação foi desenvolvida e testada no **Tinkercad**, utilizando dois Arduinos e comunicação entre eles por I²C.

➡️ [Ver documentação da eletrônica](docs/eletronica.md)

### Estrutura mecânica

O chassi e os elementos estruturais do robô foram modelados em **OpenSCAD**, considerando o posicionamento dos componentes eletrônicos e o compartimento destinado ao transporte da carga.

➡️ [Ver documentação mecânica](docs/mecanica.md)

### Simulação

O comportamento de navegação do robô foi representado no **Webots**, utilizando uma simulação baseada no ambiente do Centro de Tecnologia.

➡️ [Ver documentação do Webots](docs/webots.md)

---

## 🧪 Desenvolvimento e testes

O projeto foi desenvolvido de forma incremental, com testes realizados ao longo das diferentes etapas de implementação.

A eletrônica passou por versões progressivas no Tinkercad, desde uma primeira abordagem simplificada até a integração de seleção de destino, teclado, senha, display e controle do compartimento de carga.

Também foram realizados testes das rotas e do comportamento de navegação na simulação do Webots. A integração física completa do robô não foi realizada durante o período do projeto, devido às limitações de tempo e recursos disponíveis.

➡️ [Ver histórico de desenvolvimento e testes](docs/testes.md)

---

## 🔗 Recursos

### Circuito no Tinkercad

O circuito eletrônico desenvolvido durante o projeto pode ser acessado diretamente no Tinkercad:

➡️ [Abrir projeto no Tinkercad](links/recursos.md)

### Apresentação

Os slides utilizados na apresentação do projeto serão disponibilizados nesta seção futuramente.

➡️ [`media/slides/`](media/slides/)

---

## Estrutura do repositório

```text
robo-entregador/
├── README.md
├── arduino/
│   ├── mestre/
│   └── escravo/
├── docs/
├── manual/
├── media/
│   ├── images/
│   │   ├── tinkercad/
│   │   ├── openscad/
│   │   ├── webots/
│   │   └── prototipo/
│   ├── slides/
│   └── videos/
├── openscad/
├── webots/
└── links/
    └── recursos.md
```

Cada diretório possui uma função específica dentro do projeto, permitindo separar os códigos, modelos, documentação, simulações e materiais complementares.

---

## Equipe

Projeto desenvolvido por:

* **[HECTOR PIMENTA AVILA CAVALCANTE]**
* **[LIVIA SARAIVA DA COSTA]**
* **[MARCIO ROSA BEZERRA CAVALCANTE]**
* **[RENAN ARAUJO GONZALEZ]**

### Instituição

**Universidade Federal do Ceará — UFC**
**PET Engenharia de Computação**

