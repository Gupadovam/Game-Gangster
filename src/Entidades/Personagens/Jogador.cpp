#include "Entidades/Personagens/Jogador.hpp"
#include "Fases/Fase.hpp" // Full definition
#include <unistd.h>       // Para usar sleep
#include <cmath>          // Para usar std::max

const float WALK_VELOCIDADE_MAXIMA = 4;
const float RUN_VELOCIDADE_MAXIMA = 8;

const float GRAVIDADE = 9.8f; // Aceleração da gravidade (em unidades por segundo^2)

const float COOLDOWN_PULO = 800.0f; // Tempo de espera entre pulos (em milissegundos)

const float COOLDOWN_ESPINHO = 500.0f;
const float COOLDOWN_LIXO = 250.0f;
const float COOLDOWN_TIRO = 1250.0f;
const float COOLDOWN_MINA = 1250.0f;

const float SCALING_FACTOR = 3.f;

const float SHOT_ANIMATION_TIME = 150.0f;
const float HURT_ANIMATION_TIME = 250.0f;
const float DEATH_ANIMATION_TIME = 1200.0f;

Jogador::Jogador(int px, int py, int vidas) : Personagem(px, py, 0, 0, vidas, tipoDeEntidade::JOGADOR),

                                              tempoDesdeUltimoPulo(0.0f),
                                              tempoDesdeUltimoEspinho(COOLDOWN_ESPINHO),
                                              tempoDesdeUltimoLixo(COOLDOWN_LIXO),
                                              tempoDesdeUltimoTiro(COOLDOWN_TIRO),
                                              tempoDesdeUltimaMina(COOLDOWN_MINA),
                                              slowness(1)

{
    animacao.addTrilha("idle", new TrilhaAnimacao(5, 15, 128, 128, 3.0, 3.0, "./assets/Gangsters_1/Idle.png"));
    animacao.addTrilha("running", new TrilhaAnimacao(9, 10, 128, 128, 3.0, 3.0, "./assets/Gangsters_1/Run.png"));
    animacao.addTrilha("walking", new TrilhaAnimacao(9, 10, 128, 128, 3.0, 3.0, "./assets/Gangsters_1/Walk.png"));
    animacao.addTrilha("jump", new TrilhaAnimacao(9, 10, 128, 128, 3.0, 3.0, "./assets/Gangsters_1/Jump.png"));
    animacao.addTrilha("hurt", new TrilhaAnimacao(4, 5, 128, 128, 3.0, 3.0, "./assets/Gangsters_1/Hurt.png"));
    animacao.addTrilha("dead", new TrilhaAnimacao(4, 20, 128, 128, 3.0, 3.0, "./assets/Gangsters_1/Dead.png", false));
    animacao.addTrilha("shot", new TrilhaAnimacao(3, 5, 128, 128, 3.0, 3.0, "./assets/Gangsters_1/Shot.png", false));
    animacao.setPosition(px, py);
    animacao.setScale(SCALING_FACTOR, SCALING_FACTOR);
    setColisionBoxSize(sf::Vector2f(30 * SCALING_FACTOR, 128 * SCALING_FACTOR));
    setAnimationState();
}

void Jogador::atacar()
{
    if (tempoDesdeUltimoTiro >= COOLDOWN_TIRO)
    {
        pFase->addProjetil(x + direcao * 60, y + (getSize().height / 5), direcao, JOGADOR);
        tempoDesdeUltimoTiro = 0;
    }
}

void Jogador::mover()
{
    if (state == DEAD)
    {
        return; // Bloqueia qualquer movimento se o jogador estiver morto.
    }

    newState = IDLE;
    mudouDirecao = false;
    float elapsed_time = pGG->getElapsedTime();

    // Verifica se o Shift está pressionado para correr
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        newState = RUN;

    // Verifica se a tecla A (esquerda) está pressionada
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        if (direcao != -1 && tempoDesdeUltimaMina >= COOLDOWN_MINA)
        {
            mudouDirecao = true;
            direcao = -1;
        }
        newState = (newState == RUN) ? RUN : WALK;
    }
    // Verifica se a tecla D (direita) está pressionada
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        if (direcao != 1 && tempoDesdeUltimaMina >= COOLDOWN_MINA)
        {
            mudouDirecao = true;
            direcao = 1;
        }
        newState = (newState == RUN) ? RUN : WALK;
    }

    // Verifica o pulo
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        if (noChao && tempoDesdeUltimoPulo >= COOLDOWN_PULO)
        {
            velocidadeY = -sqrt(2 * GRAVIDADE) * 1.5;
            y -= 15;
            noChao = false;
            tempoDesdeUltimoPulo = 0.0f; // Reseta o tempo desde o último pulo
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
    {
        atacar();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        Gerenciador_Estado::getInstance().setEstadoJogo(PAUSE);
    }

    if (state == IDLE)
        velocidadeX = 0;
    if (state == WALK)
    {
        velocidadeX += (elapsed_time / 50.0f) / slowness; // taking up speed
        velocidadeX = min(velocidadeX, WALK_VELOCIDADE_MAXIMA);
    }
    if (state == RUN)
    {
        velocidadeX += (elapsed_time / 100.0f) / slowness; // taking up speed
        velocidadeX = min(velocidadeX, RUN_VELOCIDADE_MAXIMA);
    }

    x += direcao * velocidadeX;
    y += velocidadeY;
}

