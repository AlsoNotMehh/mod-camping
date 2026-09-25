/*
 * AzerothCore Module: mod-camping
 * Author: AlsoNotMehh
 * License: GNU AGPL v3
 */

#include "CampingMgr.h"
#include "Config.h"
#include "GameTime.h"
#include "GameObject.h"
#include "Map.h"
#include "Player.h"
#include "SpellAuras.h"
#include "World.h"
#include "WorldSessionMgr.h"

#include <cmath>

namespace
{
    CampObjectInfo const s_objectInfoTable[CAMP_OBJ_COUNT] =
    {
        // CAMP_OBJ_CAMPFIRE: Cooking -> Cozy Fire (Spell 7353)
        { CAMP_OBJ_CAMPFIRE,  "Basic Campfire",           SKILL_COOKING,        29784, 7353,  "Cozy Fire (Increases Spirit by 4 and allows cooking)." },
        // CAMP_OBJ_TENT: Leatherworking -> Fortitude (Spell 72590)
        { CAMP_OBJ_TENT,      "Camp Tent",                SKILL_LEATHERWORKING, 19319, 72590, "Fortitude (Increases Stamina & accelerated Rested XP)." },
        // CAMP_OBJ_ANVIL: Blacksmithing -> Might / Sharpened (Spell 48931)
        { CAMP_OBJ_ANVIL,     "Sharpening Wheel & Anvil", SKILL_BLACKSMITHING,  1748,  48931, "Sharpened Edge (Increases Attack Power & mobile repair)." },
        // CAMP_OBJ_INCENSE: Alchemy -> Arcane Intellect (Spell 36880)
        { CAMP_OBJ_INCENSE,   "Alchemical Lab & Incense", SKILL_ALCHEMY,        2686,  36880, "Alchemical Intellect (Increases Intellect & Mana)." },
        // CAMP_OBJ_BANNER: Tailoring -> Battle Standard (Spell 23034)
        { CAMP_OBJ_BANNER,    "Faction Banner & Bedroll", SKILL_TAILORING,      21680, 23034, "Camp Standard (Increases Health & Morale)." },
        // CAMP_OBJ_FORGE: Mining / Engineering -> Well Fed Crit (Spell 57329)
        { CAMP_OBJ_FORGE,     "Portable Forge",           SKILL_MINING,         1749,  57329, "Tempered Forge (Increases Critical Strike Rating)." },
        // CAMP_OBJ_HERB_RACK: Herbalism -> Herbal Ward / Armor (Spell 48040)
        { CAMP_OBJ_HERB_RACK, "Herbal Drying Rack",       SKILL_HERBALISM,      18077, 48040, "Herbal Ward (Increases Armor & Nature Resist)." },
        // CAMP_OBJ_STEW_POT: Cooking -> Well Fed Feast (Spell 57399)
        { CAMP_OBJ_STEW_POT,  "Camp Stew Pot",            SKILL_COOKING,        3266,  57399, "Well Fed (Great Feast food bonus)." }
    };

    std::vector<CampRecipeInfo> const s_recipeTable =
    {
        // 90001: Basic Campfire Kit (Cooking 1+)
        {
            90001, "Basic Campfire Kit", SKILL_COOKING, 1, CAMP_OBJ_CAMPFIRE,
            { { 4470, 1, "Simple Wood" }, { 4471, 1, "Flint and Tinder" } }
        },
        // 90002: Camp Tent (Leatherworking 20+)
        {
            90002, "Camp Tent", SKILL_LEATHERWORKING, 20, CAMP_OBJ_TENT,
            { { 2318, 6, "Light Leather" }, { 2320, 2, "Coarse Thread" } }
        },
        // 90003: Sharpening Wheel (Blacksmithing 20+)
        {
            90003, "Sharpening Wheel", SKILL_BLACKSMITHING, 20, CAMP_OBJ_ANVIL,
            { { 2835, 4, "Rough Stone" }, { 2840, 2, "Copper Bar" } }
        },
        // 90004: Alchemical Incense (Alchemy 20+)
        {
            90004, "Alchemical Incense", SKILL_ALCHEMY, 20, CAMP_OBJ_INCENSE,
            { { 2447, 2, "Peacebloom" }, { 765, 2, "Silverleaf" }, { 3371, 1, "Empty Vial" } }
        },
        // 90005: Faction Bedroll & Banner (Tailoring 20+)
        {
            90005, "Faction Bedroll & Banner", SKILL_TAILORING, 20, CAMP_OBJ_BANNER,
            { { 2589, 6, "Linen Cloth" }, { 2320, 2, "Coarse Thread" } }
        },
        // 90006: Portable Forge (Mining 20+ / Engineering 20+)
        {
            90006, "Portable Forge", SKILL_MINING, 20, CAMP_OBJ_FORGE,
            { { 2835, 6, "Rough Stone" }, { 2840, 4, "Copper Bar" }, { 4359, 2, "Handful of Copper Bolts" } }
        },
        // 90007: Herbal Drying Rack (Herbalism 20+)
        {
            90007, "Herbal Drying Rack", SKILL_HERBALISM, 20, CAMP_OBJ_HERB_RACK,
            { { 4470, 2, "Simple Wood" }, { 2449, 2, "Earthroot" } }
        },
        // 90008: Camp Stew Pot (Cooking 50+)
        {
            90008, "Camp Stew Pot", SKILL_COOKING, 50, CAMP_OBJ_STEW_POT,
            { { 4470, 2, "Simple Wood" }, { 769, 3, "Chunk of Boar Meat" }, { 159, 1, "Refreshing Spring Water" } }
        }
    };
}

