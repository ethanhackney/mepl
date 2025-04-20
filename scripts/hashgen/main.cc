#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct kvp {
        string k;
        string v;
};

static size_t strhash(const string& s);

int main(int argc, char **argv)
{
        vector<kvp> kvps {};

        for (auto p = argv + 1; *p != nullptr; p++) {
                string path {*p};
                ifstream f {path};

                if (!f.is_open()) {
                        cerr << "could not open: " << path << endl;
                        continue;
                }

                string line;
                while (getline(f, line)) {
                        stringstream ss {line};
                        string k {};
                        string v {};

                        getline(ss, k, '=');
                        getline(ss, v);
                        kvps.push_back({ k, v });
                }

                f.close();
        }

        vector<const kvp*> hash (1);
        size_t cap = 1;

again:
        for (const auto& k : kvps) {
                auto i = strhash(k.k) % cap;
                if (hash[i] == nullptr) {
                        hash[i] = &k;
                        continue;
                }
                if (hash[i]->k == k.k) {
                        cerr << "repeat key: " << k.k << endl;
                        exit(1);
                }
                cap++;
                hash.resize(cap);
                fill(begin(hash), end(hash), nullptr);
                goto again;
        }

        cout << "struct __STRUCT__ __ID__[" << cap << "] = {\n";
        for (const auto& p : hash) {
                if (p != nullptr)
                        cout << "\t{ \"" << p->k << "\", " << p->v << " },\n";
                else
                        cout << "\t{ NULL },\n";
        }
        cout << "};\n";
}

static size_t
strhash(const string& s)
{
        size_t hash = 0;

        for (const auto& c : s)
                hash = hash * 31 + c;

        return hash;
}
