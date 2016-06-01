
#include "stdafx.h"
#include <cstdio>
#include <ctime>
#include <cmath>

#include "mmsystem.h"//��������ͷ�ļ�
#pragma comment(lib,"winmm.lib")//��������ͷ�ļ���

#define NUM_HUANG 15  //ͬһ����������Ļ��������Ŀ
#define NUM_HEART 2   //��ʾ���ĳ��ֵ�����
#define NUM_KISS 30
#define TOTAL_SCORE 100
#define IDB_YI 10001
#define IDB_ZHONG 10002
#define IDB_NAN 10003

HINSTANCE hInst;
HBITMAP   bg,bmp_huang_bo[3],gao_yi_xiang[3],lin_zhi_ling,bmp_kiss,gameover[2],process_bar,start,heart;  //�������λͼ����Ϊ���飬��ʾ�������Ѷ�
HDC	      mdc,hdc,bufdc;
HWND      hWnd,h_wnd,button1,button2,button3,button4;
DWORD  	  tPre,tNow;

int x_bg=0,person[NUM_HUANG][4],kiss[NUM_KISS][3]; //personΪ��ά���飬һ����Ļ���15��ͬһ����ĸ���־λ�ֱ��ʾ�Ƿ���ڣ��Լ��������� ͼƬ���ѡ��
													//kiss����ҲΪ��ά���飬��������ͼƬ���ѡ��ı�־λ
int red_heart[NUM_HEART][3];					//���ĵı�־���飬���ͬʱ�����������ģ�������λ��ʾ�Ƿ���ڣ�����
int x_lin=790,y_lin=200,bcount=30,count=0;//count������
int nowX,nowY,i;
int random = 0;							//�����
int life = 3,score = 0;							//����ֵ�ͷ���
int level = 1;							//�Ѷ�ϵ������1/2/3�����ȼ���1��򵥣�3���ѣ�Խ���˵�ͷԽС��Ĭ���Ѷ�ϵ��Ϊ1��
int width_of_pic=70,height_of_pic=70;   //����Ŀ��ͼƬ�Ŀ�Ⱥ͸߶ȣ�Ĭ�϶�Ϊ70px, �����Ϊ60,50px.
double distance = 3;
bool state_heart=true;
int heart_state = 0;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);

