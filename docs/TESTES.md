# Testes, Validação e Evolução do Projeto

## 1. Visão geral

O desenvolvimento do ROBÔ ENTREGADOR foi realizado de forma incremental, com a implementação e validação de diferentes módulos antes da integração do sistema completo.

Como o projeto envolvia eletrônica, firmware, modelagem mecânica e simulação, a equipe optou por desenvolver as partes separadamente e posteriormente integrá-las.

Essa estratégia permitiu identificar problemas de hardware, software e arquitetura antes da montagem do sistema completo.

---

## 2. Desenvolvimento incremental

O projeto passou por diferentes versões até chegar ao protótipo atual.

A primeira etapa foi desenvolvida no Tinkercad e possuía uma lógica mais simples de seleção de destinos.

### 2.1 Primeira versão do sistema

Na primeira versão, a seleção da estação de destino era realizada por meio de **botões individuais**.

Essa versão ainda não possuía:

* teclado matricial;
* sistema de senha;
* deslocamento completo entre as diferentes estações;
* sistema de autenticação para retirada da carga.

O objetivo dessa etapa era validar inicialmente a lógica de movimentação e seleção de destino.

---

## 3. Desenvolvimento separado do sistema de senha

Paralelamente ao desenvolvimento da primeira versão, foi desenvolvido um módulo independente relacionado ao sistema de carga.

Esse módulo envolvia:

* teclado;
* display LCD;
* servo motor;
* abertura e fechamento do compartimento;
* geração e utilização de senha.

A decisão de desenvolver essa parte separadamente permitiu testar a lógica de controle da carga sem depender inicialmente de todo o sistema de navegação.

Posteriormente, esse módulo foi integrado ao restante do projeto.

---

## 4. Integração dos módulos

Após a validação dos módulos individuais, as funcionalidades foram integradas em um único projeto.

Entre as principais mudanças realizadas estão:

* substituição dos botões pelo teclado matricial;
* integração do teclado ao sistema principal;
* implementação da seleção de destino;
* implementação do sistema de senha;
* integração do servo motor;
* integração do LCD;
* implementação do deslocamento entre diferentes estações;
* integração da navegação com o sistema de carga.

A evolução pode ser representada da seguinte maneira:

```text
Versão inicial
     │
     ├── Botões para seleção
     ├── Navegação simplificada
     └── Sem autenticação
     │
     ▼
Módulo de carga
     │
     ├── Teclado
     ├── LCD
     ├── Servo
     └── Senha
     │
     ▼
Integração
     │
     ├── Teclado + navegação
     ├── Senha + carga
     ├── Seleção de destino
     └── Deslocamento entre estações
     │
     ▼
Protótipo atual
```

---

## 5. Evolução das estações

A lógica de navegação também evoluiu durante o desenvolvimento.

Nas primeiras versões, a representação das estações era mais limitada. A versão atual trabalha com as estações:

```text
A
B
C
D
```

O sistema permite selecionar uma estação de destino e realizar o deslocamento considerando a posição atual do robô.

Essa evolução foi importante para aproximar o protótipo da proposta original de um robô capaz de realizar entregas entre diferentes pontos do Centro de Tecnologia.

---

## 6. Estratégia de testes por módulos

Uma das principais estratégias adotadas durante o desenvolvimento foi realizar os testes de maneira modular.

Em vez de desenvolver todo o sistema e realizar apenas um teste ao final, os componentes foram implementados e testados individualmente antes da integração.

Entre os módulos desenvolvidos e testados estão:

* controle dos motores;
* sensores de seguimento de linha;
* detecção de estações;
* sensores ultrassônicos;
* LCD;
* teclado;
* servo motor;
* sistema de senha;
* comunicação I2C;
* seleção de destino;
* lógica de navegação;
* integração dos módulos.

Essa abordagem reduziu a possibilidade de um erro em um único módulo impedir a identificação do funcionamento dos demais.

---

## 7. Testes no Tinkercad

O circuito desenvolvido no Tinkercad foi testado diversas vezes durante o desenvolvimento.

Foram realizadas diferentes combinações de rotas e situações de operação para verificar o comportamento da lógica implementada.

De acordo com os testes realizados pela equipe, as funcionalidades presentes na versão final do circuito do Tinkercad foram validadas dentro do ambiente de simulação.

Os testes envolveram principalmente:

* seleção de diferentes destinos;
* deslocamento entre estações;
* funcionamento do sistema de senha;
* abertura e fechamento do compartimento;
* utilização do teclado;
* exibição de informações no LCD;
* detecção de obstáculos;
* funcionamento do seguimento de linha;
* comunicação entre os Arduinos.

