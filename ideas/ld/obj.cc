#include <string>
#include <unordered_set>
#include <vector>
#include <iostream>

using namespace std;

struct sym {
        uint64_t def_off; // if def_off is zero, then the symbol is undefined
        string   id;
};

vector<vector<sym>> symtabs {
        {
                {
                        0xef,
                        "m",
                },
                {
                        0x00,
                        "n",
                },
        },
        {
                {
                        0xffff,
                        "n",
                },
                {
                        0x00,
                        "m",
                },
        },
};

pthread
unordered_set<string> def;
vector<string> undef;



int main(void)
{

        for (const auto& tab : symtabs) {
                for (const auto& sym : tab) {
                        if (sym.def_off) {
                                def.insert(sym.id);
                        } else {
                                // use condition variable to wait for symbols def
                                // instead
                                undef.push_back(sym.id);
                        }
                }
        }

        cout << undefs.size() << endl;
}
