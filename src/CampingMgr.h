/*
 * AzerothCore Module: mod-camping
 * Author: AlsoNotMehh
 * License: GNU AGPL v3
 */

#ifndef CAMPING_MGR_H
#define CAMPING_MGR_H

#include "Common.h"
#include "ObjectGuid.h"
#include "SharedDefines.h"

#include <map>
#include <string>
#include <vector>

class Player;
class GameObject;
class Map;
class Creature;

enum CampObjectType : uint8
{
    CAMP_OBJ_CAMPFIRE  = 0, // Cooking: Rested XP + Warmth
    CAMP_OBJ_TENT      = 1, // Leatherworking: Rested XP + Stamina
    CAMP_OBJ_ANVIL     = 2, // Blacksmithing: Attack Power + Field Repair
    CAMP_OBJ_INCENSE   = 3, // Alchemy: Mana Regen + Intellect
    CAMP_OBJ_BANNER    = 4, // Tailoring: Spirit + Health Regen
    CAMP_OBJ_FORGE     = 5, // Mining / Engineering: Critical Strike Rating
    CAMP_OBJ_HERB_RACK = 6, // Herbalism: Nature & Armor
    CAMP_OBJ_STEW_POT  = 7, // Cooking: Well Fed Feast
    CAMP_OBJ_COUNT     = 8
};

struct RecipeReagent
{
    uint32      itemId;
    uint32      count;
    char const* name;
};

struct CampRecipeInfo
{
    uint32                     itemId;
    char const*                name;
    uint32                     skillId;
    uint32                     reqSkillRank;
    CampObjectType             objType;
    std::vector<RecipeReagent> reagents;
};

struct CampObjectInfo
{
    CampObjectType  type;
    char const*     name;
    uint32          skillId;
    uint32          goEntry;
    uint32          buffSpellId;
    char const*     buffDesc;
};

struct PlacedCampObject
{
    ObjectGuid     guid;
    CampObjectType type;
    ObjectGuid     ownerGuid;
    uint32         mapId{0};
    float          x{0.0f};
    float          y{0.0f};
    float          z{0.0f};
    float          o{0.0f};
    time_t         expireTime{0};
};

class CampingMgr
{
public:
    static CampingMgr* instance();

    void LoadConfig();
    void Update(uint32 diff);

    bool IsEnabled() const { return m_enabled; }
    float GetCampRadius() const { return m_campRadius; }

    bool CanPlaceObjectAt(Player* player, Map* map, float x, float y, float z);
    bool PlaceCampObject(Player* player, CampObjectType type, float x, float y, float z, float o);
    void RegisterCampfireGO(GameObject* go, Player* owner);
    void RemoveCampObject(ObjectGuid goGuid);

    // Profession Crafting
    bool CraftCampingItem(Player* player, uint32 itemId);
    static CampRecipeInfo const* GetRecipeInfo(uint32 itemId);
    static std::vector<CampRecipeInfo const*> GetRecipesForTrainer(uint32 skillId, uint32 playerSkillRank);

    // Player update
    void HandlePlayerUpdate(Player* player, uint32 diff);
    void HandlePlayerLeave(Player* player);

    static CampObjectInfo const* GetObjectInfo(CampObjectType type);
    static CampObjectType GetTypeByItemId(uint32 itemId);

private:
    CampingMgr();
    ~CampingMgr() = default;

    bool   m_enabled{true};
    uint32 m_campDuration{1800}; // 30 minutes
    float  m_campRadius{15.0f};
    bool   m_allowInCities{false};
    bool   m_allowInDungeons{false};

    uint32 m_updateTimer{0};

    // Active placed camp objects indexed by GameObject GUID
    std::map<ObjectGuid, PlacedCampObject> m_objects;

    // Track sitting duration for 1-hour buff lock-in
    std::map<ObjectGuid, uint32> m_playerSitTime;
};

#define sCampingMgr CampingMgr::instance()

#endif // CAMPING_MGR_H