---

## 8. Limitação dos sensores no Tinkercad

Uma dificuldade encontrada durante a implementação do seguidor de linha foi a disponibilidade de sensores no Tinkercad.

O ambiente não disponibiliza diretamente o sensor utilizado em muitas aplicações de robótica competitiva para seguimento de linha.

Para contornar essa limitação, foram utilizados sensores de luminosidade organizados em uma configuração em formato de cruz.

Essa solução foi utilizada como uma aproximação do funcionamento de um conjunto de sensores capaz de identificar a posição do robô em relação à linha.

Dessa forma, a solução utilizada no Tinkercad deve ser entendida como uma adaptação necessária às limitações do ambiente de simulação.

---

## 9. Decisão entre um ou dois Arduinos

Uma das principais discussões técnicas durante o desenvolvimento foi a definição da arquitetura eletrônica.

Foram consideradas duas possibilidades:

```text
Opção 1
Um Arduino
+
Expansores de I/O

Opção 2
Arduino principal
+
Arduino secundário
+
Comunicação I2C
```

Inicialmente, a equipe também tentou utilizar expansores de I/O.

Entretanto, foram encontradas limitações no ambiente do Tinkercad relacionadas à utilização de múltiplos expansores com diferentes endereços, principalmente quando um deles estava associado ao teclado.

Essa configuração apresentou dificuldades na leitura dos dispositivos.

---

## 10. Escolha da arquitetura com dois Arduinos

Após analisar as alternativas, a equipe optou pela utilização de dois Arduinos.

A arquitetura final utiliza:

```text
Arduino principal
      │
      │ I2C
      ▼
Arduino secundário
      │
      ▼
Teclado matricial
```

O Arduino principal permanece responsável pelo controle geral do robô, enquanto o Arduino secundário realiza a leitura do teclado e disponibiliza as teclas ao controlador principal.

---

## 11. Motivos para utilização de dois Arduinos

Além das limitações encontradas com os expansores no Tinkercad, a utilização de apenas um Arduino deixaria a utilização dos pinos bastante limitada.

A equipe identificou que uma arquitetura com apenas um Arduino exigiria alterações em outras partes do projeto.

Entre os problemas considerados estavam:

* limitação da quantidade de pinos disponíveis;
* necessidade de reorganizar o controle dos motores;
* necessidade de alterações na leitura dos sensores;
* possibilidade de utilização de pinos que também possuem funções importantes para comunicação serial;
* necessidade de substituir sensores por alternativas diferentes.

Um exemplo considerado pela equipe foi a necessidade de utilizar diferentes pinos de controle dos motores para manter determinadas funcionalidades de velocidade.

Também foi identificada a possibilidade de utilização de um sensor de três pinos em uma arquitetura alternativa, aumentando o custo e dificultando a obtenção do componente.

A arquitetura com dois Arduinos, portanto, permitiu distribuir melhor as funções do sistema.

---

## 12. Comunicação I2C

A comunicação I2C aumentou a complexidade do projeto, principalmente devido à necessidade de trabalhar com endereços diferentes para os dispositivos.

Entretanto, depois que a arquitetura de endereçamento foi definida, a comunicação pôde ser implementada e utilizada no sistema.

A arquitetura final utiliza o barramento I2C para comunicação entre:

* LCD;
* expansor PCF8574;
* Arduino secundário.

O Arduino secundário possui endereço próprio no barramento e fornece ao Arduino principal as teclas identificadas no teclado matricial.

---

## 13. Uso dos pinos de comunicação serial

Durante a análise da arquitetura com apenas um Arduino, também foi identificada a necessidade de utilizar pinos que possuem funções associadas à comunicação serial.

Essa solução poderia representar uma dificuldade adicional em uma futura implementação física, especialmente durante processos de programação, depuração e manutenção do robô.

Esse fator contribuiu para a decisão de distribuir as funções entre dois Arduinos.

---

## 14. Testes do sistema de senha

O sistema de senha foi desenvolvido inicialmente como um módulo separado e posteriormente integrado ao sistema principal.

Foram testados:

* geração da senha;
* exibição da senha no LCD;
* utilização do teclado;
* confirmação da carga;
* abertura do compartimento;
* solicitação da senha no destino;
* validação da senha;
* controle das tentativas;
* bloqueio após tentativas incorretas;
* abertura do compartimento após autenticação.

