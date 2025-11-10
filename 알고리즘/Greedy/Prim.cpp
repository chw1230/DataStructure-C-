/*
 * ============================================================
 * Prim 알고리즘 (Minimum Spanning Tree - MST)
 * ============================================================
 *
 * [알고리즘 개요]
 * 1. 임의의 한 정점(start)을 MST 집합에 넣는다.
 * 2. 그 정점에서 나가는 모든 간선을 우선순위큐(최소 힙)에 넣는다.
 * 3. 큐에서 가장 가중치가 작은 간선을 꺼낸다.
 *    - 그 간선이 'MST에 있는 정점 → MST에 없는 정점' 을 연결하면 채택한다.
 *    - 이미 두 정점이 모두 MST에 들어있으면 버린다.
 * 4. 새로 포함된 정점에서 나가는 간선들을 다시 큐에 넣고, 전체 정점이 들어갈 때까지 반복.
 *
 * [핵심 아이디어]
 * - "현재까지 만든 트리"와 "아직 포함 안 된 정점" 사이의 간선 중 가장 가중치가 작은 걸 계속 선택!
 * - Greedy하게 매번 최소 가중치를 선택하면 전체 MST가 완성됨
 *
 * [시간 복잡도]
 * - O(E log V) : E개의 간선을 우선순위큐에 넣고 빼는 작업
 *   (인접 리스트 사용 시)
 *
 * [사용 자료구조]
 * - 인접 리스트 (vector<pair<int,int>> graph[])
 *   : 정점마다 연결된 (다음 정점, 가중치)를 저장 → sparse graph에 효율적
 *   : Dense graph면 인접 행렬이 나을 수도 있지만, 대부분의 경우 리스트가 효율적
 *
 * - 우선순위 큐 (priority_queue)
 *   : "현재 MST 바깥으로 나가는 간선들" 중에서 가장 가중치가 작은 것을 빠르게 꺼내기 위해 사용
 *   : C++ 기본 pq는 max-heap이므로 greater<>를 써서 min-heap으로 만들기
 *   : 삽입/삭제 모두 O(log N) → 효율적!
 *
 * [큐에 넣는 정보 형태]
 *   weight, (from, to) 를 같이 넣어두기
 *   priority_queue< pair<weight, pair<u,v>> , ... >
 *   → weight를 첫 번째로 두면 자동으로 weight 기준 정렬되는 것을 이용!
 */

#include <iostream>
#include <vector>
#include <queue>
#include <utility> 
using namespace std;

int n, m; // n: 정점의 개수, m: 엣지의 개수
int sum; // 가중치의 합

// 인접 리스트: graph[u] = { (v, w), ... }  
// → 정점 u에서 정점 v로 가는 간선의 가중치가 w
// → 무방향 그래프이므로 양방향으로 모두 저장됨
vector<pair<int, int>> graph[10001];  // 최대 10000개 정점 

// 방문 배열: visited[i] = 1이면 정점 i가 MST에 이미 포함됨을 의미
// → Prim의 핵심! MST 포함되고 안되고를 체크
int visited[10001];


