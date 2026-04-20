#ifndef ENEMY_H
#define ENEMY_H

#include <QString>
#include <QPointF>
#include <QVector>
#include <QPainter>
#include <QImage>
#include <QFont>

class Enemy {
public:
    Enemy(const QString &path, float speedPlus);
    void update(int playerX);
    void draw(QPainter &painter);
    void generatePose();
    void updateBoss(int playerX);

    QPointF pos;
    float xSpeed, ySpeed;
    int r, g, b;
    int movementType, blockSize;
    QString path, name;
    QVector<QPointF> blocks, blocks2;
    int pose, poseCounter, hp, maxHp;

    bool isBoss;
    int bossType;
    bool dead, rush;
    float sinX;
    int xSize, ySize;

    int shootTimer;
    QVector<QPointF> bossBullets;
    bool bossActive;
    int bossPattern;
    float bossSinOffset;
};

#endif
