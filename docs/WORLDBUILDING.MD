# PhantomLite — World & Enemy Reference

> **Version 1.0 · April 18 2025**  
> One-stop reference for every region, dungeon, faction, creature archetype, and combat schema. Use this during design and implementation to ensure consistency.

---

## 1 ▸ World Overview & Biomes

Phantos is divided into five elemental regions, each centered on a shattered Obelisk shard. Travel, environments, and foes vary by biome.

| Code | Region Name      | Element   | Key Terrain & Features                | Hub Settlement       |
|------|------------------|-----------|---------------------------------------|----------------------|
| FOR  | Veriden Woods    | Earth/Wood | Ancient forests, vine‑choked temples   | Greenshade Village   |
| CAV  | Umbragrotto      | Shadow/Stone | Bioluminescent caverns, echo chambers | Blackroot Outpost    |
| DES  | Sandscar Dunes   | Fire/Sand  | Endless dunes, sunken oasis ruins     | Oasaria Oasis        |
| SNW  | Frostveil Fields | Ice/Water  | Frozen plains, crystal caverns        | Icewind Hold         |
| RUN  | Aetherruins      | Arcane/Metal | Rune‑etched ruins, floating obelisks  | Highforge Citadel    |

**The Silent Belt**—a storm‑wracked no‑man’s‑land—encircles the Obelisk crater at the continent’s heart.

---

## 2 ▸ Historical Timeline

| Era              | Years   | Highlights                                                |
|------------------|---------|-----------------------------------------------------------|
| **Genesis**      | Mythic  | Titans forge Phantos; Celestial Obelisk whole.           |
| **Concord**      | 0–300   | Unified Empire; Leviathan airships.                      |
| **Sundering War**| 301–332 | Obelisk shattered; regions isolated.                      |
| **Reclamation**  | 333–650 | Small kingdoms rise; monsters reclaim wilds.             |
| **Present**      | 651–    | Cult seeks shards; leyline destabilises.                |

---

## 3 ▸ Major Factions & NPCs

| Faction               | Base                    | Agenda                                                                 |
|-----------------------|-------------------------|------------------------------------------------------------------------|
| Circle of Fragments   | Rotating capitals       | Preserve regional stability; guard shards.                            |
| Cult of the Shard     | Secret shrines          | Reforge Obelisk to dominate Phantos.                                  |
| Gearwright Guild      | Highforge Citadel       | Recover pre‑Sundering tech for commerce.                              |
| Wayfarer Caravan      | Mobile (FOR⇄DES route)  | Facilitate trade; intelligence network.                               |

**Key NPCs:**
- *Eira* (Player) — Wanderer seeking cure for Veriden blight.  
- *Sylvara Leafsong* (FOR) — Archwarden of Veriden shard.  
- *Orrin Blackroot* (CAV) — Guildmaster & cavern guide.  
- *Selima Dunestrider* (DES) — Envoy of the Sun Priests.  
- *Kaelen Snowheart* (SNW) — Frostlord of Icewind Hold.  
- *Celene Ironquill* (RUN) — Archmagister of rune‑tech.  
- *Oracle Veyl* — Masked Cult antagonist.

---

## 4 ▸ Core Locations & Dungeons

| Biome | Location Name       | Type     | Key Mechanic / Puzzle                      |
|-------|---------------------|----------|--------------------------------------------|
| FOR   | Temple of Seed      | Dungeon  | Vine growth puzzles, grapple points        |
| CAV   | Hall of Echoes      | Dungeon  | Sound‑wave switches, shadow traversal      |
| DES   | Sunken Spire        | Dungeon  | Sunbeam‑timed doors, heat hazards          |
| SNW   | Glacial Bastion     | Dungeon  | Ice‑slip floors, freeze‑switch levers      |
| RUN   | Obelisk Core        | Dungeon  | Rune‑tech constructs, dual‑shard locks     |

Each dungeon awards one Obelisk Shard and a new gameplay mechanic.

---

## 5 ▸ Enemy Reference & Behavior Spec

### 5.1 Data Columns

