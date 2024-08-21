#pragma once

#include <SFML/Graphics.hpp>
#include "Entidades/Entidade.hpp"
#include "ElementosGraficos/ObjetoEstatico.hpp"

const int PROJETIL_WIDTH = 80;
const int PROJETIL_HEIGHT = 64 * 2;
class Projetil : public Entidade
{
private:
  ObjetoEstatico visual;
  int dano;
  int direcao;

public:
  Projetil(int px, int py, int direcao); // posição e escala não mudam
  ~Projetil();
  void desenhar();
  void executar();
  sf::Vector2f getCenter();
  sf::FloatRect getSize();
  void lidarColisao(sf::Vector2f intersecao, Entidade *other);
};