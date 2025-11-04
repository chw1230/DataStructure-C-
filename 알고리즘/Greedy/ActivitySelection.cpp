#include <iostream>
#include <queue>
#include <utility>
#include <vector>

using namespace std;

/*
 * Activity Selection Problem 
 * ------------------------------------------
 * 주어진 활동들의 (start, end) 시간을 입력받아
 * 서로 겹치지 않게 가장 많은 활동을 선택한다.
 *
 * 핵심 아이디어:
 *   → "끝나는 시간이 가장 빠른 활동"부터 선택해서 최적해를 구하기
 */

int n;  // 활동 개수
pair<pair<int, int>, int> jobs[100]; // (end, start, index)

void solve() {
    int cnt = 0; // 선택된 활동 개수
	int sol[100]; // 선택된 활동을 저장할 배열

	// end가 작은 활동부터 나오는 최소 힙 / 종료 시간, 시작 시간, 활동 번호 / 종료 시간을 기준으로 pq 할 것이므로 가장 앞에 두기!
    priority_queue<
        pair<pair<int, int>, int>,
        vector<pair<pair<int, int>, int>>,
        greater<pair<pair<int, int>, int>>
    > myq;

    // 모든 활동을 PQ에 삽입
    for (int i = 0; i < n; i++) {
        myq.push(jobs[i]);
    }

    int idx, si, ei;
    int last_end_time = 0;  // 마지막으로 선택된 활동의 종료시간

    while (!myq.empty()) {
        ei = myq.top().first.first;   // end
        si = myq.top().first.second;  // start
        idx = myq.top().second;        // index
        myq.pop();

        // 겹치지 않으면 선택
		if (si >= last_end_time) { // 마지막에 한 선택된 활동의 종료시간 이후에 시작하는 활동으면 선택!
			sol[cnt++] = idx; // 선택된 활동 번호 저장
			last_end_time = ei; // 마지막 선택된 활동의 종료시간 최신화
        }
    }

    // 결과 출력
    cout << "선택된 활동 번호: ";
    for (int i = 0; i < cnt; i++) {
        cout << sol[i];
        if (i < cnt - 1) {
            cout << ", ";
        }
    }
    cout << "\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int tc;
    cout << "테스트 케이스 수 입력: ";
    cin >> tc;

    for (int t = 0; t < tc; t++) {
        cout << "\n활동 개수 입력: ";
        cin >> n;

        cout << "시작시간과 종료시간을 입력하세요 (예: 1 4):\n";
        for (int i = 0; i < n; i++) {
            int s, e;
            cin >> s >> e;
			jobs[i] = make_pair(make_pair(e, s), i + 1); // 종료 시간,시작 시간, 활동 번호
        }

        cout << "\n테스트 케이스 #" << t + 1 << " 결과:\n";
        solve();
    }

    return 0;
}


/*
* 입력 예시 :
1
6
1 4
3 5
0 6
5 7
8 9
5 9

* 출력 예시 :
테스트 케이스 수 입력: 1

활동 개수 입력: 6
시작시간과 종료시간을 입력하세요 (예: 1 4):
1 4
3 5
0 6
5 7
8 9
5 9

테스트 케이스 #1 결과:
선택된 활동 번호: 1, 4, 5
*/