#include "grammar.h"
#include <algorithm>
#include <fstream>
#include <iostream>
Grammar::Grammar(std::string bg, std::string non_ter, std::string ter,
                 std::vector<std::string> formula) {
    this->bg = bg;
    while (non_ter.find(' ') != std::string::npos)
    {
        this->non_ter.insert(non_ter.substr(0, non_ter.find(' ')));
        non_ter = non_ter.substr(non_ter.find(' ') + 1);
    }
    this->non_ter.insert(non_ter);

    while (ter.find(' ') != std::string::npos)
    {
        this->ter.insert(ter.substr(0, ter.find(' ')));
        ter = ter.substr(ter.find(' ') + 1);
    }
    this->ter.insert(ter);

    for (auto s : formula)
    {
        std::string left, right;
        left = s.substr(0, s.find('-'));
        right = s.substr(s.find('>') + 2);
        if (this->formula.find(left) == this->formula.end())
        {
            std::vector<std::string> tmp;
            while (right.find(' ') != std::string::npos)
            {
                tmp.push_back(right.substr(0, right.find(' ')));
                right = right.substr(right.find(' ') + 1);
            }
            if (right != "epsilon")
            {
                tmp.push_back(right);
            }
            this->formula[left] = std::vector<std::vector<std::string>>({tmp});
        }
        else
        {
            std::vector<std::string> tmp;
            while (right.find(' ') != std::string::npos)
            {
                tmp.push_back(right.substr(0, right.find(' ')));
                right = right.substr(right.find(' ') + 1);
            }
            if (right != "epsilon")
            {
                tmp.push_back(right);
            }
            this->formula[left].push_back(tmp);
        }
    }
    getFirst();
    getDFA();
    getTable();
}

void Grammar::getFirst() {
    std::set<std::string> sym = ter;
    sym.insert(non_ter.begin(), non_ter.end());
    for (auto &s : sym)
    {
        first[s] = {};
    }
    first["$"] = {"$S"};
    auto first_old = first;

    do
    {
        first_old = first;
        for (auto &s : sym)
        {
            if (ter.find(s) != ter.end())
            {
                this->first[s] = {s};
                continue;
            }
            auto &formula = this->formula[s];
            for (auto &f : formula)
            {
                if (this->ter.find(f[0]) != this->ter.end() ||
                    f[0] == "epsilon")
                {
                    first[s].insert(f[0]);
                }
                else
                {
                    int cnt_epsl = 0, cnt_new = 0;
                    for (int i = 0; i < f.size(); ++i)
                    {
                        if (this->non_ter.find(f[i]) != this->non_ter.end())
                        {
                            for (auto &terminator : first.find(f[i])->second)
                            {
                                if (terminator != "epsilon")
                                {
                                    first[s].insert(terminator);
                                }
                                else
                                { cnt_new++; }
                            }
                            if (cnt_new != cnt_epsl)
                            {
                                cnt_epsl = cnt_new;
                            }
                            else
                            { break; }
                        }
                        else
                        {
                            first[s].insert(f[i]);
                            break;
                        }
                    }
                    if (cnt_epsl == f.size())
                    {
                        first[s].insert("epsilon");
                    }
                }
            }
        }
    } while (first != first_old);
}

std::set<std::string> Grammar::getFirst(std::vector<std::string> f) {
    std::set<std::string> tmp;
    if (this->ter.find(f[0]) != this->ter.end() || f[0] == "epsilon")
    {
        tmp.insert(f[0]);
    }
    else
    {
        int cnt_epsl = 0, cnt_new = 0;
        for (int i = 0; i < f.size(); ++i)
        {
            if (this->non_ter.find(f[i]) != this->non_ter.end())
            {
                for (auto &terminator : (*this->first.find(f[i])).second)
                {
                    if (terminator != "epsilon")
                    {
                        tmp.insert(terminator);
                    }
                    else
                    { cnt_new++; }
                }
                if (cnt_new != cnt_epsl)
                {
                    cnt_epsl = cnt_new;
                }
                else
                { break; }
            }
            else
            {
                tmp.insert(f[i]);
                break;
            }
        }
        if (cnt_epsl == f.size())
        {
            tmp.insert("epsilon");
        }
    }
    return tmp;
}

