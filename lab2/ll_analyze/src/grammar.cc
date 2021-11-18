#include "grammar.h"

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
            tmp.push_back(right);
            this->formula[left] = std::vector<std::vector<std::string>>{tmp};
        }
        else
        {
            std::vector<std::string> tmp;
            while (right.find(' ') != std::string::npos)
            {
                tmp.push_back(right.substr(0, right.find(' ')));
                right = right.substr(right.find(' ') + 1);
            }
            tmp.push_back(right);
            this->formula[left].push_back(tmp);
        }
    }
    for (auto t : this->ter)
    {
        getFirst(t);
    }
    for (auto t : this->non_ter)
    {
        getFirst(t);
    }
    getFollow();
    getTable();
}

void Grammar::getFirst(std::string s) {
    if (ter.find(s) != ter.end())
    {
        this->first[s] = {s};
        return;
    }
    std::set<std::string> tmp;
    auto &formula = this->formula[s];
    for (auto &f : formula)
    {
        if (this->ter.find(f[0]) != this->ter.end() || f[0] == "epsilon")
        {
            tmp.insert(f[0]);
        }
        else
        {
            auto rt_tmp = getFirst(f);
            tmp.insert(rt_tmp.begin(), rt_tmp.end());
        }
    }
    this->first[s] = tmp;
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
                if (this->first.find(f[i]) == this->first.end())
                {
                    getFirst(f[i]);
                }
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

void Grammar::getFollow() {
    for (auto &nt : non_ter)
    {
        follow[nt] = {};
    }
    follow[bg].insert("$");
    auto tmp_f = follow;
    do
    {
        for (auto &it : tmp_f)
        {
            follow[it.first].insert(it.second.begin(), it.second.end());
        }
        for (auto &nt : non_ter)
        {
            for (auto &p : formula)
            {
                for (auto &right : p.second)
                {
                    for (int i = 0; i < right.size() - 1; i++)
                    {
                        if (right[i] == nt)
                        {
                            auto first = getFirst(std::vector<std::string>(
                                right.begin() + i + 1, right.end()));
                            if (first.find("epsilon") != first.end())
                            {
                                first.erase(first.find("epsilon"));
                                tmp_f[nt].insert(tmp_f[p.first].begin(),
                                                 tmp_f[p.first].end());
                            }
                            tmp_f[nt].insert(first.begin(), first.end());
                        }
                    }
                    if (right[right.size() - 1] == nt)
                    {
                        tmp_f[nt].insert(tmp_f[p.first].begin(),
                                         tmp_f[p.first].end());
                    }
                }
            }
        }
    } while (tmp_f != follow);
}

void Grammar::getTable() {
    for (auto &nt : non_ter)
    {
        for (auto &t : ter)
        {
            table[{nt, t}] = nullptr;
        }
        table[{nt, "$"}] = nullptr;
    }
    for (auto &nt : non_ter)
    {
        for (auto &fml : formula[nt])
        {
            auto s = getFirst(fml);
            if (s.find("epsilon") != s.end())
            {
                s.erase(s.find("epsilon"));
                for (auto &b : follow[nt])
                {
                    table[{nt, b}] = &fml;
                }
            }
            for (auto &item : s)
            {
                table[{nt, item}] = &fml;
            }
        }
    }
}