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
- **Proper audio mixing** using Qt Multimedia module
- **Procedural enemy sprites** generated from block patterns
- **Animated enemy poses** with alternating frames

---

## About The Original

"Lose/Lose" was created by Zach Gage in 2009 as a video game art piece exploring the relationship between virtual actions and real-world consequences. The game challenges players to consider what their in-game choices actually mean when translated to real effects.

- **Original release:** 2009
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
- Deleting a game when player is destroyed is not implemented

---

## Legal & Ethical Disclaimer

This software is provided for educational and artistic purposes only. The author:
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
- Remake Implementation: Jack Starling
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

---

## Philosophical Reflection: Thoughts on Lose/Lose (2009–2026)

*The following are excerpts from Zach Gage's original 2009 artist statement, followed by my personal reflections on how the questions he raised have evolved—and how my remake attempts to engage with them.*

---

### Zach Gage, 2009

> Although touching aliens will cause the player to lose the game, and killing aliens awards points, the aliens will never actually fire at the player. This calls into question the player's mission, which is never explicitly stated, only hinted at through classic game mechanics. Is the player supposed to be an aggressor? Or merely an observer, traversing through a dangerous land?
>
> Why do we assume that because we are given a weapon and awarded for using it, that doing so is right?
>
> By way of exploring what it means to kill in a video-game, Lose/Lose broaches bigger questions. As technology grows, our understanding of it diminishes, yet, at the same time, it becomes increasingly important in our lives. At what point does our virtual data become as important to us as physical possessions? If we have reached that point already, what real objects do we value less than our data? What implications does trusting something so important to something we understand so poorly have?

---

## My Response (2026 Remake)

Zach's questions cut deeper than typical game design discourse—they probe the unspoken assumptions we carry into every digital interaction. Working on this remake gave me space to sit with those questions and offer my own tentative answers.

### Should the player be an aggressor?

In my remake, the aliens are far more aggressive than in the original—they shoot back, and bosses arrive to defend them. Yet there's a subtle catch: if you simply *dodge* and refuse to fire, nothing punishes you. The bosses never spawn. The endless cosmos just... continues.

I see a parallel here with my own approach to life. My default tactic is non-confrontation. I avoid conflict, I sidestep provocation. And maybe—just maybe—at the far end of that endless, ambiguous journey, something like genuine peace is waiting. For both the player and for me.

The game doesn't demand aggression. It merely *rewards* it. There's a difference.

### Why do we assume that being given a weapon and rewarded for using it makes it right?

There's an instinctive trap wired into all of us: we are, from birth, inclined to cause one another pain—even when it's not physical. Every person strives to be *better* than someone else. That's the ruthless engine of progress, and you can't outrun it.

Now apply that to aliens who don't look like us, who exist inside ships that *obstruct* our path. The leap from "they're in my way" to "they deserve to be destroyed" is terrifyingly short. The game simply hands you a gun and a score counter, then waits to see what you'll do with them.

### When does virtual data become as important as physical possessions?

Nearly two decades after the original, I'm certain Zach couldn't have predicted just how thoroughly our lives would become entangled with our data. Today, data isn't just *like* a part of life—it **is** a part of life.

Consider this: nearly every aspect of existence has been digitized. To live without data as an integral component of daily life, you'd have to withdraw from society entirely—become a hermit. And that's just the social layer.

Everyone has an unremarkable folder somewhere named `Anapa 2009` filled with vacation photos. You might open it twice in your entire life. What harm could deleting it possibly do? But those two visits will hit you with a wave of nostalgia so potent you'll ache to return to those days. That folder isn't just files—it's a **fragment of a life lived**. And that's merely the tip of the iceberg.

From the moment personal computing and the internet became cultural phenomena, data transformed into something more than information. It became memory. Identity. *Proof*.

### What real objects do we value less than our data?

*Lose/Lose* is typically run in a virtual machine, on a sacrificial system, or on a thoroughly backed-up drive. Files get deleted. It's amusing. Nothing catastrophic happens.

Real life offers no save-scumming.

That's why, even in 2026, no virtual object outweighs anything that holds genuine significance in the physical world. A corrupted hard drive is devastating. A lost photograph is heartbreaking. But a lost *person*? A lost *home*? The hierarchy remains intact. Data matters *because* it connects to the real—not because it replaces it.

### What are the consequences of entrusting something so vital to something we understand so poorly?

Understanding itself has become a charged issue over the past twenty years. We've witnessed an explosion of new technologies, yet the average person's comprehension of them has arguably *shrunk*. Information is easier to find than ever—but so is misinformation. Disinformation. Outright, algorithmically-generated nonsense.

As for consequences? We were never immune to them, then or now. But the virtual life of the average user has become significantly more *protected*, which in turn has made real life meaningfully *easier*. Backups are automated. Encryption is ubiquitous. The systems we don't understand at least try to guard us from our own ignorance.

Whether that's comforting or concerning is an open question.

---

## Closing Thoughts

I hope that over the years, Zach found answers to his questions—answers that brought him some measure of satisfaction. I'm still young. Still inexperienced. But grappling with his ideas while rebuilding his game has been genuinely rewarding.

I'll leave a few questions of my own—for Zach, and for anyone else who stumbles across this project.

---

## Questions for Zach Gage (and Anyone Else)

1. **Does the weight of a virtual action change if you can't see its consequences?** If a deleted file vanishes silently, never visibly mourned, is the act less violent than breaking a physical object that leaves visible wreckage?

2. **What would a "pacifist ending" for Lose/Lose look like?** If a player navigated the entire game without firing a single shot, what should the game *owe* them for that restraint?

3. **Two decades on, do you feel more or less optimistic about our relationship with the data we create?** Has the rise of cloud storage and streaming made us more cavalier or more careful?

4. **If you could redesign Lose/Lose today with all the technological and cultural shifts of the last twenty years in mind, what would you change—and what would remain untouchable?**

