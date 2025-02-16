#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <cmath>
using namespace std;

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
    
    const int R = 2;  // 評価の半径

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

    int bestW = -1;
    tuple<int,int,int,int> bestCandidate = make_tuple(0,0,0,0);
    for (auto cand : candidates) {
        int cand_hi, cand_hj, cand_wi, cand_wj;
        tie(cand_hi, cand_hj, cand_wi, cand_wj) = cand;
        int W = 0;
        for (int i = 0; i < M; i++){
            int hi, hj, wi, wj;
            tie(hi, hj, wi, wj) = people[i];
            if ( abs(hi - cand_hi) + abs(hj - cand_hj) <= R &&
                 abs(wi - cand_wi) + abs(wj - cand_wj) <= R )
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
        for (int t = 0; t < T; t++){
            cout << -1 << "\n";
        }
        return 0;
    }

    int a_i, a_j, b_i, b_j;
    tie(a_i, a_j, b_i, b_j) = bestCandidate;
    
    vector<string> commands;
    commands.push_back("0 " + to_string(a_i) + " " + to_string(a_j));
    commands.push_back("0 " + to_string(b_i) + " " + to_string(b_j));
    
    int x = a_i, y = a_j;
    
    // X方向移動
    while(abs(x - b_i) > 0){
        int prev_x = x;
        if(x < b_i) x++;
        else x--;

        if(abs(x - b_i) > 0){
            commands.push_back("2 " + to_string(x) + " " + to_string(y));
        }
        // 曲がる
        else {
            if(a_i < b_i && a_j < b_j) commands.push_back("5 " + to_string(x) + " " + to_string(y));
            if(a_i < b_i && a_j > b_j) commands.push_back("4 " + to_string(x) + " " + to_string(y));
            if(a_i > b_i && a_j < b_j) commands.push_back("6 " + to_string(x) + " " + to_string(y));
            if(a_i > b_i && a_j > b_j) commands.push_back("3 " + to_string(x) + " " + to_string(y));
        }
    }
    
    // Y方向移動
    while(abs(y - b_j) > 1){
        if(y < b_j) y++;
        else y--;
        commands.push_back("1 " + to_string(x) + " " + to_string(y));
    }
    
    while((int)commands.size() < T){
        commands.push_back("-1");
    }
    
    for (int i = 0; i < T; i++){
        cout << commands[i] << "\n";
    }
    
    return 0;
}
