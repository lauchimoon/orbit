// Primer Trabajo Práctico para la materia Modelos Físicos.
// autor: Laurentino Luna
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "raylib.h"

#define N_PARAMS 6
#define sq(x)    ((x)*(x))
#define cb(x)    ((x)*(x)*(x))
#define G        6.674e-11
#define STEPS    10

#define MAX_TRAIL      250
#define SCALE          (250.0 / 1.496e11)
#define SCALE_FIGURES  3

#define MAX_STARS      400
#define EXPLOSION_TEXT "El planeta ha explotado :("

#define T_MAX    3.156e7

#define MAX_INPUT_CHARS 15

typedef struct {
    Rectangle bounds;
    char text[MAX_INPUT_CHARS + 1];
    int len;
    bool active;
} TextBox;

Vector2 versor(Vector2 v);
double rand_float(void);
void textbox_update(TextBox *tb, bool mouse_pressed, Vector2 mouse);
void textbox_draw(TextBox tb, const char *label);
void sim_console(double x0, double y0, double vx0, double vy0);
void sim_graphic(void);

double mass_sun = 1.989e30;
double mass_planet = 5.972e24;

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("uso: %s c | g\n", argv[0]);
        return 1;
    }

    if (!strcmp(argv[1], "c")) {
        if (argc < 6) {
            printf("uso: %s c x0 y0 vx0 vy0\n", argv[0]);
            return 1;
        }

        double x0, y0, vx0, vy0;
        x0 = atof(argv[2]); y0 = atof(argv[3]);
        vx0 = atof(argv[4]); vy0 = atof(argv[5]);
        sim_console(x0, y0, vx0, vy0);
    } else if (!strcmp(argv[1], "g"))
        sim_graphic();
    else {
        printf("error: opcion '%s' desconocida\n", argv[1]);
        return 1;
    }
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

