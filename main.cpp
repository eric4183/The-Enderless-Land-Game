// main.cpp
#include "GameEntities.hpp"
#include "DataManager.hpp"
#include <windows.h>
#include <memory>

// ==================== 武器类实现 ====================
Weapon::Weapon(const string& n, const string& desc, int val, int dmg, int dur, bool infinite)
    : Item(n, desc, val, ItemType::WEAPON), damage(dmg), 
      durability(dur), maxDurability(dur), infiniteDurability(infinite) {}

void Weapon::use(Character& user) {
    user.removeItem(this);
    Weapon* oldWeapon = user.getEquippedWeapon();
    user.setEquippedWeapon(this);
    cout << user.getName() << " 装备了 " << getName() << "！\n";
    if (oldWeapon != nullptr && oldWeapon != this) user.addItem(oldWeapon);
}

string Weapon::getStats() const {
    string stats = "攻击力: " + to_string(damage);
    if (infiniteDurability) stats += " | 耐久度: 无限";
    else {
        stats += " | 耐久度: " + to_string(durability) + "/" + to_string(maxDurability);
        if (durability <= maxDurability * 0.3) stats += " (危险)";
        else if (durability <= maxDurability * 0.6) stats += " (磨损)";
    }
    return stats;
}

string Weapon::getSaveString() const {
    return "WEAPON|" + name + "|" + description + "|" + to_string(value) + "|" + 
           to_string(damage) + "|" + to_string(durability) + "|" + 
           to_string(maxDurability) + "|" + to_string(infiniteDurability);
}

void Weapon::reduceDurability(int amount) {
    if (!infiniteDurability) {
        durability -= amount;
        if (durability < 0) durability = 0;
    }
}

void Weapon::repair(int amount) {
    if (!infiniteDurability) {
        durability += amount;
        if (durability > maxDurability) durability = maxDurability;
    }
}

// ==================== 防具类实现 ====================
Armor::Armor(const string& n, const string& desc, int val, int def, int dur, bool infinite)
    : Item(n, desc, val, ItemType::ARMOR), defense(def), 
      durability(dur), maxDurability(dur), infiniteDurability(infinite) {}

void Armor::use(Character& user) {
    user.removeItem(this);
    Armor* oldArmor = user.getEquippedArmor();
    user.setEquippedArmor(this);
    cout << user.getName() << " 装备了 " << getName() << "！\n";
    if (oldArmor != nullptr && oldArmor != this) user.addItem(oldArmor);
}

string Armor::getStats() const {
    string stats = "防御力: " + to_string(defense);
    if (infiniteDurability) stats += " | 耐久度: 无限";
    else {
        stats += " | 耐久度: " + to_string(durability) + "/" + to_string(maxDurability);
        if (durability <= maxDurability * 0.3) stats += " (危险)";
        else if (durability <= maxDurability * 0.6) stats += " (磨损)";
    }
    return stats;
}

string Armor::getSaveString() const {
    return "ARMOR|" + name + "|" + description + "|" + to_string(value) + "|" + 
           to_string(defense) + "|" + to_string(durability) + "|" + 
           to_string(maxDurability) + "|" + to_string(infiniteDurability);
}

void Armor::reduceDurability(int amount) {
    if (!infiniteDurability) {
        durability -= amount;
        if (durability < 0) durability = 0;
    }
}

void Armor::repair(int amount) {
    if (!infiniteDurability) {
        durability += amount;
        if (durability > maxDurability) durability = maxDurability;
    }
}

// ==================== 消耗品类实现 ====================
Consumable::Consumable(const string& n, const string& desc, int val, int heal)
    : Item(n, desc, val, ItemType::CONSUMABLE), healthRestore(heal) {}

void Consumable::use(Character& user) {
    int newHealth = user.getHealth() + healthRestore;
    if (newHealth > user.getMaxHealth()) newHealth = user.getMaxHealth();
    cout << user.getName() << " 使用了 " << getName() << "，恢复了 " 
         << (newHealth - user.getHealth()) << " 点生命值！\n";
    user.setHealth(newHealth);
}

string Consumable::getStats() const {
    return "恢复: " + to_string(healthRestore) + " 点生命值";
}

string Consumable::getSaveString() const {
    return "CONSUMABLE|" + name + "|" + description + "|" + to_string(value) + "|" + to_string(healthRestore);
}

// ==================== 任务物品类实现 ====================
QuestItem::QuestItem(const string& n, const string& desc, int val, const string& qn)
    : Item(n, desc, val, ItemType::QUEST), questName(qn) {}

void QuestItem::use(Character& user) {
    cout << getName() << " 无法直接使用。\n";
}

string QuestItem::getStats() const {
    return "任务物品: " + questName;
}

string QuestItem::getSaveString() const {
    return "QUEST|" + name + "|" + description + "|" + to_string(value) + "|" + questName;
}

// ==================== 传送卷轴类实现 ====================
TeleportScroll::TeleportScroll(const string& n, const string& desc, int val, TeleportType tType)
    : Item(n, desc, val, ItemType::TELEPORT), teleportType(tType) {}

void TeleportScroll::use(Character& user) {
    cout << getName() << " 散发出空间魔法的波动...\n";
    cout << "这个物品需要在游戏中使用。\n";
}

string TeleportScroll::getStats() const {
    string locationName;
    switch (teleportType) {
        case TeleportType::NEWBIE_VILLAGE: locationName = "新手村"; break;
        case TeleportType::FIRE_LAND: locationName = "极炎之地"; break;
        case TeleportType::GOBLIN_KINGDOM: locationName = "哥布林王国"; break;
        case TeleportType::FROZEN_NIGHT: locationName = "无尽寒夜"; break;
        case TeleportType::CELESTIAL_SITE: locationName = "天璇圣地"; break;
        case TeleportType::LICH_CAVE: locationName = "巫妖王洞穴"; break;
    }
    return "传送至: " + locationName;
}

string TeleportScroll::getSaveString() const {
    return "TELEPORT|" + name + "|" + description + "|" + to_string(value) + "|" + 
           to_string(static_cast<int>(teleportType));
}

// ==================== 角色类实现 ====================
Character::Character(const string& n)
    : name(n), level(1), health(100), maxHealth(100), attack(10), defense(5), 
      gold(0), experience(0), equippedWeapon(nullptr), equippedArmor(nullptr) {}

Character::~Character() {
    for (auto item : inventory) delete item;
}

void Character::addExperience(int amount) {
    experience += amount;
    cout << getName() << " 获得了 " << amount << " 点经验值！\n";
    
    while (experience >= level * 100) {
        experience -= level * 100;
        level++;
        maxHealth += 20;
        health = maxHealth;
        attack += 5;
        defense += 2;
        
        cout << getName() << " 升级到 " << level << " 级！\n";
        cout << "生命值增加 20 点！\n";
        cout << "攻击力增加 5 点！\n";
        cout << "防御力增加 2 点！\n";
    }
}

void Character::takeDamage(int amount) {
    int damageTaken = amount - defense;
    if (damageTaken < 1) damageTaken = 1;
    
    health -= damageTaken;
    if (health < 0) health = 0;
    
    cout << getName() << " 受到了 " << damageTaken << " 点伤害！生命值: " 
         << health << "/" << maxHealth << "\n";
    
    if (equippedArmor != nullptr) reduceArmorDurability();
}

void Character::displayStats() const {
    cout << "=== " << name << " ===\n";
    cout << "等级: " << level << "\n";
    cout << "生命值: " << health << "/" << maxHealth << "\n";
    cout << "攻击力: " << attack;
    if (equippedWeapon != nullptr) cout << " + " << equippedWeapon->getDamage();
    cout << "\n";
    cout << "防御力: " << defense;
    if (equippedArmor != nullptr) cout << " + " << equippedArmor->getDefense();
    cout << "\n";
    cout << "金币: " << gold << " 枚\n";
    cout << "经验值: " << experience << "/" << level * 100 << "\n";
    
    if (equippedWeapon != nullptr) {
        cout << "武器: " << equippedWeapon->getName();
        if (equippedWeapon->isBroken()) cout << " (已损坏)";
        else if (!equippedWeapon->isInfinite()) 
            cout << " (耐久度: " << equippedWeapon->getDurability() << "/" << equippedWeapon->getMaxDurability() << ")";
        else cout << " (无限耐久)";
        cout << "\n";
    }
    
    if (equippedArmor != nullptr) {
        cout << "防具: " << equippedArmor->getName();
        if (equippedArmor->isBroken()) cout << " (已损坏)";
        else if (!equippedArmor->isInfinite()) 
            cout << " (耐久度: " << equippedArmor->getDurability() << "/" << equippedArmor->getMaxDurability() << ")";
        else cout << " (无限耐久)";
        cout << "\n";
    }
    
    cout << "==================\n";
}

void Character::reduceWeaponDurability() {
    if (equippedWeapon != nullptr) {
        equippedWeapon->reduceDurability();
        if (equippedWeapon->isBroken()) {
            cout << getName() << " 的 " << equippedWeapon->getName() << " 损坏了！\n";
            addItem(equippedWeapon);
            equippedWeapon = nullptr;
        }
    }
}

void Character::reduceArmorDurability() {
    if (equippedArmor != nullptr) {
        equippedArmor->reduceDurability();
        if (equippedArmor->isBroken()) {
            cout << getName() << " 的 " << equippedArmor->getName() << " 损坏了！\n";
            addItem(equippedArmor);
            equippedArmor = nullptr;
        }
    }
}

bool Character::removeItem(Item* item) {
    auto it = find(inventory.begin(), inventory.end(), item);
    if (it != inventory.end()) {
        inventory.erase(it);
        return true;
    }
    return false;
}

void Character::clearInventory() {
    for (auto item : inventory) delete item;
    inventory.clear();
}

string Character::getSaveString() const {
    stringstream ss;
    ss << name << "|" << level << "|" << health << "|" << maxHealth << "|"
       << attack << "|" << defense << "|" << gold << "|" << experience;
    return ss.str();
}

// ==================== 玩家类实现 ====================
Player::Player(const string& n) : Character(n), locationX(4), locationY(4) {}

void Player::move(Direction direction) {
    switch (direction) {
        case Direction::NORTH: locationY--; break;
        case Direction::SOUTH: locationY++; break;
        case Direction::EAST: locationX++; break;
        case Direction::WEST: locationX--; break;
    }
    
    if (locationX < 0) locationX = 0;
    if (locationX >= MAP_WIDTH) locationX = MAP_WIDTH - 1;
    if (locationY < 0) locationY = 0;
    if (locationY >= MAP_HEIGHT) locationY = MAP_HEIGHT - 1;
}

void Player::teleport(int x, int y) {
    locationX = x;
    locationY = y;
}

void Player::attackEnemy(Character& enemy) {
    int totalAttack = attack;
    if (equippedWeapon != nullptr && !equippedWeapon->isBroken()) {
        totalAttack += equippedWeapon->getDamage();
    }
    
    cout << getName() << " 攻击了 " << enemy.getName() << "，造成了 " << totalAttack << " 点伤害！\n";
    enemy.takeDamage(totalAttack);
    
    if (equippedWeapon != nullptr) reduceWeaponDurability();
}

void Player::displayStats() const {
    Character::displayStats();
    
    if (equippedWeapon != nullptr && !equippedWeapon->isBroken())
        cout << "装备武器: " << equippedWeapon->getName() << " (" << equippedWeapon->getStats() << ")\n";
    else if (equippedWeapon != nullptr && equippedWeapon->isBroken())
        cout << "损坏武器: " << equippedWeapon->getName() << " (已损坏)\n";
    
    if (equippedArmor != nullptr && !equippedArmor->isBroken())
        cout << "装备防具: " << equippedArmor->getName() << " (" << equippedArmor->getStats() << ")\n";
    else if (equippedArmor != nullptr && equippedArmor->isBroken())
        cout << "损坏防具: " << equippedArmor->getName() << " (已损坏)\n";
    
    cout << "==================\n";
}

