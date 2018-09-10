//2048 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdint.h>
#include <time.h>
#define _XOPEN_SOURCE 500
#define SIZE 4
uint32_t score=0;
uint8_t scheme=0;
// ��ȡ��Ӧ����ɫ
void getColor(uint16_t value, char *color, size_t length) {
	// ����������ɫ���飬ÿ������λ��ż��λ���һ��ǰ��ɫ
	// ����������ֱ��Ӧ���������ѡ��"blackwhite","bluered"
	uint8_t bluered[] = {235,255,63,255,57,255,93,255,129,255,165,255,201,255,200,255,199,255,198,255,197,255,196,255,196,255,196,255,196,255,196,255};	
	uint8_t original[] = {8,255,1,255,2,255,3,255,4,255,5,255,6,255,7,255,9,0,10,0,11,0,12,0,13,0,14,0,255,0,255,0};
	uint8_t blackwhite[] = {232,255,234,255,236,255,238,255,240,255,242,255,244,255,246,0,248,0,249,0,250,0,251,0,252,0,253,0,254,0,255,0};
	uint8_t *schemes[] = {original,blackwhite,bluered};
	uint8_t *background = schemes[scheme]+0;
	uint8_t *foreground = schemes[scheme]+1;
	if (value > 0) while (value >>= 1)// value��������һλ��ֱ��ֵ��Ϊ0��ʵ��ÿ��������һ����ͬ����ɫ
	{
		if (background+2<schemes[scheme]+sizeof(original)) {
			background+=2;
			foreground+=2;
		}
	}
	//linux���ն˼�������ɫ���������ַ���
	snprintf(color,length,"\033[38;5;%d;48;5;%dm",*foreground,*background);
}
// �������ݰ�
void drawBoard(uint16_t board[SIZE][SIZE]) {
	int8_t x,y;
	// \033[m:�ر���������
	char color[40], reset[] = "\033[m";
	// \033[H:�������λ��
	printf("\033[H");
	printf("2048.c %17d pts\n\n",score);
	//���ݰ干3��4�У�7��4��
	for (y=0;y<SIZE;y++) {
		//���д�ӡ�հ�
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			printf("       ");
			printf("%s",reset);
		}
		printf("\n");
		//���д�ӡ����,���־���
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			if (board[x][y]!=0) {
				char s[8];
				snprintf(s,8,"%u",board[x][y]);
				int8_t t = 7-strlen(s);
				printf("%*s%s%*s",t-t/2,"",s,t/2,"");
			} else {
				printf("   ��   ");
			}
			printf("%s",reset);
		}
		printf("\n");
		//ĩ�д�ӡ�հ�
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			printf("       ");
			printf("%s",reset);
		}
		printf("\n");
	}
	printf("\n");
	printf("        ��,��,��,�� or q        \n");
	//���ƻس�
	printf("\033[A");
}
// ����һά������x�����ϲ��������꣬stopΪ����
int8_t findTarget(uint16_t array[SIZE],int8_t x,int8_t stop) {
	int8_t t;
	//��xΪ��һ���������������ֱ�ӷ���x 
	if (x==0) {
		return x;
	}
	//����x��ߵ�����
	for(t=x-1;t>=0;t--) {
		//�ϲ��㷨��
		//1.t��������Ϊ0����x����������ȣ�����t+1
		//2.t��������Ϊ0����x��������ȣ�����t
		//3.t������Ϊ0������stop�ж��Ƿ���ǰ���ң���ֹ��κϲ�
		if (array[t]!=0) {
			if (array[t]!=array[x]) {
				return t+1;
			}
			return t;
		} else {
			if (t==stop) {
				return t;
			}
		}
	}
	return x;
}
//��һά��������ƶ�
bool slideArray(uint16_t array[SIZE]) {
	bool success = false;
	//������ǰλ�ã����ϲ���λ�ã�����
	int8_t x,t,stop=0;
	for (x=0;x<SIZE;x++) {
		if (array[x]!=0) {
			t = findTarget(array,x,stop);
			// ������ϲ���λ���뵱ǰλ�ò���ȣ������ƶ����ߺϲ�
			if (t!=x) {
				// ������ϲ���λ�ò���0,���Ƽ���stop
				if (array[t]!=0) {
					score+=array[t]+array[x];
					stop = t+1;
				}
				array[t]+=array[x];
				array[x]=0;
				success = true;
			}
		}
	}
	return success;
}
//��ת���ݰ壬������ת90�ȣ���һ������������ƶ���ӿ����ĸ�������ƶ�
void rotateBoard(uint16_t board[SIZE][SIZE]) {
	int8_t i,j,n=SIZE;
	uint16_t tmp;
	//������ת��������ڣ��������
	for (i=0; i<n/2; i++){
		for (j=i; j<n-i-1; j++){
			tmp = board[i][j];
			board[i][j] = board[j][n-i-1];
			board[j][n-i-1] = board[n-i-1][n-j-1];
			board[n-i-1][n-j-1] = board[n-j-1][i];
			board[n-j-1][i] = tmp;
		}
	}
}
//�����ƶ����ݰ�
bool moveUp(uint16_t board[SIZE][SIZE]) {
	bool success = false;
	int8_t x;
	for (x=0;x<SIZE;x++) {
		//��ÿһ�����ƶ����ߺϲ�����
		//�������ж������У���ǰ������˳���й�
		success |= slideArray(board[x]);
		//ֻҪ��һ�гɹ����ͳɹ�
	}
	return success;
}
// ���ƣ�������ת90�ȣ����Ϻϲ�������ת3��90��
bool moveLeft(uint16_t board[SIZE][SIZE]) {
	bool success;
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}
// ���ƣ�������ת2��90�ȣ����Ϻϲ�������ת2��90��
bool moveDown(uint16_t board[SIZE][SIZE]) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}
// ���ƣ�������ת3��90�ȣ����Ϻϲ�������ת1��90��
bool moveRight(uint16_t board[SIZE][SIZE]) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	return success;
}
bool findPairDown(uint16_t board[SIZE][SIZE]) {
	bool success = false;
	int8_t x,y;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE-1;y++) {
			if (board[x][y]==board[x][y+1]) return true;
		}
	}
	return success;
}
// �������ݰ��Ƿ�����
int16_t countEmpty(uint16_t board[SIZE][SIZE]) {
	int8_t x,y;
	int16_t count=0;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			if (board[x][y]==0) {
				count++;
			}
		}
	}
	return count;
}
// �����Ϸ�Ƿ����
bool gameEnded(uint16_t board[SIZE][SIZE]) {
	bool ended = true;
	// ����п�λ��δ����
	if (countEmpty(board)>0) return false;
	// �����飬�������������δ����
	if (findPairDown(board)) return false;
	rotateBoard(board);
	// ��תһ�Σ������飬�������������δ����
	if (findPairDown(board)) ended = false;
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	return ended;
}
// ����������ݰ�
void addRandom(uint16_t board[SIZE][SIZE]) {
	// ȫ�ֱ������Ƿ��ѳ�ʼ��
	static bool initialized = false;
	// x,y ����
	int8_t x,y;
	// r ���λ�ã�len ����Ϊ�յ����ݰ����ݳ���
	int16_t r,len=0;
	// n ������ݣ� list ����Ϊ�յ����ݰ�λ��
	uint16_t n,list[SIZE*SIZE][2];
	if (!initialized) {
		srand(time(NULL));
		initialized = true;
	}
	// �ҳ����ݰ�������Ϊ�յ�����
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			if (board[x][y]==0) {
				list[len][0]=x;
				list[len][1]=y;
				len++;
			}
		}
	}
	// �����Ϊ�յ���������������
	if (len>0) {
		r = rand()%len;
		x = list[r][0];
		y = list[r][1];
		n = ((rand()%10)/9+1)*2;
		board[x][y]=n;
	}
}
// ��������ģʽ�����л�����޻������л�
void setBufferedInput(bool enable) {
	static bool enabled = true;
	static struct termios old;
	struct termios new;
	if (enable && !enabled) {
		tcsetattr(STDIN_FILENO,TCSANOW,&old);
		enabled = true;
	} else if (!enable && enabled) {
		tcgetattr(STDIN_FILENO,&new);
		old = new;
		new.c_lflag &=(~ICANON & ~ECHO);
		tcsetattr(STDIN_FILENO,TCSANOW,&new);
		enabled = false;
	}
}
int test() {
	uint16_t array[SIZE];
	uint16_t data[] = {
		0,0,0,2,	2,0,0,0,
		0,0,2,2,	4,0,0,0,
		0,2,0,2,	4,0,0,0,
		2,0,0,2,	4,0,0,0,
		2,0,2,0,	4,0,0,0,
		2,2,2,0,	4,2,0,0,
		2,0,2,2,	4,2,0,0,
		2,2,0,2,	4,2,0,0,
		2,2,2,2,	4,4,0,0,
		4,4,2,2,	8,4,0,0,
		2,2,4,4,	4,8,0,0,
		8,0,2,2,	8,4,0,0,
		4,0,2,2,	4,4,0,0
	};
	uint16_t *in,*out;
	uint16_t t,tests;
	uint8_t i;
	bool success = true;
	tests = (sizeof(data)/sizeof(data[0]))/(2*SIZE);
	for (t=0;t<tests;t++) {
		in = data+t*2*SIZE;
		out = in + SIZE;
		for (i=0;i<SIZE;i++) {
			array[i] = in[i];
		}
		slideArray(array);
		for (i=0;i<SIZE;i++) {
			if (array[i] != out[i]) {
				success = false;
			}
		}
		if (success==false) {
			for (i=0;i<SIZE;i++) {
				printf("%d ",in[i]);
			}
			printf("=> ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",array[i]);
			}
			printf("expected ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",in[i]);
			}
			printf("=> ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",out[i]);
			}
			printf("\n");
			break;
		}
	}
	if (success) {
		printf("All %u tests executed successfully\n",tests);
	}
	return !success;
}
void signal_callback_handler(int signum) {
	printf("         TERMINATED         \n");
	setBufferedInput(true);
	printf("\033[?25h");
	exit(signum);
}
int main(int argc, char *argv[]) {
	uint16_t board[SIZE][SIZE];
	char c;
	bool success;
	if (argc == 2 && strcmp(argv[1],"test")==0) {
		return test();
	}
	if (argc == 2 && strcmp(argv[1],"blackwhite")==0) {
		scheme = 1;
	}
	if (argc == 2 && strcmp(argv[1],"bluered")==0) {
		scheme = 2;
	}
	// 33[?25l ���ع��
	// 33[2J ����
	// 33[H ���ù��λ��
	printf("\033[?25l\033[2J\033[H");
	signal(SIGINT, signal_callback_handler);// ��������Ϊ0
	memset(board,0,sizeof(board));// ������������,��Ϊ��ʼ��ʱ����2�������
	addRandom(board);addRandom(board);// �������ݰ�
	drawBoard(board);// ���û������룬�ն�֧�ְ��ַ���ȡ�Ҳ�����
	setBufferedInput(false);// ��Ϸ��ѭ��
	while (true) {
		c=getchar();
		switch(c) {
			case 68:	// left arrow
				success = moveLeft(board);  break;
			case 67:	// right arrow
				success = moveRight(board); break;
			case 65:	// up arrow
				success = moveUp(board);    break;
			case 66:	// down arrow
				success = moveDown(board);  break;
			default: success = false;
		}
		//�ϲ��ɹ��������»���
		if (success) {
			drawBoard(board);
			usleep(150000);
			addRandom(board);
			drawBoard(board);
			if (gameEnded(board)) {
				printf("         GAME OVER          \n");
				break;
			}
		}
		// ��������� q �Ļ������л��壬��ʾ���
		if (c=='q') {
			printf("        QUIT? (y/n)         \n");
			while (true) {
				c=getchar();
				if (c=='y'){
					setBufferedInput(true);
					printf("\033[?25h");
					exit(0);
				}
				else {
					drawBoard(board);
					break;
				}
			}
		}
	}
	setBufferedInput(true);
	printf("\033[?25h");
	return EXIT_SUCCESS;
}
