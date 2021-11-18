#include "grammar.h"
#include <fstream>
#include <iostream>
#include <stack>

class Parser {
  public:
    Grammar *gm;
    Parser(std::string path) {
        std::fstream fin(path, std::ios::in);
        std::string non_ter, ter, bg;
        std::vector<std::string> formula;
        std::getline(fin, bg);
        std::getline(fin, non_ter);
        std::getline(fin, ter);
        while (!fin.eof())
        {
            std::string tmp;
            std::getline(fin, tmp);
            formula.push_back(tmp);
        }
        this->gm = new Grammar(bg, non_ter, ter, formula);
    }
    void parsePhrase(std::string path) {
        std::fstream fin(path, std::ios::in);
        std::vector<std::string> phrase;
        while (!fin.eof())
        {
            std::string tmp;
            fin >> tmp;
            phrase.push_back(tmp);
        }
        phrase.push_back("$");
        auto ip = 0, top = 0;
        std::vector<std::pair<int, std::string>> stk(1 << 10);
        stk[0] = {0, ""};
        do
        {
            if (gm->action.at({stk[top].first, phrase[ip]}).action_type == "S")
            {
                auto p = std::make_pair(
                    (gm->action).at({stk[top].first, phrase[ip]}).s_target,
                    phrase[ip]);
                top++;
                stk[top].first = p.first;
                stk[top].second = p.second;
                ip++;
            }
            else if (gm->action.at({stk[top].first, phrase[ip]}).action_type ==
                     "R")
            {
                auto fml = gm->action.at({stk[top].first, phrase[ip]}).fml;
                auto len = fml.second.size();
                for (auto copy = top; copy - top < len; top--)
                {
                    stk[top] = {};
                }
                auto top_sym = fml.first;
                auto st = gm->goto_.at(std::make_pair(stk[top].first, top_sym));
                stk[++top] = std::make_pair(st, top_sym);
            }
            else if (gm->action.at({stk[top].first, phrase[ip]}).action_type ==
                     "ACC")
            { return; }
        } while (true);
    }
};

int main() {
    std::string path = "./grammar.in";
    Parser psr(path);
    psr.parsePhrase("./phrase.in");
    return 0;
};