ItemSet Grammar::getClosure(ItemSet is) {
    ItemSet j = is, j_new = j;
    do
    {
        j_new = j;
        for (int i = 0; i < j_new.items.size(); ++i)
        {
            if (j.items[i].fml.second.size() <= j.items[i].dot)
            {
                continue;
            }
            auto nt = j_new.items[i].fml.second[j.items[i].dot];
            if (non_ter.find(nt) == non_ter.end())
            {
                continue;
            }
            for (auto &fwd : j_new.items[i].fwd)
            {
                std::vector<std::string> back;
                for (int k = 1; j_new.items[i].fml.second.begin() +
                                    j_new.items[i].dot + k !=
                                j_new.items[i].fml.second.end();
                     k++)
                {
                    back.push_back(
                        j_new.items[i].fml.second[j_new.items[i].dot + k]);
                }
                back.push_back(fwd);
                auto fset = getFirst(back);
                if (fset.find("epsilon") != fset.end())
                {
                    fset.erase(fset.find("epsilon"));
                }
                for (auto &fml : formula[nt])
                {
                    Item new_item({nt, fml}, 0, fset);
                    auto itr = j.findFormula({nt, fml}, 0);
                    if (itr == j.items.end())
                    {
                        j.items.push_back(Item({nt, fml}, 0, fset));
                    }
                    else
                    { itr->fwd.insert(fset.begin(), fset.end()); }
                }
            }
        }
    } while (j_new.items != j.items);
    return j;
}

void Grammar::getDFA() {
    ItemSet origin{};
    origin.items.push_back(Item({bg, formula[bg][0]}, 0, {"$"}));
    dfa.push_back(getClosure(origin));
    dfa[0].id = 0;
    auto copy = dfa;
    do
    {
        copy = dfa;
        for (int i = 0; i < copy.size(); ++i)
        {
            std::set<std::string> sym = ter;
            sym.insert(non_ter.begin(), non_ter.end());
            for (auto &t : sym)
            {
                ItemSet new_set{};
                new_set = go(dfa[i], t);
                if (new_set.items.empty() == false)
                {
                    auto itr = std::find(dfa.begin(), dfa.end(), new_set);
                    if (itr != dfa.end())
                    {
                        dfa[i].transfer.insert({t, itr - dfa.begin()});
                    }
                    else
                    {
                        int seq = dfa.end() - dfa.begin();
                        dfa.push_back(new_set);
                        dfa[seq].id = seq;
                        dfa[i].transfer.insert({t, seq});
                    }
                }
            }
        }
    } while (copy != dfa);
}

ItemSet Grammar::go(ItemSet is, std::string sym) {
    ItemSet tmp;
    for (auto &item : is.items)
    {
        if (item.fml.second.size() > item.dot &&
            item.fml.second[item.dot] == sym)
        {
            tmp.items.push_back(Item(item.fml, item.dot + 1, item.fwd));
        }
    }
    return getClosure(tmp);
}

std::vector<Item>::iterator
ItemSet::findFormula(std::pair<std::string, std::vector<std::string>> formula,
                     int dot_pos) {
    std::vector<Item>::iterator itr = items.begin();
    for (; itr != items.end(); itr++)
    {
        if (itr->fml == formula && itr->dot == dot_pos)
        {
            return itr;
        }
    }
    return items.end();
}

void Grammar::getTable() {
    std::fstream fa("action.out", std::ios::out), fg("goto.out", std::ios::out);
    auto ter_sym = ter;
    ter_sym.insert("$");
    for (int i = 0; i < dfa.size(); ++i)
    {
        for (auto &s : ter_sym)
        {
            for (auto &item : dfa[i].items)
            {
                if (item.fml.second.size() == item.dot)
                {
                    if (item.fml.first == bg)
                    {
                        auto ao = ActionObject{"ACC"};
                        action[{i, "$"}] = ao;
                    }
                    else
                    {
                        action[{i, s}] = ActionObject{"R"};
                        action[{i, s}].fml = item.fml;
                    }
                }
                else
                {
                    if (item.fml.second[item.dot] == s)
                    {
                        auto target = dfa[i].transfer[s];
                        action[{i, s}] = ActionObject{"S"};
                        action[{i, s}].s_target = target;
                        fa << "action[{" << i << ", " << s << "}] = " << target
                           << std::endl;
                    }
                }
            }
        }
        for (auto &s : non_ter)
        {
            auto itr = dfa[i].transfer.find(s);
            if (itr != dfa[i].transfer.end())
            {
                goto_[{i, s}] = itr->second;
                fg << "goto_[{" << i << ", " << s << "}] = " << itr->second
                   << std::endl;
            }
        }
    }
}

ItemSet::ItemSet() { id = 0; }