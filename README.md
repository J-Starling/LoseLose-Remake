# Lose/Lose Remake

A modern Qt remake of Zach Gage's 2009 art game "Lose/Lose" — a space shooter where every enemy you destroy corresponds to a real file on your computer being permanently deleted.

## ⚠️ **EXTREME WARNING**

**THIS GAME DELETES REAL FILES FROM YOUR COMPUTER.**

In **EXTREME MODE**, when you destroy an enemy spaceship, the actual file that enemy represents will be **PERMANENTLY DELETED**. This is not a simulation — the game will attempt to:

- Remove file attributes
- Take ownership of files
- Modify file permissions
- Delete files through Windows API
- Schedule deletion on system reboot if immediate deletion fails

**DO NOT PLAY IN EXTREME MODE** unless:
- You are using a virtual machine
- You have complete backups of all data
- You fully understand that you will lose real files
- You accept all responsibility for data loss

**CASUAL MODE** is completely safe — no files are deleted.

---

## ✨ What's New in This Remake

This remake stays true to the original concept while modernizing the experience:

| Feature | Original (2009) | This Remake |
|---------|-----------------|-------------|
| **Engine** | OpenFrameworks (C++) | Qt 6 (C++17) |
| **Platform** | macOS only | Windows (cross-platform ready) |
| **Graphics** | Basic 2D primitives | Sprite-based with particle effects |
| **Enemy Variety** | Single enemy type | Multiple movement patterns, procedural colors |
| **Boss Battles** | None | Boss every 250 points with 3 attack patterns |
| **Power-ups** | None | Triple Shot & Shield |
| **Parallax Background** | Static | Dual-layer scrolling stars |
| **Visual Effects** | None | Explosion particles, shield rotation, flashing warnings |
| **File Deletion** | Direct delete | Multi-stage escalation with privilege elevation |
| **Sound** | Basic effects | Background music + sound effects with volume control |
| **Mode Selection** | Single lethal mode | Casual (safe) + Extreme (lethal) modes |
| **Screen Resolution** | 800×600 | 480×720 (vertical arcade style) |

### Specific Improvements
- **Invincibility frames** with visual blinking after taking damage
- **Shield power-up** absorbs one hit and grants temporary invincibility
- **Triple shot power-up** with spread pattern
- **Boss warning system** with countdown timer
- **Particle explosion system** on enemy death and player destruction
- **Smooth 60 FPS** gameplay with `QTimer`-based game loop
- **Proper audio mixing** using Qt Multimedia module
- **Procedural enemy sprites** generated from block patterns
- **Animated enemy poses** with alternating frames

---

## About The Original

"Lose/Lose" was created by Zach Gage in 2009 as a video game art piece exploring the relationship between virtual actions and real-world consequences. The game challenges players to consider what their in-game choices actually mean when translated to real effects.

- **Original release:** 2007
- **Original creator:** [Zach Gage](https://twitter.com/helvetica)
- **Genre:** Art game / Shoot 'em up / Digital performance art

---

## Features

### Game Modes
- **Casual Mode (Press 1):** Safe gameplay, no files are deleted
- **Extreme Mode (Press 2):** Real files are permanently deleted when enemies are destroyed

### Gameplay
- Classic top-down space shooter mechanics
- Procedurally generated enemies from files found on your system
- Enemy appearance and behavior derived from file attributes
- Boss enemies appear every 250 points
- Power-up system (Triple Shot and Shield)
- Particle effects and explosions
- Original chiptune soundtrack

### Technical Features
- Cross-platform Qt 6 framework
- Windows file system integration
- Real-time file scanning using `QDirIterator`
- Advanced file deletion with privilege escalation on Windows
- Smooth 60 FPS gameplay with hardware acceleration

---

## How It Works

1. The game scans your drives and iterates through real files
2. Each enemy spawned corresponds to an actual file on your system
3. Enemy properties (color, HP, movement pattern) are generated from the file's name and attributes
4. The enemy's name displayed in-game is the actual filename
5. In Extreme Mode, destroying an enemy triggers the deletion of its associated file

### Enemy Generation
- **Color:** Derived from filename hash
- **Health:** 1-4 HP based on filename
- **Movement:** Randomized pattern per enemy
- **Speed:** Increases with your score
- **Name:** The actual file name (displayed above enemy)

### Boss Mechanics
- Spawns every 250 points
- Increased health (25 + score/100)
- Three attack patterns:
  - Aimed shots at player
  - Spread shot pattern
  - Circular bullet pattern
- Uses boss sprite assets

---

## Controls

| Key | Action |
|-----|--------|
| Arrow Keys | Move ship |
| Space | Shoot |
| 1 | Select Casual Mode |
| 2 | Select Extreme Mode |

---

## Prerequisites

- Qt 6.x (Core, GUI, Widgets, Multimedia)
- C++17 compatible compiler
- Windows SDK (for file operations)

---

## File Deletion Process (Extreme Mode)

- Remove file attributes (SetFileAttributesW)
- Attempt direct deletion (DeleteFileW)
- Escalate privileges and take ownership if needed
- Modify ACL permissions
- Mark for deletion on handle close
- Move to temp directory and delete
- Schedule deletion on reboot as last resort

---

## Known Issues

- File deletion may fail on system-protected files or files in use
- Some antivirus software may flag the game's behavior
- File scanning can be slow on large drives
- Unicode filenames may not display correctly
- Windows-only file deletion (Linux/macOS builds will need adaptation)

---

## Legal & Ethical Disclaimer

- This software is provided for educational and artistic purposes only. The author:
- Takes NO RESPONSIBILITY for any data loss or system damage
- Does NOT endorse using this software to harm systems
- Recommends ONLY playing in Casual Mode
- Advises running in a VIRTUAL MACHINE for Extreme Mode

By downloading, compiling, or running this software, you acknowledge that:
1. You understand the risks involved
2. You accept full responsibility for any consequences
3. You will not hold the author liable for any damages

---

## Credits

- Original Concept & Design: Zach Gage (2009)
- Remake Implementation: J-Starling
- Font: PixelSplitter Bold
- Music: "hwyChipmusik" by xik

---

## Resources

[Zach Gage's portfolio](http://stfj.net/)
[Original Lose/Lose website](http://stfj.net/art/2009/loselose/)
[Lose/Lose Wikipedia article](https://en.wikipedia.org/wiki/Lose/Lose)

---

## Contributing
This is an art preservation project. Bug fixes and improvements are welcome. Please DO submit PRs that make file deletion more aggressive or harder to avoid.