void  ChangeCoordinate();   //�ı����ͼ�������λ��
void judge();				//�жϸ��ֳ�ײ
VOID CALLBACK OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime ); ///��ʱ���ص�����   ���ڴ�������¼�
void Degree(int deg);				//��ʼ����Ϸ���Ѷ�

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
		}
	}

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
	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	bmp_kiss = (HBITMAP)LoadImage(NULL,"kiss.bmp",IMAGE_BITMAP,80,27,LR_LOADFROMFILE); 
	gameover[0] = (HBITMAP)LoadImage(NULL,"gameover1.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	gameover[1] = (HBITMAP)LoadImage(NULL,"gameover2.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	process_bar = (HBITMAP)LoadImage(NULL,"process_bar.bmp",IMAGE_BITMAP,600,225,LR_LOADFROMFILE); 
	start = (HBITMAP)LoadImage(NULL,"start.bmp",IMAGE_BITMAP,900,500,LR_LOADFROMFILE); 
	heart = (HBITMAP)LoadImage(NULL,"heart.bmp",IMAGE_BITMAP,100,43,LR_LOADFROMFILE); 


	SelectObject(mdc,start);
	BitBlt(hdc,0,0,900,500,mdc,0,0,SRCCOPY);

	button1 = CreateWindow("Button", "һ���Ѷ�", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
            100, 210, 100, 35, hWnd,  (HMENU)IDB_YI, hInstance, NULL);
    button2 = CreateWindow("Button", "�е��Ѷ�", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
            100, 280, 100, 35, hWnd,  (HMENU)IDB_ZHONG, hInstance, NULL);
	button3 = CreateWindow("Button", "�߼��Ѷ�", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,     
            100, 350, 100, 35, hWnd,  (HMENU)IDB_NAN, hInstance, NULL);


//	MyPaint(hdc);

//	SetTimer(hWnd,1,20,(TIMERPROC)OnTimer);     �ں���İ�ť�жϴ����ʱ������Ŀ�ʼ

//	ReleaseDC(hWnd,hdc);

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
	

//�������ֵ���ͼ  
	for(int i = 0; i < NUM_HUANG;i++){
		if(person[i][0] == 0) continue;
		if(person[i][3] == 0){
			SelectObject(bufdc,gao_yi_xiang[level-1]); 
			BitBlt(mdc,person[i][1],person[i][2],width_of_pic,height_of_pic,bufdc,width_of_pic,0,SRCAND);   
			BitBlt(mdc,person[i][1],person[i][2],width_of_pic,height_of_pic,bufdc,0,0,SRCPAINT);
		}
		else if(person[i][3] == 1){
			SelectObject(bufdc,bmp_huang_bo[level-1]); 
			BitBlt(mdc,person[i][1],person[i][2],width_of_pic,height_of_pic,bufdc,width_of_pic,0,SRCAND);   
			BitBlt(mdc,person[i][1],person[i][2],width_of_pic,height_of_pic,bufdc,0,0,SRCPAINT);
		}
	}
//	BitBlt(mdc,x_huang,280,70,70,bufdc,70,0,SRCAND);   
//	BitBlt(mdc,x_huang,280,70,70,bufdc,0,0,SRCPAINT);  

//��־��
	SelectObject(bufdc,lin_zhi_ling);                     
	BitBlt(mdc,x_lin,y_lin,90,90,bufdc,90,0,SRCAND);  
	BitBlt(mdc,x_lin,y_lin,90,90,bufdc,0,0,SRCPAINT); 

//����ֵ����������ͼ
	SelectObject(bufdc,process_bar); 
	BitBlt(mdc,50,0,150,32,bufdc,450,64*life,SRCAND);  
	BitBlt(mdc,50,0,150,32,bufdc,150,64*life,SRCPAINT); 
//����ֵ������������ͼ
	SelectObject(bufdc,process_bar); 
	BitBlt(mdc,400,6,150,30,bufdc,450,6,SRCAND);  
	BitBlt(mdc,400,6,150,30,bufdc,150,6,SRCPAINT); 
//�˴��Ľ�������Ҫ����������ʾ
/*	SelectObject(bufdc,process_bar); 
	BitBlt(mdc,406,-3,120,30,bufdc,323,30,SRCAND);  
	BitBlt(mdc,406,-3,120,30,bufdc,23,30,SRCPAINT); 
*/
	SelectObject(bufdc,process_bar); 
	BitBlt(mdc,406,-3,120*((float)score/(float)TOTAL_SCORE),30,bufdc,323,30,SRCAND);  
	BitBlt(mdc,406,-3,120*((float)score/(float)TOTAL_SCORE),30,bufdc,23,30,SRCPAINT); 

/*	SelectObject(bufdc,process_bar); 		
	BitBlt(mdc,406+60*((float)score/(float)TOTAL_SCORE),-3,60*((float)score/(float)TOTAL_SCORE),30,bufdc,323+60*((float)score/(float)TOTAL_SCORE),30,SRCAND);  
	BitBlt(mdc,406+60*((float)score/(float)TOTAL_SCORE),-3,60*((float)score/(float)TOTAL_SCORE),30,bufdc,23+60*((float)score/(float)TOTAL_SCORE),30,SRCPAINT); 
*/
	//	sprintf(str,"����ֵ  %d    ",life);
	sprintf(str,"����ֵ");
	TextOut(mdc,0,10,str,strlen(str));
	sprintf(str,"����ֵ  %d    ",score);
	TextOut(mdc,300,10,str,strlen(str));

/*	sprintf(str,"%d  %d   %d   %d",person[0][0],person[0][1],person[0][2],person[0][3]);
	TextOut(mdc,0,40,str,strlen(str));
	
	sprintf(str,"%d  %d   %d   %d",person[1][0],person[1][1],person[1][2],person[1][3]);
	TextOut(mdc,0,60,str,strlen(str));

	sprintf(str,"%d",life);
	TextOut(mdc,0,80,str,strlen(str));

	sprintf(str,"%d",red_heart[0][2]);
	TextOut(mdc,0,100,str,strlen(str)); 
	
	sprintf(str,"%f",60*((float)score/(float)TOTAL_SCORE));
	TextOut(mdc,0,100,str,strlen(str));
 */
	if(life == 0){
		KillTimer(h_wnd, 1);
		SelectObject(bufdc,gameover[0]);
		BitBlt(mdc,0,0,900,500,bufdc,0,0,SRCCOPY);
	}
	if(score == TOTAL_SCORE){   //�÷�***����Ϸ�ɹ�
		KillTimer(h_wnd, 1);
		SelectObject(bufdc,gameover[1]);
		BitBlt(mdc,0,0,900,500,bufdc,0,0,SRCCOPY);
	}

	BitBlt(hdc,0,0,900,500,mdc,0,0,SRCCOPY);
	
	
	tPre = GetTickCount();

  }