Essa integração permitiu conectar o sistema de navegação ao mecanismo de entrega da carga.

---

## 15. Testes de rota

Foram realizados testes com diferentes combinações de origem e destino no ambiente de simulação.

A lógica de navegação foi desenvolvida considerando as estações A, B, C e D e a orientação do robô.

O objetivo dos testes foi verificar se o sistema conseguia:

1. identificar a estação de origem;
2. receber a estação de destino;
3. determinar a orientação necessária;
4. executar a manobra inicial;
5. sair da estação de origem;
6. seguir a rota;
7. identificar a estação de destino;
8. solicitar a autenticação;
9. liberar o compartimento após a validação.

---

## 16. Testes no Webots

Além do Tinkercad, foram realizados testes de rota no Webots.

A simulação foi desenvolvida de forma a representar o conceito do projeto no ambiente do **Centro de Tecnologia da UFC**.

Nesse contexto, o Webots funciona como uma etapa intermediária entre a simulação abstrata dos componentes e a implementação física do robô.

A ideia pode ser representada como:

```text
Tinkercad
   │
   │ validação dos componentes
   ▼
Protótipo eletrônico
   │
   │
Webots + CT
   │
   │ representação do ambiente
   ▼
Protótipo físico futuro
```

A simulação no Webots permite representar o robô em uma escala de ambiente maior, aproximando a proposta do cenário em que o sistema seria utilizado.

---

## 17. Webots como representação do CT

Um dos objetivos da simulação no Webots foi levar a ideia das estações para o ambiente do Centro de Tecnologia.

Dessa forma, o ambiente simulado funciona como uma representação em maior escala do problema que o robô deverá resolver.

O processo pode ser entendido como:

```text
Estações do protótipo
        ↓
Rotas entre estações
        ↓
Representação do CT
        ↓
Simulação de deslocamento
        ↓
Futura aplicação física
```

Assim, o Webots não representa apenas um teste isolado do robô, mas uma tentativa de aproximar o protótipo do cenário real de utilização.

---

## 18. Desenvolvimento da modelagem mecânica

A modelagem mecânica foi desenvolvida separadamente das demais partes do projeto.

O objetivo inicial da equipe era construir efetivamente o robô e realizar sua operação no mundo real.

Entretanto, devido às limitações de tempo, recursos e logística da etapa de desenvolvimento, a construção de uma versão física móvel completa não pôde ser realizada.

---

## 19. Alternativa considerada para o protótipo físico

Após perceber as limitações para a construção do robô completo, foi considerada uma segunda alternativa.

A proposta seria utilizar:

* uma base de LEGO;
* controle remoto;
* chassi produzido em impressora 3D.

Essa alternativa permitiria demonstrar fisicamente a estrutura do robô sem exigir a construção de todo o sistema autônomo.

Posteriormente, entretanto, a equipe conseguiu desenvolver uma simulação no Webots que representava de forma mais adequada o comportamento esperado do robô.

Por isso, optou-se por utilizar a modelagem mecânica juntamente com a simulação virtual como principal forma de demonstração do protótipo.

---

## 20. Estado atual da modelagem

O chassi foi desenvolvido em OpenSCAD e representa a estrutura que seria utilizada em uma futura construção física.

A modelagem contempla elementos necessários ao sistema, incluindo:

* estrutura principal;
* compartimento de carga;
* alçapão;
* pontos de fixação;
* abertura para sensores ultrassônicos;
* suporte para os componentes;
* elementos relacionados ao LCD e teclado.

Dessa forma, mesmo sem a construção física completa, a modelagem fornece uma representação concreta de como o robô poderia ser construído.

---

## 21. O que não foi validado fisicamente

A principal limitação da etapa de desenvolvimento foi a ausência de uma integração física completa.

Não foi possível realizar, em hardware real, a integração de:

* chassi;
* motores;
* sensores;
* Arduinos;
* sistema de alimentação;
* servo;
* teclado;
* LCD;
* sistema de navegação;
* sistema de carga.

Portanto, não se deve interpretar os resultados das simulações como equivalentes a testes completos em um robô físico.

A validação realizada pela equipe foi predominantemente virtual, especialmente por meio do Tinkercad e do Webots.

---

## 22. Limitações e decisões de projeto

As principais limitações encontradas durante o desenvolvimento foram:

### Tinkercad

* disponibilidade limitada de determinados sensores;
* limitações na utilização de múltiplos expansores;
* dificuldades relacionadas a endereçamento de dispositivos;
* necessidade de adaptar o sistema de seguimento de linha.

