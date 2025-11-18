#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <limits>
using namespace std;

// ========================================================================================
// Bellman-Ford Algorithm
// 목적: 단일 출발점에서 모든 정점까지의 최단 거리를 구하는 알고리즘
// 특징: 음수 가중치를 가진 간선이 있어도 동작하며, 음수 사이클 탐지 가능
// 시간복잡도: O(VE) - V는 정점 수, E는 간선 수
// 다익스트라와의 차이: 다익스트라는 음수 간선 처리 불가능하지만 더 빠름 O(ElogV)
// ========================================================================================

int n, m;  // n: 정점의 개수, m: 간선의 개수
vector<pair<int, int>> graph[10001];  // graph[u] = {(v, w), ...} : u에서 v로 가는 간선의 가중치 w
int dist[10001];  // dist[i]: 출발점에서 정점 i까지의 최단 거리
int pre[10001];   // pre[i]: 최단 경로에서 정점 i의 바로 이전 정점 (경로 역추적용도)
const int INF = 1000000; 

// ========================================================================================
// bellman_ford 함수: 출발점 s에서 모든 정점까지의 최단 거리 계산
// 반환값: 음수 사이클이 존재하면 true, 아니면 false
bool bellman_ford(int s) {

    // 모든 정점의 거리를 무한대로, 이전 정점을 -1로 초기화
    for (int i = 1; i <= n; i++) {
        pre[i] = -1;      // 이전 정점 없음 표시
        dist[i] = INF;    // 아직 도달 방법을 모르는 상태 -> 무한대 설정
    }
    pre[0] = -1;    // 0번 정점은 사용하지 않지만 일관성을 위해 초기화
    dist[s] = 0;    // 출발점의 거리는 0 (자기 자신까지의 거리)

    // --------------------------------------------------------------------------------
    // 최단 거리 계산 - Edge Relaxation을 (n-1)번 반복
    // 핵심 아이디어: 최단 경로는 최대 (n-1)개의 간선을 사용
    // 왜 n-1번 반복할까?: n개의 정점이 있을 때, 사이클 없이 갈 수 있는 최대 간선 수는 n-1개
    int u, v, luv;  // 출발, 도착, 가중치

    for (int i = 1; i < n; i++) { 

        // 모든 간선에 대해 relaxation 수행
        for (int u = 1; u <= n; u++) {  // 모든 정점 u를 출발점으로

            for (int k = 0; k < graph[u].size(); k++) {  // u에서 나가는 모든 간선 루프
                v = graph[u][k].first;      // 도착
                luv = graph[u][k].second;   // 가중치

                // ------------------------------------------------------------------------
                // 조건 1: u까지 도달 가능해야 함 => dist[u] != INF
                // 조건 2: u를 거쳐가는 것이 더 짧은 경로 => dist[v] > dist[u] + luv
                if (dist[u] != INF && dist[v] > dist[u] + luv) { 
                    dist[v] = dist[u] + luv;  // 더 짧은 거리로 갱신
                    pre[v] = u;               // v로 가는 최단 경로에서 바로 이전 정점은 u를 저장하기
                }
            }
        }
    }

    // --------------------------------------------------------------------------------
    // 음수 사이클(Negative Cycle)의 존재 확인
    // 음수 사이클: 사이클을 돌면 돌수록 총 거리가 계속 줄어드는 사이클
    // 
    // 검증 방법: (n-1)번 반복 후에도 갱신 가능한 간선이 있다면 음수 사이클 존재하는 것!
    // 정상적인 최단 경로는 최대 (n-1)개 간선만 사용하므로, n번째에도 갱신이 된다는 것은 사이클을 이용해 더 짧게 만들 수 있다는 의미
    bool hasNegativeCycle = false;

    for (u = 1; u <= n; u++) {  // 모든 간선을 한 번 더 검사
        for (int k = 0; k < (int)graph[u].size(); k++) {
            v = graph[u][k].first;
            luv = graph[u][k].second;

            // n번째 반복에서도 갱신이 가능하다면 → 음수 사이클 존재!
            if (dist[u] != INF && dist[v] > dist[u] + luv) { // 실제 최신화가 이뤄지는게 아니라 그냥 최신화 가능한 조건인지만 확인
                hasNegativeCycle = true;
            }
        }
    }

    return hasNegativeCycle;
}

int main()
{
    ios::sync_with_stdio(false); 
    cin.tie(nullptr);              

    int tc;  
    cout << "테스트 횟수 입력: ";
    cin >> tc;

    while (tc--) { 

        cin >> n >> m;

        for (int i = 1; i <= n; i++) {
            graph[i].clear();
        }

        for (int i = 1; i <= m; i++) {
            int u, v, w;  // u에서 v로 가는 가중치 w
            cin >> u >> v >> w;
            graph[u].push_back({ v, w });  
        }

        // 정렬: 각 정점의 인접 리스트를 정렬 (문제 요구사항에 따라 필요할 수 있음, 일단 필수 사항은 아님)
        for (int i = 1; i <= n; i++) {
            sort(graph[i].begin(), graph[i].end());
        }

		bool hasNegativeCycle = bellman_ford(1); // 1번 정점을 출발점으로 설정

        cout << "=== Test Case " << tc << " ===\n";

		if (hasNegativeCycle) { // negative cycle 존재 여부 출력
            cout << "Negative cycle exists\n";
        }

        // dist[i] == INF → 1번 정점에서 i번 정점으로 도달 불가능
        // dist[i] != INF → 1번 정점에서 i번 정점까지의 최단 거리는 dist[i]
        for (int i = 1; i <= n; i++) {
            cout << i << " --> ";
            if (dist[i] == INF) {
                cout << "INF\n";  // 도달 불가능
            }
            else {
                cout << dist[i] << "\n";  // 최단 거리 출력
            }
        }
        cout << "\n";
    }

    return 0;
}

/*
입력:
2
5 9
1 2 4
1 3 2
2 3 -3
2 4 2
2 5 3
3 2 4
3 4 4
3 5 5
5 4 1
5 9
1 2 4
1 3 2
2 3 -5
2 4 2
2 5 3
3 2 4
3 4 4
3 5 5
5 4 1

출력:
=== Test Case 1 ===
1 --> 0
2 --> 4
3 --> 1
4 --> 5
5 --> 6

=== Test Case 2 ===
Negative cycle exists
1 --> 0
2 --> 0
3 --> -4
4 --> 0
5 --> 1

- 첫 번째 케이스: 음수 간선은 있지만 음수 사이클은 없음
- 두 번째 케이스: 1→2→3→2 사이클이 음수 사이클
*/