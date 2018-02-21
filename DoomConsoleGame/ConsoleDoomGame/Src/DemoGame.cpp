#include "DemoGame.h"
#include "platform.h"
#include "File.h"

void DemoGame::LoadContent()
{
	playerX = 1;
	playerY = 1;
	playerAngle = 0;
	FOV = PI / 4.0f;
	depth = 40.0f;

	mapW = 40;
	mapH = 20;

	map =
	{
		L"########################################"
		L"#..........D...........................#"
		L"#..........#######.....................#"
		L"#..........#.....#.....................#"
		L"#......S...#.....#.....................#"
		L"############.....#.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................D.....................#"
		L".................D.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................#.....................#"
		L".................#######################"
	};

	// Load the pillar
	int pillarHandle = platform_fileOpen("data/pillar_01.sprt", "rb");
	FileReadData pillarData = platform_fileReadEntire(pillarHandle);
	platform_fileClose(pillarHandle);
	SpriteHeader* pillarHeader = (SpriteHeader*)pillarData.Data;
	if (pillarHeader)
	{
		if (pillarHeader->Sentinal[0] == 'S' && pillarHeader->Sentinal[1] == 'P' && pillarHeader->Sentinal[2] == 'R' && pillarHeader->Sentinal[3] == 'T')
		{
			pillar.Width = pillarHeader->Width;
			pillar.Height = pillarHeader->Height;
			u16* colors = (u16*)((u8*)pillarData.Data + pillarHeader->ColorOffset);
			wchar_t* pixels = (wchar_t*)((u8*)pillarData.Data + pillarHeader->PixelOffset);
			pillar.Colors = CreateArray(Memory::GetPersistantHandle(), u16, pillar.Width * pillar.Height);
			pillar.Pixels = CreateArray(Memory::GetPersistantHandle(), wchar_t, pillar.Width * pillar.Height);

			memcpy_s(pillar.Colors, pillar.Width*pillar.Height * sizeof(u16), colors, pillar.Width*pillar.Height * sizeof(u16));
			memcpy_s(pillar.Pixels, pillar.Width*pillar.Height * sizeof(wchar_t), pixels, pillar.Width*pillar.Height * sizeof(wchar_t));

			if(pillarData.Data)
				free(pillarData.Data);
		}
	}
	
	pillarX = 27.5f;
	pillarY = 5.5f;
	
	for (int y = 0; y < mapH; ++y)
	{
		for (int x = 0; x < mapW; ++x)
		{
			if (map[y * mapW + x] == 'S')
			{
				playerX = x;
				playerY = y;
			}
		}
	}
}

