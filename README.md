# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore Module: mod-camping

[![AzerothCore Module](https://img.shields.io/badge/AzerothCore-Module-red?style=flat-square&logo=github)](https://github.com/azerothcore/azerothcore-wotlk)
[![C++20](https://img.shields.io/badge/Language-C++20-00599C?style=flat-square&logo=c%2B%2B)](https://isocpp.org/)
[![Branch 3.3.5a](https://img.shields.io/badge/Branch-3.3.5a-orange?style=flat-square)](https://github.com/azerothcore/azerothcore-wotlk)
[![License AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue?style=flat-square)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/AlsoNotMehh/mod-camping?style=flat-square&color=yellow&logo=github)](https://github.com/AlsoNotMehh/mod-camping/stargazers)

An open-world **Camping & Campsite System** for **AzerothCore (WotLK 3.3.5a)** inspired by *WoW Forever* and *Turtle WoW*.

Players use their **professions** to craft camping items, pitch campfires in the wilderness, and collaborate to deploy 3D profession fixtures (tents, anvils, alchemy cauldrons, banners, and feasts). Resting by the fire (`/sit`) accumulates Rested XP and grants 1-hour stat buffs!

---

## 🛠️ Profession Items & Fixtures

All items are crafted by players through their primary and secondary professions and used directly from the inventory:

| Profession | Crafted Item | Item ID | Deployed Fixture & 1-Hour Buff |
| :--- | :--- | :---: | :--- |
| **Cooking** | **Basic Campfire Kit** *(or Spell 818)* | `90001` | Pitches the campfire campsite in the open world (30 min duration). |
| **Leatherworking** | **Camp Tent** | `90002` | Deploys a 3D *Camp Tent* granting rapid Rested XP and **+10% Stamina**. |
| **Blacksmithing** | **Sharpening Wheel** | `90003` | Deploys a *Sharpening Wheel & Anvil* granting **+10% Attack Power** and field repair. |
| **Alchemy** | **Alchemical Incense** | `90004` | Deploys an *Incense Burner & Field Lab* granting **+10% Intellect** and Mana Regeneration. |
| **Tailoring** | **Faction Bedroll & Banner** | `90005` | Deploys a *Faction Banner* granting **+10% Spirit** and out-of-combat Health Regen. |
| **Mining / Eng.** | **Portable Forge** | `90006` | Deploys a *Portable Forge* granting **+5% Critical Strike Chance** and smelting. |
| **Herbalism** | **Herbal Drying Rack** | `90007` | Deploys a *Herbal Rack* granting **+10% Armor** and Nature Resistance. |
| **Cooking** | **Camp Stew Pot** | `90008` | Deploys a *Stew Pot & Feast* granting **Well Fed** bonuses and upgrades camp slots. |

---

## 🏕️ How It Works (100% In-Game)

1. **Pitching a Camp:**
   - A player uses a **Basic Campfire Kit** from their bag (or casts the Cooking **Basic Campfire** spell).
   - The campsite is registered in the open world for 30 minutes.
   - *Rules:* Cannot pitch inside capital cities, sanctuaries, dungeons, or within 40 yards of another camp.

2. **Deploying Profession Fixtures:**
   - Any player with a corresponding profession can right-click their crafted item (e.g. `Camp Tent`, `Sharpening Wheel`) while standing near the campfire.
   - Alternatively, players can **right-click the campfire** to select from the interactive menu.
   - The fixture spawns in 3D around the campfire. Each player can contribute 1 fixture per camp.

3. **Resting & 1-Hour Buffs:**
   - **Rested XP:** Standing in the 15-yard campsite radius grants the `Resting` flag (accumulates Rested XP in the wild and enables instant logout).
   - **Gaining Buffs:** Sitting down (**`/sit`**) near the campfire for **60 seconds** grants all active 1-hour buffs from the deployed fixtures.
   - **Cooldown:** Applies a 1-hour exhaustion cooldown to keep the system balanced.

---

## ⚙️ Configuration (`mod_camping.conf`)

| Setting | Default | Description |
| :--- | :---: | :--- |
| `Camping.Enable` | `1` | Enable or disable the camping system. |
| `Camping.Duration` | `1800` | Duration in seconds before a camp despawns (30 mins). |
| `Camping.SitDurationToBuff` | `60` | Seconds a player must sit to gain the 1-hour buffs. |
| `Camping.CooldownDuration` | `3600` | Cooldown in seconds before gaining camp buffs again (1 hour). |
| `Camping.CampRadius` | `15.0` | Radius in yards where resting and buffs apply. |
| `Camping.MinDistanceBetweenCamps` | `40.0` | Minimum distance in yards required between two active camps. |
| `Camping.BasicCampSlots` | `3` | Max fixtures in a Basic Camp. |
| `Camping.SturdyCampSlots` | `5` | Max fixtures in a Sturdy Camp. |
| `Camping.GrandCampSlots` | `10` | Max fixtures in a Grand Camp. |
| `Camping.AllowInCities` | `0` | Allow placing camps inside capital cities (0 = Disabled). |
| `Camping.AllowInDungeons` | `0` | Allow placing camps inside dungeons/raids (0 = Disabled). |
| `Camping.MinSkillRequired` | `20` | Minimum profession skill required to deploy a fixture. |

---

## 📦 Installation

1. Clone into your AzerothCore `modules` directory:
   ```bash
   cd azerothcore/modules
   git clone https://github.com/AlsoNotMehh/mod-camping.git
   ```
2. Import the SQL file to your `acore_world` database:
   ```bash
   mysql -u root -p acore_world < mod-camping/data/sql/db-world/camping_items.sql
   ```
3. Re-run CMake and compile:
   ```bash
   cmake -B build
   cmake --build build --config Release
   ```
4. Copy `conf/mod_camping.conf.dist` to your `configs` directory as `mod_camping.conf`.

---

## 📄 License

This project is licensed under the [GNU AGPL v3 License](LICENSE).
