#include <iostream>
#include "objsdl/objsdl.h"
#include "mylibraries/dynarr.h"
#include "mylibraries/mat.h"

using namespace std;
using containers::DynArr;

constexpr int unit=20;
constexpr SDL::Point square(unit, unit);

template<typename mapa_typ>
bool IsFreePoint(const mapa_typ& mapa, SDL::Point point)
{
    return mapa[point.y/unit][point.x/unit]==' ';
}

template<typename mapa_typ>
bool IsFreeLine(const mapa_typ& mapa, SDL::Line line)
{
    SDL::Vector way(line);
    for(SDL::Vector piece(way.begin, 1, way.move.angle); piece.move.lenght<way.move.lenght; ++piece.move.lenght)
	{
        if(!IsFreePoint(mapa, piece.GetEnd()))
		{
			return false;
		}
	}
	return true;
}

struct Formule
{
	SDL::Point pos;
	SDL::Point move;
	Formule(SDL::Point begin):pos(begin), move(0,0){}
	SDL::Rect Destination()const
	{
		return SDL::Rect(pos+move-SDL::Point(unit,unit), square*3);
	}
	SDL::Rect Location()const
	{
		return SDL::Rect(pos, square);
	}
	template<typename mapa_typ>
	DynArr<SDL::Rect> FreeDestinations(const mapa_typ& mapa)const
	{
		auto position=Destination().Position();
		DynArr<SDL::Rect> destinations(9);
		destinations[0]=SDL::Rect(position+SDL::Point(0, 0), square);
		destinations[1]=SDL::Rect(position+SDL::Point(0, unit), square);
		destinations[2]=SDL::Rect(position+SDL::Point(0, 2*unit), square);
		destinations[3]=SDL::Rect(position+SDL::Point(unit, 0), square);
		destinations[4]=SDL::Rect(position+SDL::Point(unit, unit), square);
		destinations[5]=SDL::Rect(position+SDL::Point(unit, 2*unit), square);
		destinations[6]=SDL::Rect(position+SDL::Point(2*unit, 0), square);
		destinations[7]=SDL::Rect(position+SDL::Point(2*unit, unit), square);
		destinations[8]=SDL::Rect(position+SDL::Point(2*unit, 2*unit), square);
		for(size_t i=0;i<destinations.size();++i)
		{
			if(!IsFreeLine(mapa, SDL::Line(Location().Center(), destinations[i].Center())))
			{
				destinations.erase(i);
				--i;
			}
		}
		return destinations;
	}
};

template<typename mapa_typ>
void DrawMap(SDL::Renderer& rend, const mapa_typ& mapa)
{
	for(size_t y=0;y<func::Size(mapa);++y)
	{
		for(size_t x=0;x<func::Size(mapa[0]);++x)
		{
			if(mapa[y][x]=='#')
			{
				rend.Draw(SDL::Rect(x*unit, y*unit, square), {0, 0, 0});
			}
			rend.Draw(SDL::Line(SDL::Point(0, y)*unit, SDL::Point(mapa[0].size(), y)*unit), {0, 0, 0});
			rend.Draw(SDL::Line(SDL::Point(x, 0)*unit, SDL::Point(x, func::Size(mapa))*unit), {0, 0, 0});
		}
	}
}

int main()
{
	string mapa[]={
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
				"#################################################"
	};
	SDL::Window window("Formule", SDL::Rect(20, 40, 1000, 600));
	SDL::Renderer rend(window);

    Formule f1({200, 200});
    Formule f2({320, 200});
	bool red=true, repeat=true;
    while(repeat)
	{
		auto f1_dst=f1.FreeDestinations(mapa);
		auto f2_dst=f2.FreeDestinations(mapa);
		rend.Repaint(SDL::Color(255,255,255));
		for(auto& dst:red?f1_dst:f2_dst)
		{
			rend.Draw(dst, red?SDL::Color(255, 160, 160):SDL::Color(160, 160, 255));
		}
		rend.Draw(f1.Location(), {255, 0, 0});
		rend.Draw(f2.Location(), {0, 0, 255});
		DrawMap(rend, mapa);
		rend.Show();
		if(red&&f1_dst.empty())
		{
			SDL::MessageBox::Show("The end", "Red crashed!");
			break;
		}
		else if(f2_dst.empty())
		{
			SDL::MessageBox::Show("The end", "Blue crashed!");
			break;
		}
		for(auto& evt: SDL::events::Handler())
		{
			if(evt.Type()==SDL::events::Type::Quit)
			{
				repeat=false;
			}
			else if(evt.Type()==SDL::events::Type::MouseButtonDown)
			{
				SDL::Point click_pos=evt.MouseButton().Position;
				if(red)
				{
					for(auto& dest:f1_dst)
					{
						if(dest.Encloses(click_pos))
						{
							f1.move=click_pos/unit*unit-f1.pos;
							f1.pos=f1.pos+f1.move;
							red=false;
							break;
						}
					}
				}
				else
				{
					for(auto& dest:f2_dst)
					{
						if(dest.Encloses(click_pos))
						{
							f2.move=click_pos/unit*unit-f2.pos;
							f2.pos=f2.pos+f2.move;
							red=true;
							break;
						}
					}
				}
			}
		}
		SDL::Wait(20);
	}
    return 0;
}