#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <string>

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
    elapsedTimeSinceLastUpdate(0), worldTime(0),
    verticalLine(sf::LineStrip, 2),
    horizontalLine(sf::LineStrip, 2), 
    rigidBody(sf::Quads, 4) {
    verticalLine[0].color = sf::Color::Red;
    verticalLine[0].position = Vec2(100, 0);
    verticalLine[1].color = sf::Color::Red;
    verticalLine[1].position = Vec2(100, HEIGHT);

    horizontalLine[0].color = sf::Color::Red;
    horizontalLine[0].position = Vec2(0, 900);
    horizontalLine[1].color = sf::Color::Red;
    horizontalLine[1].position = Vec2(WIDTH, 900);

    rigidBody[0].color = sf::Color::Green;
    rigidBody[0].position = Vec2(-3, -3);
    rigidBody[1].color = sf::Color::Green;
    rigidBody[1].position = Vec2(-3, 3);
    rigidBody[2].color = sf::Color::Green;
    rigidBody[2].position = Vec2(3, 3);
    rigidBody[3].color = sf::Color::Green;
    rigidBody[3].position = Vec2(3, -3);

    font.loadFromFile("8bit.otf");
    text.setFont(font);
    text.setPosition(20, 20);

    clear = true;
    paused = false;
    scale = 1.0f;
    zoom = 1.0f;
    
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
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
          restart();
        }
        if (event.key.code == sf::Keyboard::P) {
          paused = !paused;
        }
        if (event.key.code == sf::Keyboard::A) {
          scale *= 0.90f;
        }
        if (event.key.code == sf::Keyboard::D) {
          scale /= 0.90f;
        }
        if (event.key.code == sf::Keyboard::W) {
          zoom *= 0.99f;
        }
        if (event.key.code == sf::Keyboard::S) {
          zoom /= 0.99f;
        }
        if (event.key.code == sf::Keyboard::T) {
          clear = !clear;
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
      sf::String s = "r(" + std::to_string(worldTime) + ") = (" + std::to_string(rt.x) + ", " + std::to_string(rt.y) + ")\n";
      s += "x" + std::to_string(scale) + "\n";
      s += "z = " + std::to_string(zoom);
      text.setString(s);
      Vec2 t = Vec2(rt.x * zoom + 100, HEIGHT - 100 - rt.y * zoom);
      moveBody(t);
    }
  }

  void render(float deltaTime) {
    if (clear)
    window.clear();
    window.draw(verticalLine);
    window.draw(horizontalLine);
    window.draw(rigidBody);
    window.draw(text);
    window.display();
  }

  void moveBody(Vec2 newPos) {
    for (int i = 0; i < 4; i++) {
      rigidBody[0].position = Vec2(-3, -3) + newPos;
      rigidBody[1].position = Vec2(-3, 3) + newPos;
      rigidBody[2].position = Vec2(3, 3) + newPos;
      rigidBody[3].position = Vec2(3, -3) + newPos;
    }
  }

  void restart() {
    worldTime = 0;
    //scale = 1;
    paused = false;
    //zoom = 1;
    luaL_dofile(L, "formule.lua");
  }

  sf::VertexArray verticalLine;
  sf::VertexArray horizontalLine;

  sf::VertexArray rigidBody;

  sf::Text text;
  sf::Font font;
  sf::RenderWindow window;
  sf::Clock timer;
  bool clear;
  float elapsedTimeSinceLastUpdate;
  float worldTime;
  float scale;
  float zoom;
  bool paused;
  const float TICKTIME = 1.0f / 60.f;
  const int WIDTH = 1600, HEIGHT = 1000;
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