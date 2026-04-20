#ifndef PARTICLE_H
#define PARTICLE_H
#include <QPointF>
#include <QPainter>

class Particle {
public:
    Particle();
    void setInitialCondition(float px, float py, float vx, float vy);
    void update();
    void draw(QPainter &painter) const;
    bool isDead() const { return life <= 0; }

    QPointF pos, vel, frc;
    float damping, alpha, life;
    int r, g, b, size;
};
#endif
