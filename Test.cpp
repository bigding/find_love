
#include "stdafx.h"
#include <cstdio>
#include <ctime>
#include <cmath>

#include "mmsystem.h"//��������ͷ�ļ�
#pragma comment(lib,"winmm.lib")//��������ͷ�ļ���

#define NUM_HUANG 15	//ͬһ����������Ļ��������Ŀ
#define NUM_HEART 2		//��ʾ���ĳ��ֵ�����
#define NUM_KISS 30		//����֮������Ļ����������
#define IDB_YI 10001
#define IDB_ZHONG 10002
#define IDB_NAN 10003
#define IDB_CON 10004
#define IDB_ESC 10005
#define IDB_RESTART 10006
#define IDB_ESCC 10007

HINSTANCE hInst;
HBITMAP   bg,bmp_huang_bo[3],gao_yi_xiang[3],lin_zhi_ling,lin_zhi_ling1,lin_zhi_ling2,bmp_kiss,gameover[2],process_bar,start,heart,bmp_black_heart,pause,fool_wei;  //�������λͼ����Ϊ���飬��ʾ�������Ѷ�
HDC	      mdc,hdc,bufdc;
HWND      hWnd,h_wnd,button1,button2,button3,button4,button_con,button_esc,button_esc1,button_restart;
DWORD  	  tPre,tNow;

int x_bg=0,person[NUM_HUANG][4],kiss[NUM_KISS][3];		//personΪ��ά���飬һ����Ļ���15��ͬһ����ĸ���־λ�ֱ��ʾ�Ƿ���ڣ��Լ��������� ͼƬ���ѡ��
														//kiss����ҲΪ��ά���飬��������ͼƬ���ѡ��ı�־λ
int black_heart[NUM_HUANG][3];							//��ά���飬���15���ӵ����ĸ�λ�÷ֱ��ʾ�Ƿ���ڣ�
int red_heart[NUM_HEART][3];							//���ĵı�־���飬���ͬʱ�����������ģ�������λ��ʾ�Ƿ���ڣ�����
int black_heart1[3][4][3];									//�����������������ĵı�־ black_heart1[n][0][0]��ʾ�Ƿ����
int x_lin=790,y_lin=200,bcount=30,count=0;				//count������
int nowX,nowY,i;
int random = 0;							//�����
int level = 1;							//��Ϸ�ؿ���־���������ؿ�
int life = 3,score = 0;					//����ֵ�ͷ���
int difficulty = 1;						//�Ѷ�ϵ������1/2/3�����ȼ���3��򵥣�1���ѣ�Խ���˵�ͷԽС��Ĭ���Ѷ�ϵ��Ϊ1��
int width_of_pic=70,height_of_pic=70;   //����Ŀ��ͼƬ�Ŀ�Ⱥ͸߶ȣ�Ĭ�϶�Ϊ70px, �����Ϊ60,50px.
double distance = 3;
bool state_heart=true;
int heart_state = 0;
int  total_score = 500;					//��ǰ�ؿ��Ļ�������
int boss_state[3],boss[2];						//boss��״̬���Լ���������,�Լ���ô�˶��ı�־
int width_of_lin = 90;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);

void  ChangeCoordinate();   //�ı����ͼ�������λ��
void judge();				//�жϸ��ֳ�ײ
VOID CALLBACK OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );	//��ʱ���ص�����   ���ڴ�������¼�
VOID CALLBACK OnTimer2(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );	//��ʱ���ص�����   ���ڴ�������¼�
void Degree(int deg);				//��ʼ����Ϸ���Ѷ�
void KeyState();					//���̰���ȥ������ж�
void LevelChange(int level);		//��Ϸ�ȼ������仯

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	srand((unsigned)time(NULL));
	random = rand()%100;

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//��ʼ��
	for(int i = 0; i < NUM_HUANG ;i++){
		for(int j = 0;j < 4; j++){
			person[i][j] = 0;
		}
	}
	
	for(int p = 0;p < NUM_HEART;p++){
		for(int q = 0;q < 3;q++){
			red_heart[p][q] = 0;
		}
	}

	for(int m = 0; m < NUM_KISS ; m++){
		for(int n = 0;n < 3; n++){
			kiss[m][n] = 0;
			black_heart[m][n] = 0;
		}
	}
	boss_state[0] = 0;
	boss_state[1] = 1;
	boss_state[2] = 2;
	boss[0] = 0;
	boss[1] = 0;