void Jogador::executar()
{
    // Reset noChao at the start of each frame
    noChao = tempoDesdeUltimoPiso <= COOLDOWN_PISO;
    if (tempoDesdeUltimoLixo >= COOLDOWN_LIXO)
        slowness = 1;

    aplicarGravidade();
    float elapsed_time = pGG->getElapsedTime();

    // Atualiza o tempo desde o último pulo
    tempoDesdeUltimoPulo += elapsed_time;
    tempoDesdeUltimoEspinho += elapsed_time;
    tempoDesdeUltimoPiso += elapsed_time;
    tempoDesdeUltimoLixo += elapsed_time;
    tempoDesdeUltimoTiro += elapsed_time;
    tempoDesdeUltimoDano += elapsed_time;
    tempoDesdeUltimaMina += elapsed_time;

    if (getVidas() <= 0)
    {
        tempoDesdeMorte += elapsed_time;
    }

    mover();

    if (y > 2000)
    {
        recebeDano(1);
        y = 0;
        x = 0;
        velocidadeX = 0;
        velocidadeY = 0;
    }

    if (!noChao)
    {
        newState = JUMP;
    }

    if (tempoDesdeUltimoTiro <= SHOT_ANIMATION_TIME)
    {
        newState = SHOT;
    }

    if (tempoDesdeUltimoDano < HURT_ANIMATION_TIME)
    {
        newState = HURT;
    }

    if (tempoDesdeMorte > 0.0f && tempoDesdeMorte <= DEATH_ANIMATION_TIME)
    {
        newState = DEAD;
    }
    else if (tempoDesdeMorte > DEATH_ANIMATION_TIME)
    {
        morto = true;
    }

    if (mudouDirecao || newState != state)
    {
        state = newState;
        setAnimationState();
    }

    animacao.update();
    setPosition(x, y);
}

void Jogador::lidarColisao(sf::Vector2f intersecao, Entidade *other)
{
    sf::FloatRect pisoBounds = other->getSize();
    sf::FloatRect jogadorBounds = getSize();

    // Imprimir a interseção para debug
    // std::cout << "Colisão com " << other->getTipo() << ": x = " << intersecao.x << ", y = " << (intersecao.y) << std::endl;

    if (other->getTipo() == tipoDeEntidade::PLATAFORMA)
    {
        if (intersecao.y > 0)
        {
            y -= intersecao.y - 1;
            tempoDesdeUltimoPiso = 0.0f;
            velocidadeY = 0;
        }
    }
    if (other->getTipo() == tipoDeEntidade::FIGHTER && !static_cast<Fighter *>(other)->getMorto())
    {
        if (intersecao.x > 0)
        {
            x -= intersecao.x - 1;
            tempoDesdeUltimoPiso = 0.0f;
            velocidadeY = 0;
        }
    }
    else if (other->getTipo() == tipoDeEntidade::ATIRADOR && !static_cast<Atirador *>(other)->getMorto())
    {
        if (intersecao.x > 0)
        {
            x -= intersecao.x - 2;
            tempoDesdeUltimoPiso = 0.0f;
            velocidadeY = 0;
        }
    }

    else if (other->getTipo() == tipoDeEntidade::LIXO && tempoDesdeUltimoLixo >= COOLDOWN_LIXO)
    {
        slowness = 0.5;
        tempoDesdeUltimoLixo = 0;
    }
    else if (other->getTipo() == tipoDeEntidade::ESPINHO && tempoDesdeUltimoEspinho >= COOLDOWN_ESPINHO)
    {
        recebeDano(1);
        tempoDesdeUltimoEspinho = 0;
    }

    setPosition(x, y);
}

void Jogador::reduzirVelocidade(float fator)
{
    tempoDesdeUltimoLixo = 0.0f;
    slowness = fator;
}

void Jogador::aplicarForcaRepulsao(float forcaX, float forcaY)
{
    tempoDesdeUltimaMina = 0.0f;
    velocidadeX = forcaX;
    velocidadeY = forcaY;
}