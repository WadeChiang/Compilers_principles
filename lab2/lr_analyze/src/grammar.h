#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

struct Item {
    std::pair<std::string, std::vector<std::string>> fml;
    int dot;
    std::set<std::string> fwd;
    Item(std::pair<std::string, std::vector<std::string>> fml, int d,
         std::set<std::string> fwd) {
        this->fml = fml;
        dot = d;
        this->fwd = fwd;
    }
    bool operator==(const Item &other) const {
        if (this->fml == other.fml && this->dot == other.dot &&
            this->fwd == other.fwd)
        {
            return true;
        }
        return false;
    }
};

struct ItemSet {
    int id;
    std::vector<Item> items;
    std::map<std::string, int> transfer;
    ItemSet();
    std::vector<Item>::iterator
    findFormula(std::pair<std::string, std::vector<std::string>>, int);
    bool operator==(const ItemSet &other) const {
        if (items == other.items)
        {
            return true;
        }
        return false;
    }
    ItemSet &operator=(const ItemSet other) {
        id = other.id;
        items = other.items;
        transfer = other.transfer;
        return *this;
    }
};

struct ActionObject {
    std::string action_type;
    int s_target;
    std::pair<std::string, std::vector<std::string>> fml;
    ActionObject() { s_target = 0; }
    ActionObject(std::string at) {
        action_type = at;
        s_target = 0;
    }
};

class Grammar {
  public:
    std::string bg;
    std::set<std::string> non_ter;
    std::set<std::string> ter;
    std::map<std::string, std::vector<std::vector<std::string>>> formula;
    std::map<std::string, std::set<std::string>> first;
    std::vector<ItemSet> dfa;
    std::map<std::pair<int, std::string>, ActionObject> action;
    std::map<std::pair<int, std::string>, int> goto_;

    Grammar(std::string bg, std::string non_ter, std::string ter,
            std::vector<std::string> formula);

    void getFirst();
    std::set<std::string> getFirst(std::vector<std::string>);

    void getDFA();
    ItemSet getClosure(ItemSet);
    ItemSet go(ItemSet, std::string);
    void getTable();
};