bool DemoGame::Update(float deltaTime)
{
	bool Quit = false;
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		Quit = true;

	float playerSpeed = 10.0f;

	if (Controller)
	{
		// Forward / Backward Movement
		if (Controller->MoveUp.pressed)
		{
			playerX += sinf(playerAngle) * playerSpeed * deltaTime;
			playerY += cosf(playerAngle) * playerSpeed * deltaTime;

			if (map[(int)playerY * mapW + (int)playerX] == L'#')
			{
				playerX -= sinf(playerAngle) * playerSpeed * deltaTime;
				playerY -= cosf(playerAngle) * playerSpeed * deltaTime;
			}
		}
		else if (Controller->MoveDown.pressed)
		{
			playerX -= sinf(playerAngle) * playerSpeed * deltaTime;
			playerY -= cosf(playerAngle) * playerSpeed * deltaTime;

			if (map[(int)playerY * mapW + (int)playerX] == L'#')
			{
				playerX += sinf(playerAngle) * playerSpeed * deltaTime;
				playerY += cosf(playerAngle) * playerSpeed * deltaTime;
			}
		}

		// Roatting the Angle CCW / CW
		if (Controller->MoveLeft.pressed)
		{
			playerAngle -= (playerSpeed * 0.75f) * deltaTime;
		}
		else if (Controller->MoveRight.pressed)
		{
			playerAngle += (playerSpeed * 0.75f) * deltaTime;
		}

		// Straffe Movement
		if (Controller->ActionLeft.pressed)
		{
			playerX -= cosf(playerAngle) * playerSpeed * deltaTime;
			playerY += sinf(playerAngle) * playerSpeed * deltaTime;

			if (map[(int)playerY * mapW + (int)playerX] == L'#')
			{
				playerX += cosf(playerAngle) * playerSpeed * deltaTime;
				playerY -= sinf(playerAngle) * playerSpeed * deltaTime;
			}
		}
		else if (Controller->ActionRight.pressed)
		{
			playerX += cosf(playerAngle) * playerSpeed * deltaTime;
			playerY -= sinf(playerAngle) * playerSpeed * deltaTime;

			if (map[(int)playerY * mapW + (int)playerX] == L'#')
			{
				playerX -= cosf(playerAngle) * playerSpeed * deltaTime;
				playerY += sinf(playerAngle) * playerSpeed * deltaTime;
			}
		}
	}

	// Clear buffer to certain color
	//ClearBuffer(PIXEL_COLOR_GREY);

	// Ray Casting Routine
	int screenWidth = renderer.GetRenderBuffers()->Width;
	int screenHeight = renderer.GetRenderBuffers()->Height;

	for (int x = 0; x < renderer.GetRenderBuffers()->Width; ++x)
	{
		// Find the ray angle based on the players Rotattion angle
		float RayAngle = (playerAngle - FOV / 2) + ((float)x / screenWidth) * FOV;

		float StepSize = 0.01f;			// This is how far we will advanced the ray per iteration
		float DistanceToWall = 0.0f;	// 

		bool HitWall = false;
		bool Boundary = false;

		float EyeX = sinf(RayAngle);
		float EyeY = cosf(RayAngle);

		short Color = PIXEL_COLOR_WHITE;

		while (!HitWall && DistanceToWall < depth)
		{
			DistanceToWall += StepSize;

			i32 TestX = (playerX + EyeX * DistanceToWall);
			i32 TestY = (playerY + EyeY * DistanceToWall);

			r32 DX = (playerX + EyeX * DistanceToWall);
			r32 DY = (playerY + EyeY * DistanceToWall);

			// Our ray has gone Out of bounds
			if (TestX < 0 || TestX >= mapW || TestY < 0 || TestY >= mapH)
			{
				HitWall = true;
				DistanceToWall = depth;
			}
			else
			{
				// Ray is still in bounds to test cell for walls
				if (map[(i32)TestY * mapW + (i32)TestX] == L'#')
				{
					// Ray has hit a wall
					HitWall = true;

					// Calculate the Texture coordinates here.
					Color = PIXEL_COLOR_DARK_CYAN;

					// Calculate Boundaries of walls
					// To highlight tile boundaries, cast a ray from each corner
					// of the tile, to the player. The more coincident this ray
					// is to the rendering ray, the closer we are to a tile 
					// boundary, which we'll shade to add detail to the walls
					std::vector<std::pair<float, float>> p;

					// Test each corner of hit tile, storing the distance from
					// the player, and the calculated dot product of the two rays
					for (int tx = 0; tx < 2; tx++)
						for (int ty = 0; ty < 2; ty++)
						{
							// Angle of corner to eye
							float vy = (float)TestY + ty - playerY;
							float vx = (float)TestX + tx - playerX;
							float d = sqrt(vx*vx + vy*vy);
							float dot = (EyeX * vx / d) + (EyeY * vy / d);
							p.push_back(std::make_pair(d, dot));
						}

					// Sort Pairs from closest to farthest
					sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });

					// First two/three are closest (we will never see all four)
					float Bound = 0.01;
					if (acos(p.at(0).second) < Bound) Boundary = true;
					if (acos(p.at(1).second) < Bound) Boundary = true;
					//if (acos(p.at(2).second) < Bound) Boundary = true;
				}
				// Ray is still in bounds to test cell for Doors
				else if (map[(i32)TestY * mapW + (i32)TestX] == L'D')
				{
					r32 Dh = TestX + 0.5f;

					if (DX > Dh - 0.05f && DX < Dh + 0.05f)
					{
						// Calculate the Texture coordinates here.
						Color = PIXEL_COLOR_DARK_MAGENTA;

						// Ray has hit a wall
						HitWall = true;
					}
				}
			}
		}

		// Calculate the distance to the ceiling
		int DistanceToCeiling = (((float)screenHeight / 2) - ((float)screenHeight / DistanceToWall));
		int DistanceToFloor = (screenHeight - DistanceToCeiling);
		
		renderer.GetRenderBuffers()->DepthBuffer[x] = DistanceToWall;

		// Shade walls based on distance 
		wchar_t shade = PIXEL_SOLID;
		
		if (DistanceToWall <= depth / 4)
		{
			shade = PIXEL_SOLID;
			//Color = PIXEL_COLOR_WHITE;
		}
		else if (DistanceToWall < depth / 3)
		{
			shade = PIXEL_SEMI_DARK;
			//Color = PIXEL_COLOR_WHITE;
		}
		else if (DistanceToWall < depth / 2)
		{
			shade = PIXEL_MEDIUM_DARK;
			//Color = PIXEL_COLOR_GREY;
		}
		else
		{
			shade = PIXEL_DARK;
			//Color = PIXEL_COLOR_BLACK;
		}

		if (Boundary)
		{
			shade = PIXEL_DARK;
			Color = PIXEL_COLOR_BLACK;
		}

		for (int y = 0; y < screenHeight; ++y)
		{
			if (y <= DistanceToCeiling)
			{
				renderer.DrawPixel(x, y, PIXEL_SOLID, PIXEL_COLOR_BLACK);
			}
			else if (y > DistanceToCeiling && y <= DistanceToFloor)
			{
				renderer.DrawPixel(x, y, shade, Color);
			}
			else
			{
				// Shade floor based on distance
				float b = 1.0f - (((float)y - screenHeight / 2.0f) / ((float)screenHeight / 2.0f));
				if (b < 0.25)
				{
					shade = PIXEL_SOLID;
					Color = PIXEL_COLOR_DARK_GREEN;
				}
				else if (b < 0.5)
				{
					shade = PIXEL_SEMI_DARK;
					Color = PIXEL_COLOR_DARK_GREEN;
				}
				else if (b < 0.75)
				{
					shade = PIXEL_MEDIUM_DARK;
					Color = PIXEL_COLOR_DARK_GREEN;
				}
				else if (b < 0.9)
				{
					shade = PIXEL_DARK;
					Color = PIXEL_COLOR_DARK_GREEN;
				}
				renderer.DrawPixel(x, y, shade, Color);
			}
		}
	}

	// drawing Objects
	// We want to calculate the distance between the object and the player
	float directionToObjX = pillarX - playerX;
	float directionToObjY = pillarY - playerY;
	float distanceToPlayer = sqrtf(directionToObjX*directionToObjX + directionToObjY*directionToObjY);

	// Then we want to create our forward vector
	// Calculate the objects angle between the forward vector and direction that the object is in.
	float EyeX = sinf(playerAngle);
	float EyeY = cosf(playerAngle);
	float objectAngle = atan2f(EyeY, EyeX) - atan2f(directionToObjY, directionToObjX);
	if (objectAngle < -PI)
		objectAngle += 2.0f * PI;
	if (objectAngle > PI)
		objectAngle -= 2.0f * PI;

	// Here we check to see if the angle is with in the view.
	bool ObjectInView = fabs(objectAngle) < FOV / 2.0f;

	// If the object is in view and the distance from the object to the player is with in a certain range
	if (ObjectInView && distanceToPlayer >= 0.5f && distanceToPlayer < depth)
	{
		// Then we want to calculate the objects dimensions in world space.
		// First we calculate where the objects ceiling starts based on the distance
		// Then the floor location
		// this will give us how tall the object will look with in our view. The farther the smaller it will be. The closer the bigger it will be.
		float ObjectCeiling = (float)(screenHeight / 2.0f) - screenHeight / distanceToPlayer;
		float ObjectFloor = screenHeight - ObjectCeiling;
		float ObjectHeight = ObjectFloor - ObjectCeiling;
		float ObjectAspectRatio = (float)pillar.Height / (float)pillar.Width;
		float ObjectWidth = ObjectHeight / ObjectAspectRatio;
		float ObjectCenter = (0.5f * (objectAngle / (FOV / 2.0f)) + 0.5f) * (float)screenWidth;

		for (int y = 0; y < ObjectHeight; ++y)
		{
			for (int x = 0; x < ObjectWidth; ++x)
			{
				float SampleX = x / ObjectWidth;
				float SampleY = y / ObjectHeight;
				int Row = SampleY * pillar.Height;
				int Col = SampleX * pillar.Width;
				wchar_t glyph = pillar.Pixels[Row * pillar.Width + Col];
				u16 color = pillar.Colors[Row * pillar.Width + Col];
				int ObjectCol = (int)(ObjectCenter + x - (ObjectWidth / 2.0f));
				if (ObjectCol >= 0 && ObjectCol < screenWidth)
				{
					if (glyph != ' ' && renderer.GetRenderBuffers()->DepthBuffer[ObjectCol] >= distanceToPlayer)
						renderer.DrawPixel(ObjectCol, ObjectCeiling + y, glyph, color);
				}
			}
		}
	}

	// 2D Map
	int Screen1X = 10;
	int Screen1Y = 10;

	// Draw Map
	for (int y = 0; y < mapH; ++y)
	{
		for (int x = 0; x < mapW; ++x)
		{
			if (map[y * mapW + x] == '#')
			{
				renderer.DrawPixel(Screen1X + x, Screen1Y + y, PIXEL_SOLID, PIXEL_COLOR_DARK_RED); // Walls
			}
			else
				renderer.DrawPixel(Screen1X + x, Screen1Y + y, PIXEL_SOLID, PIXEL_COLOR_GREY); // Floor
		}
	}

	// This will draw the players field of view on the 2D map.
	for (int x = 0; x < mapW; ++x)
	{
		float ViewAngle = (playerAngle - FOV / 2) + ((float)x / mapW) * FOV;
		float EyeX = sinf(ViewAngle);
		float EyeY = cosf(ViewAngle);

		for (int y = 0; y < mapH; ++y)
		{
			renderer.DrawPixel((int)(playerX + Screen1X + EyeX * y), (int)(playerY + Screen1Y + EyeY * y), PIXEL_SOLID, PIXEL_COLOR_LIGHT_BLUE);
		}
	}
	// Draw player
	renderer.DrawPixel((int)(playerX + Screen1X), (int)(playerY + Screen1Y), PIXEL_SOLID, PIXEL_COLOR_LIGHT_GREEN);

	renderer.DrawPixel((int)(pillarX + Screen1X), (int)(pillarY + Screen1Y), PIXEL_SEMI_DARK, PIXEL_COLOR_LIGHT_RED);

	// Present buffers to the screen
	renderer.PresentBuffer();

	return Quit;
}

void DemoGame::UnloadContent()
{

}