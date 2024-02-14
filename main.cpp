#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <SDL.h>

constexpr int unit = 25;

using Terrain = std::vector<std::string>;

// Solve n1/d1 < n2/d2 for non-negative integers
bool IsLesserRatio(int n1, int d1, int n2, int d2)
{
	if (d1 == 0) return false;
	if (d2 == 0) return true;
	return std::abs(d2) * std::abs(n1) < std::abs(d1) * std::abs(n2);
}

class RaceCar
{
private:
	int x, y, dx = 0, dy = 0;

	static bool MovePossible(const Terrain &terrain, int x, int y, int x2, int y2, int avoid_x, int avoid_y)
	{
		int w = x2 - x, h = y2 - y, rx = 0, ry = 0;
		int dx = w < 0 ? -1 : w > 0 ? 1 : 0, dy = h < 0 ? -1 : h > 0 ? 1 : 0;

		bool possible = terrain[y][x] == ' ' && !(x == avoid_x && y == avoid_y);
		while (possible && !(rx == w && ry == h))
		{
			if (ry == h || IsLesserRatio(rx + dx, w, ry + dy, h))
			{
				rx += dx;
			}
			else
			{
				ry += dy;
			}
			possible &= terrain[y + ry][x + rx] == ' ' && !(x + rx == avoid_x && y + ry == avoid_y);
		}
		return possible;
	}
public:
	RaceCar(int x, int y) : x(x), y(y) {}

	void Draw(SDL_Renderer *rend)
	{
		SDL_Rect rect{x * unit, y * unit, unit, unit};
		SDL_RenderFillRect(rend, &rect);
	}

	std::vector<SDL_Point> FreeDestinations(const Terrain &terrain, const RaceCar& other)
	{
		int x2 = x + dx, y2 = y + dy;
		SDL_Point possibilities[] = {
			SDL_Point{x2, y2},
			SDL_Point{x2, y2 + 1},
			SDL_Point{x2, y2 - 1},
			SDL_Point{x2 + 1, y2},
			SDL_Point{x2 + 1, y2 + 1},
			SDL_Point{x2 + 1, y2 - 1},
			SDL_Point{x2 - 1, y2},
			SDL_Point{x2 - 1, y2 + 1},
			SDL_Point{x2 - 1, y2 - 1}
		};

		std::vector<SDL_Point> destinations;
		for (auto point : possibilities)
		{
			if (MovePossible(terrain, x, y, point.x, point.y, other.x, other.y))
			{
				destinations.push_back(point);
			}
		}
		return destinations;
	}

	void Move(int dest_x, int dest_y)
	{
		dx = dest_x - x;
		dy = dest_y - y;
		x += dx;
		y += dy;
	}
};

void DrawTerrain(SDL_Renderer *rend, const Terrain &terrain)
{
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	for (int y = 0; y < terrain.size(); ++y)
	{
		for (int x = 0; x < terrain[y].size(); ++x)
		{
			if (terrain[y][x] == '#')
			{
				SDL_Rect rect{x * unit, y * unit, unit, unit};
				SDL_RenderFillRect(rend, &rect);
			}
			SDL_RenderDrawLine(rend, 0, y * unit, terrain[y].size() * unit, y * unit);
			SDL_RenderDrawLine(rend, x * unit, 0, x * unit, terrain.size() * unit);
		}
	}
}

int main()
{
	Terrain terrain = {
		"#################################################",
		"####               ##############         #######",
		"####               ##############         #######",
		"####               #        #             #######",
		"####                   #        #         #######",
		"####               ##############         #######",
		"####               ##############         #######",
		"####               ##############         #######",
		"####               ##############         #######",
		"####               ##############         #######",
		"####               ############           #######",
		"####               ############      ############",
		"####               ############           #######",
		"####               ##############         #######",
		"####               ##############         #######",
		"####               ##############         #######",
		"####               ##############         #######",
		"####               ##############         #######",
		"####    #          ##############         #######",
		"####               ##############         #######",
		"####                                      #######",
		"####                                      #######",
		"####                                      #######",
		"####      ######                          #######",
		"####      #    #                          #######",
		"####      #    #                          #######",
		"####           #                          #######",
		"####           #   ##############         #######",
		"####           #   ##############         #######",
		"#################################################"};

	SDL_Window *window = SDL_CreateWindow("RaceCar", 20, 40, unit * terrain[0].size(), unit * terrain.size(), 0);
	SDL_Renderer *rend = SDL_CreateRenderer(window, -1, 0);

	RaceCar car1(10, 10);
	RaceCar car2(16, 10);
	bool red = true, repeat = true;
	while (repeat)
	{
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
		SDL_RenderClear(rend);

		auto &playing_car = red ? car1 : car2;
		auto destinations = playing_car.FreeDestinations(terrain, red ? car2 : car1);

		SDL_SetRenderDrawColor(rend, red ? 255 : 160, 160, red ? 160 : 255, 255);
		for (auto dst : destinations)
		{
			SDL_Rect rect{dst.x * unit, dst.y * unit, unit, unit};
			SDL_RenderFillRect(rend, &rect);
		}

		SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
		car1.Draw(rend);
		SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
		car2.Draw(rend);

		DrawTerrain(rend, terrain);
		SDL_RenderPresent(rend);
		if (destinations.empty())
		{
			SDL_ShowSimpleMessageBox(0, "The end", red ? "Red crashed!" : "Blue crashed!", 0);
			break;
		}
		SDL_Event evt;
		while (SDL_PollEvent(&evt))
		{
			if (evt.type == SDL_QUIT)
			{
				repeat = false;
			}
			else if (evt.type == SDL_MOUSEBUTTONDOWN)
			{
				for (auto dst : destinations)
				{
					if (evt.button.x / unit == dst.x && evt.button.y / unit == dst.y)
					{
						playing_car.Move(evt.button.x / unit, evt.button.y / unit);
						red = !red;
						break;
					}
				}
			}
		}
		SDL_Delay(20);
	}

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);
	return 0;
}