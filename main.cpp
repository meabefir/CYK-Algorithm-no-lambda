#include <iostream>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

ifstream f ("cyk.in");

const string LAMBDA = "e";

// luat de pe net, desparte un string dupa un delimitator
void tokenize(std::string const &str, const char delim,
            std::vector<std::string> &out)
{
    // construct a stream from the string
    std::stringstream ss(str);

    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}

// pune in al doilea parametru neterminalele care duc la primul string
void calcNeterminaleCareDucLa(string sub, set<string>& neterminale, unordered_map<string, set<string>>& cfg, bool lambdaBug = false)
{
    for (auto& per: cfg)
    {
        for (auto& s: per.second)
        {
            if (s == sub || (lambdaBug && s == LAMBDA))
            {
                neterminale.insert(per.first);
                continue;
            }
        }
    }
}

unordered_map<string, set<string>> cfg;

int main()
{
    // nr de randuri
    int n;
    f >> n;

    // neterminala de start
    string simbolStart;
    f >> simbolStart;

    f.ignore();

    for (int i = 0; i < n; i++)
    {
        string line;
        getline(f, line);

        // despart dupa space
        vector<string> split_vector;
        tokenize(line, ' ', split_vector);

        // split_vector[0] o sa aiba neterminala din care pleaca, split_vector[2] o sa aiba productiile in care merge
        string neterminala = split_vector[0];
        set<string> productii;

        string s_productii = split_vector[2];
        split_vector.clear();

        tokenize(s_productii, '|', split_vector);
        // acum in split vector am fiecare productie

        for (auto& s: split_vector)
            productii.insert(s);

        // daca neterminalul exista deja in map, adaugam si stringurile din setul curent
        // daca nu exista, facem map[neterminala] = productii

        if (cfg.find(neterminala) != cfg.end())
        {
            for (auto& s: productii)
                cfg[neterminala].insert(s);
        }
        else
        {
            cfg[neterminala] = productii;
        }
    }

    #if 0
    // elimin lambda productions
    bool hasLambda = true;
    while (hasLambda)
    {
        set<string> neterminaleLambda;
        hasLambda = false;
        for (auto& per: cfg)
        {
            bool eraseLambdaHere = false;

            for (auto& s: per.second)
                if (s == LAMBDA)
                {
                    hasLambda = true;
                    neterminaleLambda.insert(per.first);
                    eraseLambdaHere = true;
                }
            if (eraseLambdaHere)
            {
                per.second.erase(LAMBDA);
            }
        }

        for (auto& s: neterminaleLambda)
    }
    #endif

    for (auto& k: cfg)
    {
        cout << k.first << " -> ";
        for (auto& s: k.second)
            cout << s << "|";
        cout << '\n';
    }

    int nrCuvinte;
    f >> nrCuvinte;

    for (int k = 0; k < nrCuvinte; k++)
    {
        string cuvant;
        f >> cuvant;
        int lCuv = cuvant.length();

        // construiesc un fel de matrice pe care o completez doar sub diagonala principala, incluzand diagonala
        // (matrice este usor de inteles vizual, eu o sa tin informatile generate intr-un map

        // map.first va contine substringuri ale cuvantului
        // map.second va contine neterminalele care pot duce la acest substring
        unordered_map<string, set<string>> d;

        // generez prima linie din matrice
        for (int i = 0; i < lCuv; i++)
        {
            string litera = string(1, cuvant[i]);
            // pt fiecare cuvant retin neterminala de unde il pot obtine

            set<string> neterminale;
            calcNeterminaleCareDucLa(litera, neterminale, cfg);

            d[litera] = neterminale;
        }

        // pt urmatoarele n-1 linii, generez pt fiecare toate modurile de a desparti cuvantul in 2 cuvinte
        for (int i = 2; i <= lCuv; i++)
        {
            //cout << i << '\n';
            for (int j = 0; j < lCuv - i + 1; j++)
            {
                string subCuvant = cuvant.substr(j, i);
                set<string> neterminaliSubCuvant;

                //cout << subCuvant << '\n';
                // acum impart fiecare subcuvant in alte 2 subcuvinte (toate posibilitatile)
                int lSubCuvant = subCuvant.length();

                for (int l = 1; l < lSubCuvant; l++)
                {
                    string s1 = subCuvant.substr(0, l);
                    string s2 = subCuvant.substr(l, lSubCuvant - l);

                    // aici facem produs cartezian intre neterminalii care se gasesc in d[s1] si d[s2]

                    set<string> neterminaliS1;
                    if (d.find(s1) != d.end())
                        neterminaliS1 = d[s1];
                    else
                        continue;

                    set<string> neterminaliS2;
                    if (d.find(s2) != d.end())
                        neterminaliS2 = d[s2];
                    else
                        continue;

                    // daca am ajuns aici inseamna ca avem intre ce face produs cartezian

                    set<string> produsCartezian;
                    for (auto& ss1: neterminaliS1)
                    {
                        for (auto& ss2: neterminaliS2)
                        {
                            string tmp;
                            tmp += ss1;
                            tmp += ss2;

                            // verificam ce neterminale duc in productia tmp
                            set<string> neterminale;
                            calcNeterminaleCareDucLa(tmp, neterminale, cfg);

                            neterminaliSubCuvant.insert(neterminale.begin(), neterminale.end());
                        }
                    }

                    //cout << s1 << ' ' << s2 << " - ";
                }
                //cout << '\n';

                d[subCuvant] = neterminaliSubCuvant;

                // !!!!!!!!
                //for (auto& s: neterminaliSubCuvant)
                //    cout << s << ' ';
                //cout << '\n';
            }
            //cout << '\n';
        }

        // verifica daca exista simbolul de start in d[cuvant]
        bool found = false;
        for (auto& s: d[cuvant])
            if (s == simbolStart)
            {
                found = true;
                break;
            }
        if (found)
            cout << cuvant << " poate fi generat de gramatica\n";
        else
            cout << cuvant << " NU poate fi generat de gramatica\n";
    }

    return 0;
}
