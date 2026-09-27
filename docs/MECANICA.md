# Mecânica e Modelagem 3D

## 1. Visão geral

A estrutura mecânica do **ROBÔ ENTREGADOR** foi desenvolvida utilizando **OpenSCAD**, permitindo criar um modelo paramétrico do chassi e dos principais elementos externos do protótipo.

O projeto foi pensado para representar a estrutura física de um robô móvel capaz de transportar uma carga, integrando:

* chassi;
* tampa superior;
* compartimento de carga;
* alçapão;
* pontos de fixação;
* suportes internos;
* aberturas para sensores;
* representação dos componentes eletrônicos externos.

A modelagem foi construída de forma paramétrica, permitindo alterar dimensões e características da estrutura diretamente através das variáveis do código.

---

## 2. Arquivo do modelo

O modelo mecânico está localizado em:

```text
openscad/
└── chassi.scad
```

O arquivo contém a definição das principais peças e também uma montagem final do conjunto.

---

## 3. Dimensões principais

As dimensões principais utilizadas no modelo são:

| Parâmetro                  |  Valor |
| -------------------------- | -----: |
| Largura do chassi          |  48 mm |
| Comprimento do chassi      | 104 mm |
| Altura do chassi           |  65 mm |
| Espessura das paredes      |   3 mm |
| Distância da quina         |   4 mm |
| Raio dos furos de parafuso | 1,6 mm |

Esses valores são definidos no início do arquivo OpenSCAD, permitindo que o modelo seja facilmente ajustado.

---

## 4. Estrutura do chassi

O corpo principal do chassi é construído a partir de um volume retangular com:

```text
48 mm × 104 mm × 65 mm
```

A estrutura externa é posteriormente modificada para criar o espaço interno do robô.

A espessura das paredes é definida por:

```text
3 mm
```

Isso permite representar uma carcaça com volume interno suficiente para acomodar os componentes do sistema.

---

## 5. Base modular

A parte inferior do chassi possui uma matriz de furos inspirada no padrão de montagem utilizado em peças LEGO.

A configuração principal utiliza uma matriz de:

```text
6 × 13
```

Essa característica permite representar pontos de fixação distribuídos pela base.

Além da matriz principal, existem furos adicionais nas extensões laterais da estrutura.

---

## 6. Extensões laterais

O modelo possui uma extensão lateral localizada na região inferior do chassi.

Essa extensão possui aproximadamente:

```text
64 mm de largura
24 mm de comprimento
5 mm de altura
```

Também são adicionados furos de montagem nessa região.

Essas extensões fazem parte da geometria da base e ampliam as possibilidades de fixação da estrutura.

---

## 7. Compartimento de carga

Um dos principais elementos mecânicos do projeto é o compartimento destinado ao transporte da carga.

A estrutura possui uma região elevada na tampa para representar o compartimento.

As dimensões principais utilizadas nessa região são aproximadamente:

```text
26 mm × 35 mm × 15 mm
```

O compartimento é integrado ao modelo da tampa e possui pontos próprios de fixação.

---

## 8. Alçapão

O acesso ao compartimento de carga é representado através de um **alçapão**.

A peça possui espessura aproximada de:

```text
1,2 mm
```

O modelo também inclui elementos geométricos para representar:

* a porta do compartimento;
* a região de abertura;
* uma alça;
* o mecanismo semelhante a uma dobradiça.

No protótipo eletrônico, a abertura do compartimento é realizada por um micro servo. O modelo mecânico representa a estrutura física correspondente a esse mecanismo.

---

## 9. Fixação da tampa

A tampa superior possui quatro pontos de fixação localizados nas regiões de canto.

Esses pontos são representados através de furos para parafusos.

O modelo também possui quatro torres internas de montagem, destinadas à representação dos pontos de suporte internos da estrutura.

---

## 10. Aberturas para sensores ultrassônicos

A parte frontal do chassi possui duas aberturas destinadas aos sensores ultrassônicos.

Os furos são modelados com raio aproximado de:

```text
8,2 mm
```

A disposição dos dois furos representa a posição dos sensores utilizados para detectar obstáculos à frente do robô.

---

## 11. Abertura para chave

Na região traseira do chassi existe um recorte destinado à representação de uma **chave gangorra**.

O modelo da chave também foi criado no próprio OpenSCAD para permitir sua visualização durante a montagem.

