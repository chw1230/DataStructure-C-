#include <stdio.h>

struct employee 
{
	char name[20];
	char number[14];
	int n;
};

int main_b(void) {
	struct employee emp;
	printf("�̸� �Է�: "); 
	scanf("%s", emp.name);

	printf("\n�ֹ� ��� ��ȣ �Է�: ");
	scanf("%s", emp.number);

	printf("\n�޿� �Է�: ");
	scanf("%d", &(emp.n));

	printf("�̸�: %s\n", emp.name);
	printf("�ֹ� ��� ��ȣ : %s\n", emp.number);
	printf("�޿� : %d",emp.n);
	return 0;
}