void prim()
{
    // pair<int, int> = (출발 정점, 도착 정점)
    vector<pair<int, int>> mcst; // MST에 포함된 간선들을 저장 

    vector<int> U;  // MST에 포함된 정점들을 저장 (현재 트리의 정점 집합)

    // 우선순위 큐 (최소 힙) 선언
    // pair<가중치, pair<시작정점, 끝정점>> 형태
    // greater<>를 사용해서 min-heap으로 설정 (가중치가 작은 게 top)
    // → C++의 기본 priority_queue는 max-heap이므로 주의!
    priority_queue <
                pair<int, pair<int, int>>,           // 저장할 데이터 타입
                vector<pair<int, pair<int, int>>>,   // 내부 컨테이너
                greater<>                            // 비교 함수 (min-heap 만들기)
                    > pq;

    for (int i = 1; i <= n; i++) {
		visited[i] = 0;  // 아직 어떤 정점도 MST에 포함 안 됨 -> 0으로 초기화
    }

    int u, v, w;  // u: 시작정점, v: 끝정점, w: 가중치
    u = 1;        // 1번 정점부터 시작 
    U.push_back(u);  // U 집합(MST 정점 집합)에 1번(시작 정점) 추가
	visited[1] = 1; // 시작 정점 mst에 포함되어 있음을 표시
	sum = 0;	  // 가중치 합 초기화

	// 시작 정점과 연결된 모든 간선들을 pq에 추가
    // graph[u] = { (v1, w1), (v2, w2), ... }
    // → u와 연결된 모든 정점들에 대해
    for (int i = 0; i < graph[u].size(); i++) {
        v = graph[u][i].first;   // 연결된 정점 번호
        // pq에 넣을 때: (가중치, (출발, 도착)) 순서로!
        pq.push(make_pair(graph[u][i].second, make_pair(u, v)));
        // → 가중치를 첫 번째에 두면 자동으로 가중치 기준 정렬!
    }

    // no_mcst : MST가 존재하지 않는 경우를 체크하는 플래그
    int no_mcst = 0;

    // MST가 모든 정점을 포함할 때까지 반복
    // U.size() < n : 아직 모든 정점이 MST에 포함 안 됨
    while (U.size() < n) {

        // 만약 큐가 비었다면?
        // →모든 정점을 방문 못 했는데(U.size() < n)) 더 이상 연결할 간선이 없다면 → 그래프가 disconnected! (연결 그래프가 아님 - 홀로 떨어진 노드가 있는 것!)
        if (pq.empty()) {
            no_mcst = 1;  // MST 없음 표시
            break;
        }

        // 현재 큐의 top에 있는 간선 정보 가져오기
        u = pq.top().second.first;   // 시작 정점
        v = pq.top().second.second;  // 끝 정점
		w = pq.top().first;          // 가중치 ( 가중치 합 구하기 위해서 )
        pq.pop();  // 실제로 큐에서 제거

       

        // visited[v] == 1 : 도착 정점 v가 이미 MST에 있음 → 사이클 생성됨( mst 조건 만족하지 않아서! )
        if (visited[v] == 1) {
            continue;  // 다음 간선으로
        }

        // → MST에 추가
        mcst.push_back(make_pair(u, v));  // 간선 저장
		sum += w;        // 가중치 합 최신화
        U.push_back(v);     // v를 MST 정점 집합에 추가
        visited[v] = 1;     // v를 방문 처리

        // 새로 추가된 정점 v에서 나가는 엣지들을 큐에 추가
        // → MST가 확장되었으므로, 새로운 후보 엣지들 추가
        for (int i = 0; i < graph[v].size(); i++)
        {
            u = graph[v][i].first;   // v와 연결된 정점
            w = graph[v][i].second;  // 가중치

            // 이미 MST에 포함된 정점이면 간선 추가 안 함 -> 사이클 방지 목적
            if (visited[u] == 1) {
                continue;
            }

            // 유효한 간선이면 큐에 추가
            pq.push(make_pair(w, make_pair(v, u)));
        }
    }

	if (no_mcst) { // MST가 없는 경우
        cout << "No MST\n";  // MST가 없음 (disconnected graph)
    } else {
        cout << "\nMST edges:\n"; // MST에 포함된 모든 출발 정점, 도착 정점 출력
        for (int i = 0; i < mcst.size(); i++) {
            cout << "<" << mcst[i].first << ", " << mcst[i].second << ">\n"; 
        }
        cout << "\nMST 가중치 합: " << sum << "\n";
    }
}

int main()
{
    ios::sync_with_stdio(false);  
    cin.tie(nullptr);             

	int t; // test case 수

	cin >> t;  

    for (int i = 0; i < t; i++) {
        cin >> n >> m;  // n: 정점 수, m: 간선 수
        for (int j = 0; j < m; j++) {
            int a, b, w;
            cin >> a >> b >> w;

            graph[a].push_back({ b, w });
            graph[b].push_back({ a, w });
        }
		cout << "Test case " << i + 1 << ":\n";
        prim();

		cout << "\n";
        for (int k = 1; k <= n; k++) {
            graph[k].clear();
        }
    }

    return 0;
}

