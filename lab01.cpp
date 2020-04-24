// Calosc mozna dowolnie edytowac wedle uznania. 

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "font.h"

class Menu : public sf::Drawable
{
private:
	sf::Font font;
	sf::Text *text;
	sf::RectangleShape *rectangle;
  
	sf::Texture *colors_texture;
	sf::Sprite *colors_sprite;
	sf::Uint8 *colors_pixels;
	const unsigned int colors_size_x = 765;
	const unsigned int colors_size_y = 60;
	inline void draw_to_color_pixels(unsigned int x, unsigned int y,unsigned char r, unsigned char g, unsigned char b)
	{
		colors_pixels[4 * (y * colors_size_x + x) + 0] = r;
		colors_pixels[4 * (y * colors_size_x + x) + 1] = g;
		colors_pixels[4 * (y * colors_size_x + x) + 2] = b;
		colors_pixels[4 * (y * colors_size_x + x) + 3] = 255;
	}

public:
	Menu()
	{
		font.loadFromMemory(font_data, font_data_size);
		text = new sf::Text;
		text->setFont(font);
		text->setCharacterSize(12);
		text->setFillColor(sf::Color::White);

		rectangle = new sf::RectangleShape(sf::Vector2f(796, 536));
		rectangle->setFillColor(sf::Color::Transparent);
		rectangle->setOutlineColor(sf::Color::White);
		rectangle->setOutlineThickness(1.0f);
		rectangle->setPosition(2, 62);

		unsigned int x, y;
		colors_pixels = new sf::Uint8[colors_size_x * colors_size_y * 4];
		for (x = 0; x < 255; x++)
		{
			for (y = 0; y < 30; y++)
			{
				draw_to_color_pixels(x, y, x, 255, 0);
				draw_to_color_pixels(x + 255, y, 255, 255 - x, 0);
				draw_to_color_pixels(x + 510, y, 255, 0, x);
				draw_to_color_pixels(254 - x, y + 30, 0, 255, 255 - x);
				draw_to_color_pixels(509 - x, y + 30, 0, x, 255);
				draw_to_color_pixels(764 - x, y + 30, 255 - x, 0, 255);
			}
		}
		colors_texture = new sf::Texture();
		colors_texture->create(colors_size_x, colors_size_y);
		colors_texture->update(colors_pixels);

		colors_sprite = new sf::Sprite();
		colors_sprite->setTexture(*colors_texture);
		colors_sprite->setPosition(1, 1);
	}

	void outtextxy(sf::RenderTarget& target, float x, float y, const wchar_t *str) const
	{
		text->setPosition(x, y); 
		text->setString(str); 
		target.draw(*text);
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		outtextxy(target, 5, 600, L"f - wybór koloru rysowania");
		outtextxy(target, 5, 615, L"b - wybór koloru wype³niania");
		outtextxy(target, 5, 630, L"l - rysowanie linii");

		outtextxy(target, 200, 600, L"r - rysowanie prostok¹ta");
		outtextxy(target, 200, 615, L"a - rysowanie wype³nionego prostok¹ta");
		outtextxy(target, 200, 630, L"c - rysowanie okrêgu");

		outtextxy(target, 470, 600, L"w - zapis do pliku");
		outtextxy(target, 470, 615, L"o - odczyt z pliku");
		outtextxy(target, 470, 630, L"esc - wyjœcie");

		outtextxy(target, 650, 615, L"Aktualne:");

		target.draw(*rectangle);
		target.draw(*colors_sprite);
	}

	// funkcja zwracaj¹ca paletê kolorów - pobieranie koloru prze GetPixel()
	sf::Image ColorPallet()
	{
		return colors_texture->copyToImage();
	}

	// destruktor Menu - zwalnianie alokacji pamieci
	~Menu()
	{
		delete text;
		delete rectangle;
		delete colors_texture;
		delete colors_sprite;
		delete colors_pixels;
	}
	
};