void Player::displayInventory() const {
    cout << "=== 背包 ===\n";
    
    if (inventory.empty()) {
        cout << "你的背包是空的。\n";
    } else {
        for (size_t i = 0; i < inventory.size(); ++i) {
            cout << (i + 1) << ". " << inventory[i]->getName() << " - " 
                 << inventory[i]->getStats() << " (价值: " 
                 << inventory[i]->getValue() << " 枚金币)\n";
        }
    }
    cout << "==================\n";
}

void Player::displayEquipment() const {
    cout << "=== 当前装备 ===\n";
    
    if (equippedWeapon == nullptr) cout << "武器: 空手\n";
    else {
        cout << "武器: " << equippedWeapon->getName();
        if (equippedWeapon->isBroken()) cout << " (已损坏)";
        else if (equippedWeapon->isInfinite()) cout << " (无限耐久)";
        else cout << " (耐久度: " << equippedWeapon->getDurability() << "/" << equippedWeapon->getMaxDurability() << ")";
        cout << "\n";
    }
    
    if (equippedArmor == nullptr) cout << "防具: 无\n";
    else {
        cout << "防具: " << equippedArmor->getName();
        if (equippedArmor->isBroken()) cout << " (已损坏)";
        else if (equippedArmor->isInfinite()) cout << " (无限耐久)";
        else cout << " (耐久度: " << equippedArmor->getDurability() << "/" << equippedArmor->getMaxDurability() << ")";
        cout << "\n";
    }
    cout << "==================\n";
}

void Player::useItem(int index) {
    if (index >= 0 && static_cast<size_t>(index) < inventory.size()) {
        Item* item = inventory[index];
        
        if (item->getType() == ItemType::WEAPON) {
            Weapon* weapon = dynamic_cast<Weapon*>(item);
            if (weapon != nullptr && weapon->isBroken()) {
                cout << weapon->getName() << " 已损坏，无法装备！\n";
                return;
            }
        } else if (item->getType() == ItemType::ARMOR) {
            Armor* armor = dynamic_cast<Armor*>(item);
            if (armor != nullptr && armor->isBroken()) {
                cout << armor->getName() << " 已损坏，无法装备！\n";
                return;
            }
        }
        
        item->use(*this);
        
        if (item->getType() == ItemType::CONSUMABLE) {
            removeItem(item);
            delete item;
        }
    } else {
        cout << "无效的物品编号。\n";
    }
}

void Player::unequipWeapon() {
    if (equippedWeapon != nullptr) {
        if (inventory.size() >= 30) {
            cout << "背包已满，无法卸下装备！\n";
            return;
        }
        cout << "你卸下了 " << equippedWeapon->getName() << "！\n";
        addItem(equippedWeapon);
        equippedWeapon = nullptr;
    } else {
        cout << "你没有装备武器。\n";
    }
}

void Player::unequipArmor() {
    if (equippedArmor != nullptr) {
        if (inventory.size() >= 30) {
            cout << "背包已满，无法卸下装备！\n";
            return;
        }
        cout << "你卸下了 " << equippedArmor->getName() << "！\n";
        addItem(equippedArmor);
        equippedArmor = nullptr;
    } else {
        cout << "你没有装备防具。\n";
    }
}

bool Player::sellItem(int index) {
    if (index >= 0 && static_cast<size_t>(index) < inventory.size()) {
        Item* item = inventory[index];
        
        if (item == equippedWeapon || item == equippedArmor) {
            cout << "不能出售已装备的物品！\n";
            return false;
        }
        
        if (item->getType() == ItemType::QUEST || item->getType() == ItemType::TELEPORT) {
            cout << "不能出售任务物品和传送卷轴！\n";
            return false;
        }
        
        int value = item->getValue();
        if (item->getType() == ItemType::WEAPON) {
            Weapon* weapon = dynamic_cast<Weapon*>(item);
            if (weapon != nullptr && weapon->isBroken()) {
                value = max(1, value / 2);
                cout << "（损坏的物品价值减半）\n";
            }
        } else if (item->getType() == ItemType::ARMOR) {
            Armor* armor = dynamic_cast<Armor*>(item);
            if (armor != nullptr && armor->isBroken()) {
                value = max(1, value / 2);
                cout << "（损坏的物品价值减半）\n";
            }
        }
        
        addGold(value);
        cout << "你卖掉了 " << item->getName() << "，获得了 " << value << " 枚金币！\n";
        
        removeItem(item);
        delete item;
        return true;
    } else {
        cout << "无效的物品编号。\n";
        return false;
    }
}

bool Player::hasCompletedQuest(const string& quest) const {
    return find(completedQuests.begin(), completedQuests.end(), quest) != completedQuests.end();
}

string Player::getPlayerSaveString() const {
    stringstream ss;
    ss << Character::getSaveString() << "|" << locationX << "|" << locationY;
    return ss.str();
}

// ==================== 敌人类实现 ====================
Enemy::Enemy(const string& n, EnemyType type, int level) : Character(n), enemyType(type), canSplit(false), splitCount(0) {
    switch (type) {
        case EnemyType::GOBLIN:
            health = 30 + (level * 10); maxHealth = health; attack = 5 + (level * 2); defense = 1 + (level * 1); gold = 5 + (rand() % 10); break;
        case EnemyType::ORC:
            health = 50 + (level * 15); maxHealth = health; attack = 10 + (level * 3); defense = 3 + (level * 2); gold = 10 + (rand() % 20); break;
        case EnemyType::TROLL:
            health = 80 + (level * 20); maxHealth = health; attack = 15 + (level * 4); defense = 5 + (level * 3); gold = 20 + (rand() % 30); break;
        case EnemyType::DRAGON:
            health = 150 + (level * 30); maxHealth = health; attack = 25 + (level * 5); defense = 10 + (level * 4); gold = 50 + (rand() % 50); break;
        case EnemyType::SLIME:
            health = 40 + (level * 8); maxHealth = health; attack = 3 + (level * 1); defense = 0; gold = 2 + (rand() % 8);
            canSplit = (rand() % 100 < 30); splitCount = canSplit ? 2 : 0; break;
        case EnemyType::SPLIT_SLIME:
            health = 20 + (level * 4); maxHealth = health; attack = 2 + (level * 1); defense = 0; gold = 1 + (rand() % 4);
            canSplit = false; splitCount = 0; break;
        case EnemyType::BAT:
            health = 25 + (level * 6); maxHealth = health; attack = 6 + (level * 2); defense = 1 + (level * 1); gold = 3 + (rand() % 6); break;
        case EnemyType::RAT:
            health = 35 + (level * 9); maxHealth = health; attack = 4 + (level * 1); defense = 2 + (level * 1); gold = 4 + (rand() % 8); break;
        case EnemyType::WISP:
            health = 20 + (level * 5); maxHealth = health; attack = 8 + (level * 2); defense = 0; gold = 5 + (rand() % 10); break;
        case EnemyType::FOREST_SPIRIT:
            health = 60 + (level * 12); maxHealth = health; attack = 7 + (level * 2); defense = 4 + (level * 2); gold = 8 + (rand() % 12); break;
        case EnemyType::BANDIT:
            health = 45 + (level * 10); maxHealth = health; attack = 9 + (level * 2); defense = 3 + (level * 1); gold = 12 + (rand() % 20); break;
        case EnemyType::LICH_KING:
            health = 500 + (level * 100); maxHealth = health; attack = 50 + (level * 10); defense = 30 + (level * 8); gold = 1000 + (rand() % 500); break;
        case EnemyType::SKELETON:
            health = 40 + (level * 9); maxHealth = health; attack = 8 + (level * 2); defense = 2 + (level * 1); gold = 6 + (rand() % 12); break;
        case EnemyType::GHOST:
            health = 30 + (level * 7); maxHealth = health; attack = 10 + (level * 3); defense = 1; gold = 8 + (rand() % 15); break;
        case EnemyType::ZOMBIE:
            health = 70 + (level * 12); maxHealth = health; attack = 6 + (level * 1); defense = 4 + (level * 2); gold = 7 + (rand() % 10); break;
        case EnemyType::SPIDER:
            health = 35 + (level * 7); maxHealth = health; attack = 9 + (level * 2); defense = 2 + (level * 1); gold = 5 + (rand() % 8); break;
        case EnemyType::WOLF:
            health = 45 + (level * 10); maxHealth = health; attack = 11 + (level * 2); defense = 3 + (level * 1); gold = 9 + (rand() % 14); break;
    }
}

void Enemy::attackEnemy(Character& enemy) {
    string attackMessage;
    switch (enemyType) {
        case EnemyType::GOBLIN: attackMessage = " 用生锈的匕首刺向你！"; break;
        case EnemyType::ORC: attackMessage = " 挥舞着粗糙的斧头砍向你！"; break;
        case EnemyType::TROLL: attackMessage = " 用巨大的拳头砸向你！"; break;
        case EnemyType::DRAGON: attackMessage = " 喷出火焰灼烧你！"; break;
        case EnemyType::SLIME: case EnemyType::SPLIT_SLIME: attackMessage = " 用胶状的身体撞击你！"; break;
        case EnemyType::BAT: attackMessage = " 俯冲下来用尖牙咬你！"; break;
        case EnemyType::RAT: attackMessage = " 用锋利的牙齿咬向你！"; break;
        case EnemyType::WISP: attackMessage = " 发射一道蓝色光芒！"; break;
        case EnemyType::FOREST_SPIRIT: attackMessage = " 召唤藤蔓缠绕你！"; break;
        case EnemyType::BANDIT: attackMessage = " 用短剑猛刺过来！"; break;
        case EnemyType::LICH_KING:
            if (rand() % 100 < 50) attackMessage = " 释放死亡之握，将你的灵魂拖向深渊！";
            else attackMessage = " 挥舞霜之哀伤，释放出冰霜之息！";
            break;
        case EnemyType::SKELETON: attackMessage = " 用骨剑劈向你！"; break;
        case EnemyType::GHOST: attackMessage = " 穿过你的身体，吸取你的生命！"; break;
        case EnemyType::ZOMBIE: attackMessage = " 用腐烂的手臂抓向你！"; break;
        case EnemyType::SPIDER: attackMessage = " 吐出毒丝并咬向你！"; break;
        case EnemyType::WOLF: attackMessage = " 猛扑过来用利爪撕咬！"; break;
    }
    
    cout << getName() << attackMessage << "\n";
    enemy.takeDamage(attack);
}

void Enemy::displayStats() const {
    cout << "=== " << name << " ===\n";
    cout << "等级: " << level << "\n";
    cout << "生命值: " << health << "/" << maxHealth << "\n";
    cout << "攻击力: " << attack << "\n";
    cout << "防御力: " << defense << "\n";
    if (enemyType == EnemyType::LICH_KING) cout << "类型: 最终BOSS\n";
    if (canSplit && enemyType == EnemyType::SLIME) cout << "特性: 可以分裂\n";
    cout << "==================\n";
}

Enemy* Enemy::split() {
    if (!canSplit || splitCount <= 0 || health <= 0) return nullptr;
    splitCount--;
    
    string smallSlimeName;
    if (name.find("冰霜") != string::npos) smallSlimeName = "小型冰霜史莱姆";
    else if (name.find("熔岩") != string::npos) smallSlimeName = "小型熔岩史莱姆";
    else if (name.find("圣光") != string::npos) smallSlimeName = "小型圣光史莱姆";
    else if (name.find("毒液") != string::npos) smallSlimeName = "小型毒液史莱姆";
    else smallSlimeName = "小型史莱姆";
    
    return new Enemy(smallSlimeName, EnemyType::SPLIT_SLIME, level);
}

// ==================== 商店类实现 ====================
Shop::Shop() { populateShop(); }

Shop::~Shop() {
    for (auto item : itemsForSale) delete item;
}