CampingMgr* CampingMgr::instance()
{
    static CampingMgr instance;
    return &instance;
}

CampingMgr::CampingMgr()
{
    LoadConfig();
}

void CampingMgr::LoadConfig()
{
    m_enabled         = sConfigMgr->GetOption<bool>("Camping.Enable", true);
    m_campDuration    = sConfigMgr->GetOption<uint32>("Camping.CampDuration", 1800);
    m_campRadius      = sConfigMgr->GetOption<float>("Camping.CampRadius", 15.0f);
    m_allowInCities   = sConfigMgr->GetOption<bool>("Camping.AllowInCities", false);
    m_allowInDungeons = sConfigMgr->GetOption<bool>("Camping.AllowInDungeons", false);
}

CampObjectInfo const* CampingMgr::GetObjectInfo(CampObjectType type)
{
    if (type < CAMP_OBJ_COUNT)
        return &s_objectInfoTable[type];
    return nullptr;
}

CampRecipeInfo const* CampingMgr::GetRecipeInfo(uint32 itemId)
{
    for (auto const& r : s_recipeTable)
    {
        if (r.itemId == itemId)
            return &r;
    }
    return nullptr;
}

CampObjectType CampingMgr::GetTypeByItemId(uint32 itemId)
{
    for (auto const& r : s_recipeTable)
    {
        if (r.itemId == itemId)
            return r.objType;
    }
    return CAMP_OBJ_CAMPFIRE;
}

std::vector<CampRecipeInfo const*> CampingMgr::GetRecipesForTrainer(uint32 skillId, uint32 playerSkillRank)
{
    std::vector<CampRecipeInfo const*> result;
    for (auto const& r : s_recipeTable)
    {
        bool matches = (r.skillId == skillId);
        if (skillId == SKILL_MINING && r.itemId == 90006)
            matches = true;
        if (skillId == SKILL_ENGINEERING && r.itemId == 90006)
            matches = true;

        if (matches && playerSkillRank >= r.reqSkillRank)
        {
            result.push_back(&r);
        }
    }
    return result;
}

bool CampingMgr::CraftCampingItem(Player* player, uint32 itemId)
{
    if (!m_enabled || !player)
        return false;

    CampRecipeInfo const* recipe = GetRecipeInfo(itemId);
    if (!recipe)
        return false;

    bool hasSkill = false;
    if (recipe->itemId == 90006)
    {
        hasSkill = (player->HasSkill(SKILL_MINING) && player->GetSkillValue(SKILL_MINING) >= recipe->reqSkillRank) ||
                   (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) >= recipe->reqSkillRank);
    }
    else
    {
        hasSkill = (player->HasSkill(recipe->skillId) && player->GetSkillValue(recipe->skillId) >= recipe->reqSkillRank);
    }

    if (!hasSkill)
        return false;

    // Check reagents
    for (auto const& reagent : recipe->reagents)
    {
        if (!player->HasItemCount(reagent.itemId, reagent.count))
            return false;
    }

    // Check inventory space
    ItemPosCountVec dest;
    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, recipe->itemId, 1);
    if (msg != EQUIP_ERR_OK)
    {
        player->SendEquipError(msg, nullptr, nullptr, recipe->itemId);
        return false;
    }

    // Consume reagents
    for (auto const& reagent : recipe->reagents)
    {
        player->DestroyItemCount(reagent.itemId, reagent.count, true);
    }

    // Add crafted item
    Item* newItem = player->StoreNewItem(dest, recipe->itemId, true);
    if (newItem)
    {
        player->SendNewItem(newItem, 1, true, false);
    }

    // Award skill point
    player->UpdateSkill(recipe->skillId, 1);
    return true;
}

bool CampingMgr::CanPlaceObjectAt(Player* player, Map* map, float /*x*/, float /*y*/, float /*z*/)
{
    if (!m_enabled || !map || !player)
        return false;

    if (!m_allowInDungeons && map->IsDungeon())
        return false;

    if (!m_allowInCities && (player->HasRestFlag(REST_FLAG_IN_CITY) || player->HasRestFlag(REST_FLAG_IN_TAVERN)))
        return false;

    return true;
}

