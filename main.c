#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#define UNIT 25
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))
#define DESTINATION_COUNT 9

const char terrain[][50] = {
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

// Solve n1/d1 < n2/d2 for non-negative integers
bool is_lesser_ratio(int n1, int d1, int n2, int d2)
{
	if (d1 == 0) return false;
	if (d2 == 0) return true;
	return abs(d2) * abs(n1) < abs(d1) * abs(n2);
}

bool is_move_possible(int x, int y, int x2, int y2, int avoid_x, int avoid_y)
{
	int w = x2 - x, h = y2 - y, rx = 0, ry = 0;
	int dx = w < 0 ? -1 : w > 0 ? 1 : 0, dy = h < 0 ? -1 : h > 0 ? 1 : 0;

	bool possible = terrain[y][x] == ' ' && !(x == avoid_x && y == avoid_y);
	while (possible && !(rx == w && ry == h))
	{
		if (ry == h || is_lesser_ratio(rx + dx, w, ry + dy, h))
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

typedef struct race_car
{
	int x, y, dx, dy;
} race_car;

void draw_race_car(SDL_Renderer *rend, race_car car, int r, int g, int b)
{
	SDL_SetRenderDrawColor(rend, r, g, b, 255);
	SDL_Rect rect = {car.x * UNIT, car.y * UNIT, UNIT, UNIT};
	SDL_RenderFillRect(rend, &rect);
}

void move_race_car(race_car *car, int dest_x, int dest_y)
{
	car->dx = dest_x - car->x;
	car->dy = dest_y - car->y;
	car->x = dest_x;
	car->y = dest_y;
}

typedef struct destinations
{
	int x[DESTINATION_COUNT], y[DESTINATION_COUNT];
	bool allowed[DESTINATION_COUNT];
} destinations;

destinations free_destinations(race_car this, race_car other)
{
	int x2 = this.x + this.dx, y2 = this.y + this.dy;
	destinations result = {{x2, x2, x2, x2 + 1, x2 + 1, x2 + 1, x2 - 1, x2 - 1, x2 - 1},
						   {y2, y2 + 1, y2 - 1, y2, y2 + 1, y2 - 1, y2, y2 + 1, y2 - 1}};

	for (int i = 0; i < DESTINATION_COUNT; ++i)
	{
		result.allowed[i] = is_move_possible(this.x, this.y, result.x[i], result.y[i], other.x, other.y);
	}
	return result;
}

void draw_terrain(SDL_Renderer *rend)
{
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	for (int y = 0; y < ARRAY_SIZE(terrain); ++y)
	{
		for (int x = 0; x < ARRAY_SIZE(terrain[0]); ++x)
		{
			if (terrain[y][x] == '#')
			{
				SDL_Rect rect = {x * UNIT, y * UNIT, UNIT, UNIT};
				SDL_RenderFillRect(rend, &rect);
			}
			SDL_RenderDrawLine(rend, 0, y * UNIT, ARRAY_SIZE(terrain[0]) * UNIT, y * UNIT);
			SDL_RenderDrawLine(rend, x * UNIT, 0, x * UNIT, ARRAY_SIZE(terrain) * UNIT);
		}
	}
}

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("RaceCar", 20, 40, UNIT * ARRAY_SIZE(terrain[0]), UNIT * ARRAY_SIZE(terrain), 0);
	SDL_Renderer *rend = SDL_CreateRenderer(window, -1, 0);

	race_car car1 = {10, 10, 0, 0};
	race_car car2 = {16, 10, 0, 0};
	bool red = true, repeat = true;
	while (repeat)
	{
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
		SDL_RenderClear(rend);

		destinations dst = free_destinations(red ? car1 : car2, red ? car2 : car1);
		SDL_SetRenderDrawColor(rend, red ? 255 : 160, 160, red ? 160 : 255, 255);
		bool any_destination = false;
		for (int i = 0; i < DESTINATION_COUNT; ++i)
		{
			if (dst.allowed[i])
			{
				SDL_Rect rect = {dst.x[i] * UNIT, dst.y[i] * UNIT, UNIT, UNIT};
				SDL_RenderFillRect(rend, &rect);
				any_destination = true;
			}
		}

		draw_race_car(rend, car1, 255, 0, 0);
		draw_race_car(rend, car2, 0, 0, 255);

		draw_terrain(rend);
		SDL_RenderPresent(rend);
		if (!any_destination)
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
				for (int i = 0; i < DESTINATION_COUNT; ++i)
				{
					if (dst.allowed[i] && evt.button.x / UNIT == dst.x[i] && evt.button.y / UNIT == dst.y[i])
					{
						move_race_car(red ? &car1 : &car2, dst.x[i], dst.y[i]);
						red = !red;
					}
				}
			}
		}
		SDL_Delay(20);
	}

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}