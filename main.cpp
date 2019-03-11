#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <string>

#include "CartesianAxis.hpp"

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

typedef sf::Vector2<float> Vec2;

/* the Lua interpreter */
lua_State* L;

Vec2 lua_r(double t)
{
  Vec2 rt;
  lua_getglobal(L, "r");
  lua_pushnumber(L, t);
  lua_call(L, 1, 2);
  rt.x = (double) lua_tonumber(L, -2);
  rt.y = (double) lua_tonumber(L, -1);
  lua_pop(L, 2);
  return rt;
}

class SimulationApp {
public:
  SimulationApp() : 
    elapsedTimeSinceLastUpdate(0),
    worldTime(0),
    worldView(sf::Vector2f(0.f, 0.f), sf::Vector2f(1200.f, 800.f)),
    guiView(sf::FloatRect(0.0, 0.0f, 1200.f, 800.f)) {

    rigidBody.setSize(sf::Vector2f(8, 8));
    rigidBody.setFillColor(sf::Color::Green);
    rigidBody.setOrigin(4, 4);

    font.loadFromFile("8bit.otf");
    text.setFont(font);
    text.setPosition(20, 20);
    text.setCharacterSize(20);

    paused = false;
    tracked = false;
    scale = 1.0f;
    
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Simulazione");
  }

  void mainLoop() {
    while (window.isOpen()) {
      float deltaTime = timer.restart().asSeconds();

      deltaTime *= scale;

      processInput();
      if (paused) deltaTime = 0;
      tick(deltaTime);
      render(deltaTime);
    }
  }
private:
  void processInput() {
    // while something is happening
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) worldView.move(sf::Vector2f(-0.3f, 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) worldView.move(sf::Vector2f(+0.3f, 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) worldView.move(sf::Vector2f(0, +0.3f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) worldView.move(sf::Vector2f(0, -0.3f));

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) worldView.zoom(1.001f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) worldView.zoom(0.999f);
    
    // something has changed
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      
      if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        case sf::Keyboard::R: restart(); break;
        case sf::Keyboard::P: paused = !paused; break;
        case sf::Keyboard::T: tracked = !tracked; break;
        }
      }
    }
  }

  void tick(float deltaTime) {
    elapsedTimeSinceLastUpdate += deltaTime;
    worldTime += deltaTime;

    if (elapsedTimeSinceLastUpdate >= TICKTIME) {
      elapsedTimeSinceLastUpdate -= TICKTIME;

      // do stuff
      Vec2 rt = lua_r(worldTime);
      
      // avoid making the square disappear
      float zoomlevel = 1200.f / worldView.getSize().x;
      if (zoomlevel < 0.3f) rigidBody.setScale(0.5f / zoomlevel, 0.5f / zoomlevel);
      else rigidBody.setScale(1.0f, 1.0f);

      sf::String s = "r(" + std::to_string(worldTime) + ") = (" + std::to_string(rt.x) + ", " + std::to_string(rt.y) + ")\n";
      s += "x" + std::to_string(zoomlevel) + "\n";
      s += "tracked: "; s += tracked ? "yes" : "no"; s += "\n";
      text.setString(s);
      rt.y = -rt.y;
      rigidBody.setPosition(rt);
      if (tracked) worldView.setCenter(rt);
    }
  }

  void render(float deltaTime) {
    window.clear(sf::Color(17, 131, 165));

    window.setView(worldView);
    window.draw(rigidBody);
    window.draw(axis);

    window.setView(guiView);
    window.draw(text);

    window.display();
  }

  void restart() {
    worldTime = 0;
    //scale = 1;
    paused = false;
    //zoom = 1;
    luaL_dofile(L, "formule.lua");
  }

  CartesianAxis axis;
  sf::RectangleShape rigidBody;

  sf::View worldView;
  sf::View guiView;
  sf::Text text;
  sf::Font font;
  sf::RenderWindow window;
  sf::Clock timer;
  float elapsedTimeSinceLastUpdate;
  float worldTime;
  float scale;
  bool paused;
  bool tracked;
  const float TICKTIME = 1.0f / 60.f;
  const int WIDTH = 1500, HEIGHT = 1000;
};


int main(int argc, char *argv[])
{
  L = luaL_newstate();
  luaL_openlibs(L);
  luaL_dofile(L, "formule.lua");

  SimulationApp s;
  s.mainLoop();
  
  lua_close(L);
  return 0;
}