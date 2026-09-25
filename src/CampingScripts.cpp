/*
 * AzerothCore Module: mod-camping
 * Author: AlsoNotMehh
 * License: GNU AGPL v3
 */

#include "CampingMgr.h"
#include "AllCreatureScript.h"
#include "AllGameObjectScript.h"
#include "GameObject.h"
#include "Item.h"
#include "ItemScript.h"
#include "Player.h"
#include "PlayerScript.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "WorldScript.h"

namespace
{
    bool IsCampfireEntry(uint32 entry)
    {
        return (entry == 29784  || // Basic Campfire (Cooking Spell 818)
                entry == 31442  || // Basic Campfire Trap
                entry == 2977   || // Camp Narache
                entry == 3474   || // Campfire
                entry == 1798   || // Campfire
                entry == 1799   || // Campfire
                entry == 1801   || // Campfire
                entry == 1802   || // Campfire
                entry == 1803   || // Campfire
                entry == 1804   || // Campfire
                entry == 1805   || // Campfire
                entry == 1806   || // Campfire
                entry == 18596  || // Crackling Campfire
                entry == 19536  || // Crackling Campfire
                entry == 20850  || // Crackling Campfire
                entry == 20879  || // Crackling Campfire
                entry == 181258 || // Campfire with Spit
                entry == 194488 || // Large Campfire
                entry == 193071);  // Grand Campfire
    }

    bool HandleGroundItemUse(Player* player, Item* item, CampObjectType type, SpellCastTargets const& targets)
    {
        if (!sCampingMgr->IsEnabled() || !player || !item)
            return false;

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float o = player->GetOrientation();

        if (WorldLocation const* loc = targets.GetDstPos())
        {
            x = loc->GetPositionX();
            y = loc->GetPositionY();
            z = loc->GetPositionZ();
        }

        if (sCampingMgr->PlaceCampObject(player, type, x, y, z, o))
        {
            player->DestroyItemCount(item->GetEntry(), 1, true);
            return true;
        }

        return false;
    }
}

// -------------------------------------------------------------------------
// Item Scripts with Ground Placement Targeting for All Camping Gear
// -------------------------------------------------------------------------

class item_camping_campfire_kit : public ItemScript
{
public:
    item_camping_campfire_kit() : ItemScript("item_camping_campfire_kit") { }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        return HandleGroundItemUse(player, item, CAMP_OBJ_CAMPFIRE, targets);
    }
};

class item_camping_tent : public ItemScript
{
public:
    item_camping_tent() : ItemScript("item_camping_tent") { }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        return HandleGroundItemUse(player, item, CAMP_OBJ_TENT, targets);
    }
};

class item_camping_sharpening_wheel : public ItemScript
{
public:
    item_camping_sharpening_wheel() : ItemScript("item_camping_sharpening_wheel") { }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        return HandleGroundItemUse(player, item, CAMP_OBJ_ANVIL, targets);
    }
};

class item_camping_incense : public ItemScript
{
public:
    item_camping_incense() : ItemScript("item_camping_incense") { }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        return HandleGroundItemUse(player, item, CAMP_OBJ_INCENSE, targets);
    }
};

class item_camping_banner : public ItemScript
{
public:
    item_camping_banner() : ItemScript("item_camping_banner") { }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        return HandleGroundItemUse(player, item, CAMP_OBJ_BANNER, targets);
    }
};

class item_camping_forge : public ItemScript
{
public:
    item_camping_forge() : ItemScript("item_camping_forge") { }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        return HandleGroundItemUse(player, item, CAMP_OBJ_FORGE, targets);
    }
};

class item_camping_herb_rack : public ItemScript
{
public:
    item_camping_herb_rack() : ItemScript("item_camping_herb_rack") { }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        return HandleGroundItemUse(player, item, CAMP_OBJ_HERB_RACK, targets);
    }
};

class item_camping_stew_pot : public ItemScript
{
public:
    item_camping_stew_pot() : ItemScript("item_camping_stew_pot") { }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        return HandleGroundItemUse(player, item, CAMP_OBJ_STEW_POT, targets);
    }
};

