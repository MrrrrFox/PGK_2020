#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "font.h"
#define Pi 3.14

// menad¿er rysowania - klasa odpowiedzialna za rysowanie suwaka i kó³ oraz obs³uge FPSów
class DrawManager : public sf::Drawable
{
private:
	// suwak
	const int slideX = 40;
	const int slideY = 256;
	sf::Uint8* slidePixels = new sf::Uint8[slideX * slideY * 4];
	sf::Texture* slideTexture = new sf::Texture();
	sf::RectangleShape* slide = new sf::RectangleShape(sf::Vector2f((float)slideX, (float)slideY));
	sf::VertexArray* slidePosition = new sf::VertexArray(sf::Lines, 2);
	// kó³ka - rozmiary
	const int circleX = 200;
	const int circleY = 200;


	// kolorowanie pikseli
	void colorPixels(sf::Uint8 *pixelsToColor, int sizeX, int pos_x, unsigned int pos_y, unsigned int r_value, unsigned int g_value, unsigned int b_value, unsigned int alpha_value)
	{
		pixelsToColor[4 * (pos_y * sizeX + pos_x) + 0] = r_value;
		pixelsToColor[4 * (pos_y * sizeX + pos_x) + 1] = g_value;
		pixelsToColor[4 * (pos_y * sizeX + pos_x) + 2] = b_value;
		pixelsToColor[4 * (pos_y * sizeX + pos_x) + 3] = alpha_value;
	}

public:
	unsigned int ratio = 128;
	// FPSy
	sf::Font font;
	sf::Text *text;

	// konstruktor
	DrawManager()
	{
		// tekst
		font.loadFromMemory(font_data, font_data_size);
		text = new sf::Text;
		text->setFont(font);
		text->setCharacterSize(14);
		text->setFillColor(sf::Color::White);

		// suwak
		for (int x = 0; x < slideX; ++x)
		{
			for (int y = 0; y < slideY; ++y)
			{
				colorPixels(slidePixels, slideX, x, y, y, y, y, 255);	// kolorujemy 3 zmienna, wiêc wraz ze wzrostem y suwak jest coraz bardziej czarny
			}
		}
		slideTexture->create(slideX, slideY);
		slideTexture->update(slidePixels);
		slide->setTexture(slideTexture);
		slide->setPosition(650, 172);
		slide->setOutlineColor(sf::Color::White);
		slide->setOutlineThickness(1.0f);
		// przesuwna czesc suwaka
		(*slidePosition)[0].color = (*slidePosition)[1].color = sf::Color::Green;
	}

	// ustawianie lub aktualizowanie tekstu
	void writeText(sf::RenderTarget& target, float x, float y, const char *str) const
	{
		text->setPosition(x, y);
		text->setString(str);
		target.draw(*text);
	}

	// rysowanie suwaka i poczatkowego ustawienia jego po³o¿enia 
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		writeText(target, 50, 50, "HSL");
		writeText(target, 380, 50, "HSV");
		writeText(target, 50, 300, "CMY");
		writeText(target, 380, 300, "RGB");

