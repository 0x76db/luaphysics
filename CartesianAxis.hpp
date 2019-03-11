#pragma once
#include <SFML/Graphics.hpp>

class CartesianAxis : public sf::Drawable, public sf::Transformable {
public:
  CartesianAxis() {
    outline = sf::Color(156, 229, 251);
    verts[0].color = outline;
    verts[0].position = sf::Vector2f(INT32_MIN, 0);
    verts[1].color = outline;
    verts[1].position = sf::Vector2f(INT32_MAX, 0);

    verts[2].color = outline;
    verts[2].position = sf::Vector2f(0, INT32_MIN);
    verts[3].color = outline;
    verts[3].position = sf::Vector2f(0, INT32_MAX);
  }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(&verts[0], 2, sf::LineStrip, states);
    target.draw(&verts[2], 2, sf::LineStrip, states);
  }

private:
  sf::Vertex verts[4];
  sf::Color outline;
};