// -------------------------------------------------------------------------
// World & Entity Scripts
// -------------------------------------------------------------------------

class Camping_WorldScript : public WorldScript
{
public:
    Camping_WorldScript() : WorldScript("Camping_WorldScript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        sCampingMgr->LoadConfig();
    }

    void OnUpdate(uint32 diff) override
    {
        sCampingMgr->Update(diff);
    }
};

class Camping_AllCreatureScript : public AllCreatureScript
{
public:
    Camping_AllCreatureScript() : AllCreatureScript("Camping_AllCreatureScript") { }

    bool CanCreatureGossipHello(Player* player, Creature* creature) override
    {
        if (!sCampingMgr->IsEnabled() || !player || !creature)
            return false;

        if (!creature->IsTrainer())
            return false;

        uint32 skills[8] = {
            SKILL_COOKING, SKILL_LEATHERWORKING, SKILL_BLACKSMITHING, SKILL_ALCHEMY,
            SKILL_TAILORING, SKILL_MINING, SKILL_ENGINEERING, SKILL_HERBALISM
        };

        for (uint32 skillId : skills)
        {
            if (player->HasSkill(skillId))
            {
                uint32 rank = player->GetSkillValue(skillId);
                auto recipes = CampingMgr::GetRecipesForTrainer(skillId, rank);
                for (auto const* r : recipes)
                {
                    std::string label = "Craft Camping Gear: " + std::string(r->name) + " (Skill " + std::to_string(r->reqSkillRank) + "+)";
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, label, GOSSIP_SENDER_MAIN, 5000 + r->itemId);
                }
            }
        }

        return false;
    }

    bool CanCreatureGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
        if (!sCampingMgr->IsEnabled() || !player)
            return false;

        if (action >= 5000 && action <= 5000 + 90020)
        {
            uint32 itemId = action - 5000;
            sCampingMgr->CraftCampingItem(player, itemId);
            CloseGossipMenuFor(player);
            return true;
        }

        return false;
    }
};

class Camping_AllGameObjectScript : public AllGameObjectScript
{
public:
    Camping_AllGameObjectScript() : AllGameObjectScript("Camping_AllGameObjectScript") { }

    void OnGameObjectAddWorld(GameObject* go) override
    {
        if (!sCampingMgr->IsEnabled() || !go)
            return;

        if (IsCampfireEntry(go->GetEntry()))
        {
            if (Unit* ownerUnit = go->GetOwner())
            {
                if (Player* owner = ownerUnit->ToPlayer())
                {
                    sCampingMgr->RegisterCampfireGO(go, owner);
                }
            }
        }
    }

    void OnGameObjectRemoveWorld(GameObject* go) override
    {
        if (!sCampingMgr->IsEnabled() || !go)
            return;

        sCampingMgr->RemoveCampObject(go->GetGUID());
    }
};

class Camping_PlayerScript : public PlayerScript
{
public:
    Camping_PlayerScript()
        : PlayerScript("Camping_PlayerScript", {
            PLAYERHOOK_ON_UPDATE,
            PLAYERHOOK_ON_LOGOUT
        }) { }

    void OnPlayerUpdate(Player* player, uint32 diff) override
    {
        sCampingMgr->HandlePlayerUpdate(player, diff);
    }

    void OnPlayerLogout(Player* player) override
    {
        if (sCampingMgr->IsEnabled())
        {
            sCampingMgr->HandlePlayerLeave(player);
        }
    }
};

void AddCampingScripts()
{
    // Item Scripts with Ground Placement Targeting
    new item_camping_campfire_kit();
    new item_camping_tent();
    new item_camping_sharpening_wheel();
    new item_camping_incense();
    new item_camping_banner();
    new item_camping_forge();
    new item_camping_herb_rack();
    new item_camping_stew_pot();

    // World & Entity Scripts
    new Camping_WorldScript();
    new Camping_AllCreatureScript();
    new Camping_AllGameObjectScript();
    new Camping_PlayerScript();
}