		target.draw(*slide);
		(*slidePosition)[0].position = sf::Vector2f(640, 173 + 255 - ratio);
		(*slidePosition)[1].position = sf::Vector2f(700, 173 + 255 - ratio);
		target.draw(*slidePosition);
	}

	// wypelnianie kurtyn do RGB i CMY
	void fillCurtainColor(sf::Texture *texture, sf::Color color)
	{
		sf::Uint8 *pixelsToFill = new sf::Uint8[circleX * circleY * 4];

		float radius = circleX / 2;
		float temp_r;

		for (int x = 0; x < circleX; ++x)
			for (int y = 0; y < circleY; ++y)
			{
				float x1 = x - radius;
				float y1 = radius - y;
				temp_r = sqrt(x1 * x1 + y1 * y1);

				if (temp_r >= radius)
				{
					colorPixels(pixelsToFill, circleX, x, y, 0, 0, 0, 0);
				}
				else
				{
					colorPixels(pixelsToFill, circleX, x, y, color.r, color.g, color.b, color.a);
				}
			}
		texture->update(pixelsToFill);
		delete[] pixelsToFill;
	}

	// wypelnianie kola HSL
	void cirlce_HSL(sf::Texture *texture, int parameter_L)
	{
		sf::Uint8 *pixelsToFill_HSL = new sf::Uint8[circleX * circleY * 4];

		float radius = circleX / 2, angle, cosinus;
		float temp_r, x1, y1;

		float H, S, L;
		float R, G, B;

		float temp1, temp2, c, light;
		float newR, newG, newB;

		for (int x = 0; x < 2 * radius; x++)
		{
			for (int y = 0; y < 2 * radius; y++)
			{
				x1 = x - radius;
				y1 = radius - y;
				temp_r = sqrt(x1 * x1 + y1 * y1);
				cosinus = x1 / temp_r;
				angle = acosl(cosinus);

				if (y > circleY / 2)
				{
					angle = 2 * Pi - angle;
				}

				H = angle / (2 * Pi);
				S = temp_r / radius;
				L = (255.0f - parameter_L) / 255.0f;

				if (temp_r <= radius)
				{
					c = (1 - fabs(2 * L - 1)) * S;
					temp1 = fmodf(H * 6.0, 2.0) - 1.0;
					temp2 = c * (1.0 - fabs(temp1));
					light = L - c / 2;

					if (x == radius && y == radius)
					{
						angle = 0;
					}

					if (angle >= 0 && angle < Pi / 3)
					{
						newR = c;
						newG = temp2;
						newB = 0;
					}
					else
					{
						if (angle >= Pi / 3.0 && angle < 2 * Pi / 3.0)
						{
							newR = temp2;
							newG = c;
							newB = 0;
						}
						else
						{
							if (angle >= 2 * Pi / 3.0 && angle < Pi)
							{
								newR = 0;
								newG = c;
								newB = temp2;
							}
							else
							{
								if (angle >= Pi && angle < Pi * 4 / 3.0)
								{
									newR = 0;
									newG = temp2;
									newB = c;
								}
								else
								{
									if (angle >= Pi * 4 / 3.0 && angle < Pi * 5 / 3.0)
									{
										newR = temp2;
										newG = 0;
										newB = c;
									}
									else
									{
										if (angle >= Pi * 5 / 3.0 && angle < 2 * Pi)
										{
											newR = c;
											newG = 0;
											newB = temp2;
										}
									}
								}
							}
						}
					}

					R = (newR + light) * 255;
					G = (newG + light) * 255;
					B = (newB + light) * 255;
				}
				if (temp_r >= radius)
				{
					colorPixels(pixelsToFill_HSL, circleX, x, y, 0, 0, 0, 0);
				}
				else
				{
					colorPixels(pixelsToFill_HSL, circleX, x, y, R, G, B, 255);
				}
			}
		}
		texture->update(pixelsToFill_HSL);
		delete[] pixelsToFill_HSL;
	}

	// wypelnianie kola HSV
	void circle_HSV(sf::Texture *texture, int parameter_V)
	{
		sf::Uint8 *pixelsToFill_HSV = new sf::Uint8[circleX * circleY * 4];

		float radius = circleX / 2, angle, cosinus;
		float temp_r, x1, y1;

		float H, S, V;
		float R, G, B;

		float temp_h, temp_h_int, temp1, temp2, temp3;
		float newR, newG, newB;


		for (int x = 0; x < circleX; x++)
		{
			for (int y = 0; y < circleY; y++)
			{
				x1 = x - radius;
				y1 = radius - y;
				temp_r = sqrt(x1 * x1 + y1 * y1);
				cosinus = x1 / temp_r;
				angle = acosl(cosinus);

				if (y > radius)
				{
					angle = 2 * Pi - angle;
				}

				H = angle / (2*Pi);
				S = temp_r / radius;
				V = (255.0f - parameter_V) / 255.0f;

				if (temp_r < radius)
				{
					if (S == 0)
					{
						R = V * 255.0;
						G = V * 255.0;
						B = V * 255.0;
					}
					else
					{
						temp1 = V * (1.0 - S);
						temp2 = V * (1.0 - S * fmodf(H * 6.0, 1.0));
						temp3 = V * (1.0 - S * (1.0 - fmodf(H * 6.0, 1.0)));

						if (angle >= 0 && angle < Pi / 3)
						{
							newR = V;
							newG = temp3;
							newB = temp1;
						}
						else
						{
							if (angle >= Pi / 3.0 && angle < 2 * Pi / 3.0)
							{
								newR = temp2;
								newG = V;
								newB = temp1;
							}
							else
							{
								if (angle >= 2 * Pi / 3.0 && angle < Pi)
								{
									newR = temp1;
									newG = V;
									newB = temp3;
								}
								else
								{
									if (angle >= Pi && angle < Pi * 4 / 3.0)
									{
										newR = temp1;
										newG = temp2;
										newB = V;
									}
									else
									{
										if (angle >= Pi * 4 / 3.0 && angle < Pi * 5 / 3.0)
										{
											newR = temp3;
											newG = temp1;
											newB = V;
										}
										else
										{
											if (angle >= Pi * 5 / 3.0 && angle < 2 * Pi)
											{
												newR = V;
												newG = temp1;
												newB = temp2;
											}
										}
									}
								}
							}
						}

						R = newR * 255;
						G = newG * 255;
						B = newB * 255;
					}
				}

				if (temp_r >= radius)
				{
					colorPixels(pixelsToFill_HSV, circleX, x, y, 0, 0, 0, 0);
				}
				else
				{
					colorPixels(pixelsToFill_HSV, circleX, x, y, R, G, B, 255);
				}
			}
		}
		texture->update(pixelsToFill_HSV);
		delete[] pixelsToFill_HSV;
	}

	// wypelnianie kola CMY
	void circle_CMY(sf::Texture *texture)
	{
		sf::Uint8 *pixelsToFill_CMY = new sf::Uint8[circleX * circleY * 4];

		float radius = circleX / 2, angle, cosinus;
		float temp_r, x1, y1;

		for (int x = 0; x < circleX; ++x)
		{
			for (int y = 0; y < circleY; ++y)
			{
				x1 = x - radius;
				y1 = radius - y;
				temp_r = sqrt(x1 * x1 + y1 * y1);
				cosinus = x1 / temp_r;
				angle = acosl(cosinus);

				if (y > radius)
				{
					angle = 2 * Pi - angle;
				}

				if (temp_r >= radius)
				{
					colorPixels(pixelsToFill_CMY, circleX, x, y, 0, 0, 0, 0);
				}
				else
				{
					colorPixels(pixelsToFill_CMY, circleX, x, y, 255 - 255 * temp_r / radius, 255 - 255 * angle / (2 * Pi), 0, 255);
				}
			}
		}
		texture->update(pixelsToFill_CMY);
		delete[] pixelsToFill_CMY;
	}

	// wypelnianie kola RGB
	void circle_RGB(sf::Texture *texture)
	{
		sf::Uint8 *pixelsToFill_RGB = new sf::Uint8[circleX * circleY * 4];

		float radius = circleX / 2, angle, cosinus;
		float temp_r, x1, y1;

		for (int x = 0; x < circleX; ++x)
		{
			for (int y = 0; y < circleY; ++y)
			{
				x1 = x - radius;
				y1 = radius - y;
				temp_r = sqrt(x1 * x1 + y1 * y1);
				cosinus = x1 / temp_r;
				angle = acosl(cosinus);

				if (y > radius)
				{
					angle = 2 * Pi - angle;
				}

				if (temp_r >= radius)
				{
					colorPixels(pixelsToFill_RGB, circleX, x, y, 0, 0, 0, 0);
				}
				else
				{
					colorPixels(pixelsToFill_RGB, circleX, x, y, 255 * temp_r / radius, 255 * angle / (2 * Pi), 0, 255);

				}
			}
		}
		texture->update(pixelsToFill_RGB);
		delete[] pixelsToFill_RGB;
	}

	// destruktor - delete wszystkiego co mia³o new :)
	~DrawManager()
	{
		delete text;
		delete slidePixels;
		delete slideTexture;
		delete slide;
		delete slidePosition;
	}
};


