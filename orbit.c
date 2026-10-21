// Primer Trabajo Práctico para la materia Modelos Físicos.
// autor: Laurentino Luna
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#define N_PARAMS 6
#define sq(x)    ((x)*(x))
#define cb(x)    ((x)*(x)*(x))
#define G        6.674e-11
#define STEPS    20

#define MAX_TRAIL      250
#define SCALE          250.0 / 1.496e11
#define SCALE_FIGURES  3

#define MAX_STARS      400

Vector2 versor(Vector2 v);
double rand_float(void);

int main(int argc, char **argv)
{
    InitWindow(800, 800, "Orbit Simulation");
    SetTargetFPS(60);

    double mass_sun = 1.989e30;
    double mass_planet = 5.972e24;
    Vector2 r = { 1.496e11, 0.0 };
    Vector2 v = { 0.0, 29780.0 };

    Vector2 trail[MAX_TRAIL] = { 0 };
    int trail_idx = 0, trail_len = 0;

    Vector2 stars[MAX_STARS] = { 0 };
    for (int i = 0; i < MAX_STARS; ++i)
        stars[i] = (Vector2){ GetRandomValue(0, GetScreenWidth()),
                              GetRandomValue(0, GetScreenHeight()) };

    while (!WindowShouldClose()) {
        double dt = 3600.0;

        for (int step = 0; step < STEPS; ++step) {
            double dist = sqrt(sq(r.x) + sq(r.y));
            double F = -G*((mass_planet*mass_sun)/sq(dist));
            Vector2 u = versor(r);
            Vector2 a = { F*u.x / mass_planet, F*u.y / mass_planet };

            // Update velocity
            v.x += a.x*dt;
            v.y += a.y*dt;

            // Update position
            r.x += v.x*dt;
            r.y += v.y*dt;
        }

        trail[trail_idx] = r;
        trail_idx = (trail_idx + 1) % MAX_TRAIL;
        trail_len += (trail_len < MAX_TRAIL);

        BeginDrawing();
        ClearBackground(BLACK);
        Vector2 sun = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
        Vector2 planet = { sun.x + r.x*SCALE, sun.y + r.y*SCALE };

        for (int i = 0; i < MAX_STARS; ++i)
            DrawPixelV(stars[i], Fade(WHITE, rand_float()));

        for (int i = 0; i < trail_len; ++i) {
            int idx = (trail_idx - 1 - i + MAX_TRAIL) % MAX_TRAIL;
            Vector2 dot = { sun.x + trail[idx].x*SCALE, sun.y + trail[idx].y*SCALE };
            DrawCircleV(dot, 2, Fade(BLUE, 0.8));
        }

        DrawCircleV(sun, 6*SCALE_FIGURES, YELLOW);
        DrawCircleV(planet, 6*SCALE_FIGURES, BLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

inline Vector2 versor(Vector2 v)
{
    double m = sqrt(sq(v.x) + sq(v.y));
    return (Vector2){ v.x/m, v.y/m };
}

double rand_float(void)
{
    return ((double)GetRandomValue(0, 100))/50.0;
}
