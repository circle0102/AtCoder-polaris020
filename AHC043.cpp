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

    // 予算内（K-10000以内）なら候補に入れる
    vector<tuple<int,int,int,int>> candidates;
    for (int i = 0; i < M; i++){
        int hi, hj, wi, wj;
        tie(hi, hj, wi, wj) = people[i];
        int trackCost = 100 * (abs(hi - wi) + abs(hj - wj));
        if(trackCost <= K - 10000){
            candidates.push_back(make_tuple(hi, hj, wi, wj));
        }
    }
    
    sort(candidates.begin(), candidates.end());
    candidates.erase(unique(candidates.begin(), candidates.end()), candidates.end());
  
    // Wを計算
    int bestW = -1;
    tuple<int,int,int,int> bestCandidate = make_tuple(0,0,0,0);
    for (auto cand : candidates) {
        int cand_hi, cand_hj, cand_wi, cand_wj;
        tie(cand_hi, cand_hj, cand_wi, cand_wj) = cand;
        int W = 0;
        for (int i = 0; i < M; i++){
            int hi, hj, wi, wj;
            tie(hi, hj, wi, wj) = people[i];
            if ( manhattan(hi, hj, cand_hi, cand_hj) <= R &&
                 manhattan(wi, wj, cand_wi, cand_wj) <= R )
            {
                W++;
            }
        }
        if(W > bestW){
            bestW = W;
            bestCandidate = cand;
        }
    }
    
    if(bestW < 0){
        // 待機
        for (int t = 0; t < T; t++){
            cout << -1 << "\n";
        }
        return 0;
    }
    
    // 二駅配置候補
    int a_i, a_j, b_i, b_j;
    tie(a_i, a_j, b_i, b_j) = bestCandidate;
    
    //三駅配置の試み(K>=15000の場合)
    bool useThree = false;
    int c_i = 0, c_j = 0;
    if(K >= 15000){
        // 追加駅候補:A,Bの範囲内
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
        /*
        戦略：
        追加駅候補C=(c_i,c_j)として、既存駅A,Bに対し、
        100*(マンハッタン距離(A,B) + マンハッタン距離(B,C)) <= K - 1500　かつ
        Bからの距離が最も"長い"もの
        */
        pair<int,int> bestAdd = {-1, -1};
        int bestDist = -1;
        int baseCost = manhattan(a_i, a_j, b_i, b_j);
        for(auto &p : addCandidates){
            int cost = baseCost + manhattan(b_i, b_j, p.first, p.second);
            if(100 * cost <= K - 15000){
                int d = manhattan(b_i, b_j, p.first, p.second);
                if(d > bestDist){
                    bestDist = d;
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
    
    //コマンド出力  
    // 出力する座標は重複なし、setで管理
    vector<string> commands;
    set<pair<int,int>> usedCoords;
    auto addCommand = [&](const string &cmd, int i, int j) {
        pair<int,int> p = {i,j};
        if(usedCoords.find(p) == usedCoords.end()){
            commands.push_back(cmd);
            usedCoords.insert(p);
        }
    };
    
    // 駅配置
    addCommand("0 " + to_string(a_i) + " " + to_string(a_j), a_i, a_j);
    addCommand("0 " + to_string(b_i) + " " + to_string(b_j), b_i, b_j);
    if(useThree){
        addCommand("0 " + to_string(c_i) + " " + to_string(c_j), c_i, c_j);
    }
    
    //線路敷設
    // Y方向の最後の出力を曲がり線路に変更します。
    auto addPath = [&](int sx, int sy, int ex, int ey, int sta_i, int sta_j, int stb_i, int stb_j) {
        int cx = sx, cy = sy;
        // Y方向移動
        while(cy != ey){
            int nextY = (sy < ey) ? cy + 1 : cy - 1;
            // 曲がり
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
            }
            else{
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
    
    // 二駅配置：A→B
    // 三駅配置：A→B かつ B→C
    addPath(a_i, a_j, b_i, b_j, a_i, a_j, b_i, b_j);
    if(useThree){
        addPath(b_i, b_j, c_i, c_j, b_i, b_j, c_i, c_j);
    }
    
    //待機
    while((int)commands.size() < T){
        commands.push_back("-1");
    }
    if(commands.size() > (size_t)T){
        commands.resize(T);
    }
    
    for(auto &cmd : commands){
        cout << cmd << "\n";
    }
    
    return 0;
}