void Shop::populateShop() {
    for (auto item : itemsForSale) delete item;
    itemsForSale.clear();
    
    itemsForSale.push_back(new Weapon("铁剑", "一把锋利的铁剑。", 50, 10, 100));
    itemsForSale.push_back(new Weapon("钢斧", "一把沉重的钢斧。", 75, 15, 90));
    itemsForSale.push_back(new Weapon("匕首", "一把小巧但锋利的匕首。", 20, 5, 120));
    itemsForSale.push_back(new Weapon("长弓", "一把坚固的木弓。", 60, 12, 80));
    itemsForSale.push_back(new Weapon("秘银长剑", "用秘银锻造的长剑。", 150, 25, 150));
    itemsForSale.push_back(new Weapon("无尽之剑", "传说中的神器，永不磨损。", 1000, 50, 0, true));
    
    itemsForSale.push_back(new Armor("皮甲", "基础的皮制护甲。", 30, 5, 80));
    itemsForSale.push_back(new Armor("锁子甲", "金属链制成的护甲。", 60, 10, 120));
    itemsForSale.push_back(new Armor("板甲", "厚重的金属板甲。", 100, 15, 150));
    itemsForSale.push_back(new Armor("秘银甲", "用秘银打造的护甲。", 200, 25, 200));
    itemsForSale.push_back(new Armor("菲拉尔战甲", "散发着神秘光芒的战甲。", 800, 35, 0, true));
    
    itemsForSale.push_back(new Consumable("小型生命药剂", "恢复20点生命值。", 10, 20));
    itemsForSale.push_back(new Consumable("中型生命药剂", "恢复50点生命值。", 30, 50));
    itemsForSale.push_back(new Consumable("大型生命药剂", "恢复100点生命值。", 50, 100));
    itemsForSale.push_back(new Consumable("超级生命药剂", "恢复200点生命值。", 100, 200));
    itemsForSale.push_back(new Consumable("神圣药水", "恢复300点生命值。", 200, 300));
    
    itemsForSale.push_back(new TeleportScroll("新手村传送卷轴", "可以传送到新手村。", 30, TeleportType::NEWBIE_VILLAGE));
    itemsForSale.push_back(new TeleportScroll("极炎之地传送卷轴", "可以传送到极炎之地。", 50, TeleportType::FIRE_LAND));
    itemsForSale.push_back(new TeleportScroll("哥布林王国传送卷轴", "可以传送到哥布林王国。", 50, TeleportType::GOBLIN_KINGDOM));
    itemsForSale.push_back(new TeleportScroll("无尽寒夜传送卷轴", "可以传送到无尽寒夜。", 50, TeleportType::FROZEN_NIGHT));
    itemsForSale.push_back(new TeleportScroll("天璇圣地传送卷轴", "可以传送到天璇圣地。", 50, TeleportType::CELESTIAL_SITE));
    
    itemsForSale.push_back(new Consumable("修理锤", "可以修理装备的耐久度。", 80, 0));
}

void Shop::displayShop() const {
    cout << "=== 商人的商店 ===\n";
    cout << "欢迎光临！你想买点什么？\n";
    
    for (size_t i = 0; i < itemsForSale.size(); ++i) {
        cout << (i + 1) << ". " << itemsForSale[i]->getName() << " - " 
             << itemsForSale[i]->getStats() << " (" 
             << itemsForSale[i]->getValue() << " 枚金币)\n";
    }
    
    cout << "0. 离开商店\n";
    cout << "========================\n";
}

void Shop::buyItem(int index, Player& player) {
    if (index >= 0 && static_cast<size_t>(index) < itemsForSale.size()) {
        Item* item = itemsForSale[index];
        int cost = item->getValue();
        
        if (player.getGold() >= cost) {
            if (player.getInventory().size() >= 30) {
                cout << "背包已满！最多只能携带30个物品。\n";
                return;
            }
            
            Item* newItem = nullptr;
            if (item->getType() == ItemType::WEAPON) {
                Weapon* weapon = dynamic_cast<Weapon*>(item);
                newItem = new Weapon(weapon->getName(), weapon->getDescription(), 
                                   weapon->getValue(), weapon->getDamage(),
                                   weapon->getMaxDurability(), weapon->isInfinite());
            } else if (item->getType() == ItemType::ARMOR) {
                Armor* armor = dynamic_cast<Armor*>(item);
                newItem = new Armor(armor->getName(), armor->getDescription(), 
                                 armor->getValue(), armor->getDefense(),
                                 armor->getMaxDurability(), armor->isInfinite());
            } else if (item->getType() == ItemType::CONSUMABLE) {
                Consumable* consumable = dynamic_cast<Consumable*>(item);
                newItem = new Consumable(consumable->getName(), consumable->getDescription(), 
                                     consumable->getValue(), consumable->getHealthRestore());
            } else if (item->getType() == ItemType::TELEPORT) {
                TeleportScroll* scroll = dynamic_cast<TeleportScroll*>(item);
                newItem = new TeleportScroll(scroll->getName(), scroll->getDescription(), 
                                         scroll->getValue(), scroll->getTeleportType());
            }
            
            if (newItem != nullptr) {
                player.addItem(newItem);
                player.setGold(player.getGold() - cost);
                cout << "你花 " << cost << " 枚金币买下了 " << item->getName() << "！\n";
                if (newItem->getName() == "修理锤")
                    cout << "修理锤可以用于修理损坏的装备（在背包中使用）。\n";
            }
        } else {
            cout << "你的金币不够！\n";
        }
    } else {
        cout << "无效的物品编号。\n";
    }
}

// ==================== 游戏世界类 ====================
class GameWorld {
private:
    Player player;
    Enemy* currentEnemy;
    GameState gameState;
    Shop shop;
    vector<Item*> worldItems;
    vector<Enemy*> activeEnemies;
    
    void giveStartingItems() {
        Weapon* startingSword = new Weapon("木剑", "一把基础的木剑。", 10, 5, 50);
        Consumable* startingPotion = new Consumable("小型生命药剂", "恢复20点生命值。", 10, 20);
        TeleportScroll* startingScroll = new TeleportScroll("新手村传送卷轴", "可以传送到新手村。", 30, TeleportType::NEWBIE_VILLAGE);
        
        player.addItem(startingSword);
        player.addItem(startingPotion);
        player.addItem(startingScroll);
        
        player.removeItem(startingSword);
        player.setEquippedWeapon(startingSword);
        
        cout << player.getName() << " 获得了木剑！\n";
        cout << player.getName() << " 获得了小型生命药剂！\n";
        cout << player.getName() << " 获得了新手村传送卷轴！\n";
        
        player.addGold(200);
        shop.populateShop();
        cout << "商人准备好了货物！\n";
    }
    
    void displayMap() const {
        cout << "\n=== 无尽之地地图 (10x10) ===\n";
        cout << "地图说明:\n";
        cout << "N - 玩家当前位置\n";
        cout << "S - 安全区（新手村）\n";
        cout << "X - 巫妖王洞穴\n";
        cout << "T - 传送点\n\n";
        
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                if (x == player.getLocationX() && y == player.getLocationY()) cout << " N ";
                else if (x == 4 && y == 4) cout << " S ";
                else if (x == 4 && y == 0) cout << " X ";
                else if ((x == 4 && y == 7) || (x == 4 && y == 2) || 
                         (x == 2 && y == 4) || (x == 7 && y == 4)) cout << " T ";
                else if (y >= 7) cout << " ~ ";
                else if (y <= 2) cout << " G ";
                else if (x <= 2) cout << " * ";
                else if (x >= 7) cout << " ^ ";
                else cout << " . ";
            }
            cout << " | ";
            
            switch (y) {
                case 0: cout << (player.getLocationY() == 0 ? "巫妖王洞穴入口" : "哥布林王国北部"); break;
                case 1: case 2:
                    cout << (player.getLocationY() == y ? getLocationName(player.getLocationX(), y) : "哥布林王国"); break;
                case 3: case 4: case 5:
                    if (player.getLocationY() == y) cout << getLocationName(player.getLocationX(), y);
                    else if (y == 4) cout << "新手村中心";
                    else cout << "新手村周边"; break;
                case 6: cout << (player.getLocationY() == 6 ? getLocationName(player.getLocationX(), y) : "交界地带"); break;
                case 7: case 8: case 9:
                    cout << (player.getLocationY() == y ? getLocationName(player.getLocationX(), y) : "极炎之地"); break;
            }
            cout << endl;
        }
        
        cout << "\n区域说明:\n";
        cout << "新手村(S): 安全区域，可在此休息和交易\n";
        cout << "哥布林王国(G): 哥布林出没，适合低级冒险者\n";
        cout << "极炎之地(~): 炎热地带，火龙和火元素栖息地\n";
        cout << "无尽寒夜(*): 冰雪覆盖，冰霜怪物出没\n";
        cout << "天璇圣地(^): 古代遗迹，神秘力量所在地\n";
        cout << "巫妖王洞穴(X): 最终BOSS所在地，极度危险！\n";
        cout << "传送点(T): 可以使用传送卷轴快速到达\n";
        cout << "===============================\n";
    }
    
    void checkRandomEvent() {
        if (isInSafeZone(player.getLocationX(), player.getLocationY())) return;
        
        int event = rand() % 100;
        
        if (event < 25) spawnEnemy();
        else if (event < 35) findItem();
        else if (event < 40) findGold();
        else if (event < 45) triggerQuest();
    }
    
    void spawnEnemy();
    void findItem();
    void findGold();
    void triggerQuest();
    void startCombat();
    bool attemptEscape();
    void openInventory();
    void enterShop();
    void openTeleportMenu();
    void openEquipmentMenu();
    void handleSlimeSplit(Enemy* originalSlime);
    void waitForKeyPress() const {
        cout << "\n按回车键继续...";
        cin.ignore();
        cin.get();
    }
    
public:
    GameWorld(const string& playerName = "冒险者")
        : player(playerName), currentEnemy(nullptr), gameState(GameState::MENU) {
        giveStartingItems();
    }
    
    ~GameWorld() {
        if (currentEnemy) delete currentEnemy;
        for (auto item : worldItems) delete item;
        for (auto enemy : activeEnemies) delete enemy;
    }
    
    void run();
    void displayMainMenu() const;
    void handleMenuInput(int choice);
    void startGame();
    void displayHowToPlay() const;
    void displayAnnouncement() const;
    void displayGameMenu() const;
    void handleGameInput(const string& input);
    void displayLoadMenu();
    void handleLoadInput(int choice);
    bool saveGame(const string& filename = "savegame.txt") {
        return ::saveGame(player, filename);
    }
    bool loadGame(const string& filename = "savegame.txt") {
        return ::loadGame(player, filename);
    }
};

