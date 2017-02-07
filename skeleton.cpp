#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
float focalLength = SCREEN_HEIGHT;
vec3 cameraPos( 0, 0, -3 );
vector<Triangle> triangles;

struct Intersection
{
vec3 position;
float distance;
int triangleIndex;
};

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();
bool CheckConstrains( float t, float u, float v);
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& ClosestIntersection);

int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	LoadTestModel( triangles );

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

void Draw()
{
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

float f = float(focalLength);
float h = float(SCREEN_HEIGHT);
float w = float(SCREEN_WIDTH);

	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			vec3 d(x - w / 2, y - h / 2, f);

			float d2 = d.x * d.x + d.y *d.y + d.z * d.z;
			d.x = d.x / d2;
			d.y = d.y / d2;
			d.z = d.z / d2;

			Intersection intsc;
			bool flag = ClosestIntersection(cameraPos, d, triangles, intsc);
			vec3 color;
			if(flag){
				color = triangles[intsc.triangleIndex].color;
				PutPixelSDL( screen, x, y, color );
			}else{
				color = vec3( 0.0, 0.0, 0.0 );
				PutPixelSDL( screen, x, y, color );
			}

		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

bool CheckConstrains( float t, float u, float v)
{
	return (u >= 0) && (v >= 0) && (u + v <= 1) && (t >=0);
}

bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& ClosestIntersection)
{
	bool result = false;
	float minDistance = std::numeric_limits<float>::max();
	for( size_t i = 0; i < triangles.size(); ++i ){
		vec3 v0 = triangles[i].v0;
		vec3 v1 = triangles[i].v1;
		vec3 v2 = triangles[i].v2;

		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = start - v0;

		mat3 A( -dir, e1, e2 );
		vec3 x = glm::inverse( A ) * b;
		if( x.x < minDistance && CheckConstrains(x[0], x[1], x[2])){
			minDistance = x.x;
			ClosestIntersection.position = start + x.x * dir;
			ClosestIntersection.distance = x.x;
			ClosestIntersection.triangleIndex = i;
		}
	}
	if( minDistance < 9999.0){
		result = true;
	}
	return result;
}
