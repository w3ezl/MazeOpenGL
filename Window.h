#ifndef WINDOW_H_
#define WINDOW_H_

#include <memory>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

class Window
{
public:
	static constexpr int DEFAULT_WIDTH = 1920;
	static constexpr int DEFAULT_HEIGHT = 1080;
protected:
	std::shared_ptr<SDL_Window> _window;
	std::shared_ptr<void> _gl_context;
	int _width, _height;
public:
	Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	virtual ~Window() = default;
	int width() const { return _width; }
	int height() const { return _height; }
	void main_loop();
	virtual void setup() { }
	virtual void render() { }
	virtual void handle_event(SDL_Event &event) { }
	virtual void handle_logic() { }
	virtual void handle_keys(const Uint8 * keys) { }
};
#endif
