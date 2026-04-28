#include "mainwindow.h"
#include <QRandomGenerator>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QFontDatabase>
#include <QTransform>
#include <math.h>

QImage imgBossFrames[4];
QFont mainFont;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , MAX_BULLETS(30)
{
    setFixedSize(480, 720);
    gameState = GAME_STATE_WARNING_MSG;
    startTimerCount = 150;
    score = 0;
    shipFrame = 0;
    player = QPointF(240, 650);
    LIVE = false;
    gameLost = false;
    currentMode = MODE_CASUAL;

    starPos[0] = 0;
    starPos[1] = 0;
    mvUp = mvDn = mvLf = mvRt = shoot = false;
    spacePressed = false;
    shootCooldown = 0;

    tripleShotActive = false;
    shieldActive = false;
    powerUpTimer = 0;
    shieldAlpha = 255;
    shieldRotation = 0;
    bossWarningFlash = 0;
    bossSpawning = false;
    invincible = false;
    invincibilityFrames = 0;

    lastBossScore = 0;
    bossSpawnCooldown = 0;

    QFileInfoList drives = QDir::drives();
    for (const QFileInfo &drive : drives) driveList.append(drive.absoluteFilePath());
    currentDriveIndex = 0;
    if (!driveList.isEmpty()) {
        currentScanPath = driveList.at(currentDriveIndex);
        dirIterator = new QDirIterator(currentScanPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    }

    loadResources();
    setupAudio();
    startTimer(16);
}

MainWindow::~MainWindow() {
    if (dirIterator) delete dirIterator;
    qDeleteAll(enemies);
    enemies.clear();
}

void MainWindow::loadResources() {
    shipImages[0].load(":/images/ship1.png");
    shipImages[1].load(":/images/ship2.png");
    shipImages[2].load(":/images/ship3.png");
    imgBossFrames[0].load(":/images/boss1.png");
    imgBossFrames[1].load(":/images/boss2.png");
    imgBossFrames[2].load(":/images/boss3.png");
    imgBossFrames[3].load(":/images/boss4.png");
    stars[0].load(":/images/stars1.png");
    stars[1].load(":/images/stars2.png");

    if (powerUpImages[POWER_TRIPLE_SHOT].isNull()) {
        powerUpImages[POWER_TRIPLE_SHOT].load(":/images/powerup_triple.png");
    }
    if (powerUpImages[POWER_SHIELD].isNull()) {
        powerUpImages[POWER_SHIELD].load(":/images/powerup_shield.png");
    }
    if (shieldAura.isNull()) {
        shieldAura.load(":/images/shield_aura.png");
    }
    if (warningBossImg.isNull()) {
        warningBossImg.load(":/images/warning_boss.png");
    }

    int fontId = QFontDatabase::addApplicationFont(":/fonts/PixelSplitter-Bold.ttf");
    if (fontId != -1) {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        mainFont = QFont(family, 10);
    } else {
        mainFont = QFont("Courier", 12, QFont::Bold);
    }
}

void MainWindow::setupAudio() {
    music = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    music->setAudioOutput(audioOutput);
    music->setSource(QUrl("qrc:/sounds/m3d049_bopSrc_03_hwyChipmusik_by_xik_vbr.mp3"));
    music->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(0.5f);
    music->play();

    laserSound.setSource(QUrl("qrc:/sounds/laser3.wav"));
    laserSound.setVolume(0.3f);

    explosionSound.setSource(QUrl("qrc:/sounds/explosion.wav"));
    explosionSound.setVolume(0.5f);

    powerUpSound.setSource(QUrl("qrc:/sounds/powerup.wav"));
    powerUpSound.setVolume(0.4f);
}

void MainWindow::newEnemy() {
    if (bossSpawning || gameLost) return;

    QString p = getNextFilePath();
    if (p.isEmpty()) return;

    Enemy* enemy = new Enemy(p, (float)score / 1000.0f);
    if (enemy) {
        enemies.append(enemy);
    }
}

QString MainWindow::getNextFilePath() {
    if (dirIterator && dirIterator->hasNext()) return dirIterator->next();
    return "";
}

void MainWindow::timerEvent(QTimerEvent *) {
    starPos[0] += 0.5f;
    starPos[1] += 1.5f;

    if (starPos[0] >= 720) starPos[0] -= 720;
    if (starPos[1] >= 720) starPos[1] -= 720;

    if (gameState == GAME_STATE_PLAYING) updateGame();
    else if (gameState == GAME_STATE_WARNING_MSG && --startTimerCount <= 0) gameState = GAME_STATE_MODE_SELECT;

    update();
}

void MainWindow::updateGame() {
    if (shootCooldown > 0) shootCooldown--;

    if (bossSpawnCooldown > 0) bossSpawnCooldown--;

    for (int i = enemyParts.size() - 1; i >= 0; --i) {
        if (i < enemyParts.size()) {
            enemyParts[i].update();
            if (enemyParts[i].isDead()) {
                enemyParts.removeAt(i);
            }
        }
    }

    if (gameLost) return;

    float speed = 5.0f;
    if (mvUp && player.y() > 20) player.ry() -= speed;
    if (mvDn && player.y() < 700) player.ry() += speed;
    if (mvLf && player.x() > 20) {
        player.rx() -= speed;
        shipFrame = 1;
    } else if (mvRt && player.x() < 460) {
        player.rx() += speed;
        shipFrame = 2;
    } else {
        shipFrame = 0;
    }

    if (shoot && !spacePressed) {
        shootBullet();
        spacePressed = true;
    } else if (!shoot) {
        spacePressed = false;
    }

    for (int i = 0; i < playerBullets.size(); ++i) {
        playerBullets[i].pos += playerBullets[i].vel;
    }
    cleanOldBullets();

    for (int i = enemies.size() - 1; i >= 0; --i) {
        if (i >= enemies.size()) continue;

        Enemy* enemy = enemies[i];
        if (!enemy) continue;

        enemy->update(player.x());

        if (!invincible && enemy) {
            QRectF enemyRect;
            if (enemy->isBoss) {
                enemyRect = QRectF(enemy->pos.x() - 64, enemy->pos.y() - 64, 128, 128);
            } else {
                enemyRect = QRectF(enemy->pos.x() - 20, enemy->pos.y() - 20, 40, 40);
            }
            QRectF playerRect(player.x(), player.y(), 32, 32);

            if (enemyRect.intersects(playerRect)) {
                if (shieldActive) {
                    shieldActive = false;
                    invincible = true;
                    invincibilityFrames = 60;

                    if (enemy) {
                        enemy->pos.ry() += 50;
                        if (!enemy->isBoss) {
                            enemy->hp--;
                        }
                    }
                } else {
                    gameLost = true;
                    explosionSound.play();

                    for (int p = 0; p < 40; p++) {
                        Particle part;
                        part.setInitialCondition(player.x() + 16, player.y() + 16,
                                                 (QRandomGenerator::global()->generateDouble() - 0.5) * 12,
                                                 (QRandomGenerator::global()->generateDouble() - 0.5) * 12);
                        part.r = 255; part.g = 100; part.b = 0;
                        part.size = 3;
                        enemyParts.append(part);
                    }
                    return;
                }
            }
        }

        if (enemy) {
            for (int j = playerBullets.size() - 1; j >= 0; --j) {
                if (j >= playerBullets.size()) continue;

                QRectF enemyRect;
                if (enemy->isBoss) {
                    enemyRect = QRectF(enemy->pos.x() - 64, enemy->pos.y() - 64, 128, 128);
                } else {
                    enemyRect = QRectF(enemy->pos.x() - 20, enemy->pos.y() - 20, 40, 40);
                }

                if (enemyRect.contains(playerBullets[j].pos)) {
                    enemy->hp--;
                    playerBullets.removeAt(j);

                    if (enemy->hp <= 0) {
                        handleEnemyDeath(enemy);
                        enemies.removeAt(i);
                        break;
                    }
                }
            }
        }

        if (enemy && enemy->isBoss) {
            for (int j = enemy->bossBullets.size() - 1; j >= 0; --j) {
                if (j >= enemy->bossBullets.size()) continue;

                QPointF bulletPos = enemy->bossBullets[j];
                bulletPos.ry() += 4;
                enemy->bossBullets[j] = bulletPos;

                if (bulletPos.y() > 720 || bulletPos.y() < -50 ||
                    bulletPos.x() < -50 || bulletPos.x() > 530) {
                    enemy->bossBullets.removeAt(j);
                    continue;
                }

                QRectF bulletRect(bulletPos.x() - 8, bulletPos.y() - 8, 16, 16);
                QRectF playerRect(player.x(), player.y(), 32, 32);

                if (bulletRect.intersects(playerRect)) {
                    if (!invincible) {
                        if (shieldActive) {
                            shieldActive = false;
                            invincible = true;
                            invincibilityFrames = 60;
                        } else {
                            gameLost = true;
                            explosionSound.play();

                            for (int p = 0; p < 40; p++) {
                                Particle part;
                                part.setInitialCondition(player.x() + 16, player.y() + 16,
                                                         (QRandomGenerator::global()->generateDouble() - 0.5) * 12,
                                                         (QRandomGenerator::global()->generateDouble() - 0.5) * 12);
                                part.r = 255; part.g = 100; part.b = 0;
                                part.size = 3;
                                enemyParts.append(part);
                            }
                            return;
                        }
                    }
                    enemy->bossBullets.removeAt(j);
                }
            }
        }
    }

    updatePowerUps();
    checkPowerUpCollisions();

    if (!bossSpawning && !gameLost && enemies.size() < 10) {
        if (QRandomGenerator::global()->bounded(100) < 2) {
            newEnemy();
        }
    }

    bool hasBoss = false;
    for (Enemy* e : enemies) {
        if (e && e->isBoss) {
            hasBoss = true;
            break;
        }
    }

    if (!hasBoss && !bossSpawning && !gameLost && bossSpawnCooldown <= 0) {
        int currentBossLevel = score / 250;
        if (currentBossLevel > lastBossScore) {
            bossSpawning = true;
            bossWarningFlash = 180;
            lastBossScore = currentBossLevel;
        }
    }

    if (bossSpawning && !gameLost) {
        bossWarningFlash--;

        if (bossWarningFlash <= 0) {
            spawnBoss();
            bossSpawning = false;
            bossSpawnCooldown = 1800;
        }
    }
}

void MainWindow::handleEnemyDeath(Enemy *e) {
    if (!e) return;

    explosionSound.play();
    score += e->isBoss ? 100 : 10;

    if (!e->isBoss) {
        spawnPowerUp(e->pos);
    } else {
        if (QRandomGenerator::global()->bounded(100) < 70) {
            PowerUp p;
            p.pos = e->pos;
            p.type = static_cast<PowerType>(QRandomGenerator::global()->bounded(2));
            p.sinOffset = 0;
            p.lifeTime = 600;
            activePowerUps.append(p);
        }
    }

    deleteFile(e->path);

    for (int i = 0; i < 15; i++) {
        Particle p;
        p.setInitialCondition(e->pos.x(), e->pos.y(),
                              (QRandomGenerator::global()->generateDouble()-0.5)*8,
                              (QRandomGenerator::global()->generateDouble()-0.5)*8);
        p.r = e->r; p.g = e->g; p.b = e->b;
        enemyParts.append(p);
    }
}

void MainWindow::deleteFile(const QString &path) {
    if (!LIVE) return;

    QString nativePath = QDir::toNativeSeparators(path);
    std::wstring widePath = nativePath.toStdWString();

    qDebug() << "Attempting to delete:" << nativePath;

    // Step 1: Remove all attributes
    SetFileAttributesW(widePath.c_str(), FILE_ATTRIBUTE_NORMAL);

    // Step 2: Try direct delete
    if (DeleteFileW(widePath.c_str())) {
        qDebug() << "Deleted successfully";
        return;
    }

    DWORD lastError = GetLastError();
    qDebug() << "Direct delete failed, error:" << lastError;

    // Step 3: Try to take ownership and set permissions
    HANDLE hToken = NULL;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);

    if (hToken) {
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;

        // Enable take ownership privilege
        LookupPrivilegeValueW(NULL, SE_TAKE_OWNERSHIP_NAME, &tp.Privileges[0].Luid);
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);

        // Enable restore privilege
        LookupPrivilegeValueW(NULL, SE_RESTORE_NAME, &tp.Privileges[0].Luid);
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);

        CloseHandle(hToken);
    }

    // Step 4: Open file with maximum rights
    HANDLE hFile = CreateFileW(
        widePath.c_str(),
        DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        NULL
        );

    if (hFile != INVALID_HANDLE_VALUE) {
        // Try to set Administrators as owner
        PSID pAdminSid = NULL;
        SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;

        if (AllocateAndInitializeSid(&SIDAuth, 2,
                                     SECURITY_BUILTIN_DOMAIN_RID,
                                     DOMAIN_ALIAS_RID_ADMINS,
                                     0, 0, 0, 0, 0, 0, &pAdminSid)) {

            SetSecurityInfo(hFile, SE_FILE_OBJECT,
                            OWNER_SECURITY_INFORMATION,
                            pAdminSid, NULL, NULL, NULL);

            // Grant full access to Everyone
            EXPLICIT_ACCESSW ea;
            ZeroMemory(&ea, sizeof(EXPLICIT_ACCESSW));

            WCHAR everyone[] = L"EVERYONE";
            ea.grfAccessPermissions = GENERIC_ALL;
            ea.grfAccessMode = GRANT_ACCESS;
            ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
            ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
            ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
            ea.Trustee.ptstrName = everyone;

            PACL pNewDACL = NULL;
            if (SetEntriesInAclW(1, &ea, NULL, &pNewDACL) == ERROR_SUCCESS) {
                SetSecurityInfo(hFile, SE_FILE_OBJECT,
                                DACL_SECURITY_INFORMATION,
                                NULL, NULL, pNewDACL, NULL);
                LocalFree(pNewDACL);
            }

            FreeSid(pAdminSid);
        }

        // Mark for deletion
        FILE_DISPOSITION_INFO fdi;
        fdi.DeleteFile = TRUE;

        if (SetFileInformationByHandle(hFile, FileDispositionInfo, &fdi, sizeof(fdi))) {
            CloseHandle(hFile);

            // Check if deleted
            if (GetFileAttributesW(widePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                qDebug() << "Deleted successfully after taking ownership";
                return;
            }
        } else {
            CloseHandle(hFile);
        }
    }

    // Step 5: Try move to temp and delete
    wchar_t tempPath[MAX_PATH];
    if (GetTempPathW(MAX_PATH, tempPath)) {
        std::wstring tempFile = std::wstring(tempPath) + L"lostemp_" +
                                widePath.substr(widePath.find_last_of(L"\\") + 1);

        if (MoveFileExW(widePath.c_str(), tempFile.c_str(),
                        MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING |
                            MOVEFILE_COPY_ALLOWED)) {

            SetFileAttributesW(tempFile.c_str(), FILE_ATTRIBUTE_NORMAL);
            if (DeleteFileW(tempFile.c_str())) {
                qDebug() << "Deleted successfully via temp move";
                return;
            }
        }
    }

    // Step 6: Schedule for reboot
    std::wstring deleteName = widePath + L".delete";
    if (MoveFileExW(widePath.c_str(), deleteName.c_str(),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        if (MoveFileExW(deleteName.c_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT)) {
            qDebug() << "Scheduled for deletion on reboot";
            return;
        }
    }

    qDebug() << "FAILED to delete file:" << nativePath;
}

void MainWindow::cleanOldBullets() {
    for (int i = playerBullets.size() - 1; i >= 0; --i) {
        if (i >= playerBullets.size()) continue;

        Bullet &b = playerBullets[i];
        b.lifeTime++;

        if (b.pos.y() < -50 ||
            b.pos.y() > 770 ||
            b.pos.x() < -50 ||
            b.pos.x() > 530 ||
            b.lifeTime > 120) {
            playerBullets.removeAt(i);
        }
    }

    while (playerBullets.size() > MAX_BULLETS) {
        if (!playerBullets.isEmpty()) {
            playerBullets.removeFirst();
        }
    }
}

void MainWindow::shootBullet() {
    if (gameLost) return;
    if (shootCooldown > 0) return;
    if (playerBullets.size() >= MAX_BULLETS) return;

    if (tripleShotActive) {
        shootTripleBullet();
    } else {
        Bullet b;
        b.pos = QPointF(player.x() + 14, player.y());
        b.vel = QPointF(0, -10);
        b.active = true;
        b.lifeTime = 0;
        playerBullets.append(b);
    }

    if (!laserSound.isPlaying()) {
        laserSound.play();
    }

    shootCooldown = tripleShotActive ? 20 : 25;
}

void MainWindow::shootTripleBullet() {
    if (gameLost) return;

    int currentSize = playerBullets.size();
    int maxToAdd = MAX_BULLETS - currentSize;

    if (maxToAdd <= 0) return;

    QVector<Bullet> newBullets;

    if (maxToAdd >= 1) {
        Bullet b1;
        b1.pos = QPointF(player.x() + 14, player.y());
        b1.vel = QPointF(0, -10);
        b1.active = true;
        b1.lifeTime = 0;
        newBullets.append(b1);
    }

    if (maxToAdd >= 2) {
        Bullet b2;
        b2.pos = QPointF(player.x() + 4, player.y());
        b2.vel = QPointF(-2, -10);
        b2.active = true;
        b2.lifeTime = 0;
        newBullets.append(b2);
    }

    if (maxToAdd >= 3) {
        Bullet b3;
        b3.pos = QPointF(player.x() + 24, player.y());
        b3.vel = QPointF(2, -10);
        b3.active = true;
        b3.lifeTime = 0;
        newBullets.append(b3);
    }

    playerBullets.append(newBullets);
}

void MainWindow::spawnPowerUp(const QPointF &pos) {
    if (QRandomGenerator::global()->bounded(100) < 20) {
        PowerUp p;
        p.pos = pos;
        p.type = static_cast<PowerType>(QRandomGenerator::global()->bounded(2));
        p.sinOffset = QRandomGenerator::global()->generateDouble() * 6.28;
        p.lifeTime = 600;
        activePowerUps.append(p);
    }
}

void MainWindow::checkPowerUpCollisions() {
    if (gameLost) return;

    QRectF playerRect(player.x(), player.y(), 32, 32);

    for (int i = activePowerUps.size() - 1; i >= 0; i--) {
        if (i >= activePowerUps.size()) continue;

        PowerUp &p = activePowerUps[i];
        QRectF powerUpRect(p.pos.x() - 16, p.pos.y() - 16, 32, 32);

        if (playerRect.intersects(powerUpRect)) {
            if (!powerUpSound.isPlaying()) {
                powerUpSound.play();
            }

            if (p.type == POWER_TRIPLE_SHOT) {
                tripleShotActive = true;
                powerUpTimer = 300;
            } else if (p.type == POWER_SHIELD) {
                shieldActive = true;
                powerUpTimer = SHIELD_DURATION;
                invincible = true;
                invincibilityFrames = SHIELD_DURATION;
            }

            activePowerUps.removeAt(i);
        }
    }
}

void MainWindow::updatePowerUps() {
    if (powerUpTimer > 0) {
        powerUpTimer--;
        if (powerUpTimer <= 0) {
            tripleShotActive = false;
            shieldActive = false;
            invincible = false;
        }
    }

    if (invincibilityFrames > 0) {
        invincibilityFrames--;
    }

    if (shieldActive) {
        shieldRotation += 3;
        if (shieldRotation >= 360) shieldRotation = 0;
    }

    for (int i = activePowerUps.size() - 1; i >= 0; i--) {
        if (i >= activePowerUps.size()) continue;

        PowerUp &p = activePowerUps[i];
        p.sinOffset += 0.1f;
        p.pos.ry() += 2;
        p.lifeTime--;

        p.pos.rx() += qSin(p.sinOffset) * 0.5f;

        if (p.pos.y() > 720 || p.lifeTime <= 0) {
            activePowerUps.removeAt(i);
        }
    }
}

void MainWindow::drawShield(QPainter &painter) {
    if (shieldActive && !shieldAura.isNull()) {
        if (powerUpTimer < 120) {
            shieldAlpha = 128 + qSin(powerUpTimer * 0.3f) * 127;
        } else {
            shieldAlpha = 200;
        }

        painter.setOpacity(shieldAlpha / 255.0f);
        painter.save();
        painter.translate(player.x() + 16, player.y() + 16);
        painter.rotate(shieldRotation);
        painter.drawImage(-32, -32, shieldAura);
        painter.restore();
        painter.setOpacity(1.0f);

        painter.setFont(mainFont);
        painter.setPen(Qt::cyan);
        int secondsLeft = (powerUpTimer / 60) + 1;
        painter.drawText(player.x() - 30, player.y() - 30,
                         QString("SHIELD: %1s").arg(secondsLeft));
    }
}

void MainWindow::spawnBoss() {
    if (gameLost) return;

    QString bossPath = currentScanPath;
    if (bossPath.isEmpty()) {
        bossPath = "C:\\";
    }

    Enemy* boss = new Enemy(bossPath, score / 500.0f);
    if (boss) {
        boss->isBoss = true;
        boss->bossType = QRandomGenerator::global()->bounded(4);
        boss->maxHp = 25 + (score / 100);
        boss->hp = boss->maxHp;
        boss->pos = QPointF(width() / 2.0f, -100.0f);
        boss->shootTimer = 30;
        enemies.append(boss);
        bossWarningTimer = 180;
    }
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);

    if (!stars[0].isNull()) {
        p.drawImage(0, starPos[0], stars[0]);
        p.drawImage(0, starPos[0] - 720, stars[0]);
    }
    if (!stars[1].isNull()) {
        p.drawImage(0, starPos[1], stars[1]);
        p.drawImage(0, starPos[1] - 720, stars[1]);
    }

    if (bossSpawning && !warningBossImg.isNull()) {
        if (bossWarningFlash % 18 < 9) {
            p.setOpacity(0.9f);
            int x = (width() - warningBossImg.width()) / 2;
            int y = (height() - warningBossImg.height()) / 2;
            p.drawImage(x, y, warningBossImg);
            p.setOpacity(1.0f);

            p.setFont(mainFont);
            p.setPen(Qt::white);
            int secondsLeft = (bossWarningFlash / 60) + 1;
            QString warningText = QString("BOSS IN %1").arg(secondsLeft);

            QRect textRect(x, y + warningBossImg.height() + 10, warningBossImg.width(), 30);
            p.setPen(Qt::black);
            p.drawText(textRect.translated(2, 2), Qt::AlignCenter, warningText);
            p.setPen(Qt::white);
            p.drawText(textRect, Qt::AlignCenter, warningText);
        }
    }

    auto drawTextWithShadow = [&](const QRect& r, int flags, const QString& text, const QColor& color, int shadowSize) {
        p.setPen(Qt::black);
        p.drawText(r.translated(shadowSize, shadowSize), flags, text);
        p.setPen(color);
        p.drawText(r, flags, text);
    };

    if (gameState == GAME_STATE_WARNING_MSG) {
        QFont big = mainFont; big.setPointSize(24); p.setFont(big);
        drawTextWithShadow(rect(), Qt::AlignCenter, "WARNING:\nGAME MAY DELETE FILES", Qt::red, 3);
    } else if (gameState == GAME_STATE_MODE_SELECT) {
        QFont mid = mainFont; mid.setPointSize(14); p.setFont(mid);
        drawTextWithShadow(rect(), Qt::AlignCenter, "PRESS 1 FOR CASUAL MODE\n\nPRESS 2 FOR EXTREME MODE", Qt::cyan, 2);
    } else if (gameState == GAME_STATE_PLAYING) {
        for (Enemy *e : enemies) {
            if (e) e->draw(p);
        }

        p.setPen(Qt::yellow);
        p.setBrush(Qt::yellow);
        for (const auto &b : playerBullets) {
            p.drawRect(b.pos.x(), b.pos.y(), 2, 8);
        }

        drawShield(p);

        if (!gameLost && (!invincible || invincibilityFrames % 6 < 3)) {
            if (!shipImages[shipFrame].isNull()) {
                p.drawImage(player, shipImages[shipFrame]);
            }
        }

        for (const auto &pu : activePowerUps) {
            if (!powerUpImages[pu.type].isNull()) {
                p.setOpacity(0.8f + qSin(pu.sinOffset * 2) * 0.2f);
                p.drawImage(pu.pos.x() - 16, pu.pos.y() - 16, powerUpImages[pu.type]);

                p.setOpacity(1.0f);
                p.setFont(mainFont);
                p.setPen(Qt::white);
                QString powerText = (pu.type == POWER_TRIPLE_SHOT) ? "TRIPLE" : "SHIELD";
                p.drawText(pu.pos.x() - 20, pu.pos.y() - 20, powerText);
            }
        }
        p.setOpacity(1.0f);

        if (gameLost) {
            QFont big = mainFont; big.setPointSize(36); p.setFont(big);
            drawTextWithShadow(rect(), Qt::AlignCenter, "GAME OVER", Qt::red, 4);
        }

        for (const auto &part : enemyParts) {
            part.draw(p);
        }

        QFont uiFont = mainFont; uiFont.setPointSize(12); p.setFont(uiFont);
        drawTextWithShadow(QRect(10, 10, 200, 30), Qt::AlignLeft, "SCORE: " + QString::number(score), Qt::white, 2);

        if (tripleShotActive) {
            drawTextWithShadow(QRect(10, 40, 200, 30), Qt::AlignLeft,
                               QString("TRIPLE: %1").arg(powerUpTimer/60 + 1), Qt::cyan, 2);
        }
        if (shieldActive) {
            drawTextWithShadow(QRect(10, 70, 200, 30), Qt::AlignLeft, "SHIELD", Qt::green, 2);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    if (gameState == GAME_STATE_MODE_SELECT) {
        if (event->key() == Qt::Key_1) {
            LIVE = false;
            gameState = GAME_STATE_PLAYING;
            score = 0;
            lastBossScore = 0;
            bossSpawnCooldown = 0;
        }
        if (event->key() == Qt::Key_2) {
            LIVE = true;
            gameState = GAME_STATE_PLAYING;
            score = 0;
            lastBossScore = 0;
            bossSpawnCooldown = 0;
        }
    }

    if (event->key() == Qt::Key_Left) mvLf = true;
    if (event->key() == Qt::Key_Right) mvRt = true;
    if (event->key() == Qt::Key_Up) mvUp = true;
    if (event->key() == Qt::Key_Down) mvDn = true;
    if (event->key() == Qt::Key_Space && !gameLost) shoot = true;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_Left) mvLf = false;
    if (event->key() == Qt::Key_Right) mvRt = false;
    if (event->key() == Qt::Key_Up) mvUp = false;
    if (event->key() == Qt::Key_Down) mvDn = false;
    if (event->key() == Qt::Key_Space) shoot = false;
}
