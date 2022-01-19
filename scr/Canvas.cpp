#include <iostream>
#include <vector>
#include <cmath>
#include <array>
#include <fstream>
#include <iostream>
#include <string>


#include "iwd_GLI.h"; 
#include "AI.h"


void getCursorPos(int(&a)[2]); 
std::array<short, 10> parseSave(std::string inp);
void drawRect(const int, const int, const int, const int); 
void events();
void saveLists(); 
void readLists(); 

int height = 250; 
int width = 250; 
GLFWwindow* window;

struct Map {
	std::array<std::array<short, 10>, 10> pixels;
	std::string type = "none"; 
};

Map currentMap;
std::vector<Map> saveList; 

class GL_Draw {
public:

	int windowHeight = height; 
	int windowWidth = width;
	GLFWwindow* GLwindow = window;

	GL_Draw() {

	}

	void init() {
		std::cout << "initializing GL..." << std::endl;
		initGL();//
	}

	void draw() {
		glClear(GL_COLOR_BUFFER_BIT);



		glfwSwapBuffers(GLwindow);
 	}

	~GL_Draw() {
		std::cout << "Destructing GL..." << std::endl;
		glfwTerminate(); //
	}
};

std::vector<double> trainDrawingAI(std::vector<AI>& ais) {
	 

	std::vector<double> scores(ais.size());

	if (saveList.size() == 0)
				readLists();

	for (int i = 0; i < saveList.size(); i++) {

		std::vector<double> inp;

		for (int i3 = 0; i3 < saveList[i].pixels.size(); i3++) {
			for (int i4 = 0; i4 < saveList[i].pixels[i3].size(); i4++) {
				inp.push_back((double)saveList[i].pixels[i3][i4]);
			}
		}


		std::string guess = "none";

		for (int i3 = 0; i3 < ais.size(); i3++) {
			std::vector<double> out = ais[i3].input(inp);

			guess = "none"; 

			if (out[0] > 0.5) {
				guess = "triangle";
			}
			if (out[0] < 0.5) {
				guess = "rectangle";
			}

			if (saveList[i].type == "rectangle") {
				scores[i3] += (0.5 - out[0]);
			}
			else {
				scores[i3] += (out[0] - 0.5);
			}

		}

	}
	return scores; 
}


GL_Draw gl;

void canvasWithAI(AI& ai) {
	gl.init();
	while (!glfwWindowShouldClose(window)) {

		events();

		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < currentMap.pixels.size(); i++) {
			for (int i2 = 0; i2 < currentMap.pixels[i].size(); i2++) {
				if (currentMap.pixels[i][i2] == 1) {
					drawRect(i * 25, i2 * 25, 25, 25);
				}
			}
		}

		glfwSwapBuffers(window);

		glfwPollEvents();


		std::vector<double> inp;

		for (int i3 = 0; i3 < currentMap.pixels.size(); i3++) {
			for (int i4 = 0; i4 < currentMap.pixels[i3].size(); i4++) {
				inp.push_back((double)currentMap.pixels[i3][i4]);
			}
		}


		std::vector<double> out = ai.input(inp);

		if (out[0] > 0.5) {
			std::cout << "AI said: triangle (" << out[0] << ")" << std::endl;
		}
		if (out[0] < 0.5) {
			std::cout << "AI said: rectangle (" << out[0] << ")" << std::endl;
		}

	}

	saveLists();

}

void runCanvas() { //std::vector<AI>& ais

	gl.init(); 
	while (!glfwWindowShouldClose(window)) {

		events();

		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < currentMap.pixels.size(); i++) {
			for (int i2 = 0; i2 < currentMap.pixels[i].size(); i2++) {
				if (currentMap.pixels[i][i2] == 1) {
					drawRect(i * 25, i2 * 25, 25, 25);
				}
			}
		}

		glfwSwapBuffers(window);

		glfwPollEvents();


	}

	saveLists(); 


}


