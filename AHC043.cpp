#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <algorithm>
#include <queue>
using namespace std;

// 最短経路を求めるためのBFS
vector<pair<int, int>> bfs(int N, vector<vector<int>>& grid, pair<int, int> start, pair<int, int> goal) {
    vector<vector<int>> dist(N, vector<int>(N, -1));
    vector<vector<pair<int, int>>> parent(N, vector<pair<int, int>>(N, {-1, -1}));
    queue<pair<int, int>> q;

    dist[start.first][start.second] = 0;
    q.push(start);

    // 上下左右の移動
    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        
        // 目標地点に到達した場合、経路を復元
        if (current == goal) {
            vector<pair<int, int>> path;
            while (current != start) {
                path.push_back(current);
                current = parent[current.first][current.second];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        // 上下左右に移動
        for (auto& dir : directions) {
            int ni = current.first + dir.first;
            int nj = current.second + dir.second;
            if (ni >= 0 && nj >= 0 && ni < N && nj < N && dist[ni][nj] == -1 && grid[ni][nj] != 1) {
                dist[ni][nj] = dist[current.first][current.second] + 1;
                parent[ni][nj] = current;
                q.push({ni, nj});
            }
        }
    }

    return {}; // 経路が見つからなかった場合
}

int main() {
    int N, M, K, T;
    cin >> N >> M >> K >> T;

    // 入力
    vector<tuple<int, int, int, int>> people(M);
    for (int i = 0; i < M; ++i) {
        int i_s, j_s, i_t, j_t;
        cin >> i_s >> j_s >> i_t >> j_t;
        people[i] = make_tuple(i_s, j_s, i_t, j_t);
    }

    // 家と職場の密度をそれぞれ計算
    vector<vector<int>> grid(N, vector<int>(N, 0));
    // 家の位置をカウント
    for (const auto& person : people) {
        grid[get<0>(person)][get<1>(person)]++;
    }
    // 職場の位置をカウント
    for (const auto& person : people) {
        grid[get<2>(person)][get<3>(person)]++;
    }

    // 家の密度に基づいてソート
    vector<pair<int, int>> home_locations;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] > 0) {
                home_locations.push_back({grid[i][j], i * N + j});
            }
        }
    }
    sort(home_locations.rbegin(), home_locations.rend());

    // 職場の密度に基づいてソート
    vector<pair<int, int>> workplace_locations;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] > 0) {
                workplace_locations.push_back({grid[i][j], i * N + j});
            }
        }
    }
    sort(workplace_locations.rbegin(), workplace_locations.rend());

    // 駅の数を決定
    int station = K / 10000;
    int KK = K % 10000;

    // 家の位置と職場の位置をセットで管理
    vector<pair<int, int>> home_set;
    vector<pair<int, int>> workplace_set;
    for (const auto& person : people) {
        home_set.push_back({get<0>(person), get<1>(person)});
        workplace_set.push_back({get<2>(person), get<3>(person)});
    }

    // 駅の配置を行う
    vector<pair<int, int>> Stations(station * 2);
    int home_station_idx = 0;
    int workplace_station_idx = 0;

    // 家の密集地に駅を配置
    for (int i = 0; i < station; i++) {
        while (true) {
            int idx = home_locations[home_station_idx % home_locations.size()].second;
            int I = idx / N;
            int J = idx % N;
            if (find(home_set.begin(), home_set.end(), make_pair(I, J)) != home_set.end()) {
                Stations[i].first = I;
                Stations[i].second = J;
                home_station_idx++;
                break;
            }
            home_station_idx++;
        }
    }

    // 職場の密集地に駅を配置
    for (int i = station; i < station * 2; i++) {
        while (true) {
            int idx = workplace_locations[workplace_station_idx % workplace_locations.size()].second;
            int I = idx / N;
            int J = idx % N;
            if (find(workplace_set.begin(), workplace_set.end(), make_pair(I, J)) != workplace_set.end()) {
                Stations[i].first = I;
                Stations[i].second = J;
                workplace_station_idx++;
                break;
            }
            workplace_station_idx++;
        }
    }

    // 駅間の距離を調整
    for (int i = 0; i < station; i++) {
        for (int j = station; j < station * 2; j++) {
            int dist = abs(Stations[i].first - Stations[j].first) + abs(Stations[i].second - Stations[j].second);
            if (100 * dist > KK) {
                int diff = 100 * dist - KK;
                if (Stations[i].first != Stations[j].first) {
                    if (Stations[i].first < Stations[j].first) {
                        Stations[i].first += diff;
                    } else {
                        Stations[i].first -= diff;
                    }
                }
                if (Stations[i].second != Stations[j].second) {
                    if (Stations[i].second < Stations[j].second) {
                        Stations[i].second += diff;
                    } else {
                        Stations[i].second -= diff;
                    }
                }
            }
        }
    }

    // 駅同士を線路で連結する
    vector<vector<int>> grid_copy = grid;
    for (int i = 0; i < station; ++i) {
        for (int j = station; j < station * 2; ++j) {
            // 最短経路を求めて線路を配置
            vector<pair<int, int>> path = bfs(N, grid_copy, Stations[i], Stations[j]);
            for (auto& p : path) {
                if (grid_copy[p.first][p.second] == 0) {
                    grid_copy[p.first][p.second] = 1;  // 1は線路を表す
                    cout << "1 " << p.first << " " << p.second << endl;
                }
            }
        }
    }

    // 出力
    for (int turn = 0; turn < T; ++turn) {
        if (turn < station * 2) {
            cout << "0 " << Stations[turn].first << " " << Stations[turn].second << endl;
        } else {
            cout << "-1" << endl;  // 待機
        }
    }

    return 0;
}