---

## 12. Ventilação

O chassi possui aberturas laterais destinadas à representação de entradas e saídas de ventilação.

Essas aberturas fazem parte da geometria lateral da carcaça.

---

## 13. Componentes representados no modelo

Além da estrutura principal, o arquivo OpenSCAD possui modelos simplificados de alguns componentes utilizados no protótipo.

Entre eles estão:

* sensores ultrassônicos;
* chave gangorra;
* LCD;
* teclado matricial;
* parafusos.

Esses modelos não têm como objetivo reproduzir todos os detalhes construtivos dos componentes comerciais, mas representar suas dimensões e posições dentro da montagem.

---

## 14. Modelo do sensor ultrassônico

O sensor ultrassônico é representado por um modelo aproximado de:

```text
45 mm × 2 mm × 20 mm
```

A parte frontal possui dois elementos cilíndricos com aproximadamente:

```text
8 mm de raio
```

A representação permite visualizar a posição dos sensores em relação à abertura frontal do chassi.

---

## 15. Modelo do LCD

O display LCD é representado por um modelo simplificado com dimensões aproximadas de:

```text
40 mm × 20 mm × 2 mm
```

Também é incluída uma representação textual na superfície do display para facilitar sua identificação na montagem.

---

## 16. Modelo do teclado

O teclado matricial é representado por uma placa com dimensões aproximadas de:

```text
30 mm × 40 mm × 2 mm
```

A superfície possui uma representação das teclas em uma matriz de:

```text
3 × 4
```

O objetivo dessa modelagem é representar a posição e o volume ocupado pelo dispositivo na estrutura do protótipo.

---

## 17. Modelo da chave gangorra

A chave gangorra possui um modelo simplificado próprio dentro do arquivo OpenSCAD.

Sua função na montagem é representar visualmente o componente instalado na parte traseira do chassi.

---

## 18. Modelo dos parafusos

Os parafusos utilizados nos pontos de fixação são representados através de uma geometria simplificada.

A dimensão relacionada ao raio do parafuso é definida pelo parâmetro:

```text
raio_parafuso = 1,6 mm
```

Essa parametrização permite ajustar os furos de fixação de acordo com a necessidade do modelo.

---

## 19. Organização do código OpenSCAD

O arquivo foi organizado em módulos, permitindo construir cada parte da estrutura separadamente.

Entre as principais funções geométricas estão:

```text
chassi()
tampa()
alcapao()
parafuso()
sensor_ultrassonico()
chave_gangorra()
tela_lcd()
teclado()
```

Essa organização facilita a edição individual dos componentes e a montagem final do protótipo.

---

## 20. Montagem final

Após a definição das peças individuais, o arquivo realiza a montagem do conjunto.

A montagem integra:

```text
Chassi
   ↓
Tampa
   ↓
Alçapão
   ↓
Parafusos
   ↓
Sensores ultrassônicos
   ↓
Chave gangorra
   ↓
LCD
   ↓
Teclado
```

Dessa forma, é possível visualizar a relação espacial entre a estrutura mecânica e os principais componentes externos do robô.

---

## 21. Relação entre mecânica e eletrônica

A modelagem mecânica foi desenvolvida considerando os componentes utilizados na implementação eletrônica.

A estrutura possui regiões específicas para:

* sensores ultrassônicos;
* LCD;
* teclado;
* chave;
* compartimento de carga;
* pontos de fixação;
* componentes internos.

Assim, o modelo não representa somente uma carcaça, mas uma proposta de integração entre a estrutura física e os subsistemas eletrônicos do robô.

---

## 22. Relação com o projeto do robô

O modelo mecânico representa uma etapa do desenvolvimento do protótipo do **ROBÔ ENTREGADOR**.

A proposta geral do projeto é integrar:

```text
Mecânica
   +
Eletrônica
   +
Software
   +
Simulação
```

A modelagem em OpenSCAD permite representar como esses sistemas poderiam ser fisicamente integrados em uma versão construída do robô.

---

## 23. Arquivo relacionado

O modelo completo está disponível no repositório em:

```text
openscad/chassi.scad
```

O arquivo pode ser aberto e editado no **OpenSCAD**, permitindo visualizar a geometria, modificar os parâmetros e gerar novamente o modelo tridimensional.
