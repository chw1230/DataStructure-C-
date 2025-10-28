#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <cctype>
using namespace std;

/* 노드
 허프만 트리를 구성하는 기본 단위
 - freq : 문자(또는 노드)의 등장 확률
 - ch : 리프 노드일 경우 알파벳 문자
 - lchild, rchild : 왼쪽, 오른쪽 자식 포인터 */
struct Node {
    double freq; // 빈도
    char ch; // 문자
    Node* lchild;
    Node* rchild;
    Node(double f, char c, Node* l = nullptr, Node* r = nullptr)
        : freq(f), ch(c), lchild(l), rchild(r) {
    }
};

/* 우선순위 큐 비교자
 - priority_queue는 기본적으로 "큰 값이 먼저" 나오기 때문에
   freq가 작은 노드가 먼저 나오도록 비교자를 재정의 */
struct Compare {
    bool operator()(const Node* a, const Node* b) const {
        return a->freq > b->freq;
    }
};

/* 허프만 코드 출력 함수
 - 루트 노드로부터 왼쪽 자식은 0, 오른쪽 자식은 1
 - 리프 노드(문자 도달 시) 코드 출력 */
void print_hcode(const Node* tnode, const string& hcode) {
    // tnode는 현재 탐색 중인 노드를 가리키는 포인터
    // hcode는 현재까지 생성된 허프만 코드 문자열
    if (!tnode) {
        return;
    }

    // 리프 노드라면 해당 문자와 코드를 출력
    if (!tnode->lchild && !tnode->rchild) {
        cout << " " << tnode->ch << "  -->  " << hcode << endl;
        return;
    }

    // 왼쪽으로 내려갈 때: 0 추가 -> 많이 사용 하지 않음
    print_hcode(tnode->lchild, hcode + "0");
    // 오른쪽으로 내려갈 때: 1 추가 -> 많이 사용 하는 것
    print_hcode(tnode->rchild, hcode + "1");
}

/* 허프만 트리 구성 함수]
 입력 : 알파벳의 확률 벡터
 과정 :
 1. 각 알파벳(A~Z)에 대해 노드 생성 후 pq에 삽입
 2. freq가 가장 작은 두 노드를 꺼내 병합
 3. 새로운 부모 노드 생성 (freq = 두 노드 합)
 4. 남은 노드가 하나면 그것이 루트 */
Node* build_huffman_tree(const vector<char>& letters, const vector<double>& freq, double& cost_out) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    // (1) 각 알파벳에 대해 리프 노드 생성
    for (size_t i = 0; i < letters.size(); ++i) {
        pq.push(new Node(freq[i], letters[i]));
    }

    double cost = 0.0;

    // (2) 두 노드를 합치면서 트리 구성
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        // 새 부모 노드 생성
		Node* parent = new Node(left->freq + right->freq, '\0', left, right); // 문자 없음 -> 리프 노드에만 문자가 존재
        cost += parent->freq;

        pq.push(parent);
    }

    cost_out = cost;
    return pq.top();  // 최종 루트 반환
}

// 트리 메모리 해제
void free_tree(Node* t) {
    if (!t) return;
    free_tree(t->lchild);
    free_tree(t->rchild);
    delete t;
}

/* 허프만 코드 맵 생성
 - 루트에서 리프까지 내려가며 문자 -> 코드 매핑 생성 */
void build_code_map(const Node* tnode, const string& hcode, unordered_map<char, string>& cmap) {
    if (!tnode) {
        return;
    }
    if (!tnode->lchild && !tnode->rchild) {
        cmap[tnode->ch] = hcode;
        return;
    }
    build_code_map(tnode->lchild, hcode + "0", cmap);
    build_code_map(tnode->rchild, hcode + "1", cmap);
}

/* 인코딩
 - 입력 문자열 input을 통해서, 알파벳(A–Z/a–z) 은 허프만 코드 문자열(‘0’/‘1’) 로 바꾸고, 알파벳이 아닌 문자(공백·숫자·구두점 등) 는 그대로 결과 반영하는 메서드 */
string encode_with_huffman(const string& input, const unordered_map<char, string>& cmap) {
    string bits;
    for (char c : input) {
		if (isalpha(static_cast<unsigned char>(c))) { // 알파벳 확인하기
            char up = static_cast<char>(toupper(static_cast<unsigned char>(c))); // 대분자만 반영하기
			auto it = cmap.find(up); // 대문자 허프만 코드 조회
            if (it != cmap.end()) { // 존재 한다면 결과를 문자열에 추가하기
                bits += it->second;
            }
        } else {
            bits += c; // 없으면 그대로(알파벳이 아닌 문자(공백·숫자·구두점 등) 추가하기
        }
    }
    return bits;
}

