#include "Entidades/Personagens/Jogador.hpp"
#include <cmath> // Para usar std::max

const float GRAVIDADE = 9.8f;    // Aceleração da gravidade (em unidades por segundo^2)
const float TEMPO_FRAME = 0.16f; // Duração de cada frame (em segundos) - para 60 FPS

Jogador::Jogador(float px, float py, int vidas) : Personagem(px, py, vidas),
                                                  velocidadeY(0), velocidadeX(25), noChao(false),
                                                  animacao(), direcao(1), state(IDLE), velocidadeCorrida(1.5f * velocidadeX)
{
    animacao.addTrilha("idle", new TrilhaAnimacao(5, 15, 128, 128, 3, 3, "./assets/Gangsters_1/Idle.png"));
    animacao.addTrilha("running", new TrilhaAnimacao(9, 10, 128, 128, 3, 3, "./assets/Gangsters_1/Run.png"));
    animacao.addTrilha("walking", new TrilhaAnimacao(9, 10, 128, 128, 3, 3, "./assets/Gangsters_1/Walk.png"));
    animacao.addTrilha("jump", new TrilhaAnimacao(9, 10, 128, 128, 3, 3, "./assets/Gangsters_1/Jump.png"));
    animacao.setPosition(x, y);
    animacao.setScale(3.f, 3.f);
    setAnimationState();
}

void Jogador::setAnimationState()
{
    if (state == IDLE)
    {
        animacao.setTrilha("idle");
    }
    else if (state == WALK)
    {
        animacao.setTrilha("walking");
    }
    else if (state == RUN)
    {
        animacao.setTrilha("running");
    }
    else if (state == JUMP)
    {
        animacao.setTrilha("jump");
    }

    // Ajusta a escala com base na direção atual
    animacao.setScale(direcao * 3.f, 3.f);
}


void Jogador::atacar()
{
    // Implementação do ataque do jogador
}

void Jogador::executar()
{
    aplicarGravidade();
    estadoJogador newState = IDLE;

    float elapsed_time = pGG->getElapsedTime();
    bool mudouDirecao = false;

    // Verifica se o Shift está pressionado para correr
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        velocidadeX = velocidadeCorrida; // Velocidade de corrida
        newState = RUN;

        // Executa a corrida na direção atual
        if (direcao == -1) // Esquerda
        {
            x -= velocidadeX * (elapsed_time / 100.0);
        }
        else if (direcao == 1) // Direita
        {
            x += velocidadeX * (elapsed_time / 100.0);
        }
    }
    else
    {
        // Retorna à velocidade normal de caminhada
        velocidadeX = velocidadeCorrida / 1.5f;
    }

    // Verifica se a tecla A (esquerda) está pressionada
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        if (direcao != -1)
        {
            mudouDirecao = true;
            direcao = -1;
        }
        x -= velocidadeX * (elapsed_time / 100.0);
        newState = (newState == RUN) ? RUN : WALK;
    }
    // Verifica se a tecla D (direita) está pressionada
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        if (direcao != 1)
        {
            mudouDirecao = true;
            direcao = 1;
        }
        x += velocidadeX * (elapsed_time / 100.0);
        newState = (newState == RUN) ? RUN : WALK;
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        velocidadeX = velocidadeCorrida; // Velocidade de corrida
        if (direcao == -1)
        {
            x -= velocidadeX * (elapsed_time / 100.0);
        }
        else if (direcao == 1)
        {
            x += velocidadeX * (elapsed_time / 100.0);
        }
    }

    // Verifica o pulo
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && noChao)
    {
        velocidadeY = -sqrt(2 * GRAVIDADE * 100);
        noChao = false;
    }

    if (!noChao)
    {
        newState = JUMP;
    }

    // Atualiza a animação se a direção ou o estado mudar
    if (mudouDirecao || newState != state)
    {
        state = newState;
        setAnimationState();
    }

    animacao.update();
    animacao.setPosition(x, y);
}





void Jogador::aplicarGravidade()
{
    if (!noChao)
    {
        velocidadeY += GRAVIDADE * TEMPO_FRAME; // Aceleração devido à gravidade
        y += velocidadeY * TEMPO_FRAME;
    }

    if (y >= 686)
    { // "Chão" do jogo
        y = 686;
        velocidadeY = 0;
        noChao = true;
    }
}

void Jogador::desenhar()
{
    animacao.desenhar();
}
