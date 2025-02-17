#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <cmath>
#include <set>
#include <sstream>
using namespace std;

// マンハッタン距離を返す
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

    //【① 二駅配置候補の抽出】
    // 各利用者の家と職場のペアについて、家→駅と職場→駅として
    // 線路費用 100 * (マンハッタン距離) が K-10000（駅2基分5000ずつ）以内なら候補に入れる
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
    
    //【② 候補評価】  
    // 各候補について、家側駅候補の半径R内と職場側駅候補の半径R内に存在する利用者数Wを計算
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
        // 候補が見つからなければ全ターン待機
        for (int t = 0; t < T; t++){
            cout << -1 << "\n";
        }
        return 0;
    }
    
    // 二駅配置候補として、駅Aと駅Bを決定
    int a_i, a_j, b_i, b_j;
    tie(a_i, a_j, b_i, b_j) = bestCandidate;
    
    //【③ 三駅配置の試み（K>=15000の場合）】
    bool useThree = false;
    int c_i = 0, c_j = 0;
    if(K >= 15000){
        // 追加駅候補は、既に配置した駅A, Bの影響範囲内にある利用者について、
        // 「家が駅Aの半径R以内ならその利用者の職場を、職場が駅Bの半径R以内なら家を」候補にする
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
        
        //【④ 追加候補の評価基準】
        // 戦略：追加駅候補C = (c_i,c_j)として、既存の駅A, Bに対し
        // 100*(マンハッタン距離(A,B) + マンハッタン距離(B,C)) <= K - 15000 である必要がある。
        // その中で、Bからの距離が最も長い（＝範囲が広い）駅を選ぶ。
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
    
    //【⑤ コマンド出力準備】  
    // 出力する駅配置や線路の座標は重複しないよう、setで管理する
    vector<string> commands;
    set<pair<int,int>> usedCoords;
    auto addCommand = [&](const string &cmd, int i, int j) {
        pair<int,int> p = {i,j};
        if(usedCoords.find(p) == usedCoords.end()){
            commands.push_back(cmd);
            usedCoords.insert(p);
        }
        
    };
    
    // 駅配置（各駅設置コストは5000）
    addCommand("0 " + to_string(a_i) + " " + to_string(a_j), a_i, a_j);
    addCommand("0 " + to_string(b_i) + " " + to_string(b_j), b_i, b_j);
    if(useThree){
        addCommand("0 " + to_string(c_i) + " " + to_string(c_j), c_i, c_j);
    }
    
    //【⑥ 線路敷設】
    // 以下は単純なマンハッタン経路を敷く例です。左右（水平）と上下（垂直）を順に進めます。
    auto addPath = [&](int sx, int sy, int ex, int ey) {
        int cx = sx, cy = sy;
        // X方向移動
        while(cx != ex){
            if(cx < ex) cx++;
            else cx--;
            addCommand("2 " + to_string(cx) + " " + to_string(cy), cx, cy);
        }
        // Y方向移動
        while(cy != ey){
            if(cy < ey) cy++;
            else cy--;
            addCommand("1 " + to_string(cx) + " " + to_string(cy), cx, cy);
        }
    };
    
    // 二駅配置の場合：A→B
    // 三駅配置の場合：A→B かつ B→C
    addPath(a_i, a_j, b_i, b_j);
    if(useThree){
        addPath(b_i, b_j, c_i, c_j);
    }
    
    //【⑦ 残りターンは待機コマンド "-1" を出力】
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
