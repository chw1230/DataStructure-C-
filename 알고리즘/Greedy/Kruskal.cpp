/*
 * 크루스칼 알고리즘 (Kruskal's Algorithm)
 * * [핵심 아이디어 - 그리디(Greedy)]
 * 1. 모든 간선을 가중치(비용)가 낮은 순서대로 정렬한다.
 * 2. 가중치가 낮은 간선부터 하나씩 확인하며,
 * 3. 이 간선을 추가했을 때 '사이클(Cycle)'이 생기지 않으면 트리에 포함시킨다. ( 스패닝 트리 제약 사항 )
 * 4. 2번,3번 과정을 (노드 개수 - 1)개의 간선을 선택할 때까지 반복한다. ( 스패닝 트리 제약 사항 )
 *
 * [사용하는 자료구조]
 * - Union-Find (Disjoint Set): 3번 단계(사이클 탐지)를 효율적으로 하기 위해 사용
 * - Priority Queue (최소 힙): 1번 단계(정렬)를 효율적으로 처리하기 위해 사용
 */

#include <iostream>     
#include <vector>      
#include <algorithm>    
#include <queue>       
#include <utility>   

using namespace std; 

int n, m; // n: 노드의 개수, m: 간선의 개수
int sum; // 가중치 MCST의 가중치 총합

// <가중치, <노드1, 노드2>> 형태 -> 노드1에서 노드2로 가는데 가중치 만큼 소요됨
vector<pair<int, pair<int, int>>> edges;

// 3. Union-Find (Disjoint Set) 클래스
// : 두 노드가 이미 같은 집합에 속해있는가? (= 사이클이 되는가?) 를 판단하기 위해서 사용
class UnionFind {
    vector<int> parent; // i번 노드의 부모 노드 번호를 담는 백터
    vector<int> rank;   // i를 루트로 하는 트리의 높이

public:
    // 생성자: n개의 노드로 Union-Find 구조를 초기화
    UnionFind(int n) {
        parent.resize(n + 1); // 노드 번호를 1번부터 쓰기 위해 n+1 크기로
        rank.resize(n + 1, 0);  // 모든 노드의 랭크는 0으로 초기화

        // 모든 노드가 자기 자신을 부모로 가리키며, n개의 노드가 모두 '별개의 집합'에 위치해 있음
        for (int i = 0; i <= n; i++) {
            parent[i] = i;
        }
    }

    // find(u): 노드 u가 속한 집합의 '루트' 노드를 찾는 함수
    int find(int u) {
        // 만약 parent[u]가 u 자신이 아니라면 (루트가 아니라면)
        if (parent[u] != u) {
            // 재귀적으로 루트를 찾아 올라가면서,
            // 그 과정에 있는 모든 노드의 부모를 '최종 루트'로 갱신
            parent[u] = find(parent[u]);
        }
        // 루트 노드를 반환
        return parent[u];
    }

    // unite(x, y): 노드 x가 속한 집합과 노드 y가 속한 집합을 합치는 함수
    bool unite(int x, int y) {
        // 1. 두 노드의 루트를 각각 찾음
        int rootX = find(x);
        int rootY = find(y);

        // 2. 사이클 판별
        // 만약 두 노드의 루트가 같다면? -> 이미 같은 집합(연결 그래프)에 속해있다는 뜻
        if (rootX == rootY) {
            return false; // 합치기(간선 추가) 실패
        }

        // 3. 랭크 기반 합치기
        // 트리의 높이가 더 커지지 않도록 '랭크가 낮은 쪽'을 '랭크가 높은 쪽'에 붙임
		if (rank[rootX] < rank[rootY]) { // X의 랭크가 Y의 랭크보다 낮다면
            parent[rootX] = rootY; // X의 루트가 Y의 루트를 가리키도록 함 (= 낮은 쪽을 높은 쪽에 붙이는 것과 동일)
        } else if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        } else {
            // 두 랭크가 같다면, 붙임(Y를 X에 붙임 - 우리는 그냥 이렇게 하기로 한 것!)
			parent[rootY] = rootX; // 기존에 루트였던 y가 이제 x를 가리키도록 하니까
            rank[rootX]++; // x의 랭크를 하나 증가 시킬 수 있음
        }
		return true; // 합치기( 간선 추가 ) 성공
    }
};