void textbox_update(TextBox *tb, bool mouse_pressed, Vector2 mouse)
{
    if (mouse_pressed)
        tb->active = CheckCollisionPointRec(mouse, tb->bounds);

    if (!tb->active)
        return;

    int c;
    while ((c = GetCharPressed()) > 0) {
        bool digit = (c >= '0' && c <= '9');
        bool point = (c == '.' && strchr(tb->text, '.') == NULL);
        bool minus = (c == '-' && tb->len == 0);

        if (tb->len < MAX_INPUT_CHARS && (digit || point || minus)) {
            tb->text[tb->len++] = (char)c;
            tb->text[tb->len] = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && tb->len > 0)
        tb->text[--tb->len] = '\0';

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        tb->active = false;
}

void textbox_draw(TextBox tb, const char *label)
{
    DrawText(label, (int)tb.bounds.x, (int)tb.bounds.y - 20, 20, LIGHTGRAY);

    DrawRectangleRec(tb.bounds, Fade(DARKGRAY, 0.85f));
    DrawRectangleLinesEx(tb.bounds, tb.active ? 2.0f : 1.0f,
                         tb.active ? BLUE : GRAY);

    int text_y = (int)(tb.bounds.y + (tb.bounds.height - 20)/2.0f);
    DrawText(tb.text, (int)tb.bounds.x + 8, text_y, 20, WHITE);

    if (tb.active && ((int)(GetTime()*2) % 2 == 0)) {
        int tw = MeasureText(tb.text, 20);
        DrawRectangle((int)tb.bounds.x + 8 + tw, (int)tb.bounds.y + 6,
                      2, (int)tb.bounds.height - 12, WHITE);
    }
}

void sim_console(double x0, double y0, double vx0, double vy0)
{
    printf("t,x,y,vx,vy\n");
    double dt = 3600.0;
    Vector2 r = { x0, y0 };
    Vector2 v = { vx0, vy0 };

    for (double t = 0.0; t <= T_MAX; t += dt) {
        printf("%g,%g,%g,%g,%g\n", t, r.x, r.y, v.x, v.y);
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
}

void sim_graphic(void)
{
    InitWindow(800, 800, "Orbit Simulation");
    SetTargetFPS(60);

    Texture explosion = LoadTexture("explosion.png");
    int explosion_anim_counter = 0;

    Vector2 r = { 1.496e11, 0.0 };
    Vector2 v = { 0.0, 29780.0 };

    Vector2 trail[MAX_TRAIL] = { 0 };
    int trail_idx = 0, trail_len = 0;

    Vector2 stars[MAX_STARS] = { 0 };
    for (int i = 0; i < MAX_STARS; ++i)
        stars[i] = (Vector2){ GetRandomValue(0, GetScreenWidth()),
                              GetRandomValue(0, GetScreenHeight()) };

    bool collision = false;
    bool placed = true;

    TextBox vx_box = { .bounds = { 20, 55, 140, 30 }, .text = "0", .len = 1 };
    TextBox vy_box = { .bounds = { 180, 55, 140, 30 }, .text = "29780", .len = 5 };
    Rectangle ui_panel = { 10, 15, 330, 80 };

    while (!WindowShouldClose()) {
        double dt = 3600.0;
        Vector2 sun = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
        Vector2 mouse = GetMousePosition();
        bool mouse_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        textbox_update(&vx_box, mouse_pressed, mouse);
        textbox_update(&vy_box, mouse_pressed, mouse);

        bool over_ui = CheckCollisionPointRec(mouse, ui_panel);

        if (mouse_pressed && !over_ui) {
            Vector2 r_click = { (mouse.x - sun.x)/SCALE, (mouse.y - sun.y)/SCALE };

            r = r_click;
            v = (Vector2){ (float)atof(vx_box.text), (float)atof(vy_box.text) };

            trail_idx = 0;
            trail_len = 0;
            collision = false;
            explosion_anim_counter = 0;
            placed = true;
        }

        if (placed && !collision) {
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
        }

        trail[trail_idx] = r;
        trail_idx = (trail_idx + 1) % MAX_TRAIL;
        trail_len += (trail_len < MAX_TRAIL);

        BeginDrawing();
        ClearBackground(BLACK);
        Vector2 planet = { sun.x + r.x*SCALE, sun.y + r.y*SCALE };

        // In case the planet gets a little too close to the sun...
        if (placed && !collision &&
            CheckCollisionPointCircle(planet, sun, 6*SCALE_FIGURES)) {
            collision = true;
            explosion_anim_counter = 0;
        }

        for (int i = 0; i < MAX_STARS; ++i)
            DrawPixelV(stars[i], Fade(WHITE, rand_float()));

        for (int i = 0; i < trail_len; ++i) {
            int idx = (trail_idx - 1 - i + MAX_TRAIL) % MAX_TRAIL;
            Vector2 dot = { sun.x + trail[idx].x*SCALE, sun.y + trail[idx].y*SCALE };
            DrawCircleV(dot, 2, Fade(BLUE, rand_float()));
        }

        DrawCircleV(GetMousePosition(), 6*SCALE_FIGURES, Fade(BLUE, 0.6f));

        DrawCircleV(sun, 6*SCALE_FIGURES, YELLOW);
        if (placed) {
            if (!collision)
                DrawCircleV(planet, 6*SCALE_FIGURES, BLUE);
            else {
                int total_frames = 7;
                int frame = explosion_anim_counter / 5;
                if (frame >= total_frames) frame = total_frames - 1;
                else {
                    ++explosion_anim_counter;
                    Rectangle src_rec = { (float)(frame * 71), 0.0f, 71.0f, 100.0f };
                    float dest_width = 71.0f * SCALE_FIGURES;
                    float dest_height = 100.0f * SCALE_FIGURES;
                    Rectangle dest_rec = { planet.x, planet.y, dest_width, dest_height };
                    Vector2 origin = { dest_width / 2.0f, dest_height / 2.0f };
                    DrawTexturePro(explosion, src_rec, dest_rec, origin, 0.0f, WHITE);
                }

                DrawText(EXPLOSION_TEXT, (GetScreenWidth()/2 - MeasureText(EXPLOSION_TEXT, 30)/2) + 150, 10, 30, WHITE);
            }
        }

        // Textboxes for initial velocity
        DrawRectangleRec(ui_panel, Fade(BLACK, 0.6f));
        textbox_draw(vx_box, "Vx (m/s)");
        textbox_draw(vy_box, "Vy (m/s)");

        EndDrawing();
    }

    CloseWindow();
}