// ==================== GameWorld方法实现 ====================
void GameWorld::spawnEnemy() {
    EnemyType enemyType;
    int playerLevel = player.getLevel();
    int x = player.getLocationX();
    int y = player.getLocationY();
    
    if (x == 4 && y <= 1 && rand() % 100 < 50) {
        enemyType = EnemyType::LICH_KING;
    } 
    else if (y <= 2) {
        int randNum = rand() % 100;
        if (playerLevel <= 3) {
            if (randNum < 40) enemyType = EnemyType::GOBLIN;
            else if (randNum < 70) enemyType = EnemyType::RAT;
            else if (randNum < 85) enemyType = EnemyType::BAT;
            else if (randNum < 95) enemyType = EnemyType::SKELETON;
            else enemyType = EnemyType::ZOMBIE;
        }
        else if (playerLevel <= 7) {
            if (randNum < 25) enemyType = EnemyType::GOBLIN;
            else if (randNum < 45) enemyType = EnemyType::ORC;
            else if (randNum < 60) enemyType = EnemyType::TROLL;
            else if (randNum < 70) enemyType = EnemyType::ZOMBIE;
            else if (randNum < 80) enemyType = EnemyType::SKELETON;
            else if (randNum < 90) enemyType = EnemyType::WOLF;
            else enemyType = EnemyType::BANDIT;
        }
        else {
            if (randNum < 10) enemyType = EnemyType::ORC;
            else if (randNum < 25) enemyType = EnemyType::TROLL;
            else if (randNum < 40) enemyType = EnemyType::SKELETON;
            else if (randNum < 55) enemyType = EnemyType::ZOMBIE;
            else if (randNum < 70) enemyType = EnemyType::GHOST;
            else if (randNum < 85) enemyType = EnemyType::WOLF;
            else enemyType = EnemyType::BANDIT;
        }
    } 
    else if (y >= 7) {
        int randNum = rand() % 100;
        if (playerLevel <= 3) {
            if (randNum < 30) enemyType = EnemyType::SLIME;
            else if (randNum < 55) enemyType = EnemyType::BAT;
            else if (randNum < 75) enemyType = EnemyType::RAT;
            else if (randNum < 90) enemyType = EnemyType::GOBLIN;
            else enemyType = EnemyType::SPIDER;
        }
        else if (playerLevel <= 7) {
            if (randNum < 15) enemyType = EnemyType::DRAGON;
            else if (randNum < 35) enemyType = EnemyType::TROLL;
            else if (randNum < 50) enemyType = EnemyType::ORC;
            else if (randNum < 65) enemyType = EnemyType::BAT;
            else if (randNum < 75) enemyType = EnemyType::SPIDER;
            else if (randNum < 85) enemyType = EnemyType::SLIME;
            else if (randNum < 95) enemyType = EnemyType::WOLF;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
        else {
            if (randNum < 20) enemyType = EnemyType::DRAGON;
            else if (randNum < 35) enemyType = EnemyType::TROLL;
            else if (randNum < 50) enemyType = EnemyType::ORC;
            else if (randNum < 65) enemyType = EnemyType::SPIDER;
            else if (randNum < 75) enemyType = EnemyType::WOLF;
            else if (randNum < 85) enemyType = EnemyType::SLIME;
            else if (randNum < 95) enemyType = EnemyType::BANDIT;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
    } 
    else if (x <= 2) {
        int randNum = rand() % 100;
        if (playerLevel <= 3) {
            if (randNum < 30) enemyType = EnemyType::SLIME;
            else if (randNum < 55) enemyType = EnemyType::WISP;
            else if (randNum < 75) enemyType = EnemyType::BAT;
            else if (randNum < 90) enemyType = EnemyType::RAT;
            else enemyType = EnemyType::GOBLIN;
        }
        else if (playerLevel <= 7) {
            if (randNum < 15) enemyType = EnemyType::WISP;
            else if (randNum < 35) enemyType = EnemyType::GHOST;
            else if (randNum < 55) enemyType = EnemyType::SLIME;
            else if (randNum < 65) enemyType = EnemyType::FOREST_SPIRIT;
            else if (randNum < 75) enemyType = EnemyType::ZOMBIE;
            else if (randNum < 85) enemyType = EnemyType::BAT;
            else if (randNum < 95) enemyType = EnemyType::WOLF;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
        else {
            if (randNum < 20) enemyType = EnemyType::WISP;
            else if (randNum < 35) enemyType = EnemyType::GHOST;
            else if (randNum < 50) enemyType = EnemyType::FOREST_SPIRIT;
            else if (randNum < 65) enemyType = EnemyType::SLIME;
            else if (randNum < 75) enemyType = EnemyType::ZOMBIE;
            else if (randNum < 85) enemyType = EnemyType::WOLF;
            else if (randNum < 95) enemyType = EnemyType::SPIDER;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
    } 
    else if (x >= 7) {
        int randNum = rand() % 100;
        if (playerLevel <= 3) {
            if (randNum < 30) enemyType = EnemyType::WISP;
            else if (randNum < 55) enemyType = EnemyType::FOREST_SPIRIT;
            else if (randNum < 75) enemyType = EnemyType::SLIME;
            else if (randNum < 90) enemyType = EnemyType::BAT;
            else enemyType = EnemyType::RAT;
        }
        else if (playerLevel <= 7) {
            if (randNum < 15) enemyType = EnemyType::FOREST_SPIRIT;
            else if (randNum < 35) enemyType = EnemyType::WISP;
            else if (randNum < 50) enemyType = EnemyType::DRAGON;
            else if (randNum < 65) enemyType = EnemyType::BANDIT;
            else if (randNum < 75) enemyType = EnemyType::GHOST;
            else if (randNum < 85) enemyType = EnemyType::WOLF;
            else if (randNum < 95) enemyType = EnemyType::SPIDER;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
        else {
            if (randNum < 20) enemyType = EnemyType::FOREST_SPIRIT;
            else if (randNum < 35) enemyType = EnemyType::DRAGON;
            else if (randNum < 50) enemyType = EnemyType::WISP;
            else if (randNum < 65) enemyType = EnemyType::BANDIT;
            else if (randNum < 75) enemyType = EnemyType::GHOST;
            else if (randNum < 85) enemyType = EnemyType::WOLF;
            else if (randNum < 95) enemyType = EnemyType::SPIDER;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
    } 
    else {
        int randNum = rand() % 100;
        if (playerLevel <= 3) {
            if (randNum < 20) enemyType = EnemyType::RAT;
            else if (randNum < 45) enemyType = EnemyType::SLIME;
            else if (randNum < 65) enemyType = EnemyType::BAT;
            else if (randNum < 80) enemyType = EnemyType::GOBLIN;
            else if (randNum < 90) enemyType = EnemyType::WISP;
            else if (randNum < 97) enemyType = EnemyType::BANDIT;
            else if (randNum < 99) enemyType = EnemyType::WOLF;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
        else if (playerLevel <= 7) {
            if (randNum < 15) enemyType = EnemyType::GOBLIN;
            else if (randNum < 35) enemyType = EnemyType::ORC;
            else if (randNum < 50) enemyType = EnemyType::BANDIT;
            else if (randNum < 65) enemyType = EnemyType::FOREST_SPIRIT;
            else if (randNum < 75) enemyType = EnemyType::TROLL;
            else if (randNum < 85) enemyType = EnemyType::DRAGON;
            else if (randNum < 93) enemyType = EnemyType::WISP;
            else if (randNum < 99) enemyType = EnemyType::SPIDER;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
        else {
            if (randNum < 10) enemyType = EnemyType::ORC;
            else if (randNum < 25) enemyType = EnemyType::TROLL;
            else if (randNum < 40) enemyType = EnemyType::DRAGON;
            else if (randNum < 55) enemyType = EnemyType::FOREST_SPIRIT;
            else if (randNum < 65) enemyType = EnemyType::BANDIT;
            else if (randNum < 75) enemyType = EnemyType::SKELETON;
            else if (randNum < 85) enemyType = EnemyType::GHOST;
            else if (randNum < 93) enemyType = EnemyType::ZOMBIE;
            else if (randNum < 99) enemyType = EnemyType::SPIDER;
            else enemyType = EnemyType::SPLIT_SLIME;
        }
    }
    
    int enemyLevel = player.getLevel();
    if (y >= 7) enemyLevel += 2;
    else if (x >= 7) enemyLevel += 1;
    else if (x <= 2) enemyLevel += 1;
    else if (y <= 2 && y > 0) enemyLevel += 1;
    
    switch (enemyType) {
        case EnemyType::LICH_KING: enemyLevel = max(player.getLevel() + 5, 10); break;
        case EnemyType::DRAGON: enemyLevel += 3; break;
        case EnemyType::TROLL: case EnemyType::FOREST_SPIRIT: enemyLevel += 2; break;
        case EnemyType::ORC: case EnemyType::BANDIT: case EnemyType::GHOST: enemyLevel += 1; break;
        case EnemyType::SLIME: case EnemyType::BAT: case EnemyType::RAT: case EnemyType::SPLIT_SLIME:
            enemyLevel = max(enemyLevel - 1, 1); break;
    }
    
    enemyLevel += (rand() % 3) - 1;
    if (enemyLevel < 1) enemyLevel = 1;
    if (enemyLevel > player.getLevel() + 5) enemyLevel = player.getLevel() + 5;
    
    string enemyName;
    string titlePrefix = "";
    string elementSuffix = "";
    
    switch (enemyType) {
        case EnemyType::SLIME:
            enemyName = "史莱姆";
            if (x <= 2) elementSuffix = "（冰霜）";
            else if (y >= 7) elementSuffix = "（熔岩）";
            else if (x >= 7) elementSuffix = "（圣光）";
            else if (rand() % 100 < 20) elementSuffix = "（毒液）";
            break;
        case EnemyType::SPLIT_SLIME:
            enemyName = "分裂史莱姆";
            if (x <= 2) elementSuffix = "（冰霜）";
            else if (y >= 7) elementSuffix = "（熔岩）";
            else if (x >= 7) elementSuffix = "（圣光）";
            else if (rand() % 100 < 20) elementSuffix = "（毒液）";
            break;
        case EnemyType::BAT:
            enemyName = (x <= 2) ? "冰霜蝙蝠" : (y >= 7) ? "火焰蝙蝠" : (x >= 7) ? "圣光蝙蝠" : "吸血蝙蝠";
            break;
        case EnemyType::RAT:
            enemyName = (rand() % 100 < 20) ? "瘟疫鼠" : (rand() % 100 < 30) ? "变异巨鼠" : "巨鼠";
            break;
        case EnemyType::WISP:
            enemyName = (x >= 7) ? "圣光之灵" : "鬼火";
            break;
        case EnemyType::FOREST_SPIRIT:
            enemyName = (x >= 7) ? "天璇守卫" : "森林精灵";
            break;
        case EnemyType::BANDIT:
            enemyName = (rand() % 100 < 15) ? "盗贼" : "强盗";
            break;
        case EnemyType::GOBLIN:
            enemyName = (rand() % 100 < 15) ? "哥布林战士" : "哥布林";
            break;
        case EnemyType::ORC:
            enemyName = (rand() % 100 < 20) ? "兽人战士" : "兽人";
            break;
        case EnemyType::TROLL:
            enemyName = (rand() % 100 < 20) ? "巨魔猎人" : "巨魔";
            break;
        case EnemyType::DRAGON:
            if (y >= 7) enemyName = "火龙";
            else if (x >= 7) enemyName = "圣龙";
            else if (x <= 2) enemyName = "冰龙";
            else enemyName = "幼龙";
            break;
        case EnemyType::LICH_KING:
            enemyName = "巫妖王莫德雷德";
            break;
        case EnemyType::SKELETON:
            enemyName = (rand() % 100 < 30) ? "骷髅战士" : "骷髅兵";
            break;
        case EnemyType::GHOST:
            enemyName = (rand() % 100 < 30) ? "怨灵" : "幽灵";
            break;
        case EnemyType::ZOMBIE:
            enemyName = (rand() % 100 < 25) ? "腐尸" : "僵尸";
            break;
        case EnemyType::SPIDER:
            enemyName = (rand() % 100 < 25) ? "毒蜘蛛" : "蜘蛛";
            break;
        case EnemyType::WOLF:
            enemyName = (rand() % 100 < 25) ? "森林狼" : "狼";
            break;
    }
    
    enemyName += elementSuffix;
    
    if (enemyType == EnemyType::GOBLIN && y <= 2 && y > 0 && 
        !player.hasCompletedQuest("哥布林国王") && rand() % 100 < 5) {
        enemyName = "哥布林国王格鲁尔";
        enemyLevel = max(player.getLevel() + 3, 8);
    }
    
    if (enemyType == EnemyType::DRAGON && y >= 7 && rand() % 100 < 3) {
        enemyName = "炎龙王巴尔扎克";
        enemyLevel = max(player.getLevel() + 4, 12);
    }
    
    if (enemyType == EnemyType::GHOST && x <= 2 && rand() % 100 < 3) {
        enemyName = "亡灵统帅萨拉查";
        enemyLevel = max(player.getLevel() + 3, 10);
    }
    
    int levelDiff = enemyLevel - player.getLevel();
    if (levelDiff >= 5) titlePrefix = "恐怖的";
    else if (levelDiff >= 3) titlePrefix = "强大的";
    else if (levelDiff >= 1) titlePrefix = "强壮的";
    else if (levelDiff <= -3) titlePrefix = "虚弱的";
    else if (levelDiff <= -1) titlePrefix = "受伤的";
    
    if (rand() % 100 < 15 && titlePrefix.empty()) {
        const vector<string> titles = {"迅捷的", "坚硬的", "狂暴的", "狡诈的", "古老的"};
        titlePrefix = titles[rand() % titles.size()];
    }
    
    if (rand() % 100 < 5 && enemyType != EnemyType::LICH_KING) {
        if (titlePrefix.empty()) titlePrefix = "精英";
        else titlePrefix = "精英" + titlePrefix;
        enemyLevel += 2;
    }
    
    if (!titlePrefix.empty()) enemyName = titlePrefix + enemyName;
    
    if (currentEnemy) delete currentEnemy;
    
    if (enemyName.find("国王") != string::npos || 
        enemyName.find("龙王") != string::npos ||
        enemyName.find("统帅") != string::npos) {
        currentEnemy = new Enemy(enemyName, enemyType, enemyLevel);
        currentEnemy->setHealth(currentEnemy->getHealth() * 2);
        currentEnemy->setMaxHealth(currentEnemy->getHealth());
        currentEnemy->setAttack(currentEnemy->getAttack() + 5);
        currentEnemy->setDefense(currentEnemy->getDefense() + 3);
        currentEnemy->setGold(currentEnemy->getGold() * 3);
    } else {
        currentEnemy = new Enemy(enemyName, enemyType, enemyLevel);
    }
    
    if (x == 4 && y == 0) {
        cout << "洞穴深处的死亡气息让你不寒而栗...\n";
        Sleep(800);
    } else if (x == 4 && y == 1) {
        cout << "冰冷的气息在空气中弥漫...\n";
        Sleep(800);
    }
    
    string introLine = "你遇到了一只 " + enemyName + "！";
    
    switch (enemyType) {
        case EnemyType::SLIME: case EnemyType::SPLIT_SLIME:
            cout << introLine << "\n";
            if (elementSuffix.find("冰霜") != string::npos) cout << "冰冷的寒气从它身上散发出来，地面结了一层白霜。\n";
            else if (elementSuffix.find("熔岩") != string::npos) cout << "灼热的高温让周围的空气都在扭曲，地面发出滋滋声。\n";
            else if (elementSuffix.find("圣光") != string::npos) cout << "散发着金色光芒的史莱姆，看起来有些神圣却充满敌意。\n";
            else if (elementSuffix.find("毒液") != string::npos) cout << "墨绿色的史莱姆，滴落的液体腐蚀着地面，散发出刺鼻的气味。\n";
            else cout << "一团粘糊糊的胶状生物缓缓蠕动，发出咕嘟咕嘟的声音。\n";
            if (enemyType == EnemyType::SPLIT_SLIME) cout << "这个史莱姆看起来很不稳定，可能会分裂！\n";
            break;
        case EnemyType::LICH_KING:
            cout << "\n" << string(60, '=') << "\n";
            cout << "巫妖王莫德雷德降临了！\n";
            Sleep(1000);
            cout << "\"凡人，你竟敢闯入我的领域！\"\n";
            Sleep(1000);
            cout << "\"我的霜之哀伤渴望着你的灵魂！\"\n";
            Sleep(1000);
            cout << "\"今天，奥瑞亚将彻底成为历史！\"\n";
            cout << string(60, '=') << "\n";
            Sleep(1000);
            break;
        case EnemyType::DRAGON:
            cout << introLine << "\n";
            if (enemyName.find("火龙") != string::npos) cout << "火龙张开巨口，火焰在喉咙中聚集，周围的温度急剧升高。\n";
            else if (enemyName.find("圣龙") != string::npos) cout << "圣龙展开金色的翅膀，发出震天的咆哮，神圣的光芒照亮四周。\n";
            else if (enemyName.find("冰龙") != string::npos) cout << "冰龙呼出的寒气让空气瞬间冻结，雪花在它周围飞舞。\n";
            else cout << "龙族生物用冰冷的眼神盯着你，喉咙深处发出低沉的吼声。\n";
            break;
        default:
            cout << introLine << "\n";
    }
    
    currentEnemy->displayStats();
    
    cout << "你要做什么？(战斗/逃跑)\n";
    
    string combatChoice;
    cin >> combatChoice;
    
    if (combatChoice == "战斗" || combatChoice == "fight" || combatChoice == "攻击") {
        startCombat();
    } else {
        if (enemyType == EnemyType::LICH_KING || 
            enemyName.find("国王") != string::npos ||
            enemyName.find("龙王") != string::npos ||
            enemyName.find("统帅") != string::npos) {
            cout << enemyName << " 封锁了所有退路！无法逃跑！\n";
            startCombat();
            return;
        }
        
        cout << "你试图逃离 " << enemyName << "！\n";
        
        int escapeChance = 60;
        
        switch (enemyType) {
            case EnemyType::BAT: case EnemyType::GHOST: case EnemyType::WISP: case EnemyType::WOLF: case EnemyType::SPIDER:
                escapeChance = 40; break;
            case EnemyType::SLIME: case EnemyType::SPLIT_SLIME: case EnemyType::ZOMBIE: case EnemyType::TROLL:
                escapeChance = 80; break;
            case EnemyType::DRAGON: escapeChance = 30; break;
            case EnemyType::LICH_KING: escapeChance = 0; break;
        }
        
        int levelDiff = player.getLevel() - currentEnemy->getLevel();
        escapeChance += levelDiff * 5;
        
        if (escapeChance < 10) escapeChance = 10;
        if (escapeChance > 90) escapeChance = 90;
        
        cout << "逃跑成功率：" << escapeChance << "%\n";
        
        if (rand() % 100 < escapeChance) {
            cout << "你成功逃脱了 " << enemyName << "！\n";
            delete currentEnemy;
            currentEnemy = nullptr;
        } else {
            cout << "但 " << enemyName << " 追上了你并发起了攻击！\n";
            currentEnemy->attackEnemy(player);
            player.reduceArmorDurability();
            startCombat();
        }
    }
}

void GameWorld::findItem() {
    Item* item = nullptr;
    int itemType = rand() % 100;
    
    if (itemType < 5) {
        item = new Consumable("修理锤", "可以修理装备的耐久度。", 80, 0);
    } else if (itemType < 30) {
        vector<string> weaponNames = {"铁剑", "钢斧", "匕首", "长弓"};
        vector<int> damages = {10, 15, 5, 12};
        int idx = rand() % weaponNames.size();
        item = new Weapon(weaponNames[idx], "一把实用的武器。", 
                         damages[idx] * 5, damages[idx], 100);
    } else if (itemType < 55) {
        vector<string> armorNames = {"皮甲", "锁子甲", "板甲"};
        vector<int> defenses = {5, 10, 15};
        int idx = rand() % armorNames.size();
        item = new Armor(armorNames[idx], "一件实用的防具。",
                        defenses[idx] * 6, defenses[idx], 80);
    } else if (itemType < 80) {
        vector<string> potionNames = {"小型生命药剂", "中型生命药剂", "大型生命药剂"};
        vector<int> heals = {20, 50, 100};
        int idx = rand() % potionNames.size();
        item = new Consumable(potionNames[idx], "恢复生命值的药剂。",
                             heals[idx] / 2, heals[idx]);
    } else {
        int goldAmount = 10 + (rand() % 30);
        player.addGold(goldAmount);
        cout << "\n你找到了一个金币包，获得了 " << goldAmount << " 枚金币！\n";
        return;
    }
    
    if (item != nullptr) {
        worldItems.push_back(item);
        player.addItem(item);
        cout << "\n你找到了一把 " << item->getName() << "！\n";
    }
}

void GameWorld::findGold() {
    int goldAmount = 5 + (rand() % 20);
    player.addGold(goldAmount);
    cout << "\n你找到了 " << goldAmount << " 枚金币！\n";
}

void GameWorld::triggerQuest() {
    cout << "\n一位村庄长老向你走来。\n";
    cout << "\"勇敢的冒险者，我们的村庄正被哥布林国王骚扰！\"\n";
    cout << "\"他偷走了我们的神圣护身符。你能帮我们找回来吗？\"\n";
    cout << "\"如果你成功了，我们会给你丰厚的奖励。\"\n";
    
    cout << "\n是否接受任务？(是/yes/no)：";
    string choice; cin >> choice;
    
    if (choice == "是" || choice == "yes" || choice == "Y" || choice == "y") {
        cout << "任务接受：击败哥布林国王并夺回护身符。\n";
        cout << "长老说哥布林藏在北边的一个洞穴里。\n";
    } else {
        cout << "长老叹了口气，转身离开了。\n";
    }
}

void GameWorld::startCombat() {
    gameState = GameState::COMBAT;
    
    while (gameState == GameState::COMBAT) {
        if (!activeEnemies.empty()) {
            cout << "\n--- 与分裂史莱姆的战斗 --- \n";
            
            for (size_t i = 0; i < activeEnemies.size(); ++i) {
                cout << (i + 1) << ". ";
                activeEnemies[i]->displayStats();
            }
            
            cout << "\n你要攻击哪个敌人？(输入编号，0使用物品/逃跑)\n";
            
            string input;
            cin >> input;
            
            try {
                int choice = stoi(input);
                if (choice == 0) {
                    cout << "1. 使用物品\n";
                    cout << "2. 尝试逃跑\n";
                    cout << "请选择：";
                    cin >> input;
                    
                    if (input == "1") {
                        player.displayInventory();
                        cout << "请输入要使用的物品编号（0取消）：";
                        cin >> input;
                        
                        try {
                            int itemIndex = stoi(input) - 1;
                            if (itemIndex >= 0) player.useItem(itemIndex);
                        } catch (...) {
                            cout << "无效的物品编号。\n";
                        }
                    } else if (input == "2") {
                        if (rand() % 100 < 85) {
                            cout << "你成功逃离了分裂史莱姆！\n";
                            for (auto e : activeEnemies) delete e;
                            activeEnemies.clear();
                            gameState = GameState::PLAYING;
                            return;
                        } else {
                            cout << "逃跑失败！\n";
                        }
                    }
                    
                    for (auto e : activeEnemies) {
                        if (e->getHealth() > 0) {
                            e->attackEnemy(player);
                            player.reduceArmorDurability();
                        }
                        
                        if (player.getHealth() <= 0) {
                            cout << "\n你被击败了！\n";
                            gameState = GameState::GAME_OVER;
                            for (auto e : activeEnemies) delete e;
                            activeEnemies.clear();
                            return;
                        }
                    }
                    continue;
                }
                
                if (choice > 0 && static_cast<size_t>(choice) <= activeEnemies.size()) {
                    Enemy* target = activeEnemies[choice - 1];
                    
                    player.attackEnemy(*target);
                    
                    if (target->getHealth() <= 0) {
                        cout << "\n你击败了 " << target->getName() << "！\n";
                        
                        int expGain = target->getLevel() * 20;
                        int goldGain = target->getGold();
                        
                        player.addExperience(expGain);
                        player.addGold(goldGain);
                        
                        cout << "你获得了 " << expGain << " 点经验值和 " << goldGain << " 枚金币！\n";
                        
                        delete target;
                        activeEnemies.erase(activeEnemies.begin() + (choice - 1));
                    }
                    
                    if (activeEnemies.empty()) {
                        cout << "\n你击败了所有分裂史莱姆！战斗胜利！\n";
                        gameState = GameState::PLAYING;
                        return;
                    }
                    
                    for (auto e : activeEnemies) {
                        if (e->getHealth() > 0) {
                            e->attackEnemy(player);
                            player.reduceArmorDurability();
                        }
                        
                        if (player.getHealth() <= 0) {
                            cout << "\n你被击败了！\n";
                            gameState = GameState::GAME_OVER;
                            for (auto e : activeEnemies) delete e;
                            activeEnemies.clear();
                            return;
                        }
                    }
                } else {
                    cout << "无效的目标编号。\n";
                }
            } catch (...) {
                cout << "无效输入。\n";
            }
            continue;
        }
        
        if (currentEnemy != nullptr && player.getHealth() > 0) {
            cout << "\n--- 战斗中 --- \n";
            player.displayStats();
            currentEnemy->displayStats();
            
            if (player.getEquippedWeapon() != nullptr && player.getEquippedWeapon()->isBroken())
                cout << "\n警告：你的武器已损坏！攻击力降低！\n";
            if (player.getEquippedArmor() != nullptr && player.getEquippedArmor()->isBroken())
                cout << "\n警告：你的防具已损坏！防御力降低！\n";
            
            cout << "\n你要做什么？(攻击/使用/逃跑)\n";
            string choice;
            cin >> choice;
            
            if (choice == "攻击" || choice == "attack" || choice == "a") {
                player.attackEnemy(*currentEnemy);
                
                if (currentEnemy->getHealth() <= 0) {
                    cout << "\n你击败了 " << currentEnemy->getName() << "！\n";
                    
                    int expGain = currentEnemy->getLevel() * 50;
                    int goldGain = currentEnemy->getGold();
                    
                    if (currentEnemy->getEnemyType() == EnemyType::LICH_KING) {
                        cout << "\n" << string(50, '=') << "\n";
                        cout << "恭喜你！你击败了巫妖王！\n";
                        cout << "无尽之地恢复了和平！\n";
                        cout << "你成为了传说中的英雄！\n";
                        cout << string(50, '=') << "\n";
                        
                        expGain *= 10;
                        goldGain *= 5;
                        
                        cout << "\n游戏通关！感谢你的游玩！\n";
                        cout << "是否要重新开始游戏？(是/否)：";
                        string restartChoice;
                        cin >> restartChoice;
                        
                        if (restartChoice == "是" || restartChoice == "y" || restartChoice == "Y") {
                            gameState = GameState::MENU;
                        } else {
                            exit(0);
                        }
                        
                        delete currentEnemy;
                        currentEnemy = nullptr;
                        gameState = GameState::PLAYING;
                        return;
                    }
                    
                    if ((currentEnemy->getEnemyType() == EnemyType::SLIME || 
                         currentEnemy->getEnemyType() == EnemyType::SPLIT_SLIME) && 
                        currentEnemy->getCanSplit() && currentEnemy->getSplitCount() > 0) {
                        handleSlimeSplit(currentEnemy);
                        continue;
                    }
                    
                    player.addExperience(expGain);
                    player.addGold(goldGain);
                    
                    cout << "你获得了 " << expGain << " 点经验值和 " << goldGain << " 枚金币！\n";
                    
                    if (rand() % 100 < (currentEnemy->getEnemyType() == EnemyType::LICH_KING ? 100 : 40)) {
                        Item* loot = nullptr;
                        int lootType = rand() % 5;
                        
                        switch (lootType) {
                            case 0:
                                if (currentEnemy->getEnemyType() == EnemyType::LICH_KING)
                                    loot = new Weapon("霜之哀伤", "巫妖王的佩剑，蕴含着无尽的黑暗力量。", 1000, 60, 0, true);
                                else
                                    loot = new Weapon("怪物掉落的武器", "从怪物身上获得的武器。", 
                                                     currentEnemy->getLevel() * 20, currentEnemy->getLevel() * 2 + 5, 
                                                     50 + currentEnemy->getLevel() * 10);
                                break;
                            case 1:
                                if (currentEnemy->getEnemyType() == EnemyType::LICH_KING)
                                    loot = new Armor("巫妖王战甲", "巫妖王的战甲，散发着死亡的气息。", 800, 45, 0, true);
                                else
                                    loot = new Armor("怪物掉落的防具", "从怪物身上获得的防具。", 
                                                   currentEnemy->getLevel() * 8, currentEnemy->getLevel() + 2,
                                                   40 + currentEnemy->getLevel() * 8);
                                break;
                            case 2:
                                loot = new Consumable("小型生命药剂", "恢复20点生命值。", 10, 20);
                                break;
                            case 3:
                                if (currentEnemy->getEnemyType() == EnemyType::DRAGON) {
                                    if (player.getLocationY() >= 7)
                                        loot = new Weapon("龙息法杖", "可以释放龙息的法杖。", 250, 28, 150);
                                    else if (player.getLocationX() >= 7)
                                        loot = new Armor("圣龙鳞甲", "圣龙的鳞片制成的护甲。", 220, 26, 200);
                                    else if (player.getLocationX() <= 1)
                                        loot = new Weapon("冰霜龙牙剑", "冰龙的牙齿制成的剑。", 230, 27, 160);
                                    else
                                        loot = new Weapon("龙牙匕首", "用龙牙制成的匕首。", 200, 25, 140);
                                } else if (currentEnemy->getEnemyType() == EnemyType::FOREST_SPIRIT) {
                                    loot = new Consumable("精灵药水", "森林精灵制作的药水。", 40, 60);
                                } else if (rand() % 100 < 10) {
                                    loot = new Consumable("修理锤", "可以修理装备的耐久度。", 80, 0);
                                } else {
                                    loot = new Consumable("中型生命药剂", "恢复50点生命值。", 30, 50);
                                }
                                break;
                            case 4:
                                if (currentEnemy->getEnemyType() == EnemyType::LICH_KING)
                                    loot = new QuestItem("巫妖王冠冕", "巫妖王的王冠，证明你击败了他的证据。", 500, "击败巫妖王");
                                else
                                    loot = new Consumable("稀有药水", "非常稀有的药水。", 80, 120);
                                break;
                        }
                        
                        if (loot != nullptr) {
                            worldItems.push_back(loot);
                            player.addItem(loot);
                            cout << currentEnemy->getName() << " 掉落了 " << loot->getName() << "！\n";
                        }
                    }
                    
                    if (currentEnemy->getEnemyType() == EnemyType::GOBLIN && 
                        currentEnemy->getName().find("国王") != string::npos) {
                        QuestItem* amulet = new QuestItem("神圣护身符", "村庄被盗的神圣护身符。", 100, "哥布林国王");
                        worldItems.push_back(amulet);
                        player.addItem(amulet);
                        player.addCompletedQuest("哥布林国王");
                        cout << "你找到了神圣护身符！任务完成！\n";
                        cout << "你获得了额外的 200 点经验值和 100 枚金币作为奖励！\n";
                        player.addExperience(200);
                        player.addGold(100);
                    }
                    
                    delete currentEnemy;
                    currentEnemy = nullptr;
                    gameState = GameState::PLAYING;
                } else {
                    currentEnemy->attackEnemy(player);
                    player.reduceArmorDurability();
                    
                    if (player.getHealth() <= 0) {
                        cout << "\n你被 " << currentEnemy->getName() << " 击败了！\n";
                        
                        if (player.getEquippedWeapon() != nullptr && player.getEquippedWeapon()->isBroken())
                            cout << "你的 " << player.getEquippedWeapon()->getName() << " 在战斗中彻底损坏了！\n";
                        if (player.getEquippedArmor() != nullptr && player.getEquippedArmor()->isBroken())
                            cout << "你的 " << player.getEquippedArmor()->getName() << " 在战斗中彻底损坏了！\n";
                        
                        gameState = GameState::GAME_OVER;
                    }
                }
            } else if (choice == "使用" || choice == "use" || choice == "u") {
                player.displayInventory();
                cout << "请输入要使用的物品编号（0取消）：";
                
                string input;
                cin >> input;
                
                try {
                    int itemIndex = stoi(input) - 1;
                    if (itemIndex >= 0) {
                        player.useItem(itemIndex);
                        
                        if (currentEnemy != nullptr && currentEnemy->getHealth() > 0) {
                            currentEnemy->attackEnemy(player);
                            player.reduceArmorDurability();
                            
                            if (player.getHealth() <= 0) {
                                cout << "\n你被 " << currentEnemy->getName() << " 击败了！\n";
                                gameState = GameState::GAME_OVER;
                            }
                        }
                    }
                } catch (...) {
                    cout << "无效的物品编号。\n";
                }
            } else if (choice == "逃跑" || choice == "run" || choice == "r") {
                if (currentEnemy->getEnemyType() == EnemyType::LICH_KING) {
                    cout << "巫妖王的黑暗力量封锁了所有退路！无法逃跑！\n";
                    continue;
                }
                
                if (attemptEscape()) {
                    gameState = GameState::PLAYING;
                } else {
                    currentEnemy->attackEnemy(player);
                    player.reduceArmorDurability();
                    
                    if (player.getHealth() <= 0) {
                        cout << "\n你在逃跑时被击败了！\n";
                        gameState = GameState::GAME_OVER;
                    }
                }
            } else {
                cout << "无效指令。请输入 '攻击:attack'、'使用:use' 或 '逃跑:run'。\n";
            }
        } else {
            break;
        }
    }
    
    if (player.getHealth() <= 0) gameState = GameState::GAME_OVER;
}

bool GameWorld::attemptEscape() {
    int escapeChance = 50 + (player.getLevel() - currentEnemy->getLevel()) * 10;
    
    switch (currentEnemy->getEnemyType()) {
        case EnemyType::SLIME: case EnemyType::SPLIT_SLIME: case EnemyType::ZOMBIE:
            escapeChance += 20; break;
        case EnemyType::BAT: case EnemyType::GHOST: case EnemyType::WOLF: case EnemyType::SPIDER:
            escapeChance -= 15; break;
        case EnemyType::DRAGON: escapeChance -= 10; break;
        case EnemyType::LICH_KING: escapeChance = 0; break;
        case EnemyType::GOBLIN:
            if (currentEnemy->getName().find("国王") != string::npos) escapeChance -= 20; break;
    }
    
    if (escapeChance < 10) escapeChance = 10;
    if (escapeChance > 90) escapeChance = 90;
    
    cout << "逃跑成功率：" << escapeChance << "%\n";
    
    if (rand() % 100 < escapeChance) {
        cout << "你成功逃离了 " << currentEnemy->getName() << "！\n";
        delete currentEnemy;
        currentEnemy = nullptr;
        return true;
    } else {
        cout << "逃跑失败！\n";
        return false;
    }
}

void GameWorld::handleSlimeSplit(Enemy* originalSlime) {
    if (!originalSlime->getCanSplit() || originalSlime->getSplitCount() <= 0) return;
    
    cout << "\n" << originalSlime->getName() << " 开始分裂了！\n";
    cout << "它分成了两个小史莱姆！\n";
    
    delete originalSlime;
    currentEnemy = nullptr;
    
    for (int i = 0; i < 2; i++) {
        string slimeName;
        if (originalSlime->getName().find("冰霜") != string::npos) slimeName = "小型冰霜史莱姆";
        else if (originalSlime->getName().find("熔岩") != string::npos) slimeName = "小型熔岩史莱姆";
        else if (originalSlime->getName().find("圣光") != string::npos) slimeName = "小型圣光史莱姆";
        else if (originalSlime->getName().find("毒液") != string::npos) slimeName = "小型毒液史莱姆";
        else slimeName = "小型史莱姆";
        
        Enemy* smallSlime = new Enemy(slimeName, EnemyType::SPLIT_SLIME, player.getLevel());
        activeEnemies.push_back(smallSlime);
    }
    
    if (!activeEnemies.empty()) {
        cout << "\n你需要同时面对 " << activeEnemies.size() << " 个敌人！\n";
    }
}

void GameWorld::openInventory() {
    gameState = GameState::INVENTORY;
    
    while (gameState == GameState::INVENTORY) {
        player.displayInventory();
        cout << "1. 使用物品\n";
        cout << "2. 出售物品\n";
        cout << "3. 关闭背包\n";
        cout << "请输入你的选择：";
        
        string input; cin >> input;
        int choice = 0;
        
        try { choice = stoi(input); }
        catch (...) {
            if (input == "关闭" || input == "exit" || input == "退出") choice = 3;
            else { cout << "无效选择，请重试。\n"; continue; }
        }
        
        switch (choice) {
            case 1: {
                cout << "请输入要使用的物品编号：";
                string itemInput; cin >> itemInput;
                try {
                    int itemIndex = stoi(itemInput) - 1;
                    player.useItem(itemIndex);
                } catch (...) { cout << "无效的物品编号。\n"; }
                break;
            }
            case 2: {
                cout << "请输入要出售的物品编号：";
                string itemInput; cin >> itemInput;
                try {
                    int itemIndex = stoi(itemInput) - 1;
                    player.sellItem(itemIndex);
                } catch (...) { cout << "无效的物品编号。\n"; }
                break;
            }
            case 3:
                gameState = GameState::PLAYING;
                break;
            default:
                cout << "无效选择。\n";
        }
    }
}

void GameWorld::enterShop() {
    gameState = GameState::SHOP;
    
    while (gameState == GameState::SHOP) {
        shop.displayShop();
        cout << "你的金币：" << player.getGold() << " 枚\n";
        cout << "请输入要购买的物品编号（0离开）：";
        
        string input; cin >> input;
        int choice = 0;
        
        try { choice = stoi(input); }
        catch (...) {
            if (input == "离开" || input == "exit" || input == "关闭") choice = 0;
            else { cout << "无效选择，请重试。\n"; continue; }
        }
        
        if (choice == 0) gameState = GameState::PLAYING;
        else shop.buyItem(choice - 1, player);
    }
}

void GameWorld::openTeleportMenu() {
    gameState = GameState::TELEPORT;
    
    cout << "\n=== 传送系统 ===\n";
    cout << "1. 新手村\n";
    cout << "2. 极炎之地\n";
    cout << "3. 哥布林王国\n";
    cout << "4. 无尽寒夜\n";
    cout << "5. 天璇圣地\n";
    cout << "6. 巫妖王洞穴\n";
    cout << "0. 取消\n";
    cout << "请选择要传送的位置：";
    
    string input; cin >> input;
    int choice = 0;
    
    try { choice = stoi(input); }
    catch (...) {
        if (input == "取消" || input == "exit") choice = 0;
        else { cout << "无效选择。\n"; gameState = GameState::PLAYING; return; }
    }
    
    if (choice >= 1 && choice <= 6) {
    	TeleportType dest = static_cast<TeleportType>(choice - 1);
    	pair<int, int> loc = getTeleportLocation(dest);
    	player.teleport(loc.first, loc.second);
    	cout << "你使用传送	卷轴来到了 " << getLocationName(loc.first, loc.second) << "！\n";
	}
    
    gameState = GameState::PLAYING;
}

void GameWorld::openEquipmentMenu() {
    gameState = GameState::EQUIPMENT;
    
    while (gameState == GameState::EQUIPMENT) {
        player.displayEquipment();
        cout << "1. 卸下武器\n";
        cout << "2. 卸下防具\n";
        cout << "3. 修理装备（使用修理锤）\n";
        cout << "4. 返回游戏\n";
        cout << "请输入你的选择：";
        
        string input; cin >> input;
        int choice = 0;
        
        try { choice = stoi(input); }
        catch (...) {
            if (input == "返回" || input == "exit" || input == "退出") choice = 4;
            else { cout << "无效选择，请重试。\n"; continue; }
        }
        
        switch (choice) {
            case 1: player.unequipWeapon(); break;
            case 2: player.unequipArmor(); break;
            case 3: {
                cout << "请选择要修理的装备（输入背包中的编号）：\n";
                player.displayInventory();
                
                string repairInput; cin >> repairInput;
                
                try {
                    int index = stoi(repairInput) - 1;
                    if (index >= 0 && static_cast<size_t>(index) < player.getInventory().size()) {
                        Item* item = player.getInventory()[index];
                        
                        if (item->getName() == "修理锤") {
                            cout << "请选择要修理的装备：\n";
                            cout << "1. 修理武器\n";
                            cout << "2. 修理防具\n";
                            cout << "3. 修理背包中的装备\n";
                            cout << "请选择：";
                            
                            string repairType; cin >> repairType;
                            
                            if (repairType == "1") {
                                Weapon* weapon = player.getEquippedWeapon();
                                if (weapon != nullptr && !weapon->isInfinite()) {
                                    if (weapon->isBroken()) {
                                        weapon->repair(weapon->getMaxDurability());
                                        cout << weapon->getName() << " 已完全修复！\n";
                                        player.removeItem(item);
                                        delete item;
                                    } else cout << weapon->getName() << " 尚未损坏，无需修理。\n";
                                } else if (weapon == nullptr) cout << "你没有装备武器。\n";
                                else cout << weapon->getName() << " 是传奇装备，无需修理。\n";
                            }
                            else if (repairType == "2") {
                                Armor* armor = player.getEquippedArmor();
                                if (armor != nullptr && !armor->isInfinite()) {
                                    if (armor->isBroken()) {
                                        armor->repair(armor->getMaxDurability());
                                        cout << armor->getName() << " 已完全修复！\n";
                                        player.removeItem(item);
                                        delete item;
                                    } else cout << armor->getName() << " 尚未损坏，无需修理。\n";
                                } else if (armor == nullptr) cout << "你没有装备防具。\n";
                                else cout << armor->getName() << " 是传奇装备，无需修理。\n";
                            }
                            else if (repairType == "3") {
                                cout << "请输入背包中要修理的装备编号：\n";
                                player.displayInventory();
                                
                                string itemInput; cin >> itemInput;
                                
                                try {
                                    int itemIndex = stoi(itemInput) - 1;
                                    if (itemIndex >= 0 && static_cast<size_t>(itemIndex) < player.getInventory().size()) {
                                        Item* targetItem = player.getInventory()[itemIndex];
                                        
                                        if (targetItem->getType() == ItemType::WEAPON) {
                                            Weapon* targetWeapon = dynamic_cast<Weapon*>(targetItem);
                                            if (targetWeapon != nullptr && !targetWeapon->isInfinite()) {
                                                if (targetWeapon->isBroken()) {
                                                    targetWeapon->repair(targetWeapon->getMaxDurability());
                                                    cout << targetWeapon->getName() << " 已完全修复！\n";
                                                    player.removeItem(item);
                                                    delete item;
                                                } else cout << targetWeapon->getName() << " 尚未损坏，无需修理。\n";
                                            } else if (targetWeapon != nullptr && targetWeapon->isInfinite())
                                                cout << targetWeapon->getName() << " 是传奇装备，无需修理。\n";
                                        }
                                        else if (targetItem->getType() == ItemType::ARMOR) {
                                            Armor* targetArmor = dynamic_cast<Armor*>(targetItem);
                                            if (targetArmor != nullptr && !targetArmor->isInfinite()) {
                                                if (targetArmor->isBroken()) {
                                                    targetArmor->repair(targetArmor->getMaxDurability());
                                                    cout << targetArmor->getName() << " 已完全修复！\n";
                                                    player.removeItem(item);
                                                    delete item;
                                                } else cout << targetArmor->getName() << " 尚未损坏，无需修理。\n";
                                            } else if (targetArmor != nullptr && targetArmor->isInfinite())
                                                cout << targetArmor->getName() << " 是传奇装备，无需修理。\n";
                                        }
                                        else cout << "只能修理武器或防具。\n";
                                    } else cout << "无效的装备编号。\n";
                                } catch (...) { cout << "无效输入。\n"; }
                            }
                            else cout << "无效的选择。\n";
                        } else cout << "这不是修理锤！\n";
                    } else cout << "无效的物品编号。\n";
                } catch (...) { cout << "无效输入。\n"; }
                break;
            }
            case 4: gameState = GameState::PLAYING; break;
            default: cout << "无效选择。\n";
        }
    }
}

void GameWorld::displayMainMenu() const {
    system("cls");
    cout << "=== 无尽之地 ===\n";
    cout << "当前版本：V" << version << "\n";
    cout << "1. 开始新游戏\n";
    cout << "2. 游戏玩法\n";
    cout << "3. 保存游戏\n";
    cout << "4. 加载存档\n";
    cout << "5. 版本公告\n";
    cout << "6. 退出游戏\n";
    cout << "==========================\n";
    cout << "请输入你的选择：";
}

void GameWorld::displayHowToPlay() const {
    cout << "\n=== 游戏玩法 ===\n";
    cout << "一般情况下照着数字输入\n";
    cout << "\n探索时指令：\n";
    cout << "1/n/北 - 向北移动\n";
    cout << "1/s/南 - 向南移动\n";
    cout << "1/e/东 - 向东移动\n";
    cout << "1/w/西 - 向西移动\n";
    cout << "2/属性 - 查看角色属性\n";
    cout << "3/背包 - 打开背包\n";
    cout << "4/商店 - 访问商店（仅新手村）\n";
    cout << "5/map - 查看地图\n";
    cout << "6/quit/退出 - 退出游戏\n";
    cout << "7/teleport/传送 - 使用传送功能\n";
    cout << "8/save/保存 - 保存游戏进度\n";
    cout << "9/equip/装备 - 管理装备\n";
    cout << "\n战斗中指令：\n";
    cout << "攻击/attack - 攻击敌人\n";
    cout << "使用/use - 使用物品\n";
    cout << "逃跑/run - 尝试逃离\n";
    cout << "\n装备耐久系统：\n";
    cout << "- 每次攻击会消耗武器耐久度\n";
    cout << "- 每次受到攻击会消耗防具耐久度\n";
    cout << "- 耐久度为0时装备损坏，自动卸下\n";
    cout << "- 损坏的装备可以修理（使用修理锤）\n";
    cout << "- 损坏的装备出售价格减半\n";
    cout << "- 传奇装备（无尽之剑、菲拉尔战甲）无限耐久\n";
    cout << "\n传送系统：\n";
    cout << "- 初始自带新手村传送卷轴\n";
    cout << "- 可以在商店购买更多传送卷轴\n";
    cout << "- 使用传送卷轴可以快速移动到对应区域\n";
    cout << "- 传送卷轴不会消耗，可以重复使用\n";
    cout << "\n存档系统：\n";
    cout << "- 游戏内按8可以保存当前进度\n";
    cout << "- 主菜单可以选择加载存档\n";
    cout << "- 有4个存档位可用\n";
    cout << "\n史莱姆特性：\n";
    cout << "- 部分史莱姆被击败时会分裂成更小的史莱姆\n";
    cout << "- 分裂史莱姆属性较低但数量多，需要小心应对\n";
    cout << "==================\n";
    
    waitForKeyPress();
}

void GameWorld::displayAnnouncement() const {
    system("cls");
    cout << "=== 无尽之地 V" << version << " 版本公告 ===\n\n";
    cout << "V0.50 版本更新内容:\n";
    cout << "  1. 新增装备耐久度系统：每次攻击或防御都会消耗耐久度\n";
    cout << "  2. 装备耐久度为0时会损坏，损坏后自动卸下\n";
    cout << "  3. 新增装备管理功能：可以卸下装备\n";
    cout << "  4. 空手状态：卸下武器后只使用基础攻击力\n";
    cout << "  5. 新增传奇装备：无尽之剑、菲拉尔战甲（无限耐久）\n";
    cout << "  6. 新增修理系统：损坏的装备可以修理\n";
    cout << "  7. 损坏的装备出售价格减半\n";
    cout << "  8. 平衡游戏经济：初始金币增加\n";
    cout << "  9. 优化物品系统：所有装备现在显示耐久度\n";
    cout << "\n装备耐久度详细信息：\n";
    cout << "  - 普通武器：攻击时消耗1点耐久度\n";
    cout << "  - 普通防具：受到攻击时消耗1点耐久度\n";
    cout << "  - 损坏的装备无法装备，需要修理\n";
    cout << "  - 商店出售修理锤，可以修理装备\n";
    cout << "  - 损坏装备自动卸下，放入背包\n";
    cout << "\n传奇装备特性：\n";
    cout << "  - 无尽之剑：攻击力50，无限耐久，价值1000金币\n";
    cout << "  - 菲拉尔战甲：防御力35，无限耐久，价值800金币\n";
    cout << "  - 这两件装备不会损坏，是游戏中最强的装备\n";
    cout << "\n历史版本更新内容：\n";
    cout << "  V0.41: 巫妖王生成概率50%，优化菜单，使用C++11\n";
    cout << "  V0.40: 新增完整存档系统，4个存档位，传送卷轴\n";
    cout << "  V0.30: 新增地图系统，传送功能，区域特色\n";
    cout << "  V0.20: 新增战斗系统，商店系统，物品系统\n";
    cout << "  V0.10: 基础框架，移动系统，属性系统\n";
    cout << "\n注意事项：\n";
    cout << "  1. 注意装备耐久度，及时修理或更换装备\n";
    cout << "  2. 损坏的装备仍然可以出售，但价格减半\n";
    cout << "  3. 背包最多只能携带30个物品\n";
    cout << "  4. 传奇装备非常稀有，需要大量金币购买\n";
    cout << "\n开发者：DING、Chu、Eric\n";
    cout << "开发日期：2024年\n";
    cout << "======================================\n";
    
    waitForKeyPress();
}

void GameWorld::startGame() {
    gameState = GameState::PLAYING;
    
    system("cls");
    cout << "\n欢迎来到无尽之地！\n";
    Sleep(500);
    cout << "冒险即将开始...\n";
    Sleep(500);
    system("cls");
    
    cout << "\n================================\n"; 
    cout << "\n在艾瑟拉大陆的东北角，有一个被遗忘的王国——奥瑞亚。\n";
    Sleep(800);
    cout << "\n三年前，巫妖王莫德雷德从冰封王座苏醒，\n";
    Sleep(800);
    cout << "率领天灾军团席卷了整个北方大陆。\n";
    Sleep(800);
    cout << "\n曾经繁荣的奥瑞亚王国如今只剩下断壁残垣，\n";
    Sleep(800);
    cout << "生者要么被转化为亡灵，要么躲藏在最后的据点中。\n";
    Sleep(800);
    cout << "\n你，一名奥瑞亚的幸存者，在巫妖王的追捕下逃到了南方。\n";
    Sleep(800);
    cout << "但你知道，逃避不能解决问题。\n";
    Sleep(800);
    cout << "\n现在，你站在最后的新手村——圣光之愿礼拜堂的废墟上，\n";
    Sleep(800);
    cout << "手中紧握着父亲留下的木剑，心中只有一个信念：\n";
    Sleep(800);
    cout << "\n「要么夺回我们的家园，要么在战斗中光荣地死去。」\n";
    Sleep(800);
    cout << "\n================================\n";
    Sleep(500);
    cout << "\n这片大陆有五大区域等待你去探索：\n";
    cout << "1. 新手村 - 你开始的地方，最后的避难所\n";
    cout << "2. 哥布林王国 - 被巫妖王腐化的绿皮部落\n";
    cout << "3. 极炎之地 - 地心熔岩涌出的危险区域\n";
    cout << "4. 无尽寒夜 - 永恒的冰雪之地，冰龙巢穴\n";
    cout << "5. 天璇圣地 - 古老文明的遗迹，隐藏着秘密\n";
    cout << "\n而在最北方，就是巫妖王的老巢——冰冠冰川。\n";
    Sleep(500);
    cout << "\n注意：装备现在有耐久度系统，每次攻击或受到攻击都会消耗耐久度！\n";
    cout << "耐久度为0时装备会损坏，损坏的装备需要修理才能使用。\n";
    cout << "传奇装备（无尽之剑、菲拉尔战甲）拥有无限耐久！\n";
    cout << "\n（输入'7'可以使用传送功能快速移动）\n";
    cout << "\n（输入'8'可以保存游戏，主菜单可以加载存档）\n";
    cout << "\n（输入'9'可以管理装备）\n";
    cout << "\n现在，冒险开始了。你要做什么？\n";
}

void GameWorld::displayGameMenu() const {
    cout << "\n当前位置: " << getLocationName(player.getLocationX(), player.getLocationY()) 
         << " (" << player.getLocationX() << ", " << player.getLocationY() << ")\n";
    cout << "你要做什么？" << endl;
    cout << "1. 移动（北/南/东/西 或 n/s/e/w）" << endl;
    cout << "2. 查看属性" << endl;
    cout << "3. 打开背包" << endl;
    cout << "4. 访问商店（如可用）" << endl;
    cout << "5. 查看地图" << endl;
    cout << "6. 退出游戏:quit" << endl;
    cout << "7. 传送功能" << endl;
    cout << "8. 保存游戏" << endl;
    cout << "9. 管理装备" << endl;
    cout << "请输入你的选择：";
}

void GameWorld::handleGameInput(const string& input) {
    if (input == "n" || input == "北" || input == "北方") {
        player.move(Direction::NORTH);
        cout << "你向北移动到了 " << getLocationName(player.getLocationX(), player.getLocationY()) 
             << " (" << player.getLocationX() << ", " << player.getLocationY() << ")\n";
        
        if (player.getLocationX() == 4 && player.getLocationY() == 0) {
            cout << "\n警告：你来到了巫妖王洞穴入口！\n";
            cout << "冰冷的死亡气息几乎让你窒息。\n";
            cout << "墙壁上刻着古老的符文：\n";
            cout << "「踏入此地者，必死无疑。」\n";
        }
        
        checkRandomEvent();
    } else if (input == "s" || input == "南" || input == "南方") {
        player.move(Direction::SOUTH);
        cout << "你向南移动到了 " << getLocationName(player.getLocationX(), player.getLocationY()) 
             << " (" << player.getLocationX() << ", " << player.getLocationY() << ")\n";
        
        if (player.getLocationY() >= 7) {
            cout << "热浪扑面而来，空气中弥漫着硫磺的味道。\n";
            cout << "这里就是传说中的极炎之地！\n";
        }
        
        checkRandomEvent();
    } else if (input == "e" || input == "东" || input == "东方") {
        player.move(Direction::EAST);
        cout << "你向东移动到了 " << getLocationName(player.getLocationX(), player.getLocationY()) 
             << " (" << player.getLocationX() << ", " << player.getLocationY() << ")\n";
        
        if (player.getLocationX() >= 7) {
            cout << "眼前突然出现一座巨大的古代遗迹。\n";
            cout << "石柱上刻着神秘的符号，这里就是天璇圣地！\n";
        }
        
        checkRandomEvent();
    } else if (input == "w" || input == "西" || input == "西方") {
        player.move(Direction::WEST);
        cout << "你向西移动到了 " << getLocationName(player.getLocationX(), player.getLocationY()) 
             << " (" << player.getLocationX() << ", " << player.getLocationY() << ")\n";
        
        if (player.getLocationX() <= 2) {
            cout << "刺骨的寒风让你打了个寒颤。\n";
            cout << "整个世界都被冰雪覆盖，这里是无尽寒夜！\n";
        }
        
        checkRandomEvent();
    } else if (input == "2" || input == "属性") {
        player.displayStats();
    } else if (input == "3" || input == "inv" || input == "背包") {
        openInventory();
    } else if (input == "4" || input == "商店") {
        if (player.getLocationX() == 4 && player.getLocationY() == 4) {
            enterShop();
        } else {
            cout << "这里没有商店！只有在新手村中心才有商店。\n";
        }
    } else if (input == "5" || input == "map" || input == "地图") {
        displayMap();
    } else if (input == "7" || input == "teleport" || input == "传送") {
        openTeleportMenu();
    } else if (input == "8" || input == "save" || input == "保存") {
        saveGame();
    } else if (input == "9" || input == "equip" || input == "装备") {
        openEquipmentMenu();
    } else if (input == "fight" || input == "战斗" || input == "攻击") {
        if (currentEnemy) {
            startCombat();
        } else {
            cout << "没有敌人可以攻击！\n";
        }
    } else if (input == "quit" || input == "退出" || input == "6") {
        cout << "是否保存游戏？(是/否)：";
        string choice;
        cin >> choice;
        
        if (choice == "是" || choice == "y" || choice == "Y") {
            saveGame();
        }
        cout << "感谢游玩！再见。\n";
        exit(0);
    } else {
        cout << "无效指令。\n";
    }
}

void GameWorld::displayLoadMenu() {
    system("cls");
    cout << "=== 加载存档 ===\n";
    cout << "1. 加载默认存档 (savegame.txt)\n";
    cout << "2. 加载存档1 (save1.txt)\n";
    cout << "3. 加载存档2 (save2.txt)\n";
    cout << "4. 加载存档3 (save3.txt)\n";
    cout << "0. 返回主菜单\n";
    cout << "================\n";
    cout << "请输入你的选择：";
}

void GameWorld::handleLoadInput(int choice) {
    bool loaded = false;
    
    switch (choice) {
        case 0: gameState = GameState::MENU; break;
        case 1: loaded = loadGame("savegame.txt"); break;
        case 2: loaded = loadGame("save1.txt"); break;
        case 3: loaded = loadGame("save2.txt"); break;
        case 4: loaded = loadGame("save3.txt"); break;
        default: cout << "无效选择，请重试。\n"; return;
    }
    
    if (loaded) {
        gameState = GameState::PLAYING;
        cout << "存档加载成功！\n";
        cout << "欢迎回来，" << player.getName() << "！\n";
        Sleep(1000);
        system("cls");
    } else {
        gameState = GameState::MENU;
    }
}

void GameWorld::handleMenuInput(int choice) {
    switch (choice) {
        case 1: startGame(); break;
        case 2: displayHowToPlay(); break;
        case 3:
            if (gameState == GameState::PLAYING) saveGame();
            else cout << "只能在游戏进行中保存！\n";
            break;
        case 4: gameState = GameState::LOAD; break;
        case 5: gameState = GameState::ANNOUNCEMENT; break;
        case 6: cout << "感谢游玩！再见。\n"; exit(0);
        default: cout << "无效选择，请重试。\n";
    }
}

void GameWorld::run() {
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
    
    while (true) {
        switch (gameState) {
            case GameState::MENU:
                displayMainMenu();
                {
                    string input; cin >> input;
                    int choice = 0;
                    
                    try { choice = stoi(input); }
                    catch (...) {
                        if (input == "开始" || input == "新游戏") choice = 1;
                        else if (input == "帮助" || input == "玩法") choice = 2;
                        else if (input == "保存" || input == "存档") choice = 3;
                        else if (input == "读档" || input == "加载") choice = 4;
                        else if (input == "公告" || input == "更新") choice = 5;
                        else if (input == "退出" || input == "离开") choice = 6;
                        else { cout << "无效选择，请重试。\n"; continue; }
                    }
                    
                    handleMenuInput(choice);
                }
                break;
                
            case GameState::PLAYING:
                displayGameMenu();
                { string input; cin >> input; handleGameInput(input); }
                break;
                
            case GameState::GAME_OVER:
                {
                    cout << "是否要重新开始游戏？(是/否)：";
                    string choice; cin >> choice;
                    
                    if (choice == "是" || choice == "y" || choice == "Y") {
                        if (currentEnemy) { delete currentEnemy; currentEnemy = nullptr; }
                        player = Player(player.getName());
                        giveStartingItems();
                        gameState = GameState::PLAYING;
                        startGame();
                    } else {
                        cout << "感谢游玩！再见。\n";
                        return;
                    }
                }
                break;
                
            case GameState::LOAD:
                displayLoadMenu();
                {
                    string input; cin >> input;
                    int choice = 0;
                    
                    try { choice = stoi(input); }
                    catch (...) { if (input == "返回" || input == "back" || input == "b") choice = 0; }
                    
                    handleLoadInput(choice);
                }
                break;
                
            case GameState::ANNOUNCEMENT:
                displayAnnouncement();
                gameState = GameState::MENU;
                break;
                
            case GameState::EQUIPMENT:
                openEquipmentMenu();
                break;
                
            case GameState::COMBAT:
            case GameState::SHOP:
            case GameState::INVENTORY:
            case GameState::TELEPORT:
                break;
        }
    }
}

// ==================== 主函数 ====================
int main() {
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
    
    srand(static_cast<unsigned int>(time(nullptr)));
    
    cout << "欢迎来到无尽之地 文本游戏！\n";
    cout << "当前版本为V" << version << "\n";
    cout << "\n";
    cout << "V" << version << " 版本更新公告:\n";
    cout << "  1.新增装备耐久度系统：每次攻击或防御消耗耐久度\n";
    cout << "  2.装备耐久度为0时损坏，损坏后自动卸下\n";
    cout << "  3.新增装备管理功能：可以卸下装备\n";
    cout << "  4.空手状态：卸下武器后只使用基础攻击力\n";
    cout << "  5.新增传奇装备：无尽之剑、菲拉尔战甲（无限耐久）\n";
    cout << "  6.新增修理系统：损坏的装备可以修理\n";
    cout << "  7.损坏的装备出售价格减半\n";
    cout << "  8.平衡游戏经济：初始金币增加\n";
    cout << "  9.优化物品系统：所有装备现在显示耐久度\n";
    cout << "\n";
    cout << "请输入你的角色名称：";
    
    string playerName;
    cin >> playerName;
    
    if (playerName.empty()) {
        playerName = "奥瑞亚勇士";
    }
    
    GameWorld game(playerName);
    game.run();
    
    return 0;
}