//	boss_state[1] = 40;
	boss_state[1] = 400;
	boss_state[2] = 150;

/*	while (msg.message!=WM_QUIT) 
	{
		if( PeekMessage( &msg, NULL, 0,0 ,PM_REMOVE) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		else
		{
			tNow = GetTickCount();
			if(tNow-tPre >= 20)
				MyPaint(hdc);
		}
	}
	*/
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "canvas";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	HBITMAP bmp;

	hInst = hInstance;

	hWnd = CreateWindow("canvas", "Windows" , WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,100,100,900,500,true);
	ShowWindow(hWnd, nCmdShow);
	h_wnd = hWnd;
	UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);

	bmp = CreateCompatibleBitmap(hdc,900,500);
	SelectObject(mdc,bmp);

//	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,600, 450, LR_LOADFROMFILE); 
	bmp_huang_bo[0]=(HBITMAP)LoadImage(NULL,"huang.bmp",IMAGE_BITMAP,140,70,LR_LOADFROMFILE);
	bmp_huang_bo[1]=(HBITMAP)LoadImage(NULL,"huang2.bmp",IMAGE_BITMAP,120,60,LR_LOADFROMFILE);
	bmp_huang_bo[2]=(HBITMAP)LoadImage(NULL,"huang3.bmp",IMAGE_BITMAP,100,50,LR_LOADFROMFILE);
	gao_yi_xiang[0]=(HBITMAP)LoadImage(NULL,"gao.bmp",IMAGE_BITMAP,140,70,LR_LOADFROMFILE);
	gao_yi_xiang[1]=(HBITMAP)LoadImage(NULL,"gao2.bmp",IMAGE_BITMAP,120,60,LR_LOADFROMFILE);
	gao_yi_xiang[2]=(HBITMAP)LoadImage(NULL,"gao3.bmp",IMAGE_BITMAP,100,50,LR_LOADFROMFILE);
	lin_zhi_ling=(HBITMAP)LoadImage(NULL,"lin.bmp",IMAGE_BITMAP,180,90,LR_LOADFROMFILE);
	lin_zhi_ling1=(HBITMAP)LoadImage(NULL,"lin1.bmp",IMAGE_BITMAP,160,80,LR_LOADFROMFILE);
	lin_zhi_ling2=(HBITMAP)LoadImage(NULL,"lin2.bmp",IMAGE_BITMAP,140,70,LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	bmp_kiss = (HBITMAP)LoadImage(NULL,"kiss.bmp",IMAGE_BITMAP,80,27,LR_LOADFROMFILE); 
	gameover[0] = (HBITMAP)LoadImage(NULL,"gameover1.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	gameover[1] = (HBITMAP)LoadImage(NULL,"gameover2.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	start = (HBITMAP)LoadImage(NULL,"start.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	process_bar = (HBITMAP)LoadImage(NULL,"process_bar.bmp",IMAGE_BITMAP,600,225,LR_LOADFROMFILE); 
	heart = (HBITMAP)LoadImage(NULL,"heart.bmp",IMAGE_BITMAP,100,43,LR_LOADFROMFILE); 
	bmp_black_heart = (HBITMAP)LoadImage(NULL,"black_heart.bmp",IMAGE_BITMAP,80,34,LR_LOADFROMFILE); 
	pause = (HBITMAP)LoadImage(NULL,"pause.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	fool_wei = (HBITMAP)LoadImage(NULL,"fool.bmp",IMAGE_BITMAP,240,190,LR_LOADFROMFILE); 


	SelectObject(mdc,start);
	BitBlt(hdc,0,0,900,500,mdc,0,0,SRCCOPY);

	button1 = CreateWindow("Button", "һ���Ѷ�", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
            100, 210, 100, 35, hWnd,  (HMENU)IDB_YI, hInstance, NULL);
    button2 = CreateWindow("Button", "�е��Ѷ�", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
            100, 280, 100, 35, hWnd,  (HMENU)IDB_ZHONG, hInstance, NULL);
	button3 = CreateWindow("Button", "�߼��Ѷ�", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
            100, 350, 100, 35, hWnd,  (HMENU)IDB_NAN, hInstance, NULL);

	button_con = CreateWindow("Button", "��һ��", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
			100, 210, 100, 35, hWnd,  (HMENU)IDB_CON, hInstance, NULL);
	button_esc = CreateWindow("Button", "�˳���Ϸ",WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
			100, 280, 100, 35, hWnd,  (HMENU)IDB_ESC, hInstance, NULL);
	ShowWindow(button_con,SW_HIDE);
	ShowWindow(button_esc,SW_HIDE);

	return TRUE;
}

void MyPaint(HDC hdc)
{
	char str[20]={""};

//����
	SelectObject(bufdc, bg);
	BitBlt(mdc,0,0,x_bg,500,bufdc,900-x_bg,0,SRCCOPY);
	BitBlt(mdc,x_bg,0,900-x_bg,500,bufdc,0,0,SRCCOPY);

//��ͷ����ͼ
	SelectObject(bufdc,bmp_kiss);
	for(int m = 0;m < NUM_KISS;m++){
		if(kiss[m][0] == 0) continue;
		BitBlt(mdc,kiss[m][1],kiss[m][2],40,27,bufdc,40,0,SRCAND);  
		BitBlt(mdc,kiss[m][1],kiss[m][2],40,27,bufdc,0,0,SRCPAINT); 
	}


//���ĵ���ͼ
	for(int n = 0;n < 2;n++){
		if(red_heart[n][0]==1){
			SelectObject(bufdc,heart);                     
			BitBlt(mdc,red_heart[n][1],red_heart[n][2],50,43,bufdc,50,0,SRCAND);  
			BitBlt(mdc,red_heart[n][1],red_heart[n][2],50,43,bufdc,0,0,SRCPAINT); 
		}
	}
//���ĵ���ͼ
	if(level == 3 || level == 5){
		for(int y = 0;y < NUM_HUANG;y++){
			if(black_heart[y][0]==1){
				SelectObject(bufdc,bmp_black_heart);                     
				BitBlt(mdc,black_heart[y][1],black_heart[y][2],40,34,bufdc,40,0,SRCAND);  
				BitBlt(mdc,black_heart[y][1],black_heart[y][2],40,34,bufdc,0,0,SRCPAINT); 
			}
		}	
	}

	if(level == 5){
		for(int ko = 0; ko < 3;ko++){
			if(black_heart1[ko][0][0] == 1){
				for(int ki = 1; ki < 4;ki++){
					if(black_heart1[ko][ki][0] == 1){
						SelectObject(bufdc,bmp_black_heart);                     
						BitBlt(mdc,black_heart1[ko][ki][1],black_heart1[ko][ki][2],40,34,bufdc,40,0,SRCAND);  
						BitBlt(mdc,black_heart1[ko][ki][1],black_heart1[ko][ki][2],40,34,bufdc,0,0,SRCPAINT);
					}
				}
			}
		}
	}


//�������ֵ���ͼ  
	for(int i = 0; i < NUM_HUANG;i++){
		if(person[i][0] == 0) continue;
		if(person[i][3] == 0){
			SelectObject(bufdc,gao_yi_xiang[difficulty-1]); 
			BitBlt(mdc,person[i][1],person[i][2],width_of_pic,height_of_pic,bufdc,width_of_pic,0,SRCAND);   
			BitBlt(mdc,person[i][1],person[i][2],width_of_pic,height_of_pic,bufdc,0,0,SRCPAINT);
		}
		else if(person[i][3] == 1){
			SelectObject(bufdc,bmp_huang_bo[difficulty-1]); 
			BitBlt(mdc,person[i][1],person[i][2],width_of_pic,height_of_pic,bufdc,width_of_pic,0,SRCAND);   
			BitBlt(mdc,person[i][1],person[i][2],width_of_pic,height_of_pic,bufdc,0,0,SRCPAINT);
		}
	}


//��־��
	if(difficulty == 1)
		SelectObject(bufdc,lin_zhi_ling);  
	if(difficulty == 2)
		SelectObject(bufdc,lin_zhi_ling1);  
	if(difficulty == 3)
		SelectObject(bufdc,lin_zhi_ling2);  
	BitBlt(mdc,x_lin,y_lin,width_of_lin,width_of_lin,bufdc,width_of_lin,0,SRCAND);  
	BitBlt(mdc,x_lin,y_lin,width_of_lin,width_of_lin,bufdc,0,0,SRCPAINT); 

//boss����ͼ
	if(boss_state[0] == 1){
		SelectObject(bufdc,fool_wei);                     
		BitBlt(mdc,boss_state[1],boss_state[2],120,190,bufdc,120,0,SRCAND);  
		BitBlt(mdc,boss_state[1],boss_state[2],120,190,bufdc,0,0,SRCPAINT); 
	}

//����ֵ����������ͼ
	SelectObject(bufdc,process_bar); 
	BitBlt(mdc,50,0,150,32,bufdc,450,64*life,SRCAND);  
	BitBlt(mdc,50,0,150,32,bufdc,150,64*life,SRCPAINT); 

//����ֵ������������ͼ
	SelectObject(bufdc,process_bar); 
	BitBlt(mdc,400,6,150,30,bufdc,450,6,SRCAND);  
	BitBlt(mdc,400,6,150,30,bufdc,150,6,SRCPAINT); 

//�˴��Ľ�������Ҫ����������ʾ
	SelectObject(bufdc,process_bar); 
	BitBlt(mdc,406,-3,120*((float)score/(float)total_score),30,bufdc,323,30,SRCAND);  
	BitBlt(mdc,406,-3,120*((float)score/(float)total_score),30,bufdc,23,30,SRCPAINT); 

	sprintf(str,"����ֵ");
	TextOut(mdc,0,10,str,strlen(str));
//	sprintf(str,"����ֵ  %d    ",score);
	sprintf(str,"����ֵ ");
	TextOut(mdc,320,10,str,strlen(str));
	//��������ʾ����
//	sprintf(str,"level  %d    ",level);
//	TextOut(mdc,300,50,str,strlen(str));
//	sprintf(str,"%d  %d    ",boss_state[1],boss_state[2]);
//	TextOut(mdc,300,80,str,strlen(str));

	if(life == 0){
		KillTimer(h_wnd, 1);

		SelectObject(bufdc,gameover[0]);
		BitBlt(mdc,0,0,900,500,bufdc,0,0,SRCCOPY);
	}
	if(score >= total_score){   //�÷�***����Ϸ�ɹ�		
		level++;
		KillTimer(h_wnd, 1);
	//���ϱ���ͼƬ
		if(level > 5){
			SelectObject(bufdc,gameover[1]);
			BitBlt(mdc,0,0,900,500,bufdc,0,0,SRCCOPY);
		}
		else{
			SelectObject(bufdc,pause);
			BitBlt(mdc,0,0,900,500,bufdc,0,0,SRCCOPY);
	//		SetTimer(hWnd,1,20,(TIMERPROC)OnTimer2);
	//		button1 = CreateWindow("Button", "һ���Ѷ�", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
	//            100, 210, 100, 35, hWnd,  (HMENU)IDB_YI, hInst, NULL);

			ShowWindow(button_con,SW_SHOW);
			ShowWindow(button_esc,SW_SHOW);
		}



	}

	BitBlt(hdc,0,0,900,500,mdc,0,0,SRCCOPY);
	
	
	tPre = GetTickCount();

  }

VOID CALLBACK OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime ){

//	Degree(difficulty);			//��ʼ����Ϸ���Ѷ�
   
	MyPaint(GetDC(h_wnd));

	judge();			// �жϸ��ֳ�ײ

	ChangeCoordinate(); //����ͼ������ı仯

	KeyState();			//���̰��µ��ж�
}

VOID CALLBACK OnTimer2(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime ){

//	Degree(difficulty);			//��ʼ����Ϸ���Ѷ�
   
	MyPaint(GetDC(h_wnd));

	judge();			// �жϸ��ֳ�ײ

	ChangeCoordinate(); //����ͼ������ı仯

	KeyState();			//���̰��µ��ж�
}

//��ʼ����Ϸ���Ѷ�

void Degree(int deg){
	difficulty = deg;  //�Ѷ�ϵ��Ϊ1���Ѷȹ�����������1,2,3�� 3Ϊ���ѣ�������С
	if(difficulty == 1){
		width_of_pic = 70;
		height_of_pic = 70;
		width_of_lin = 90;
	}
	if(difficulty == 2){
		width_of_pic = 60;
		height_of_pic = 60;
		width_of_lin = 80;
	}
	if(difficulty == 3){
		width_of_pic = 50;
		height_of_pic = 50;
		width_of_lin = 70;
	}
}

/*�ı����ͼ�������λ��*/
void ChangeCoordinate(){
	x_bg += 0;		//����
	if(x_bg==900)
		x_bg = 0;

	//�Ʋ���ͼ��仯
		/*���Ƴ���*/
	count++;
	if(count%20 == 0){
		for(int j = 0;j < 15;j++){
			if(person[j][0] == 1) continue;
			if(person[j][0] == 0){
				person[j][0] = 1;
				random+=j;
				srand(random);
				int y =26  * (rand()%15);
				int tmp = rand()%3;
				person[j][2] = y+30;
				if(tmp == 2) person[j][3] = 1;
				else{
					person[j][3] = 0;
				}
				break;
			}
		}
	}
		/*��������仯*/
	if(count%30 == 0) distance+=0.05;
	if(distance > 5) distance = 5;
	for(int i = 0;i < NUM_HUANG;i++){
		if(person[i][0] == 1){
			person[i][1] += distance;  //�������ʾ��  ��������Լ�,����Ϸ���У�����Խ��Խ��
			/*����߳����������򣬾ͽ���Ӧ��־��ԭ*/
			if(person[i][1] > 900){
				person[i][0] = 0;
				person[i][1] = 0;
				person[i][2] = 0;
				person[i][3] = 0;
			}
		}
	}
	if(level == 3){//�ڶ���ʱ
		//�Ʋ���������ĺ��Ŀ��ƴ���
			/*���ƺ��ĵĳ���*/
		for(int t = 0; t < NUM_HUANG;t++){
			random+=t;
			if(person[t][0] == 1 && person[t][3] == 1 && black_heart[t][0] == 0){	//������Ϊ�Ʋ�ʱ
				srand(random);
				int tmp = rand() % 6;
				if(tmp == 1){
					black_heart[t][0] = 1;  //һ�ź��ĳ���
					black_heart[t][1] = person[t][1]+5;
					black_heart[t][2] = person[t][2]+15;
				}
			}
		}
			/*���ƺ��ĵ�����仯*/
		for(int r = 0 ; r < NUM_HUANG; r++){
			if(black_heart[r][0] == 1){
				black_heart[r][1]+=6;
				if(black_heart[r][1] > 900){
				//	black_heart[r][0] == 0;
				}
			}
		}
	}

	if(level == 5){ //������ʱ
			//�Ʋ���������ĺ��Ŀ��ƴ���
			/*���ƺ��ĵĳ���*/
		for(int t = 0; t < NUM_HUANG;t++){
			random+=t;
			if(person[t][0] == 1 && person[t][3] == 1 && (black_heart[t][0] == 0)){	//������Ϊ�Ʋ�ʱ
				if(true){
					int tmp2 = rand()%2;
			//		tmp2 = 1;
					if(tmp2){
						black_heart[t][0] = 1;  //һ�ź��ĳ���
						black_heart[t][1] = person[t][1]+5;
						black_heart[t][2] = person[t][2]+15;
					}
					else{
						for(int mm = 0; mm < 3; mm++){
							if(black_heart1[mm][0][0] == 0){
								black_heart[t][0] = 1;			//���ź��ĳ���
								black_heart[t][1] = person[t][1]+10;
								black_heart[t][2] = person[t][2]+15;
								black_heart1[mm][0][0] = 1;		//���ź��ĳ���
								black_heart1[mm][1][0] = 1;  
								black_heart1[mm][2][0] = 1;
								black_heart1[mm][3][0] = 1;
								black_heart1[mm][1][1] = person[t][1]+5;
								black_heart1[mm][2][1] = person[t][1]+10;
								black_heart1[mm][3][1] = person[t][1]+5;
								black_heart1[mm][1][2] = person[t][2]+0; 
								black_heart1[mm][2][2] = person[t][2]+15;
								black_heart1[mm][3][2] = person[t][2]+30;
							}
						}
					}
					
					
				}
			}
		}
			/*���ƺ��ĵ�����仯*/
		for(int rr = 0 ; rr < NUM_HUANG; rr++){
			if(black_heart[rr][0] == 1){
				black_heart[rr][1]+=6;
				if(black_heart[rr][1] > 900){
					black_heart[rr][0] = 0;
				}
			}
		}
		for(int kr = 0 ;kr < 3;kr++){
			if(black_heart1[kr][0][0] == 1){
				black_heart1[kr][1][1] += 6;
				black_heart1[kr][2][1] += 6;
				black_heart1[kr][3][1] += 6;
				black_heart1[kr][1][2] -= 1;
				black_heart1[kr][3][2] += 1;
				if(black_heart1[kr][1][1] > 900 || black_heart1[kr][1][2] < -34) black_heart1[kr][1][0] =0;
				if(black_heart1[kr][2][1] > 900) black_heart1[kr][2][0] =0;
				if(black_heart1[kr][3][1] > 900 || black_heart1[kr][3][2]<500) black_heart1[kr][0][0] +=0;
				if(black_heart1[kr][1][0] ==0&&black_heart1[kr][2][0] ==0&&black_heart1[kr][3][0] ==0) black_heart1[kr][0][0] = 0 ;
			}
		}
	}

	/*kiss������仯*/
	for(int  m = 0;m < NUM_KISS; m++){
		if(kiss[m][0] == 1){
			kiss[m][1] -= 5;
			random = (random+m)%100;
			if(kiss[m][1] < 0){
				kiss[m][0] = 0;
				kiss[m][1] = 0;
				kiss[m][2] = 0;
			}
		}
	}

	//���ĵ�����仯
	for(int p = 0;p < NUM_HEART; p++){
		if(red_heart[p][0]==1){
			red_heart[p][1]+=3;
			if(heart_state == 0){	//���������˶�
				red_heart[p][2]+=3;
			}
			if(heart_state == 1){	//���������˶�
				red_heart[p][2]-=3;
			}
			if(red_heart[p][2] < 0) heart_state = 0;
			else if(red_heart[p][2] > 420) heart_state = 1;
			if(red_heart[p][1] > 900){
				red_heart[p][0]=0;
				red_heart[p][1]=0;
				red_heart[p][2]=0;
			}
		}
	}
	if(level == 5){
		//boss����
		if((float)score/(float)total_score >= 0.5){
			boss_state[0] = 1;
		}
		if(boss_state[0] == 1){
			if(boss[0] == 0) boss_state[1] += 5;
			else boss_state[1] -= 5;
			if(boss[1] == 0) boss_state[2] += 5;
			else boss_state[2] -= 5;
			if(boss_state[1] > 600) boss[0] = 1;
			if(boss_state[1] < 0) boss[0] = 0;
			if(boss_state[2] > 250) boss[1] = 1;
			if(boss_state[2] < 40) boss[1] = 0;
		}	
	}

}
/*�жϸ��ֳ�ײ*/
void judge(){
	//�ӵ��Ƿ������˻Ʋ������������ϣ�������Ӧ���жϣ�person[i][3]Ϊ1��ʾ�Ʋ���Ϊ0��ʾ������
	for(int i = 0;i < NUM_KISS;i++){
		int state = 0;									//״̬����
		if(kiss[i][0] == 1){
			random+=i;
			for(int j = 0; j < NUM_HUANG;j++){
				if(person[j][0] == 1){
					if(kiss[i][1] - person[j][1] < 38){   //������
						if((kiss[i][2] - person[j][2]) < 60 && (kiss[i][2] - person[j][2]) > -20){	//������
							PlaySound("b.wav", NULL, SND_FILENAME | SND_ASYNC);
							if(person[j][3] == 1){			//����Ϊ�Ʋ�
								life -= 1;	
							}
							if(person[j][3] == 0){			//����Ϊ������
								score += 25;
								int tmp = rand()%6;
								if(tmp == 1){			//�ú��ĳ���
									for(int t = 0;t < NUM_HEART ;t++){
										if(red_heart[t][0] == 0){
											red_heart[t][0] = 1;
											red_heart[t][1] = person[j][1]+10;
											red_heart[t][2] = person[j][2]+10;
											break;
										}
									}
								}
							}	
							if(life > 3){
								life = 3;
							}
							if(life <= 0){
								life = 0;
							}
							kiss[i][0] = 0;
							kiss[i][1] = 0;
							kiss[i][2] = 0;
							person[j][0] = 0;
							person[j][1] = 0;
							person[j][2] = 0;
							person[j][3] = 0;
							state = 1;
						}
					}
				}
				if(state) break;
			}
		}
	}
//�ӵ��Ƿ��boss��ײ
	for(int ii = 0;ii < NUM_KISS;ii++){
		if(kiss[ii][0] == 1){							//�ӵ�����
			if(boss_state[0] == 1){						//boss����
				if(kiss[ii][1] - boss_state[1] < 120 && kiss[ii][1] - boss_state[1] > -27){		//������
					if((kiss[ii][2] - boss_state[2]) < 190 && (kiss[ii][2] - boss_state[2]) > -20){	//������
						score-=25;
						if(score < 0) score = 0;
					}
				}
			}
		}
	}
	//��־���boss�ǲ�����ײ
	if(boss_state[0] == 1){
		if(x_lin - boss_state[1] < 120 && x_lin >boss_state[1] -width_of_lin){
			if(y_lin -boss_state[2] <190 && y_lin >boss_state[2] -width_of_lin){
				life-=1;
				x_lin = 790;
				y_lin = 180;
			}
		}
	}
	//�����Ƿ�����־����ײ
	for(int p = 0;p < NUM_HEART;p++){
		if(red_heart[p][0]==1){
			if(x_lin - red_heart[p][1] <50){
				if(y_lin - red_heart[p][2] <43 && y_lin >red_heart[p][2] -width_of_lin){
					if(life < 3) life++;
					red_heart[p][0] = 0;
					red_heart[p][1] = 0;
					red_heart[p][2] = 0;

				}
			}
		}
	}
	//��ɫ�İ����Ƿ����־����ײ,��ײ���50��
	for(int tt = 0 ; tt < NUM_HUANG;tt++){
		if(black_heart[tt][0] == 1){
			if(x_lin - black_heart[tt][1] < 40  && x_lin >black_heart[tt][1] -width_of_lin){
				if(y_lin - black_heart[tt][2] <34 && y_lin> black_heart[tt][2]-70){
					if(score > 0){
						score -= 50;
						if(score < 0) score = 0;
					}
						black_heart[tt][0] = 0;
						black_heart[tt][1] = 0;
						black_heart[tt][2] = 0;
				}
			}
		}
	}
	for(int mn = 0; mn < 3; mn++){
		if(black_heart1[mn][0][0] == 1){
			for(int ml = 1; ml < 4; ml++){
				if(black_heart1[mn][ml][0] == 1){
					if(x_lin - black_heart1[mn][ml][1] <40 && x_lin - black_heart1[mn][ml][1] > -70){
						if(y_lin - black_heart1[mn][ml][2] <34 && y_lin -black_heart1[mn][ml][2] > -70){
							if(score > 0){
								score -= 50;
								if(score < 0) score = 0;	
							}
							black_heart1[mn][ml][0] = 0;
							black_heart1[mn][ml][1] = 0;
							black_heart1[mn][ml][2] = 0;
						
						}
					}
				}
			}
		}
	}

	//�Ʋ��Ƿ����־����ײ
	for(int m = 0; m < NUM_HUANG;m++){
		if(person[m][0] == 1){
			if(x_lin - person[m][1] < 60 && x_lin  >person[m][1] -width_of_lin){
				if(y_lin - person[m][2] < 70 && y_lin > person[m][2] - width_of_lin +5){
					if(person[m][3] == 1){
						life -= 1;
					}
					else if(person[m][3] == 0){
						score += 25;
					}
			/*		if(score > 500){
						life += 1;
						score = 0 ;
					}*/
					if(life > 3){
						life = 3;
					}
					if(life <= 0){
						life = 0;
					}
					PlaySound("b.wav", NULL, SND_FILENAME | SND_ASYNC);
					person[m][0] = 0;
					person[m][1] = 0;
					person[m][2] = 0;
					person[m][3] = 0;
				}
			}
		}
	}
}

//��������һ��ʱ������Ӧ�Ĳ���
void LevelChange(int level){
		//��ʼ��
	for(int i = 0; i < NUM_HUANG ;i++){
		for(int j = 0;j < 4; j++){
			person[i][j] = 0;
		}
	}
	
	for(int p = 0;p < NUM_HEART;p++){
		for(int q = 0;q < 3;q++){
			red_heart[p][q] = 0;
		}
	}

	for(int m = 0; m < NUM_KISS ; m++){
		for(int n = 0;n < 3; n++){
			kiss[m][n] = 0;
			black_heart[m][n] = 0;
		}
	}
	x_lin = 790;
	y_lin = 180;

	if(level == 3) total_score = 500;
	if(level == 5) total_score = 600;
}

void KeyState(){
	if(GetAsyncKeyState(VK_UP)) y_lin-=10;
	if(GetAsyncKeyState(VK_DOWN)) y_lin+=10;
	if(GetAsyncKeyState(VK_LEFT))
		if(level == 5)		x_lin-=10;
		else y_lin -= 10;
	if(GetAsyncKeyState(VK_RIGHT)) 
		if(level == 5)		x_lin+=10;
		else y_lin;
	if(GetAsyncKeyState(VK_ESCAPE))  PostQuitMessage(0);
	if(y_lin < 0){
		y_lin = 0;
	}
	if(y_lin > 370){
		y_lin = 370;
	}
	if(x_lin < 0){
		x_lin = 0;
	}
	if(x_lin > 790){
		x_lin = 790;
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int i;

	switch (message)
	{
		case WM_PAINT:						
			hdc = BeginPaint(hWnd, &ps);
			MyPaint(hdc);
			EndPaint(hWnd, &ps);
			break;

		case WM_KEYDOWN:			    
			switch (wParam) 
			{
				case VK_SPACE:		
					for(i=0;i<30;i++)
					{
						if(kiss[i][0] == 0){
							kiss[i][0] = 1;
							kiss[i][1] = x_lin;
							kiss[i][2] = y_lin + 45;
							break;
						}
					}
					break;
				case VK_ESCAPE:          
					PostQuitMessage( 0 );
					break;
				}
			break;
		case WM_DESTROY:
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(bg);
			DeleteObject(bmp_huang_bo);
			DeleteObject(gao_yi_xiang);
			DeleteObject(lin_zhi_ling);
			DeleteObject(heart);
			DeleteObject(black_heart);
			DeleteObject(kiss);
			DeleteObject(start);
			DeleteObject(gameover);
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
		{     
            switch(LOWORD(wParam))     
			{   
			    case IDB_YI:
					ShowWindow(button1, SW_HIDE);
					ShowWindow(button2, SW_HIDE);
					ShowWindow(button3, SW_HIDE);
					ShowWindow(button4, SW_HIDE);
					Degree(3);
					SetTimer(hWnd,1,20,(TIMERPROC)OnTimer);
                    break;

                case IDB_ZHONG:
                //MessageBox(hWnd, "������˵�һ����ť��", "��ʾ", MB_OK | MB_ICONINFORMATION); 
					ShowWindow(button1, SW_HIDE);
					ShowWindow(button2, SW_HIDE);
					ShowWindow(button3, SW_HIDE);
					ShowWindow(button4, SW_HIDE);
					Degree(2);
					SetTimer(hWnd,1,20,(TIMERPROC)OnTimer);
                    break;   
				case IDB_NAN:     
                //MessageBox(hWnd, "������˵�һ����ť��", "��ʾ", MB_OK | MB_ICONINFORMATION); 
					ShowWindow(button1, SW_HIDE);
					ShowWindow(button2, SW_HIDE);
					ShowWindow(button3, SW_HIDE);
					ShowWindow(button4, SW_HIDE);
					Degree(1);
					SetTimer(hWnd,1,20,(TIMERPROC)OnTimer);
                    break;
				case IDB_CON:     
                //MessageBox(hWnd, "������˵�һ����ť��", "��ʾ", MB_OK | MB_ICONINFORMATION); 
					ShowWindow(button_con, SW_HIDE);
					ShowWindow(button_esc, SW_HIDE);
					score = 0;
					LevelChange(level);
					SetTimer(hWnd,1,20,(TIMERPROC)OnTimer);
                    break;
				case IDB_ESC:     
                //MessageBox(hWnd, "������˵�һ����ť��", "��ʾ", MB_OK | MB_ICONINFORMATION); 
					ShowWindow(button_con, SW_HIDE);
					ShowWindow(button_esc, SW_HIDE);
					PostQuitMessage(0);
                    break;

                default:     
                    break;     
			}     
		}  
		break;
		default:						
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