| Col        | Meaning                                                      |
|------------|--------------------------------------------------------------|
| **ID**     | Unique code used in spawn JSON & enum (e.g. `FOR_SLIME`)     |
| **Size**   | Collision box (tiles)                                       |
| **HP**     | Heart pips                                                  |
| **DMG**    | Damage per hit (pips)                                      |
| **Speed**  | px/s (walk); dash speed where applicable                   |
| **AI Atoms**| Composed behaviour modules (see §5.2)                       |
| **Drops**  | Probability list (Heart, Coin, LootID)                     |
| **Sprite** | PNG atlas path (`sprites/<biome>/<id>.png`)                |
| **Frames** | idle/walk/attack/death frame counts                         |

### 5.2 Behaviour Atom Library

| Atom             | Description                                                |
|------------------|------------------------------------------------------------|
| `wander_random`  | Move to random point within radius; idle 1 s between moves. |
| `chase_player`   | If player within 8 tiles and line‑of‑sight → pursue.       |
| `ranged_shoot`   | Fire projectile on cooldown.                              |
| `charge_dash`    | Telegraph 0.3 s → dash at 400 px/s for 0.5 s.              |
| `armor_gate`     | Ignore `Hit` unless weak‑spot active.                     |
| `dead_poof`      | Spawn 6 particles; play SFX `enemy_die.wav`.              |

### 5.3 Biome × Enemy Table (excerpt)

| Biome | ID          | Size | HP | DMG | Speed | AI Atoms                         | Drops                        | Sprite                     | Frames   |
|-------|-------------|------|----|-----|-------|----------------------------------|------------------------------|----------------------------|----------|
| FOR   | FOR_SLIME   | 1×1  | 2  | 1   | 60    | wander_random + chase_player     | 30 % heart, 70 % coin        | `sprites/for/slime.png`    | 1/4/0/4  |
| FOR   | FOR_BOAR    | 1×1  | 3  | 2   | 140   | charge_dash                      | 50 % coin                    | `sprites/for/boar.png`     | 1/4/0/4  |
| CAV   | CAV_BAT     | 1×1  | 1  | 1   | 120   | wander_random + chase_player     | 20 % heart                   | `sprites/cav/bat.png`      | 1/4/0/4  |
| DES   | DES_SCARAB  | 1×1  | 1  | 1   | 90    | wander_random                    | 15 % coin                    | `sprites/des/scarab.png`   | 1/4/0/4  |
| SNW   | SNW_WOLF    | 1×1  | 2  | 2   | 150   | charge_dash                      | 40 % heart                   | `sprites/snw/wolf.png`     | 1/4/0/4  |
| RUN   | RUN_DRONE   | 1×1  | 2  | 1   | 80    | ranged_shoot                     | 10 % shard                   | `sprites/run/drone.png`    | 1/2/1/4  |

> _Full enemy roster (including mini‑bosses and elites) in_ `docs/enemies.csv`.

---

## 6 ▸ JSON Spawn Schema (v0.2)
```json
{
  "spawns": [
    { "id": "FOR_SLIME", "x": 12, "y": 7, "respawnable": true }
  ]
}
```
Parser: `world/atoms/spawn_loader.cpp`  
Test: `tests/world/test_spawn.cpp`

---

## 7 ▸ Hit & Damage Interface
```cpp
struct Hit {
    int dmg;            // heart pips
    Vector2 knockback;  // px/s impulse
    enum class Type { Melee, Arrow, Fire, Ice } type;
};

virtual void on_hit(const Hit&);
```
All entities implement `on_hit(Hit)`.  
Knockback handled by movement/collision atoms.

---

## 8 ▸ Sprite & Animation Guidelines

1. **Base tile size:** 32 × 32 px (64×64 for 2×2 objects).  
2. **File naming:** `sprites/<biome>/<id>_<anim>.png` (`idle`, `walk`, `attack`, `death`).  
3. **Atlas constraints:** ≤ 256×256 px per sheet; frames packed horizontally.  
4. **Transparency:** PNG w/ alpha; no extra glow.  
5. **Pivot:** Bottom‑center to simplify y‑sorting.  
6. **SVG source:** Mirror in `assets_src/`, preserve layer names (`body`, `eye`, `weapon`).

---

*If it’s not in this document, it isn’t canon. Reference this file when creating new enemies, world content, or combat systems.*

