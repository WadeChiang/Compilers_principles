#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

class Grammar {
  public:
    std::string bg;
    std::set<std::string> non_ter;
    std::set<std::string> ter;
    std::map<std::string, std::vector<std::vector<std::string>>> formula;
    std::map<std::string, std::set<std::string>> first;
    std::map<std::string, std::set<std::string>> follow;
    std::map<std::pair<std::string, std::string>, std::vector<std::string> *>
        table;
    Grammar(std::string bg, std::string non_ter, std::string ter,
            std::vector<std::string> formula);
    void getFirst(std::string);
    std::set<std::string> getFirst(std::vector<std::string>);
    void getFollow();
    void getTable();
};