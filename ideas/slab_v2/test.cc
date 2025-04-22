#include <unordered_map>
#include <string>
#include <vector>

/** TODO: write code to pretty print table
 *
 * we know the largest key before hand so we do not
 * even have to compute that, so alignment should
 * be easy
 */

using namespace std;

static size_t hashfn(const string& s);

int
main(void)
{
        vector<string> keys {
                "AST_VAR_DEF",
                "AST_STR",
        };
        unordered_map<string,string> vals {
                { "AST_VAR_DEF", "&avd.base" },
                { "AST_STR",     "&ss.base"  },
        };
        unordered_map<size_t,string> seen;
        size_t hash_cap = 1;

again:
        for (auto& k : keys) {
                auto hash = hashfn(k) % hash_cap;
                auto p = seen.find(hash);
                if (p == seen.end()) {
                        seen[hash] = k;
                        continue;
                }
                seen.clear();
                ++hash_cap;
                goto again;
        }

        printf("static const struct slab_base *slabtab[%zu] = {\n", hash_cap);
        auto kp = keys.begin();
        for (size_t i = 0; i < hash_cap; ++i) {
                auto p = seen.find(i);
                if (p == seen.end()) {
                        printf("\t{ NULL, NULL },\n");
                } else {
                        printf("\t{ \"%s\", %s },\n", (*kp).c_str(), vals[*kp].c_str());
                        ++kp;
                }
        }
        printf("};\n");
}

static size_t
hashfn(const string& s)
{
        size_t hash = 0;

        for (auto& c : s)
                hash = hash * 31 + c;

        return hash;
}