/*
 * ============================================================
 * 테스트 예제
 * ============================================================
 *
입력:
2
6 10
1 2 10
1 4 30
1 5 45
2 3 50
2 5 40
2 6 25
3 5 35
3 6 15
4 6 20
5 6 55
6 10
1 2 16
1 5 19
1 6 21
2 3 5
2 4 6
2 6 11
3 4 10
4 6 14
4 5 18
5 6 33

출력:
2
6 10
1 2 10
1 4 30
1 5 45
2 3 50
2 5 40
2 6 25
3 5 35
3 6 15
4 6 20
5 6 55
Test case 1:

MST edges:
<1, 2>
<2, 6>
<6, 3>
<6, 4>
<3, 5>

MST 가중치 합: 105

6 10
1 2 16
1 5 19
1 6 21
2 3 5
2 4 6
2 6 11
3 4 10
4 6 14
4 5 18
5 6 33
Test case 2:

MST edges:
<1, 2>
<2, 3>
<2, 4>
<2, 6>
<4, 5>

MST 가중치 합: 56
 *
 * ============================================================
 * Prim vs Kruskal 알고리즘 비교
 * ============================================================
 *
 * [공통점]
 * 둘 다 MST(최소 신장 트리)를 구하는 Greedy 알고리즘이고, 시간 복잡도도
 * O(E log E) 또는 O(E log V) 수준으로 비슷하다. 그리고 둘 다 항상 최적해를
 * 보장한다는 것도 똑같다.
 *
 *
 * [기본 전략의 차이]
 *
 * Prim은 정점(Vertex) 중심 알고리즘이다. 하나의 시작 정점에서 출발해서
 * 트리를 점점 확장해 나가는 방식이야. 마치 나무가 뿌리에서부터 가지를
 * 뻗어나가듯이 연결된 정점들을 하나씩 추가하는 거지.
 *
 * 반면 Kruskal은 간선(Edge) 중심 알고리즘
 * 모든 간선을 가중치 순으로 정렬해놓고, 
 * 가장 가벼운 간선부터 차례대로 선택해서 트리를 만들어가는 방식
 * 사이클만 안 생기면 계속 추가
 *
 *
 * [동작 방식]
 *
 * Prim의 동작 순서는 이렇다:
 * 1) 임의의 시작 정점을 하나 선택한다
 * 2) 현재 트리와 인접한 간선들 중에서 가장 가중치가 작은 걸 선택한다
 * 3) 선택된 간선으로 새로운 정점을 트리에 추가한다
 * 4) 모든 정점이 포함될 때까지 2-3을 반복한다
 *
 * Kruskal의 동작 순서는 이렇다:
 * 1) 모든 간선을 가중치 순으로 오름차순 정렬한다
 * 2) 가장 가벼운 간선부터 하나씩 꺼낸다
 * 3) 그 간선이 사이클을 만들지 않으면 MST에 추가한다
 * 4) 정점 개수 - 1 개의 간선이 선택될 때까지 2-3을 반복한다
 *
 *
 * [주요 자료구조]
 *
 * Prim은 우선순위 큐(Min Heap)와 방문 배열을 사용
 * 우선순위 큐로 현재 트리에서 나가는 간선들 중 최소 가중치를 빠르게 찾고, 
 * 방문 배열로 어떤 정점이 이미 트리에 포함되었는지 체크
 *
 * Kruskal은 Union-Find(Disjoint Set) 자료구조를 사용한다. 
 * 간선을 추가할 때 사이클이 생기는지 빠르게 판단할 때 사용
 * 두 정점이 이미 같은 집합에 속해있으면 사이클이 생기는 거니까 그 간선은 버림
 *
 *
 * [어떤 그래프에 적합한가]
 *
 * Prim은 Dense Graph(간선이 많은 그래프)에 적합
 * 간선의 개수가 정점 개수의 제곱에 가까울 때, 즉 E ≈ V² 일 때 유리
 * 왜냐하면 정점 중심으로 동작하기 때문에 간선이 많아도 크게 비효율적이지 않음
 *
 * Kruskal은 Sparse Graph(간선이 적은 그래프)에 적합
 * 간선의 개수가 정점에 비해 훨씬 적을 때, 즉 E << V² 일 때 유리하다. 
 * 간선을 정렬하는 게 핵심, 간선이 적으면 정렬 비용이 적게 들기 때문
 */