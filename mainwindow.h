#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QPainter>
#include <QVector>
#include <QPointF>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QImage>
#include <QDirIterator>
#include <QSet>
#include <QFontDatabase>
#include <QElapsedTimer>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <aclapi.h>
#include <securitybaseapi.h>
#include <winbase.h>

#include "enemy.h"
#include "particle.h"

#define GAME_STATE_WARNING_MSG 1
#define GAME_STATE_MODE_SELECT 2
#define GAME_STATE_TITLE 3
#define GAME_STATE_PLAYING 4

enum GameMode { MODE_CASUAL, MODE_EXTREME };
enum PowerType { POWER_TRIPLE_SHOT, POWER_SHIELD };

struct FadePoint { QPointF point; int time; QString text; };
struct Bullet {
    QPointF pos;
    QPointF vel;
    bool active;
    int lifeTime;
};
struct PowerUp {
    QPointF pos;
    PowerType type;
    bool active = true;
    float sinOffset;
    int lifeTime;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    int gameState, startTimerCount, score, shipFrame, bossWarningTimer;
    bool mvUp, mvDn, mvLf, mvRt, shoot, gameLost, LIVE;
    bool spacePressed;
    int shootCooldown;
    const int MAX_BULLETS;
    const int SHIELD_DURATION = 300;
    GameMode currentMode;
    QPointF player;
    QVector<Enemy*> enemies;
    QVector<Bullet> playerBullets;
    QVector<Particle> enemyParts;
    QVector<FadePoint> deadTypes;
    QVector<PowerUp> activePowerUps;

    QDirIterator *dirIterator;
    QString currentScanPath;
    QStringList driveList;
    int currentDriveIndex;
    QSet<QString> spawnedFiles;

    QImage shipImages[3], stars[2], powerUpImages[2], shieldAura, warningBossImg;
    float starPos[2];
    QSoundEffect laserSound, explosionSound, powerUpSound;

    PowerType currentPowerUp;
    int powerUpTimer;
    bool tripleShotActive, shieldActive;
    float shieldAlpha;
    float shieldRotation;
    int bossWarningFlash;
    bool bossSpawning;
    int lastBossScore;
    int bossSpawnCooldown;
    QElapsedTimer invincibilityTimer;
    bool invincible;
    int invincibilityFrames;

    void loadResources();
    void setupAudio();
    void updateGame();
    void handleEnemyDeath(Enemy *e);
    void deleteFile(const QString &path);
    void newEnemy();
    void spawnBoss();
    void shootBullet();
    void shootTripleBullet();
    QString getNextFilePath();
    void spawnPowerUp(const QPointF &pos);
    void checkPowerUpCollisions();
    void updatePowerUps();
    void drawShield(QPainter &painter);
    void cleanOldBullets();

    QMediaPlayer* music;
    QAudioOutput* audioOutput;
};

#endif