VOID CALLBACK OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime ){

//	Degree(level);			//��ʼ����Ϸ���Ѷ�
   
	MyPaint(GetDC(h_wnd));

	judge();			// �жϸ��ֳ�ײ

	ChangeCoordinate(); //����ͼ������ı仯
}

//��ʼ����Ϸ���Ѷ�

void Degree(int deg){
	level = deg;  //�Ѷ�ϵ��Ϊ1���Ѷȹ�����������1,2,3�� 3Ϊ���ѣ�������С
	if(level == 1){
		width_of_pic = 70;
		height_of_pic = 70;
	}
	if(level == 2){
		width_of_pic = 60;
		height_of_pic = 60;
	}
	if(level == 3){
		width_of_pic = 50;
		height_of_pic = 50;
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

	/*kiss������仯*/
	for(int  m = 0;m < NUM_KISS; m++){
		if(kiss[m][0] == 1){
			kiss[m][1] -= 5;
			random+=m;
			if(kiss[m][1] < 0){
				kiss[m][0] = 0;
				kiss[m][1] = 0;
				kiss[m][2] = 0;
			}
		}
	}

	//���ĵ�����仯
	for(int t = 0;t < NUM_HEART; t++){
		if(red_heart[t][0]==1){
			red_heart[t][1]+=3;
			if(heart_state == 0){
				red_heart[t][2]+=3;
			}
			if(heart_state == 1){
				red_heart[t][2]-=3;
			}
			if(red_heart[t][2] < 0) heart_state = 0;
			else if(red_heart[t][2] > 420) heart_state = 1;
			if(red_heart[t][1] > 900){
				red_heart[t][0]=0;
				red_heart[t][1]=0;
				red_heart[t][2]=0;
			}
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
							if(person[j][3] == 1){
								life -= 1;	
							}
							if(person[j][3] == 0){
								score += 25;
								int tmp = rand()%6;
								if(tmp == 1){
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

	//�����Ƿ�����־����ײ
	for(int p = 0;p < NUM_HEART;p++){
		if(red_heart[p][0]==1){
			if(x_lin - red_heart[p][1] <50){
				if(y_lin - red_heart[p][2] <43 && y_lin -red_heart[p][2] > -90){
					if(life < 3) life++;

					red_heart[p][0] = 0;
					red_heart[p][1] = 0;
					red_heart[p][2] = 0;

				}
			}
		}
	}
	//�Ʋ��Ƿ����־����ײ
	for(int m = 0; m < NUM_HUANG;m++){
		if(person[m][0] == 1){
			if(x_lin - person[m][1] < 60 && x_lin - person[m][1] > -70){
				if(y_lin - person[m][2] < 70 && y_lin - person[m][2] > -85){
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
//					PlaySound("b.wav", NULL, SND_FILENAME | SND_ASYNC);
					person[m][0] = 0;
					person[m][1] = 0;
					person[m][2] = 0;
					person[m][3] = 0;
				}
			}
		}
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
//
//						if(!b[i].exist)
//						{
//							b[i].x = x_lin;	
//							b[i].y = y_lin + 45; 
//							b[i].exist = true;
//							bcount++;		
//							break;
//						}
					}
					break;
				case VK_ESCAPE:          
					PostQuitMessage( 0 );
					break;

				case VK_UP:				 
					y_lin-=20;
					if(y_lin < 0) 
						y_lin = 0;
					break;
				case VK_DOWN:			
				
					y_lin+=20;
					if(y_lin > 370)
						y_lin = 370;
					break;
				case VK_LEFT:			
				//	x_lin-=10;
				//	if(x_lin < 0)
				//		x_lin = 0;
				//	break;
					y_lin-=20;
					if(y_lin < 0) 
						y_lin = 0;
					break;
				case VK_RIGHT:			 
				//	x_lin+=10;
				//	if(x_lin > 790)
				//		x_lin = 790;
				//	break;
					y_lin+=20;
					if(y_lin > 370)
						y_lin = 370;
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
			DeleteObject(kiss);
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
					Degree(1);
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
					Degree(3);
					SetTimer(hWnd,1,20,(TIMERPROC)OnTimer);
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