/* 디코딩
 - 비트열을 따라 트리를 내려가며 리프에 도달 시 문자 복원하기
 - 인코딩 시 비알파벳을 그대로 bits에 섞어 넣었기 때문에
   여기서는 '0/1'만 트리를 타고, 그 외 문자는 그대로 결과에 추가 */
string decode_with_huffman(const string& bits, const Node* root) {
    string out; // 최종 디코딩 결과를 저장할 변수
	const Node* cur = root; // 현재 탐색 중인 노드를 가리키는 포인터

    for (char b : bits) {
        if (b != '0' && b != '1') { // 0과 1이 아닌 문자(즉, 공백, 구두점, 숫자 등)는 그대로 결과에 추가하기
            out.push_back(b); 
            continue; 
        } 
        cur = (b == '0') ? cur->lchild : cur->rchild;             // 0이면 왼쪽, 1이면 오른쪽 이동
        if (!cur->lchild && !cur->rchild) {                       // 리프 노드(문자)에 도달하면
            out.push_back(cur->ch);                               // 결과 문자열에 추가
            cur = root;                                           // 다음 비트를 해석하기 위해서 다시 루트로 초기화
        }
    }
    return out;
}

int main() {
    vector<char> letters = {
        'A','B','C','D','E',
        'F','G','H','I','J',
        'K','L','M','N','O',
        'P','Q','R','S','T',
        'U','V','W','X','Y',
        'Z'
    };

    // [문제에서 제시된 알파벳 확률표]
    vector<double> freq = {
        0.08833, 0.01267, 0.02081, 0.04376, 0.14878,
        0.02455, 0.01521, 0.05831, 0.05644, 0.00800,
        0.00867, 0.04124, 0.02361, 0.06498, 0.07245,
        0.02575, 0.00080, 0.06872, 0.05537, 0.09351,
        0.02762, 0.01160, 0.01868, 0.00146, 0.01521,
        0.00053
    };

	cout << fixed << setprecision(6); // 소수점 6자리 고정 출력하기
    cout << "==============================" << endl;
    cout << "     Huffman Encoding Table   " << endl;
    cout << "==============================" << endl;

    double cost = 0.0;
    Node* root = build_huffman_tree(letters, freq, cost);

    // 평균 코드 길이( = 총 cost)
    cout << "[총 비용] : " << cost << endl;
    cout << "[평균 코드 길이(비트)] : " << cost << endl << endl;

    cout << "symbol   huffman-code" << endl;
    cout << "------------------------------" << endl;

    // 허프만 코드 출력
    print_hcode(root, "");

    // 문자 -> 코드 생성하기
    unordered_map<char, string> code_map;
    build_code_map(root, "", code_map);

    // 입력 받아서 인코딩/디코딩 진행하기
    cout << "\n==============================\n";
    cout << " 인코딩할 문자열을 입력하세요\n";
    cout << " (A~Z/a~z만 허프만 인코딩, 그 외 문자는 그대로 보존)\n";
    cout << "==============================\n> ";

    string line;
    getline(cin, line);

    string encoded = encode_with_huffman(line, code_map);
    cout << "\n[Encoded] " << encoded << endl;

    string decoded = decode_with_huffman(encoded, root);
    cout << "[Decoded] " << decoded << endl;

    free_tree(root);
    return 0;
}
/*
=== 실행 결과 ===

==============================
     Huffman Encoding Table
==============================
[병합 총 비용] : 4.213320
[평균 코드 길이(비트)] : 4.213320

symbol   huffman-code
------------------------------
 C  -->  00000
 M  -->  00001
 V  -->  000100
 B  -->  000101
 F  -->  00011
 T  -->  001
 P  -->  01000
 U  -->  01001
 S  -->  0101
 I  -->  0110
 H  -->  0111
 N  -->  1000
 G  -->  100100
 Y  -->  100101
 W  -->  100110
 K  -->  1001110
 Z  -->  1001111000
 Q  -->  1001111001
 X  -->  100111101
 J  -->  10011111
 R  -->  1010
 O  -->  1011
 E  -->  110
 L  -->  11100
 D  -->  11101
 A  -->  1111

==============================
 인코딩할 문자열을 입력하세요
 (A~Z/a~z만 허프만 인코딩, 그 외 문자는 그대로 보존)
==============================
> Hi! choi!

[Encoded] 01110110! 00000011110110110!
[Decoded] HI! CHOI!
*/