int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "GFK Lab 02", sf::Style::Titlebar | sf::Style::Close);
	sf::Clock clock;
	sf::Time time = sf::Time::Zero;
	unsigned int FPS = 0, frame_counter = 0;

	// menad¿er rysowania
	DrawManager drawManager;
	// tekst do FPSów i ratio
	char ratioText[15] = "ratio: ";
	char fpsCounter[10] = "0 FPS";
	// czy lewy przycisk myszki zosta³ wcisniety?
	bool lmkPressed = false;

	// ko³o HSL
	sf::Texture *HSLTexture = new sf::Texture;
	HSLTexture->create(200, 200);
	sf::Sprite HSLSprite;
	HSLSprite.setPosition(50, 50);
	drawManager.cirlce_HSL(HSLTexture, 255 - drawManager.ratio);
	HSLSprite.setTexture(*HSLTexture);

	// ko³o HSV
	sf::Texture *HSVTexture = new sf::Texture;
	HSVTexture->create(200, 200);
	sf::Sprite HSVSprite;
	HSVSprite.setPosition(380, 50);
	drawManager.circle_HSV(HSVTexture, 255 - drawManager.ratio);
	HSVSprite.setTexture(*HSVTexture);

	// ko³o CM + Y-kurtyna
	sf::Texture *CMYTexture = new sf::Texture;
	CMYTexture->create(200, 200);
	sf::Texture *YellowTexture = new sf::Texture;
	YellowTexture->create(200, 200);
	sf::Sprite CMYSprite;
	CMYSprite.setPosition(50, 300);
	sf::Sprite CMY_curtain;
	CMY_curtain.setPosition(50, 300);
	drawManager.circle_CMY(CMYTexture);
	drawManager.fillCurtainColor(YellowTexture, sf::Color(0, 0, 255, 255 - drawManager.ratio));
	CMY_curtain.setTexture(*YellowTexture);
	CMYSprite.setTexture(*CMYTexture);

	// Ko³o RG + B-kurtyna
	sf::Texture *RGBTexture = new sf::Texture;
	RGBTexture->create(200, 200);
	sf::Texture *BlueTexture = new sf::Texture;
	BlueTexture->create(200, 200);
	sf::Sprite RGBSprite;
	RGBSprite.setPosition(380, 300);
	sf::Sprite RGB_curtain;
	RGB_curtain.setPosition(380, 300);
	drawManager.circle_RGB(RGBTexture);
	drawManager.fillCurtainColor(BlueTexture, sf::Color(0, 0, 255, drawManager.ratio));
	RGB_curtain.setTexture(*BlueTexture);
	RGBSprite.setTexture(*RGBTexture);

	clock.restart().asMilliseconds();
	while (window.isOpen())
	{
		sf::Event event;
		window.clear(sf::Color::Black);

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else
			{
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Escape)
					{
						window.close();
					}
				}
				else
				{
					if (event.type == sf::Event::MouseButtonPressed)
					{
						if (event.mouseButton.button == sf::Mouse::Left)
						{
							if (event.mouseButton.x >= 651 and event.mouseButton.x <= 689 and event.mouseButton.y >= 173 and event.mouseButton.y <= 428)
							{
								drawManager.ratio = 255 - (event.mouseButton.y - 173);

								drawManager.cirlce_HSL(HSLTexture, 255 - drawManager.ratio);
								HSLSprite.setTexture(*HSLTexture);
								drawManager.circle_HSV(HSVTexture, 255 - drawManager.ratio);
								HSVSprite.setTexture(*HSVTexture);
								drawManager.fillCurtainColor(YellowTexture, sf::Color(0, 0, 255, 255 - drawManager.ratio));
								CMY_curtain.setTexture(*YellowTexture);
								drawManager.fillCurtainColor(BlueTexture, sf::Color(0, 0, 255, drawManager.ratio));
								RGB_curtain.setTexture(*BlueTexture);

								lmkPressed = true;
							}
						}
					}
					else
					{
						if (event.type == sf::Event::MouseMoved)
						{
							if (lmkPressed)
							{
								if (event.mouseMove.y >= 173 and event.mouseMove.y <= 428)
								{
									drawManager.ratio = 255 - (event.mouseMove.y - 173);

									drawManager.cirlce_HSL(HSLTexture, 255 - drawManager.ratio);
									HSLSprite.setTexture(*HSLTexture);
									drawManager.circle_HSV(HSVTexture, 255 - drawManager.ratio);
									HSVSprite.setTexture(*HSVTexture);
									drawManager.fillCurtainColor(YellowTexture, sf::Color(0, 0, 255, 255 - drawManager.ratio));
									CMY_curtain.setTexture(*YellowTexture);
									drawManager.fillCurtainColor(BlueTexture, sf::Color(0, 0, 255, drawManager.ratio));
									RGB_curtain.setTexture(*BlueTexture);
								}
							}
						}
						else
						{
							if (event.type == sf::Event::MouseButtonReleased)
							{
								if (event.mouseButton.button == sf::Mouse::Left)
								{
									if (lmkPressed)
									{
										lmkPressed = false;
									}
								}
							}
						}
					}
				}
			}
		}

		//tu wypisaæ na ekran wartoœæ FPS
		if (clock.getElapsedTime().asSeconds() >= 1.0f)
		{
			FPS = (unsigned int)((float)frame_counter / clock.getElapsedTime().asSeconds());
			clock.restart();
			frame_counter = 0;
			sprintf_s(fpsCounter, "%d FPS", FPS);
		}
		frame_counter++;
		drawManager.writeText(window, 640, 435, fpsCounter);

		sprintf_s(ratioText, "ratio: %d", drawManager.ratio);
		drawManager.writeText(window, 640, 140, ratioText);

		//tu wyrysowaæ wszystko na ekran
		window.draw(HSLSprite);
		window.draw(HSVSprite);
		window.draw(RGBSprite);
		window.draw(RGB_curtain, sf::BlendAdd);
		window.draw(CMYSprite);
		window.draw(CMY_curtain, sf::BlendAdd);
		window.draw(drawManager);
		window.display();
	}

	// zwalnianie tekstur i kurtyn
	delete HSLTexture;
	delete HSVTexture;
	delete CMYTexture;
	delete RGBTexture;
	delete BlueTexture;
	delete YellowTexture;
}

