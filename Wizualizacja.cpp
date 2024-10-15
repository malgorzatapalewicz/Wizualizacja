
// Naglowki
//#include "stdafx.h"
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <iostream>

// Kody shaderów
const GLchar* vertexSource = R"glsl(
#version 150 core
in vec3 position; //zmiana wymiaru    
in vec3 color;
out vec3 Color;
void main(){
	Color = color;
	gl_Position = vec4(position, 1.0); //zmienione z vec4(position, 0.0, 1.0);
}
)glsl"; 

const GLchar* fragmentSource = R"glsl(
#version 150 core
in vec3 Color;
out vec4 outColor;
void main(){
	outColor = vec4(Color, 1.0);
}
)glsl";


void checkShaderCompilation(GLuint shader, std::string shaderName) {
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	
	GLint logLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<GLchar> errorLog(logLength);
	glGetShaderInfoLog(shader, logLength, &logLength, errorLog.data());

	std::string message = (isCompiled == GL_TRUE)
		? ("Compilation " + shaderName + " OK")
		: ("Compilation " + shaderName + " ERROR\n" + errorLog.data());

	std::cout << message << std::endl;

	if (isCompiled == GL_FALSE) {
		glDeleteShader(shader);
	}
}


int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	// Okno renderingu
	sf::Window window(sf::VideoMode(800, 600, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);

	// Inicjalizacja GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// Utworzenie VAO (Vertex Array Object)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Utworzenie VBO (Vertex Buffer Object)
	// i skopiowanie do niego danych wierzcholkowych
	GLuint vbo;
	glGenBuffers(1, &vbo);

	GLfloat vertices[] = {
	0.0f, 0.5f, 1.0f, 1.0f, 0.7f, 0.7f, // x, y, z, rgb (zmienna normalizowana)
	0.5f, -0.5f, 0.0f, 0.7f, 1.0f, 0.7f,
	-0.5f, -0.5f, 0.0f, 0.7f, 0.7f, 1.0f
	};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Utworzenie i skompilowanie shadera wierzcholkow
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	checkShaderCompilation(vertexShader, "vertexShader");

	// Utworzenie i skompilowanie shadera fragmentow
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompilation(fragmentShader, "fragmentShader");

	// Zlinkowanie obu shaderow w jeden wspolny program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specifikacja formatu danych wierzcholkowych
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);  
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// Rozpoczecie petli zdarzen
	bool running = true;
	while (running) {
		sf::Event windowEvent;
		while (window.pollEvent(windowEvent)) {
			switch (windowEvent.type) {
			case sf::Event::Closed:
				running = false;
				break;
			}
		}
		// Nadanie scenie koloru czarnego
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Narysowanie trojkata na podstawie 3 wierzcholkow
		glDrawArrays(GL_TRIANGLES, 0, 3); //wie ze wspolrzedne trojkatne
		// Wymiana buforow tylni/przedni
		window.display();
	}
	// Kasowanie programu i czyszczenie buforów
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	// Zamkniecie okna renderingu
	window.close();
	return 0;
}