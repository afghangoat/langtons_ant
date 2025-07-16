#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstring>

class ScreenData{
	public:
		static const constexpr int SCREEN_WIDTH=300;
		static const constexpr int SCREEN_HEIGHT=200;
};

void replace_pixel(sf::Uint8* pixels, int x, int y, int width, int r, int g, int b){
	int index = (y * width + x) * 4;
	pixels[index + 0] = r;
	pixels[index + 1] = g;
	pixels[index + 2] = b;
	pixels[index + 3] = 255;
}

struct PhaseEntry{
	int nextPhase;
	int r;
	int g;
	int b;
	int turn_type; //No turn, U turn, R turn, L turn
	PhaseEntry(int _n, int _r, int _g, int _b, int _t): nextPhase(_n), r(_r), g(_g), b(_b), turn_type(_t) {
		
	}
};

class Ant{
	private:
		int posX;
		int posY;
		int dir;
		std::vector<PhaseEntry*> states;

	public:
		Ant(int px, int py){
			posX=px;
			posY=py;
			dir=0;
			
			states.push_back(new PhaseEntry(1,255,255,255,2));
			states.push_back(new PhaseEntry(0,0,0,0,3));
		}
		Ant(int px, int py,const std::vector<PhaseEntry*>& newstates){
			posX=px;
			posY=py;
			dir=0;
			
			for(size_t i=0;i<newstates.size();i++){
				states.push_back(newstates[i]);
			}
		}
		void update(sf::Uint8* pixels,std::vector<int>& map,int w, int h){
			size_t current_phase=map[posY*w+posX];
			if(current_phase<states.size()){
				map[posY*w+posX]=states[current_phase]->nextPhase;
				switch(states[current_phase]->turn_type){
					case 0: //No turn
						break;
					case 1: //U turn
						dir+=2;
						break;
					case 2: //R turn
						dir++;
						break;
					case 3: //L turn
						dir--;
						break;
				}
				replace_pixel(pixels,posX,posY,w,states[current_phase]->r,states[current_phase]->g,states[current_phase]->b);
			}
			move(w,h);
		}
		void move(int w, int h){
			if (dir < 0){
				dir += 4;
			}
			dir %= 4;
			
			switch(dir){
				case 0: //UP
					posY--;
					break;
				case 1: //LEFT
					posX--;
					break;
				case 2: //DOWN
					posY++;
					break;
				case 3: //RIGHT
					posX++;
					break;
			}
			if(posX<0){
				posX+=w;
			}
			if(posY<0){
				posY+=h;
			}
			if(posX>=w){
				posX-=w;
			}
			if(posY>=h){
				posY-=h;
			}
		}
		~Ant(){
			for(auto it = states.begin();it!=states.end();++it){
				delete *it;
			}
		}
};

class GameField{
	private:
		sf::RenderWindow window;
		std::vector<Ant*> ants;
		
		std::vector<int> data;
		
		int m_width;
		int m_height;
		
		sf::Uint8* pixels;
		
		sf::Texture texture;
		
		sf::Sprite sprite;
		
		int wait_time;
		float m_scale;
		
	public:
		GameField(int width, int height, int scale, int wait): window(sf::VideoMode(width*scale, height*scale), "Langton's ant simulation"){
			window.setFramerateLimit(60);
			wait_time=wait;
			pixels = new sf::Uint8[width * height * 4];
			memset(pixels, 0, width * height * 4);
			m_scale=static_cast<float>(scale);
			
			texture.create(width, height);
			sf::Sprite sprite2(texture);
			sprite=sprite2;
			sprite.setScale((float)scale, (float)scale);
			
			data.resize(height*width);
			
			m_width=width;
			m_height=height;
		}
		void addAnt(Ant* a){
			ants.push_back(a);
		}
		void loop(){
			while (window.isOpen()) {
				sf::Event event;
				while (window.pollEvent(event)) {
					if (event.type == sf::Event::Closed){
						window.close();
					}
					

					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

						sf::Vector2i mousePos = sf::Mouse::getPosition(window);

						if (mousePos.x >= 0 && mousePos.y >= 0 ) {
							
							addAnt(new Ant(mousePos.x/m_scale,mousePos.y/m_scale));
						}
					}
				}
				
				for (auto& ant : ants) {
					ant->update(pixels, data, m_width, m_height);
				}
				
				texture.update(pixels);

				window.clear(sf::Color::Black);
				window.draw(sprite);
				window.display();
				if(wait_time>0){
					sf::sleep(sf::milliseconds(wait_time));
				}
			}
		}
		~GameField(){
			for(auto it = ants.begin();it!=ants.end();++it){
				delete *it;
			}
			delete[] pixels;
		}
};

int main() {
	
	GameField f(ScreenData::SCREEN_WIDTH, ScreenData::SCREEN_HEIGHT,4,0);
	
	//Roadbuilder
	std::vector<PhaseEntry*> custom_states;
	custom_states.push_back(new PhaseEntry(1,255,255,255,2));
	custom_states.push_back(new PhaseEntry(2,255,0,0,3));
	custom_states.push_back(new PhaseEntry(0,0,0,0,0));
	
	f.addAnt(new Ant(200,100,custom_states));
	
	//Stuck infinitely
	/*std::vector<PhaseEntry*> custom_states;
	custom_states.push_back(new PhaseEntry(1,255,255,255,2));
	custom_states.push_back(new PhaseEntry(2,255,0,0,3));
	custom_states.push_back(new PhaseEntry(0,0,0,0,1));
	
	f.addAnt(new Ant(200,100,custom_states));*/
	
	//Double expander
	//f.addAnt(new Ant(200,100));
	//f.addAnt(new Ant(200,101));
	
	//Quad expander
	//f.addAnt(new Ant(200,100));
	//f.addAnt(new Ant(200,101));
	//f.addAnt(new Ant(200,102));
	//f.addAnt(new Ant(200,103));
	
	f.loop();

    return 0;
}
