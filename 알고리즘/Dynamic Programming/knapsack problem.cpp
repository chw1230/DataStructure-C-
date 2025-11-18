#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

int n, M;
vector<int> w, p;
// memo[i][c]: i번째 물건부터 탐색을 시작하고, 배낭 용량이 c 남았을 때의 '최대 가치'
vector<vector<int>> memo;

/**
 * * @param i 현재 고려할 물건의 인덱스 (0 ~ n-1)
 * @param capacity 현재 배낭에 남은 담을 수 있는 용량
 * @return int 현재 상태(i, capacity)에서 얻을 수 있는 최대 가치
 */
int KNAP_memoization(int i, int capacity) {

    // 1. Base Case
    // i가 n과 같다는 것은 0번부터 n-1번까지 모든 물건을 다 훑어봤다는 뜻
    // 더 이상 고려할 물건이 없음 -> 0 반환
    if (i == n) {
        return 0;
    }

    // 2. 메모이제이션 (Memoization)
    // 현재 상태 (i번 물건, 남은 용량 capacity)를 이전에 계산한 적이 있는지 확인해서 -1이 아니라면,
    // 이미 계산된 값이므로 바로 반환
    if (memo[i][capacity] != -1) {
        return memo[i][capacity];
    }

    // 3-1. 남은 물건 중 가장 가벼운 것도 못 담는 경우
    int minWeight = INT_MAX; 
    for (int j = i; j < n; j++) { // 가장 가벼운 물건의 무게 선정하기
        minWeight = min(minWeight, w[j]);
    }

    // 가장 가벼운 물건보다 용량이 적게 남았다면? -> 더 이상 아무것도 못 넣음 -> 0 리턴
    if (capacity < minWeight) {
        return memo[i][capacity] = 0; // 결과 저장 후 반환하기
    }

    // 3-2. 남은 물건을 전부 다 담을 수 있는 경우
    int sumWeight = 0;
    int sumValue = 0;
    for (int j = i; j < n; j++) { // 남은 물건의 무게의 합과 가치의 합 구하기
        sumWeight += w[j];
        sumValue += p[j];
    }

    // 남은 용량이 충분하다면? -> 다 넣기
    if (capacity >= sumWeight) {
        return memo[i][capacity] = sumValue; // 결과 저장 후 반환
    }


    // 현재 물건을 "안 담"고 가치,용량 그대로, 다음 물건(i+1)으로 넘어가기
    int skipResult = KNAP_memoization(i + 1, capacity);

    // 현재 남은 용량이 물건 무게보다 크다면 물건을 담고
    int takeResult = -1; // 초기값 
    if (capacity >= w[i]) {
        // 현재 물건의 가치(p[i])를 더하고, 무게(w[i])를 뺀 상태로 다음 물건을 탐색
        takeResult = p[i] + KNAP_memoization(i + 1, capacity - w[i]);
    }

    // 두 선택지 중 더 큰 가치를 선택
    int finalResult = max(skipResult, takeResult);

    memo[i][capacity] = finalResult; // 메모이제이션에 저장

    return finalResult;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    cin >> n >> M;

    w.resize(n);
    p.resize(n);

    for (int i = 0; i < n; i++) {
        cin >> w[i] >> p[i];
    }

    // (n+1) * (M+1) 개 , -1로 초기화
    memo.assign(n + 1, vector<int>(M + 1, -1));

    // 탐색 시작: 0번 물건부터, 배낭 용량 M인 상태로 시작
    int answer = KNAP_memoization(0, M);

    cout << "최대 가치: " << answer << "\n";
    return 0;
}