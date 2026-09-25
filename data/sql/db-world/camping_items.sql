--
-- AzerothCore Module: mod-camping (Clean & Authentic Custom Spells & DBC Item Templates)
--

DELETE FROM `item_template` WHERE `entry` BETWEEN 90001 AND 90008;

INSERT INTO `item_template` (
    `entry`, `class`, `subclass`, `name`, `displayid`, `Quality`,
    `BuyCount`, `BuyPrice`, `SellPrice`, `ItemLevel`, `RequiredLevel`,
    `RequiredSkill`, `RequiredSkillRank`, `stackable`,
    `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellcooldown_1`,
    `description`, `ScriptName`
) VALUES
-- 90001: Basic Campfire Kit (Cooking 1+) - Icon: Flint & Tinder (4720), Spell: Pitch Campsite (90001)
(90001, 15, 0, 'Basic Campfire Kit', 4720, 2, 1, 100, 25, 1, 1, 185, 1, 20, 90001, 0, 0, -1, 'Place a campsite anywhere on the ground. Pitching a camp grants immediate Rested XP generation and warmth to all nearby travelers.', 'item_camping_campfire_kit'),

-- 90002: Camp Tent (Leatherworking 20+) - Icon: Light Leather (7382), Spell: Deploy Camp Tent (90002)
(90002, 15, 0, 'Camp Tent', 7382, 2, 1, 500, 125, 10, 5, 165, 20, 5, 90002, 0, 0, -1, 'Place a tent on the ground. Grants rapid Rested XP accumulation and +10% Stamina to all resting travelers.', 'item_camping_tent'),

-- 90003: Sharpening Wheel (Blacksmithing 20+) - Icon: Sharpening Stone (24673), Spell: Deploy Sharpening Wheel (90003)
(90003, 15, 0, 'Sharpening Wheel', 24673, 2, 1, 500, 125, 10, 5, 164, 20, 5, 90003, 0, 0, -1, 'Place on the ground. Grants +10% Attack Power and field equipment repair to all resting travelers.', 'item_camping_sharpening_wheel'),

-- 90004: Alchemical Incense (Alchemy 20+) - Icon: Potion Flask (15710), Spell: Deploy Alchemical Incense (90004)
(90004, 15, 0, 'Alchemical Incense', 15710, 2, 1, 500, 125, 10, 5, 171, 20, 5, 90004, 0, 0, -1, 'Place on the ground. Grants increased Mana Regeneration and +10% Intellect to all resting travelers.', 'item_camping_incense'),

-- 90005: Faction Bedroll & Banner (Tailoring 20+) - Icon: Battle Standard (31256), Spell: Deploy Faction Banner (90005)
(90005, 15, 0, 'Faction Bedroll & Banner', 31256, 2, 1, 500, 125, 10, 5, 197, 20, 5, 90005, 0, 0, -1, 'Place on the ground. Grants +10% Spirit and rapid Health Regeneration to all resting travelers.', 'item_camping_banner'),

-- 90006: Portable Forge (Mining / Engineering 20+) - Icon: Blacksmith Hammer (8568), Spell: Deploy Portable Forge (90006)
(90006, 15, 0, 'Portable Forge', 8568, 2, 1, 500, 125, 10, 5, 186, 20, 5, 90006, 0, 0, -1, 'Place on the ground. Grants +5% Critical Strike Chance and mobile smelting to all resting travelers.', 'item_camping_forge'),

-- 90007: Herbal Drying Rack (Herbalism 20+) - Icon: Peacebloom Herb (7396), Spell: Deploy Herbal Drying Rack (90007)
(90007, 15, 0, 'Herbal Drying Rack', 7396, 2, 1, 500, 125, 10, 5, 182, 20, 5, 90007, 0, 0, -1, 'Place on the ground. Grants +10% Armor and Nature Resistance to all resting travelers.', 'item_camping_herb_rack'),

-- 90008: Camp Stew Pot (Cooking 50+) - Icon: Westfall Stew (6428), Spell: Deploy Camp Stew Pot (90008)
(90008, 15, 0, 'Camp Stew Pot', 6428, 2, 1, 500, 125, 10, 5, 185, 20, 5, 90008, 0, 0, -1, 'Place on the ground. Grants Well Fed food bonuses to all resting travelers.', 'item_camping_stew_pot');