void events() {
	if (glfwGetMouseButton(window, 0) == GL_TRUE) {
		int pos[2]; 
		getCursorPos(pos); 
		if (pos[0] > 0 && pos[0] < width && pos[1] > 0 && pos[1] < height)
			currentMap.pixels[pos[0] / 25][pos[1] / 25] = 1;
	}

	if (glfwGetMouseButton(window, 1) == GL_TRUE) {
		int pos[2];
		getCursorPos(pos);
		if (pos[0] > 0 && pos[0] < width && pos[1] > 0 && pos[1] < height)
			currentMap.pixels[pos[0] / 25][pos[1] / 25] = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GL_TRUE) {
		for (int i = 0; i < currentMap.pixels.size(); i++) {
			std::fill(currentMap.pixels[i].begin(), currentMap.pixels[i].end(), 0);
		}
	}

	static bool pressedOnce = false; 
	bool pressed = false; 

	if (glfwGetKey(window, GLFW_KEY_R) == GL_TRUE) {
		pressed = true; 
		if (!pressedOnce) {
			pressedOnce = true;
			currentMap.type = "rectangle";
			std::cout << "Changed current Map type to: rectangle" << std::endl;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GL_TRUE) {
		pressed = true;

		if (!pressedOnce) {
			pressedOnce = true;
			currentMap.type = "triangle";
			std::cout << "Changed current Map type to: triangle" << std::endl;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GL_TRUE) {
		pressed = true;

		if (!pressedOnce) {
			pressedOnce = true;
			currentMap.type = "triangle";
			std::cout << "Changed current Map type to: triangle" << std::endl;
		}
	}

	if (!pressed) {
		pressedOnce = false;
	}


	static bool saved = false; 
	if (glfwGetKey(window, GLFW_KEY_S) == GL_TRUE) {
		if (!saved) {
			std::cout << "saved current map" << std::endl; 
			saved = true;
			saveList.push_back(currentMap);
		}
	}
	else {
		saved = false; 
	}

}

void getCursorPos(int(&a)[2]) {

	double x;
	double y;

	glfwGetCursorPos(window, &x, &y);

	a[0] = (int)x;
	a[1] = (int)y;
}

void readLists() {
	std::ifstream file; 
	file.open("saveLists.txt"); 

	if (!file.is_open()) {
		std::cout << "Could not open save file" << std::endl; 
		return; 
	}

	std::string line; 

	std::getline(file, line); 

	int i = -1; 
	int pixCount = 0; 

	while (line != "end") {
		

		if (line[0] == 'r' || line[0] == 'c' || line[0] == 't') {
			i++;
			saveList.push_back(Map());
			saveList[i].type = line; 
			pixCount = 0; 

		}
		else {
			std::array<short, 10> parsed = parseSave(line); 

			saveList[i].pixels[pixCount] = parsed; 
			pixCount++; 
		}

		 
		std::getline(file, line); 
	}

	std::cout << "Loaded saved Maps into memory" << std::endl; 
}

void saveLists() {
	std::ofstream file; 
	file.open("saveLists.txt", std::ios::app); 

	if (!file.is_open()) {
		std::cout << "Could not open save file" << std::endl; 
		return; 
	}

	for (int i = 0; i < saveList.size(); i++) {
		file << saveList[i].type << "\n";
		for (int i2 = 0; i2 < saveList[i].pixels.size(); i2++) {
			for (int i3 = 0; i3 < saveList[i].pixels[i2].size(); i3++) {
				file << saveList[i].pixels[i2][i3]; 
			}
			file << "\n"; 
		}
	}

	std::cout << "Sucesfully saved Maps" << std::endl; 
	file.close(); 
}

std::array<short, 10> parseSave(std::string inp) {
	std::array<short, 10> parsed; 
	std::fill(parsed.begin(), parsed.end(), 0); 
	int counter = 0; 

	for (int i = 0; i < inp.size(); i++) {
		if (inp[i] == '1' || inp[i] == '0') {
			parsed[counter] = (int)inp[i] - (int)'0'; 
			counter++; 
		}
	}

	return parsed;
}

void drawRect(const int x, const int y, const int w, const int h) {
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}


/*
NN Inputs: 

*/
