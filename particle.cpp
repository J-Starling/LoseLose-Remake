#include "particle.h"

Particle::Particle() : damping(0.95f), alpha(255), life(1.0f), r(255), g(255), b(255), size(2) {}

void Particle::setInitialCondition(float px, float py, float vx, float vy) {
    pos = QPointF(px, py); vel = QPointF(vx, vy); frc = QPointF(0, 0);
}

void Particle::update() {
    vel += frc; pos += vel; vel *= damping;
    life -= 0.02f; alpha = qMax(0.0f, life * 255.0f);
}

void Particle::draw(QPainter &painter) const {
    painter.fillRect(pos.x(), pos.y(), size, size, QColor(r, g, b, (int)alpha));
}
