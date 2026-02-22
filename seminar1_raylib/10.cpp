#include "raylib.h"
#include "raymath.h"
#include <vector>

struct Particle {
    Vector2 pos;
    Vector2 vel;
    float mass;
    float charge; 
};

int main() {
    const int sw = 800;
    const int sh = 600;
    InitWindow(sw, sh, "N-Bodies Simulation");

    std::vector<Particle> particles;
    const float dt_limit = 15.0f;
    
    particles.push_back({
        {(float)GetRandomValue(100, sw-100), (float)GetRandomValue(100, sh-100)}, 
        {0, 0}, 
        (float)GetRandomValue(300, 600),
        (float)GetRandomValue(500, 1000) 
    });

    particles.push_back({
        {(float)GetRandomValue(100, sw-100), (float)GetRandomValue(100, sh-100)}, 
        {0, 0}, 
        (float)GetRandomValue(50, 150), 
        (float)GetRandomValue(-1000, -500)
    });

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            particles.push_back({GetMousePosition(), {0, 0}, (float)GetRandomValue(20, 80), (float)GetRandomValue(-600, -300)});
        }
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            particles.push_back({GetMousePosition(), {0, 0}, (float)GetRandomValue(400, 800), (float)GetRandomValue(600, 1200)});
        }

        for (int i = 0; i < particles.size(); i++) {
            Vector2 totalForce = {0, 0};
            for (int j = 0; j < particles.size(); j++) {
                if (i == j) continue;

                Vector2 diff = Vector2Subtract(particles[j].pos, particles[i].pos);
                float distance = Vector2Length(diff);

                if (distance > dt_limit) {
                    float forceMag = (particles[i].charge * particles[j].charge) / distance;
                    Vector2 forceDir = Vector2Scale(Vector2Normalize(diff), -forceMag);
                    totalForce = Vector2Add(totalForce, forceDir);
                }
            }
            Vector2 acc = Vector2Scale(totalForce, 1.0f / particles[i].mass);
            particles[i].vel = Vector2Add(particles[i].vel, Vector2Scale(acc, GetFrameTime() * 100.0f));
        }

        for (auto &p : particles) {
            p.pos = Vector2Add(p.pos, Vector2Scale(p.vel, GetFrameTime()));

            float radius = 5.0f + (p.mass / 50.0f);

            if ((p.pos.x - radius <= 0) || (p.pos.x + radius >= sw)) p.vel.x *= -1;
            if ((p.pos.y - radius <= 0) || (p.pos.y + radius >= sh)) p.vel.y *= -1;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            for (const auto &p : particles) {
                Color c = (p.charge > 0) ? RED : BLUE;
                float radius = 5.0f + (p.mass / 50.0f); // Та же формула для отрисовки
                DrawCircleV(p.pos, radius, c);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}