bool CampingMgr::PlaceCampObject(Player* player, CampObjectType type, float x, float y, float z, float o)
{
    if (!player)
        return false;

    CampObjectInfo const* info = GetObjectInfo(type);
    if (!info)
        return false;

    if (!CanPlaceObjectAt(player, player->GetMap(), x, y, z))
        return false;

    GameObject* go = player->SummonGameObject(info->goEntry, x, y, z, o, 0.0f, 0.0f, 0.0f, 0.0f, m_campDuration);
    if (!go)
        return false;

    time_t now = GameTime::GetGameTime().count();

    PlacedCampObject obj;
    obj.guid       = go->GetGUID();
    obj.type       = type;
    obj.ownerGuid  = player->GetGUID();
    obj.mapId      = player->GetMapId();
    obj.x          = x;
    obj.y          = y;
    obj.z          = z;
    obj.o          = o;
    obj.expireTime = now + m_campDuration;

    m_objects[go->GetGUID()] = obj;

    // Apply authentic buff immediately
    if (info->buffSpellId > 0)
    {
        player->CastSpell(player, info->buffSpellId, true);
    }

    return true;
}

void CampingMgr::RegisterCampfireGO(GameObject* go, Player* owner)
{
    if (!go || !owner)
        return;

    time_t now = GameTime::GetGameTime().count();

    PlacedCampObject obj;
    obj.guid       = go->GetGUID();
    obj.type       = CAMP_OBJ_CAMPFIRE;
    obj.ownerGuid  = owner->GetGUID();
    obj.mapId      = go->GetMapId();
    obj.x          = go->GetPositionX();
    obj.y          = go->GetPositionY();
    obj.z          = go->GetPositionZ();
    obj.o          = go->GetOrientation();
    obj.expireTime = now + m_campDuration;

    m_objects[go->GetGUID()] = obj;

    owner->CastSpell(owner, 7353, true); // Cozy Fire
}

void CampingMgr::RemoveCampObject(ObjectGuid goGuid)
{
    m_objects.erase(goGuid);
}

void CampingMgr::HandlePlayerUpdate(Player* player, uint32 diff)
{
    if (!m_enabled || !player || !player->IsInWorld())
        return;

    uint32 mapId = player->GetMapId();
    float px = player->GetPositionX();
    float py = player->GetPositionY();
    float pz = player->GetPositionZ();

    bool nearRestObject = false;
    std::vector<CampObjectType> nearTypes;

    for (auto const& pair : m_objects)
    {
        PlacedCampObject const& obj = pair.second;
        if (obj.mapId != mapId)
            continue;

        float dx = obj.x - px;
        float dy = obj.y - py;
        float dz = obj.z - pz;
        float distSq = dx * dx + dy * dy + dz * dz;

        if (distSq <= (m_campRadius * m_campRadius))
        {
            if (obj.type == CAMP_OBJ_CAMPFIRE || obj.type == CAMP_OBJ_TENT)
            {
                nearRestObject = true;
            }
            nearTypes.push_back(obj.type);
        }
    }

    if (nearRestObject)
    {
        if (!player->HasRestFlag(REST_FLAG_IN_TAVERN))
        {
            player->SetRestFlag(REST_FLAG_IN_TAVERN);
        }
    }
    else if (nearTypes.empty())
    {
        HandlePlayerLeave(player);
    }

    // Apply active auras for all nearby objects
    for (CampObjectType type : nearTypes)
    {
        CampObjectInfo const* info = GetObjectInfo(type);
        if (info && info->buffSpellId > 0 && !player->HasAura(info->buffSpellId))
        {
            player->CastSpell(player, info->buffSpellId, true);
        }
    }

    // If sitting down by campsite
    if (nearRestObject && player->IsSitState() && !player->isMoving() && !player->IsInCombat())
    {
        ObjectGuid pGuid = player->GetGUID();
        m_playerSitTime[pGuid] += diff;

        if (m_playerSitTime[pGuid] >= 30000) // 30s sit lock-in
        {
            m_playerSitTime[pGuid] = 0;
            // Refresh 1-hour aura
            for (CampObjectType type : nearTypes)
            {
                CampObjectInfo const* info = GetObjectInfo(type);
                if (info && info->buffSpellId > 0)
                {
                    player->CastSpell(player, info->buffSpellId, true);
                }
            }
        }
    }
    else
    {
        m_playerSitTime[player->GetGUID()] = 0;
    }
}

void CampingMgr::HandlePlayerLeave(Player* player)
{
    if (!player)
        return;

    m_playerSitTime.erase(player->GetGUID());

    if (player->HasRestFlag(REST_FLAG_IN_TAVERN))
    {
        player->RemoveRestFlag(REST_FLAG_IN_TAVERN);
    }
}

void CampingMgr::Update(uint32 diff)
{
    if (!m_enabled)
        return;

    m_updateTimer += diff;
    if (m_updateTimer < 2000)
        return;

    m_updateTimer = 0;
    time_t now = GameTime::GetGameTime().count();

    std::vector<ObjectGuid> toRemove;
    for (auto const& pair : m_objects)
    {
        if (pair.second.expireTime <= now)
        {
            toRemove.push_back(pair.first);
        }
    }

    for (ObjectGuid guid : toRemove)
    {
        RemoveCampObject(guid);
    }
}
