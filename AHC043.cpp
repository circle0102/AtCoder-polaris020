#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <cmath>
#include <set>
#include <sstream>
#include <map>
using namespace std;

// マンハッタン距離
int manhattan(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int N, M, K, T;
    cin >> N >> M >> K >> T;
    vector<tuple<int,int,int,int>> people(M);
    for (int i = 0; i < M; i++){
        int hi, hj, wi, wj;
        cin >> hi >> hj >> wi >> wj;
        people[i] = make_tuple(hi, hj, wi, wj);
    }
    
    const int R = 2;  // 評価半径

    vector<tuple<int,int,int,int>> candidates;
    for (int i = 0; i < M; i++){
        int hi, hj, wi, wj;
        tie(hi, hj, wi, wj) = people[i];
        int trackCost = 100 * (manhattan(hi, hj, wi, wj));
        if(trackCost <= K - 10000){
            candidates.push_back(make_tuple(hi, hj, wi, wj));
        }
    }
    sort(candidates.begin(), candidates.end());
    candidates.erase(unique(candidates.begin(), candidates.end()), candidates.end());
    
    int bestW = -1;
    int bestDist = -1;
    tuple<int,int,int,int> bestCandidate = make_tuple(0,0,0,0);
    for (auto cand : candidates) {
        int a_i, a_j, b_i, b_j;
        tie(a_i, a_j, b_i, b_j) = cand;
        int W = 0;
        for (int i = 0; i < M; i++){
            int hi, hj, wi, wj;
            tie(hi, hj, wi, wj) = people[i];
            if(manhattan(hi, hj, a_i, a_j) <= R && manhattan(wi, wj, b_i, b_j) <= R)
                W++;
        }
        int dist = manhattan(a_i, a_j, b_i, b_j);
        if(W > bestW || (W == bestW && dist > bestDist)){
            bestW = W;
            bestDist = dist;
            bestCandidate = cand;
        }
    }
    
    if(bestW < 0){
        for (int t = 0; t < T; t++){
            cout << -1 << "\n";
        }
        return 0;
    }
    
    int a_i, a_j, b_i, b_j;
    tie(a_i, a_j, b_i, b_j) = bestCandidate;
    
    bool useThree = false;
    int c_i = 0, c_j = 0;
    if(K >= 15000){
        vector<pair<int,int>> addCandidates;
        for (int i = 0; i < M; i++){
            int hi, hj, wi, wj;
            tie(hi, hj, wi, wj) = people[i];
            if(manhattan(hi, hj, a_i, a_j) <= R){
                if(!(wi == a_i && wj == a_j) && !(wi == b_i && wj == b_j))
                    addCandidates.push_back({wi, wj});
            }
            if(manhattan(wi, wj, b_i, b_j) <= R){
                if(!(hi == a_i && hj == a_j) && !(hi == b_i && hj == b_j))
                    addCandidates.push_back({hi, hj});
            }
        }
        sort(addCandidates.begin(), addCandidates.end());
        addCandidates.erase(unique(addCandidates.begin(), addCandidates.end()), addCandidates.end());
        
        int baseCost = manhattan(a_i, a_j, b_i, b_j);
        int bestWp = -1;
        int bestAddDist = -1;
        pair<int,int> bestAdd = {-1, -1};
        for(auto &p : addCandidates){
            int cost = baseCost + manhattan(b_i, b_j, p.first, p.second);
            if(100 * cost <= K - 15000){
                int Wp = 0;
                for (int i = 0; i < M; i++){
                    int hi, hj, wi, wj;
                    tie(hi, hj, wi, wj) = people[i];
                    if(manhattan(hi, hj, p.first, p.second) <= R || manhattan(wi, wj, p.first, p.second) <= R)
                        Wp++;
                }
                int d = manhattan(b_i, b_j, p.first, p.second);
                if(Wp > bestWp || (Wp == bestWp && d > bestAddDist)){
                    bestWp = Wp;
                    bestAddDist = d;
                    bestAdd = p;
                }
            }
        }
        if(bestAdd.first != -1){
            useThree = true;
            c_i = bestAdd.first;
            c_j = bestAdd.second;
        }
    }
    
    vector<string> commands;
    set<pair<int,int>> usedCoords;
    auto addCommand = [&](const string &cmd, int i, int j) {
        pair<int,int> p = {i,j};
        if(usedCoords.find(p) == usedCoords.end()){
            commands.push_back(cmd);
            usedCoords.insert(p);
        }
    };
    
    addCommand("0 " + to_string(a_i) + " " + to_string(a_j), a_i, a_j);
    addCommand("0 " + to_string(b_i) + " " + to_string(b_j), b_i, b_j);
    if(useThree){
        addCommand("0 " + to_string(c_i) + " " + to_string(c_j), c_i, c_j);
    }
    
    auto addPath = [&](int sx, int sy, int ex, int ey, int sta_i, int sta_j, int stb_i, int stb_j) {
        int cx = sx, cy = sy;
        // Y方向移動
        while(cy != ey){
            int nextY = (cy < ey) ? cy + 1 : cy - 1;
            if(nextY == ey){
                if(sta_i < stb_i && sta_j < stb_j)
                    addCommand("3 " + to_string(cx) + " " + to_string(nextY), cx, nextY);
                else if(sta_i < stb_i && sta_j > stb_j)
                    addCommand("6 " + to_string(cx) + " " + to_string(nextY), cx, nextY);
                else if(sta_i > stb_i && sta_j < stb_j)
                    addCommand("4 " + to_string(cx) + " " + to_string(nextY), cx, nextY);
                else if(sta_i > stb_i && sta_j > stb_j)
                    addCommand("5 " + to_string(cx) + " " + to_string(nextY), cx, nextY);
                cy = nextY;
            } else {
                cy = nextY;
                addCommand("1 " + to_string(cx) + " " + to_string(cy), cx, cy);
            }
        }
        // X方向移動
        while(cx != ex){
            int nextX = (cx < ex) ? cx + 1 : cx - 1;
            cx = nextX;
            addCommand("2 " + to_string(cx) + " " + to_string(cy), cx, cy);
        }
    };

    addPath(a_i, a_j, b_i, b_j, a_i, a_j, b_i, b_j);
    if(useThree){
        addPath(b_i, b_j, c_i, c_j, b_i, b_j, c_i, c_j);
    }
    
    while((int)commands.size() < T){
        commands.push_back("-1");
    }
    if(commands.size() > (size_t)T)
        commands.resize(T);
    
    for(auto &cmd : commands)
        cout << cmd << "\n";
    
    return 0;
}
