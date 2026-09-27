"""rota_3_pontos controller."""
print("===ROTA_3_PONTOS FOI EXECUTADO ===", flush=True)

from controller import Supervisor
import math

# ============================================================
# CONFIGURAÇÃO INICIAL
# ============================================================

robot = Supervisor()

TIME_STEP = int(robot.getBasicTimeStep())

# O Pioneer3at deve estar com:
# supervisor TRUE


# ============================================================
# MOTORES DO PIONEER 3-AT
# ============================================================

front_left = robot.getDevice("front left wheel")
back_left = robot.getDevice("back left wheel")

front_right = robot.getDevice("front right wheel")
back_right = robot.getDevice("back right wheel")

motors = [
    front_left,
    back_left,
    front_right,
    back_right
]

# Controle dos motores por velocidade
for motor in motors:
    motor.setPosition(float("inf"))
    motor.setVelocity(0.0)


# ============================================================
# ROTA
#
# Formato:
# (X, Y, TEMPO_DE_PARADA, NOME)
#
# tempo = 0  -> waypoint intermediário, NÃO para
# tempo > 0  -> ponto de parada
#
# SUBSTITUA AS COORDENADAS ABAIXO PELAS DO SEU MAPA
# ============================================================

rota = [

    # ------------------------------
    # Caminho até o PONTO 1
    # ------------------------------

    (-26, -73, 0, "W1"),
    (-26, -195, 0, "W2"),

    # PONTO 1 - para 5 segundos
    (-32, -195, 10.0, "P1"),


    # ------------------------------
    # Caminho do PONTO 1 ao PONTO 2
    # ------------------------------
     
     (-26, -195, 0, "W3"),
     (-26, -73, 0, "W4"),
     (-18, -73, 0, "W5"),
    (13, -73, 0, "W6"),
    
    

    # PONTO 2 - para 10 segundos
    (13, -136, 10.0, "P2"),


    # ------------------------------
    # Caminho do PONTO 2 ao PONTO 3
    # ------------------------------

    (13, -73, 0, "W7"),
    (-18.0, -73.0, 0, "W8"),
    

    # PONTO 3 - para 7 segundos
    (-18.0, 89.0, 10.0, "P3"),

    (-18, -73, 10, "PI"),
]


# ============================================================
# PARÂMETROS DO CONTROLE
# ============================================================

# Velocidade normal de deslocamento
VELOCIDADE = 4.0

# Ganho utilizado para corrigir a direção
GANHO_GIRO = 2.5

# Distância para considerar que chegou
# a um waypoint intermediário
TOLERANCIA_PASSAGEM = 0.35

# Distância para considerar que chegou
# a um ponto onde deve parar
TOLERANCIA_PARADA = 0.20

# Abaixo desta distância reduz a velocidade
DISTANCIA_REDUCAO = 1.0

# Velocidade mínima quando próximo de um ponto
VELOCIDADE_MINIMA = 1.0


# ============================================================
# VARIÁVEIS DA MISSÃO
# ============================================================

indice = 0

estado = "MOVENDO"

inicio_parada = 0.0

node = robot.getSelf()


# ============================================================
# FUNÇÕES
# ============================================================

def parar():
    """Para completamente o robô."""

    front_left.setVelocity(0.0)
    back_left.setVelocity(0.0)

    front_right.setVelocity(0.0)
    back_right.setVelocity(0.0)


def limitar(valor, minimo, maximo):
    """Limita um valor entre mínimo e máximo."""

    return max(minimo, min(maximo, valor))


# ============================================================
# INÍCIO DA MISSÃO
# ============================================================

print("====================================")
print("MISSÃO INICIADA")
print("====================================")

print("Primeiro destino:", rota[0][3])


# ============================================================
# LOOP PRINCIPAL
# ============================================================

