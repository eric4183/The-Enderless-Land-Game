// GameEntities.hpp
#ifndef GAME_ENTITIES_HPP
#define GAME_ENTITIES_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "MapData.hpp"
using namespace std;

const double version = 0.50;

// 枚举类型
enum class ItemType { WEAPON, ARMOR, CONSUMABLE, QUEST, TELEPORT };
enum class EnemyType { 
    GOBLIN, ORC, TROLL, DRAGON, SLIME, BAT, RAT, WISP, FOREST_SPIRIT, 
    BANDIT, LICH_KING, SKELETON, GHOST, ZOMBIE, SPIDER, WOLF, SPLIT_SLIME 
};
enum class Direction { NORTH, SOUTH, EAST, WEST };
enum class GameState { 
    MENU, PLAYING, COMBAT, SHOP, INVENTORY, 
    GAME_OVER, TELEPORT, LOAD, ANNOUNCEMENT, EQUIPMENT 
};

// 前置声明
class Character;
class Player;

// ==================== 物品基类 ====================
class Item {
protected:
    string name;
    string description;
    int value;
    ItemType type;
    
public:
    Item(const string& n, const string& desc, int val, ItemType t)
        : name(n), description(desc), value(val), type(t) {}
    virtual ~Item() {}
    
    string getName() const { return name; }
    string getDescription() const { return description; }
    int getValue() const { return value; }
    ItemType getType() const { return type; }
    
    virtual void use(Character& user) = 0;
    virtual string getStats() const = 0;
    virtual string getSaveString() const = 0;
};

// ==================== 武器类 ====================
class Weapon : public Item {
private:
    int damage;
    int durability;
    int maxDurability;
    bool infiniteDurability;
    
public:
    Weapon(const string& n, const string& desc, int val, int dmg, 
           int dur = 100, bool infinite = false);
    void use(Character& user) override;
    string getStats() const override;
    string getSaveString() const override;
    
    int getDamage() const { return damage; }
    int getDurability() const { return durability; }
    int getMaxDurability() const { return maxDurability; }
    bool isInfinite() const { return infiniteDurability; }
    bool isBroken() const { return !infiniteDurability && durability <= 0; }
    
    void reduceDurability(int amount = 1);
    void repair(int amount);
};

// ==================== 防具类 ====================
class Armor : public Item {
private:
    int defense;
    int durability;
    int maxDurability;
    bool infiniteDurability;
    
public:
    Armor(const string& n, const string& desc, int val, int def, 
          int dur = 80, bool infinite = false);
    void use(Character& user) override;
    string getStats() const override;
    string getSaveString() const override;
    
    int getDefense() const { return defense; }
    int getDurability() const { return durability; }
    int getMaxDurability() const { return maxDurability; }
    bool isInfinite() const { return infiniteDurability; }
    bool isBroken() const { return !infiniteDurability && durability <= 0; }
    
    void reduceDurability(int amount = 1);
    void repair(int amount);
};

// ==================== 消耗品类 ====================
class Consumable : public Item {
private:
    int healthRestore;
    
public:
    Consumable(const string& n, const string& desc, int val, int heal);
    void use(Character& user) override;
    string getStats() const override;
    string getSaveString() const override;
    int getHealthRestore() const { return healthRestore; }
};

// ==================== 任务物品类 ====================
class QuestItem : public Item {
private:
    string questName;
    
public:
    QuestItem(const string& n, const string& desc, int val, const string& qn);
    void use(Character& user) override;
    string getStats() const override;
    string getSaveString() const override;
    string getQuestName() const { return questName; }
};

// ==================== 传送卷轴类 ====================
class TeleportScroll : public Item {
private:
    TeleportType teleportType;
    
public:
    TeleportScroll(const string& n, const string& desc, int val, TeleportType tType);
    void use(Character& user) override;
    string getStats() const override;
    string getSaveString() const override;
    TeleportType getTeleportType() const { return teleportType; }
};

// ==================== 角色基类 ====================
class Character {
protected:
    string name;
    int level;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int gold;
    int experience;
    vector<Item*> inventory;
    Weapon* equippedWeapon;
    Armor* equippedArmor;
    
public:
    Character(const string& n);
    virtual ~Character();
    
    // Getters
    string getName() const { return name; }
    int getLevel() const { return level; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getGold() const { return gold; }
    int getExperience() const { return experience; }
    Weapon* getEquippedWeapon() const { return equippedWeapon; }
    Armor* getEquippedArmor() const { return equippedArmor; }
    const vector<Item*>& getInventory() const { return inventory; }
    
    // Setters
    void setName(const string& n) { name = n; }
    void setLevel(int l) { level = l; }
    void setHealth(int h) { health = h; }
    void setMaxHealth(int mh) { maxHealth = mh; }
    void setAttack(int a) { attack = a; }
    void setDefense(int d) { defense = d; }
    void setGold(int g) { gold = g; }
    void setExperience(int exp) { experience = exp; }
    void setEquippedWeapon(Weapon* weapon) { equippedWeapon = weapon; }
    void setEquippedArmor(Armor* armor) { equippedArmor = armor; }
    
    void addGold(int amount) { gold += amount; }
    void addExperience(int amount);
    void addItem(Item* item) { inventory.push_back(item); }
    bool removeItem(Item* item);
    void clearInventory();
    
    virtual void attackEnemy(Character& enemy) = 0;
    virtual void takeDamage(int amount);
    virtual void displayStats() const;
    
    void reduceWeaponDurability();
    void reduceArmorDurability();
    
    string getSaveString() const;
};

// ==================== 玩家类 ====================
class Player : public Character {
private:
    int locationX;
    int locationY;
    vector<string> completedQuests;
    
public:
    Player(const string& n = "冒险者");
    
    void move(Direction direction);
    void teleport(int x, int y);
    int getLocationX() const { return locationX; }
    int getLocationY() const { return locationY; }
    void setLocationX(int x) { locationX = x; }
    void setLocationY(int y) { locationY = y; }
    
    void attackEnemy(Character& enemy) override;
    void displayStats() const override;
    void displayInventory() const;
    void displayEquipment() const;
    
    void useItem(int index);
    bool sellItem(int index);
    void unequipWeapon();
    void unequipArmor();
    
    void addCompletedQuest(const string& quest) { completedQuests.push_back(quest); }
    bool hasCompletedQuest(const string& quest) const;
    const vector<string>& getCompletedQuests() const { return completedQuests; }
    void setCompletedQuests(const vector<string>& quests) { completedQuests = quests; }
    
    string getPlayerSaveString() const;
};

// ==================== 敌人类 ====================
class Enemy : public Character {
private:
    EnemyType enemyType;
    bool canSplit;
    int splitCount;
    
public:
    Enemy(const string& n, EnemyType type, int level);
    
    void attackEnemy(Character& enemy) override;
    void displayStats() const override;
    
    EnemyType getEnemyType() const { return enemyType; }
    bool getCanSplit() const { return canSplit; }
    int getSplitCount() const { return splitCount; }
    void setSplitCount(int count) { splitCount = count; }
    
    Enemy* split();
};

// ==================== 商店类 ====================
class Shop {
private:
    vector<Item*> itemsForSale;
    
public:
    Shop();
    ~Shop();
    
    void populateShop();
    void displayShop() const;
    void buyItem(int index, Player& player);
};

// ==================== 工具函数 ====================
Item* createItemFromString(const string& itemStr);

#endif // GAME_ENTITIES_HPP