// klasa odpowiedzialna za obsluge myszki, rysowanie itp
class DrawManager : public sf::Drawable
{
public:
	// kolory i kafelki z wybranymi kolorami
	sf::Color outsideColor = sf::Color(0, 0, 0, 0);
	sf::Color insideColor = sf::Color(0, 0, 0, 0);
	sf::RectangleShape outsideColorTile, insideColorTile;
	// mozliwe stany
	char statesTab[6] = { 'f', 'b', 'l', 'r', 'a', 'c' };
	char currentState = 'f';
	// obsluga klikniec
	sf::Vector2f firstPoint, secondPoint;
	bool mousePressed;
	// bufor
	sf::RenderTexture bufferTexture;
	sf::Sprite bufferSprite;

	// konstruktor
	DrawManager()
	{
		// kafelek zewnetrznego koloru
		outsideColorTile.setPosition(767, 1);
		outsideColorTile.setSize(sf::Vector2f(32, 29)); // 32 bo mamy rozmiar do 800, kolorowe paski do 765, 1 pixel przerwy i od 767 kafelek do konca
		outsideColorTile.setOutlineColor(sf::Color::Transparent);
		outsideColorTile.setOutlineThickness(0);
		outsideColorTile.setFillColor(outsideColor);
		// kafelek wewnetrznego koloru
		insideColorTile.setPosition(767, 31);
		insideColorTile.setSize(sf::Vector2f(32, 29));
		insideColorTile.setOutlineColor(sf::Color::Transparent);
		insideColorTile.setOutlineThickness(0);
		insideColorTile.setFillColor(insideColor);
		// ustawienie poczatkowych wartosci zmiennych zwiazanych z obsluga klikniec
		resetClickVars();
		// bufor
		bufferTexture.create(800, 600);
		bufferTexture.clear();
		bufferSprite.setTexture(bufferTexture.getTexture());
	}

	// funkcja resetujaca wartosci zmiennych zwiazanych z obsluga klikniec
	void resetClickVars()
	{
		firstPoint = secondPoint = sf::Vector2f(0, 0);
		mousePressed = false;
	}