while robot.step(TIME_STEP) != -1:

    # --------------------------------------------------------
    # TODOS OS PONTOS FORAM PERCORRIDOS
    # --------------------------------------------------------

    if indice >= len(rota):

        parar()

        continue


    # --------------------------------------------------------
    # DADOS DO DESTINO ATUAL
    # --------------------------------------------------------

    destino_x = rota[indice][0]
    destino_y = rota[indice][1]

    tempo_parada = rota[indice][2]

    nome = rota[indice][3]


    # ========================================================
    # ESTADO: PARADO EM UM PONTO
    # ========================================================

    if estado == "PARADO":

        parar()

        tempo_decorrido = robot.getTime() - inicio_parada

        # terminou o tempo de parada
        if tempo_decorrido >= tempo_parada:

            print("Fim da parada em", nome)

            indice += 1

            # Existem mais pontos?
            if indice < len(rota):

                estado = "MOVENDO"

                print(
                    "Próximo destino:",
                    rota[indice][3]
                )

            else:

                print("====================================")
                print("MISSÃO CONCLUÍDA")
                print("====================================")

        continue


    # ========================================================
    # POSIÇÃO ATUAL DO ROBÔ
    # ========================================================

    posicao = node.getPosition()

    x = posicao[0]
    y = posicao[1]


    # ========================================================
    # DISTÂNCIA ATÉ O DESTINO
    # ========================================================

    dx = destino_x - x
    dy = destino_y - y

    distancia = math.sqrt(
        dx * dx +
        dy * dy
    )


    # ========================================================
    # DESCOBRIR SE É WAYPOINT OU PONTO DE PARADA
    # ========================================================

    if tempo_parada > 0:

        tolerancia = TOLERANCIA_PARADA

    else:

        tolerancia = TOLERANCIA_PASSAGEM


    # ========================================================
    # CHEGOU AO PONTO?
    # ========================================================

    if distancia <= tolerancia:

        # ----------------------------------------------------
        # É UM PONTO DE PARADA
        # ----------------------------------------------------

        if tempo_parada > 0:

            parar()

            print(
                "Chegou ao",
                nome,
                "- parada de",
                tempo_parada,
                "segundos"
            )

            inicio_parada = robot.getTime()

            estado = "PARADO"


        # ----------------------------------------------------
        # É APENAS UM WAYPOINT
        # ----------------------------------------------------

        else:

            print("Passou por", nome)

            indice += 1

            if indice < len(rota):

                print(
                    "Próximo destino:",
                    rota[indice][3]
                )

            else:

                parar()

                print("====================================")
                print("MISSÃO CONCLUÍDA")
                print("====================================")

        continue


    # ========================================================
    # ORIENTAÇÃO ATUAL DO ROBÔ
    # ========================================================

    orientacao = node.getOrientation()

    # O eixo X local aponta para frente no Pioneer 3-AT.
    #
    # Pegamos a direção do eixo X do robô
    # expressa no sistema global.

    frente_x = orientacao[0]
    frente_y = orientacao[3]


    # ========================================================
    # VETOR UNITÁRIO DO ROBÔ ATÉ O DESTINO
    # ========================================================

    alvo_x = dx / distancia
    alvo_y = dy / distancia


    # ========================================================
    # ERRO ANGULAR
    # ========================================================

    produto_cruzado = (
        frente_x * alvo_y -
        frente_y * alvo_x
    )

    produto_escalar = (
        frente_x * alvo_x +
        frente_y * alvo_y
    )

    erro_angular = math.atan2(
        produto_cruzado,
        produto_escalar
    )


    # ========================================================
    # VELOCIDADE PARA FRENTE
    # ========================================================

    velocidade_base = VELOCIDADE


    # --------------------------------------------------------
    # Reduz velocidade próximo de pontos de PARADA
    # --------------------------------------------------------

    if tempo_parada > 0:

        if distancia < DISTANCIA_REDUCAO:

            fator = distancia / DISTANCIA_REDUCAO

            velocidade_base = (
                VELOCIDADE_MINIMA +
                (VELOCIDADE - VELOCIDADE_MINIMA) * fator
            )


    # ========================================================
    # CONTROLE DE DIREÇÃO
    # ========================================================

    giro = GANHO_GIRO * erro_angular


    # --------------------------------------------------------
    # Se estiver muito desalinhado:
    # gira praticamente no próprio eixo
    # --------------------------------------------------------

    if abs(erro_angular) > 0.7:

        velocidade_esquerda = -giro
        velocidade_direita = giro


    # --------------------------------------------------------
    # Caso contrário:
    # avança corrigindo a direção
    # --------------------------------------------------------

    else:

        velocidade_esquerda = (
            velocidade_base - giro
        )

        velocidade_direita = (
            velocidade_base + giro
        )


    # ========================================================
    # LIMITAÇÃO DAS VELOCIDADES
    # ========================================================

    MAX_VEL = 6.0

    velocidade_esquerda = limitar(
        velocidade_esquerda,
        -MAX_VEL,
        MAX_VEL
    )

    velocidade_direita = limitar(
        velocidade_direita,
        -MAX_VEL,
        MAX_VEL
    )


    # ========================================================
    # ENVIA COMANDOS ÀS QUATRO RODAS
    # ========================================================

    front_left.setVelocity(
        velocidade_esquerda
    )

    back_left.setVelocity(
        velocidade_esquerda
    )

    front_right.setVelocity(
        velocidade_direita
    )

    back_right.setVelocity(
        velocidade_direita
    )
