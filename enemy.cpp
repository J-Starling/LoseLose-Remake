#include "enemy.h"
#include <QRandomGenerator>
#include <QtMath>
#include <QDir>
#include <QFileInfo>

extern QImage imgBossFrames[4];
extern QFont mainFont;

Enemy::Enemy(const QString &_path, float speedPlus)
    : pos(0,-40), blockSize(3), pose(0), poseCounter(30), isBoss(false), bossType(0),
    dead(false), rush(false), sinX(0), shootTimer(0), bossActive(false), bossPattern(0), bossSinOffset(0)
{
    path = _path;
    name = QFileInfo(path).fileName();
    quint32 seed = 0;
    for(auto c : name) seed += c.unicode();
    QRandomGenerator rng(seed ? seed : 123);

    r = rng.bounded(100, 255); g = rng.bounded(100, 255); b = rng.bounded(100, 255);
    xSize = 12; ySize = 12;
    hp = rng.bounded(1, 4);
    maxHp = hp;

    ySpeed = 1.0f + speedPlus;
    movementType = rng.bounded(1, 4);
    generatePose();
    pos = QPointF(QRandomGenerator::global()->bounded(50, 430), -40);
}

void Enemy::update(int playerX) {
    if (isBoss) {
        updateBoss(playerX);
        return;
    }

    sinX += 0.05f;
    if (movementType == 2) pos.rx() += qSin(sinX) * 2;
    else if (movementType == 3) {
        pos.rx() += (float)(playerX - pos.x()) / 60.0f;
        if (qAbs(pos.x() - playerX) < 15) rush = true;
    }
    pos.ry() += (rush ? ySpeed * 3.5f : ySpeed);

    if (--poseCounter < 0) {
        poseCounter = 25;
        pose = (pose == 0) ? 1 : 0;
    }
}

void Enemy::updateBoss(int playerX) {
    if (pos.y() < 100) {
        pos.ry() += ySpeed;
    } else {
        bossSinOffset += 0.02f;
        pos.rx() = 240 + qSin(bossSinOffset) * 150;

        if (--shootTimer <= 0) {
            shootTimer = 30;
            bossPattern = (bossPattern + 1) % 3;

            QPointF basePos(pos.x(), pos.y() + 40);

            if (bossPattern == 0) {
                QPointF bulletPos(basePos.x(), basePos.y());
                float dx = playerX - bulletPos.x();
                float dy = 650 - bulletPos.y();
                float len = qSqrt(dx*dx + dy*dy);
                if (len > 0) {
                    dx = dx / len * 5;
                    dy = dy / len * 5;
                }
                for (int i = 0; i < 3; i++) {
                    QPointF bullet = bulletPos;
                    bossBullets.append(bullet);
                }
            }
            else if (bossPattern == 1) {
                for (int i = -1; i <= 1; i++) {
                    QPointF bulletPos(basePos.x() + i * 30, basePos.y());
                    bossBullets.append(bulletPos);
                }
            }
            else {
                for (int i = 0; i < 8; i++) {
                    float angle = i * 3.14159f * 2 / 8;
                    QPointF bulletPos(basePos.x() + qSin(angle) * 30,
                                      basePos.y() + qCos(angle) * 30);
                    bossBullets.append(bulletPos);
                }
            }
        }
    }
}

void Enemy::draw(QPainter &painter) {
    if (isBoss) {
        painter.drawImage(pos.x() - 64, pos.y() - 64, imgBossFrames[bossType]);
        painter.setBrush(QColor(255, 0, 0, 150));
        painter.drawRect(pos.x() - 64, pos.y() - 80, 128 * ((float)hp/maxHp), 5);

        painter.setBrush(Qt::red);
        for (const QPointF &bullet : bossBullets) {
            painter.drawEllipse(bullet.x() - 3, bullet.y() - 3, 6, 6);
        }
    } else {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(r, g, b, pose == 0 ? 255 : 180));

        float offsetX = (xSize * blockSize) / 2.0f;
        float offsetY = (ySize * blockSize) / 2.0f;

        for (const QPointF &p : (pose == 0 ? blocks : blocks2)) {
            painter.drawRect(pos.x() + p.x() - offsetX,
                             pos.y() + p.y() - offsetY,
                             blockSize, blockSize);
        }
    }

    painter.setFont(mainFont);
    painter.setPen(Qt::black);
    painter.drawText(pos.x() - 18, pos.y() + (isBoss ? 87 : 22), name);
    painter.setPen(isBoss ? Qt::red : Qt::white);
    painter.drawText(pos.x() - 20, pos.y() + (isBoss ? 85 : 20), name);
}

void Enemy::generatePose() {
    blocks.clear(); blocks2.clear();
    QRandomGenerator rng(r + g + b);

    int halfX = xSize / 2;

    for (int i = 0; i < halfX; i++) {
        for (int j = 0; j < ySize; j++) {
            if (rng.bounded(100) < 40) {
                QPointF p1(i * blockSize, j * blockSize);
                QPointF p2((xSize - 1 - i) * blockSize, j * blockSize);

                blocks.append(p1);
                blocks.append(p2);

                int offset = rng.bounded(2) ? blockSize : -blockSize;
                QPointF p1_anim(i * blockSize + offset, j * blockSize);
                QPointF p2_anim((xSize - 1 - i) * blockSize - offset, j * blockSize);

                blocks2.append(p1_anim);
                blocks2.append(p2_anim);
            }
        }
    }
}