	// funkcja odpowiedzialna za rysowanie figur
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		if (currentState == 'l') // linia
		{
			sf::VertexArray line(sf::LinesStrip, 2);
			line[0].position = firstPoint;
			line[0].color = outsideColor;
			line[1].position = secondPoint;
			line[1].color = insideColor;
			target.draw(line);
		}
		else
		{
			if (currentState == 'r' || currentState == 'a') // prostokaty
			{
				sf::RectangleShape rectangle(sf::Vector2f(secondPoint.x - firstPoint.x, secondPoint.y - firstPoint.y));
				rectangle.setPosition(firstPoint);
				rectangle.setOutlineColor(outsideColor);
				rectangle.setOutlineThickness(1.0f);
				rectangle.setFillColor(currentState == 'r' ? sf::Color::Transparent : insideColor);
				target.draw(rectangle);
			}
			else
			{
				if (currentState == 'c') // kolo
				{
					float rad = sqrt(pow(secondPoint.x - firstPoint.x, 2) + pow(secondPoint.y - firstPoint.y, 2)) / 2;
					sf::CircleShape circle(rad);
					circle.setPosition(firstPoint.x + (secondPoint.x - firstPoint.x) / 2 - rad, firstPoint.y + (secondPoint.y - firstPoint.y) / 2 - rad);
					circle.setRotation(atan2f(secondPoint.x - firstPoint.x, secondPoint.y - firstPoint.y));
					circle.setOutlineColor(outsideColor);
					circle.setOutlineThickness(1.0f);
					circle.setFillColor(sf::Color::Transparent);
					target.draw(circle);
				}
			}
		}
	}
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 650), "GFK Lab 01",sf::Style::Titlebar | sf::Style::Close);
	sf::Event event;
	Menu menu;

	sf::Image colorPallet = menu.ColorPallet();
	DrawManager drawManager;

 
	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		window.clear(sf::Color::Black);
   
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			//Tu trzeba obsluzyc zdarzenia zwiazane z myszka oraz klawiatura. 
			else
			{
				if (event.type == sf::Event::MouseButtonPressed) // klik
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						if (drawManager.currentState == 'f' || drawManager.currentState == 'b') // wybieranie kolorów
						{
							if (event.mouseButton.x <= 765 && event.mouseButton.y <= 60)
							{
								if (drawManager.currentState == 'f')
								{
									drawManager.outsideColor = colorPallet.getPixel(event.mouseButton.x, event.mouseButton.y);
									drawManager.outsideColorTile.setFillColor(drawManager.outsideColor);
								}
								else
								{
									if (drawManager.currentState == 'b')
									{
										drawManager.insideColor = colorPallet.getPixel(event.mouseButton.x, event.mouseButton.y);
										drawManager.insideColorTile.setFillColor(drawManager.insideColor);
									}
								}
							}
						}
						else
						{
							if (event.mouseButton.y > 60 && event.mouseButton.y <= 600) // jezeli nie na palecie, to bedziemy rysowac - zmiana flagi
							{
								drawManager.mousePressed = true;
								drawManager.firstPoint = sf::Vector2f(static_cast<float> (event.mouseButton.x), static_cast<float> (event.mouseButton.y));
								drawManager.secondPoint = drawManager.firstPoint;

							}
						}
					}
				}
				else
				{
					if (event.type == sf::Event::MouseMoved) // przemieszczanie sie myszki
					{
						if (event.mouseMove.x > 1 && event.mouseMove.x < 799 && event.mouseMove.y > 60 && event.mouseMove.y < 600 && drawManager.mousePressed)
						{
							drawManager.secondPoint = sf::Vector2f(static_cast<float> (event.mouseMove.x), static_cast<float> (event.mouseMove.y));
						}
					}
					else
					{
						if (event.type == sf::Event::MouseButtonReleased) // puszczenie myszki
						{
							if (event.mouseButton.button == sf::Mouse::Left and drawManager.mousePressed)
							{
								drawManager.mousePressed = false;
								drawManager.bufferTexture.draw(drawManager);
								drawManager.bufferTexture.display();
								drawManager.resetClickVars();
							}
						}
						else
						{
							if (event.type == sf::Event::KeyPressed) // inne przyciski
							{
								if (event.key.code == sf::Keyboard::Escape)
								{
									window.close();
								}
								else
								{
									if (event.key.code + 'a' == 'w') // zapis
									{
										drawManager.bufferSprite.getTexture()->copyToImage().saveToFile("MyPaintScreen.jpg");
									}
									else
									{
										if (event.key.code + 'a' == 'o') // wczytanie
										{
											sf::Texture fromFile;
											fromFile.loadFromFile("MyPaintScreen.jpg");
											drawManager.bufferTexture.draw(sf::Sprite(fromFile));
										}
										else
										{
											for (int i = 0; i < sizeof(drawManager.statesTab); ++i) // pozostale wbudowane opcje
											{
												if (event.key.code + 'a' == drawManager.statesTab[i])
												{
													drawManager.currentState = event.key.code + 'a';
													drawManager.resetClickVars();
													break;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
   //Draw BEGIN
   // Pomiêdzy "Draw BEGIN" a "Draw End" trzeba "cos" dopisac :-). Na pewno znajd¹ siê tam te dwie linijki:
		
		wchar_t statesTabtr[2] = { static_cast<wchar_t> (drawManager.currentState), 0 };

		window.draw(drawManager.bufferSprite);
		window.draw(drawManager);
		window.draw(drawManager.outsideColorTile);
		window.draw(drawManager.insideColorTile);
		menu.outtextxy(window, 720, 615, statesTabtr);
		
		window.draw(menu);
		window.display();

   //Draw END
	}
	return 0;
}