### Arquitetura eletrônica

* quantidade limitada de pinos do Arduino;
* necessidade de distribuir as funções entre controladores;
* preocupação com utilização de pinos associados à comunicação serial.

### Webots

* curva de aprendizado elevada;
* complexidade para compreender inicialmente a estrutura do software;
* necessidade de dedicar uma parte significativa do desenvolvimento ao estudo da ferramenta.

### Implementação física

* tempo limitado;
* recursos disponíveis;
* logística para construção;
* impossibilidade de integrar e testar todos os componentes fisicamente durante a etapa.

---

## 23. Curva de aprendizado do Webots

O Webots apresentou uma curva de aprendizado significativa durante o desenvolvimento.

A ferramenta possui uma estrutura complexa e exigiu um período de estudo e experimentação para que a equipe pudesse compreender seu funcionamento.

Um dos integrantes ficou responsável por aprofundar o estudo da ferramenta e conseguiu desenvolver a simulação utilizada no projeto.

Esse processo permitiu que o Webots fosse incorporado ao projeto como uma ferramenta efetiva de prototipagem.

---

## 24. Resultado dos testes

Considerando o escopo efetivamente desenvolvido, os resultados podem ser divididos em dois grupos.

### Validação virtual

As funcionalidades implementadas no Tinkercad foram testadas pela equipe em diferentes situações e combinações de rota.

A simulação no Webots também foi utilizada para testar a representação das rotas e do ambiente do Centro de Tecnologia.

### Validação física

A integração completa em um robô físico não foi realizada durante esta etapa do projeto.

Essa etapa permanece como uma possibilidade de evolução futura.

---

## 25. Relação entre teste e desenvolvimento

Os testes não foram realizados somente ao final do projeto.

Eles fizeram parte do próprio processo de desenvolvimento.

A estratégia adotada foi:

```text
Implementar módulo
      ↓
Testar módulo
      ↓
Corrigir
      ↓
Testar novamente
      ↓
Integrar
      ↓
Testar sistema integrado
```

Esse processo foi aplicado principalmente ao desenvolvimento da eletrônica e do firmware.

A abordagem modular permitiu reduzir a quantidade de variáveis durante a identificação de problemas.

---

## 26. Estado final do protótipo

Ao final da etapa, o projeto apresenta três frentes principais integradas conceitualmente:

```text
┌─────────────────────────────┐
│        ROBÔ ENTREGADOR      │
├─────────────────────────────┤
│                             │
│  Eletrônica + Firmware      │
│  Tinkercad / Arduino        │
│                             │
├─────────────────────────────┤
│                             │
│  Modelagem mecânica         │
│  OpenSCAD                   │
│                             │
├─────────────────────────────┤
│                             │
│  Simulação                  │
│  Webots + ambiente do CT    │
│                             │
└─────────────────────────────┘
```

O resultado não deve ser entendido apenas como um circuito simulado ou como uma modelagem isolada.

O objetivo da etapa foi construir um **protótipo conceitual integrado**, no qual diferentes ferramentas representam diferentes aspectos de uma futura implementação física.

---

## 27. Próximas validações

Uma futura continuação do projeto poderia utilizar a estrutura desenvolvida nesta etapa para realizar a implementação física.

Entre as próximas etapas possíveis estão:

* fabricação do chassi;
* montagem dos componentes eletrônicos;
* instalação dos motores;
* integração dos sensores reais;
* alimentação independente;
* testes do seguidor de linha;
* testes de detecção de obstáculos;
* testes de navegação;
* integração do sistema de carga;
* testes de rota no ambiente físico;
* comparação entre os resultados da simulação e do protótipo real.

---

## 28. Conclusão

O processo de testes foi fundamental para a evolução do projeto.

O desenvolvimento incremental permitiu que os módulos fossem validados individualmente antes da integração, enquanto as simulações possibilitaram testar o comportamento do sistema sem a necessidade de construir imediatamente o robô completo.

As dificuldades encontradas também contribuíram para decisões importantes de arquitetura, como a utilização de dois Arduinos e a adaptação dos sensores disponíveis no Tinkercad.

Ao final, o projeto alcançou uma representação integrada do conceito do robô, combinando:

```text
Eletrônica
    +
Firmware
    +
Modelagem mecânica
    +
Navegação
    +
Simulação
```

A implementação física completa permanece como uma etapa futura, mas o protótipo desenvolvido fornece uma base para sua construção e evolução.

