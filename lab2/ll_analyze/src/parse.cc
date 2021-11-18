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
        std::vector<std::string> buf;
        std::fstream fin(path, std::ios::in);
        while (!fin.eof())
        {
            std::string s;
            fin >> s;
            buf.push_back(s);
        }
        buf.push_back("$");
        auto ip = 0;
        std::vector<std::string> stk(1 << 10);
        stk[0] = "$";
        stk[1] = gm->bg;
        auto top = 1;
        std::string out = "";

        do
        {
            std::cout << "Stack: ";
            for (size_t i = 0; i <= top; i++)
            {
                std::cout << stk[i];
            }
            std::cout << std::endl << "Buffer: ";
            for (size_t i = ip; i < buf.size(); i++)
            {
                std::cout << buf[i];
            }
            std::cout << std::endl << "Output: " << out << "\n\n";
            out = "";
            if (gm->ter.find(stk[top]) != gm->ter.end() || stk[top] == "$")
            {
                if (stk[top] == buf[ip])
                {
                    stk[top--] = "";
                    ip++;
                }
                else
                {
                    std::cout << "Stack top: " << stk[top]
                              << ", buffer: " << buf[ip]
                              << ". Parsing Failed.\n";
                    exit(0);
                }
            }
            else
            {
                auto fml = gm->table[{stk[top], buf[ip]}];
                out = stk[top] + " ->";
                for (auto &s : *fml)
                {
                    out += " " + s;
                }
                stk[top--] = "";
                for (int i = (*fml).size() - 1;
                     i >= 0 && (*fml)[0] != "epsilon"; --i)
                {
                    stk[++top] = (*fml)[i];
                }
            }
        } while (stk[0] != "");
    }
};

int main() {
    std::string path = "../grammar.in";
    Parser psr(path);
    psr.parsePhrase("../phrase.in");
    return 0;
}