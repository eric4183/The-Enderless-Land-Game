// DataManager.hpp
#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP

#include "GameEntities.hpp"
#include <fstream>

inline Item* createItemFromString(const string& itemStr) {
    vector<string> parts;
    stringstream ss(itemStr);
    string part;
    
    while (getline(ss, part, '|')) parts.push_back(part);
    
    if (parts.size() < 4) return nullptr;
    
    string type = parts[0];
    string name = parts[1];
    string desc = parts[2];
    int value = stoi(parts[3]);
    
    if (type == "WEAPON" && parts.size() >= 8) {
        int damage = stoi(parts[4]);
        int durability = stoi(parts[5]);
        int maxDurability = stoi(parts[6]);
        bool infinite = stoi(parts[7]) > 0;
        return new Weapon(name, desc, value, damage, maxDurability, infinite);
    }
    else if (type == "ARMOR" && parts.size() >= 8) {
        int defense = stoi(parts[4]);
        int durability = stoi(parts[5]);
        int maxDurability = stoi(parts[6]);
        bool infinite = stoi(parts[7]) > 0;
        return new Armor(name, desc, value, defense, maxDurability, infinite);
    }
    else if (type == "CONSUMABLE" && parts.size() >= 5) {
        int heal = stoi(parts[4]);
        return new Consumable(name, desc, value, heal);
    }
    else if (type == "QUEST" && parts.size() >= 5) {
        string questName = parts[4];
        return new QuestItem(name, desc, value, questName);
    }
    else if (type == "TELEPORT" && parts.size() >= 5) {
        int teleType = stoi(parts[4]);
        return new TeleportScroll(name, desc, value, static_cast<TeleportType>(teleType));
    }
    
    return nullptr;
}

inline bool saveGame(const Player& player, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "无法创建存档文件！\n";
        return false;
    }
    
    file << "VERSION:" << version << endl;
    file << "PLAYER:" << player.getPlayerSaveString() << endl;
    
    if (player.getEquippedWeapon() != nullptr)
        file << "EQUIPPED_WEAPON:" << player.getEquippedWeapon()->getSaveString() << endl;
    
    if (player.getEquippedArmor() != nullptr)
        file << "EQUIPPED_ARMOR:" << player.getEquippedArmor()->getSaveString() << endl;
    
    const vector<Item*>& inventory = player.getInventory();
    file << "INVENTORY_COUNT:" << inventory.size() << endl;
    for (auto item : inventory)
        file << "ITEM:" << item->getSaveString() << endl;
    
    const vector<string>& quests = player.getCompletedQuests();
    file << "QUESTS_COUNT:" << quests.size() << endl;
    for (const auto& quest : quests)
        file << "QUEST:" << quest << endl;
    
    file.close();
    cout << "游戏已保存到 " << filename << "！\n";
    return true;
}

inline bool loadGame(Player& player, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "无法打开存档文件或存档不存在！\n";
        return false;
    }
    
    string line;
    string playerInfo;
    string equippedWeaponStr, equippedArmorStr;
    vector<string> inventoryItems;
    vector<string> quests;
    
    while (getline(file, line)) {
        if (line.find("VERSION:") == 0) continue;
        else if (line.find("PLAYER:") == 0) playerInfo = line.substr(7);
        else if (line.find("EQUIPPED_WEAPON:") == 0) equippedWeaponStr = line.substr(16);
        else if (line.find("EQUIPPED_ARMOR:") == 0) equippedArmorStr = line.substr(15);
        else if (line.find("ITEM:") == 0) inventoryItems.push_back(line.substr(5));
        else if (line.find("QUEST:") == 0) quests.push_back(line.substr(6));
    }
    
    file.close();
    
    if (!playerInfo.empty()) {
        vector<string> parts;
        stringstream ss(playerInfo);
        string part;
        
        while (getline(ss, part, '|')) parts.push_back(part);
        
        if (parts.size() >= 10) {
            player.setName(parts[0]);
            player.setLevel(stoi(parts[1]));
            player.setHealth(stoi(parts[2]));
            player.setMaxHealth(stoi(parts[3]));
            player.setAttack(stoi(parts[4]));
            player.setDefense(stoi(parts[5]));
            player.setGold(stoi(parts[6]));
            player.setExperience(stoi(parts[7]));
            player.setLocationX(stoi(parts[8]));
            player.setLocationY(stoi(parts[9]));
        }
    }
    
    player.clearInventory();
    player.setEquippedWeapon(nullptr);
    player.setEquippedArmor(nullptr);
    
    for (const auto& itemStr : inventoryItems) {
        Item* item = createItemFromString(itemStr);
        if (item != nullptr) player.addItem(item);
    }
    
    if (!equippedWeaponStr.empty()) {
        Item* weapon = createItemFromString(equippedWeaponStr);
        if (weapon != nullptr) {
            const vector<Item*>& inventory = player.getInventory();
            for (auto invItem : inventory) {
                Weapon* invWeapon = dynamic_cast<Weapon*>(invItem);
                if (invWeapon != nullptr && invWeapon->getName() == weapon->getName()) {
                    player.setEquippedWeapon(invWeapon);
                    delete weapon;
                    break;
                }
            }
            if (player.getEquippedWeapon() == nullptr) {
                Weapon* w = dynamic_cast<Weapon*>(weapon);
                if (w != nullptr) {
                    player.addItem(w);
                    player.setEquippedWeapon(w);
                } else delete weapon;
            }
        }
    }
    
    if (!equippedArmorStr.empty()) {
        Item* armor = createItemFromString(equippedArmorStr);
        if (armor != nullptr) {
            const vector<Item*>& inventory = player.getInventory();
            for (auto invItem : inventory) {
                Armor* invArmor = dynamic_cast<Armor*>(invItem);
                if (invArmor != nullptr && invArmor->getName() == armor->getName()) {
                    player.setEquippedArmor(invArmor);
                    delete armor;
                    break;
                }
            }
            if (player.getEquippedArmor() == nullptr) {
                Armor* a = dynamic_cast<Armor*>(armor);
                if (a != nullptr) {
                    player.addItem(a);
                    player.setEquippedArmor(a);
                } else delete armor;
            }
        }
    }
    
    player.setCompletedQuests(quests);
    
    cout << "游戏已从 " << filename << " 加载！\n";
    return true;
}

#endif // DATA_MANAGER_HPP