void kruskal() {
    // mcst: 최소 비용 신장 트리(Minimum Cost Spanning Tree) 선택된 간선들(<노드1, 노드2>)을 저장할 벡터
    vector<pair<int, int>> mcst;

    // 우선순위 큐 (Priority Queue)
    //    'greater<...>'를 사용하여 '최소 힙(Min-Heap)'으로 동작 -> 큐의 top()은 항상 '가중치가 가장 낮은' 간선!
    // 크루스칼: '간선 자체의 가중치'가 낮은 순으로 뽑기!!
    /* priority_queue<Type, Container, Compare> 
       1. Type : 무엇을 저장할래? -> 가중치, 노드1, 노드2 저장할래!
       2. Container : 어떤 통에 담아 둘래? -> vector에 담을래!
       3. 어떻게 정렬할래? -> greater => 더 작은 것이 더 우선순위 높게 할래!*/
    priority_queue<pair<int, pair<int, int>>,
        vector<pair<int, pair<int, int>>>,
        greater<pair<int, pair<int, int>>>> pq;

    // Union-Find 자료구조를 'n'개의 노드로 초기화
    UnionFind uf(n);

    // 1. 모든 간선(m개)을 우선순위 큐(pq)에 삽입
    for (int i = 0; i < m; i++) {
        pq.push(edges[i]);
    }

    int u, v, w; // u-v 노드, w 가중치

    // 2. [그리디 선택] + [사이클 검사]
    //    큐가 비어있지 않고,선택한 간선의 개수(mcst.size())가 (n-1)개보다 적을 동안(스패닝 트리 제약사항 - MST는 항상 n개의 노드, n-1개의 간선을 가짐)
    while (!pq.empty() && mcst.size() < n - 1) {

        // 2-1. [그리디 선택] 현재 큐에 남은 간선 중 '가장 가중치가 낮은' 간선을 꺼냄
        w = pq.top().first;
        u = pq.top().second.first;
        v = pq.top().second.second;
        pq.pop(); // 큐에서 제거

        // 2-2. [사이클 검사] (Union-Find의 핵심 역할)
        // 꺼낸 간선의 양쪽 노드(u, v)가 다른 집합에 속해있다(루트의 노드가 다르다)면?
        // = 이 간선을 추가해도 사이클이 생기지 않는다면?
        if (uf.find(u) != uf.find(v)) {
            // 2-3. [간선 선택]
            mcst.push_back(make_pair(u, v)); // 이 간선을 MST에 포함시킨다!
			sum += w; // MST의 가중치 합 최신화
            uf.unite(u, v); // 두 집합(u, v)을 합친다(이제 둘은 연결됨)

        }
    }

    // 3. 결과 확인
    //    만약 선택된 간선이 (n-1)개가 안 된다면?
    //    -> 모든 노드를 연결하지 못했다는 뜻 (즉, 원본 그래프가 - 노드 동 떨어진 비연결 그래프)
    if (mcst.size() < n - 1) {
        cout << "NO MCST \n"; // MST를 만들 수 없음
        return;
    }

    // 4. MST 간선 출력
	cout << "\n간선 출력 (u, v): \n";
    for (int i = 0; i < mcst.size(); i++) {
        cout << "< " << mcst[i].first << ", " << mcst[i].second << " >\n";
    }
}

int main() {
    int tc, u, v, w; // tc: 테스트 케이스 수, u/v/w: 간선 입력용
    sum = 0;

    std::cin >> tc; // 테스트 케이스 수 입력

    // 테스트 케이스만큼 반복
    for (int t = 0; t < tc; t++) {

        // 전역 변수 n, m에 값을 입력받음
        std::cin >> n >> m; // 노드의 개수, 엣지의 개수

        // 다음 테스트 케이스를 위해 전역 변수 edges 벡터를 비워줌
        edges.clear();

        // m개의 간선 정보를 입력받음
        for (int i = 0; i < m; i++) {
            std::cin >> u >> v >> w; // (노드1, 노드2, 가중치)

            // edges 벡터에 <가중치, <노드1, 노드2>> 형태로 저장
            edges.push_back(std::make_pair(w, std::make_pair(u, v)));
        }

        // 크루스칼 알고리즘 실행
        kruskal();
    }
	cout << "MCST의 가중치 총합: " << sum << "\n";
    return 0;
}

/*
입력 :
1
5 9
1 2 4
1 3 2
2 3 1
2 3 3
2 4 2
2 5 3
3 4 4
3 5 5
4 5 1

출력 :
*/