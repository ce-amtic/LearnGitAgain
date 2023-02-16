#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <climits>
#include <vector>
#include <utility>
using namespace std;
int last_x,last_y,k=1;
int chessboard[9][9];//��¼�Ǻڰ��ӻ����ǿ����̵����飬0Ϊ�գ�1Ϊ���壻2Ϊ����
int have_chessed[9][9];
int g;
int choose_black_or_white;
bool have_judged_qi[9][9]={false};
int where_allow_x[81],where_allow_y[81];
int avail_for_white=0,avail_for_black=0,avail;
vector < pair<int,int> > storage;
 //��������+�ж��Ƿ�����
void board(){ //board�Ǵ�ӡ���̺����ӵĺ��� 
	int i,j,k;
	printf("   1  2  3  4  5  6  7  8  9  y\n");
	printf("1 "); 
	if(chessboard[0][0]==0)
	printf("������");
	else if(chessboard[0][0]==1)
	printf("�񩥩�");
	else if(chessboard[0][0]==2)
	printf("�𩥩�");
	for(i=1;i<=7;i++){
		if(chessboard[0][i]==0)
	    printf("�ש���");
	    else if(chessboard[0][i]==1)
	    printf("�񩥩�");
	    else if(chessboard[0][i]==2)
	    printf("�𩥩�");
	}
	if(chessboard[0][8]==0)
	printf("��\n");
	else if(chessboard[0][8]==1)
	printf("��\n");
	else if(chessboard[0][8]==2)
	printf("��\n");
	printf("  ");
	for(i=0;i<=7;i++){
		printf("��  ");
	}
	printf("��\n");//��ӡ���̵ĵ�һ�У� 
	for(i=1;i<=7;i++){
		printf("%d ",i+1);
		if(chessboard[i][0]==0)
		printf("�ǩ���");
		else if(chessboard[i][0]==1)
		printf("�񩥩�");
		else if(chessboard[i][0]==2)
		printf("�𩥩�");
		for(j=1;j<=7;j++){
			if(chessboard[i][j]==0)
			printf("�贈��");
			else if(chessboard[i][j]==1)
			printf("�񩥩�");
			else if(chessboard[i][j]==2)
		    printf("�𩥩�");
		}
		if(chessboard[i][8]==0)
		printf("��\n");
		else if(chessboard[i][8]==1)
		printf("��\n");
		else if(chessboard[i][8]==2)
		printf("��\n");
		printf("  ");
		for(k=0;k<=7;k++){
			printf("��  ");
		}
		printf("��\n");
	}//��ӡ���̵��м䲿�֣�
	printf("9 "); 
	if(chessboard[8][0]==0)
	printf("������");
	else if(chessboard[8][0]==1)
	printf("�񩥩�");
	else if(chessboard[8][0]==2)
	printf("�𩥩�");
	for(i=1;i<=7;i++){
		if(chessboard[8][i]==0)
	    printf("�ߩ���");
	    else if(chessboard[8][i]==1)
	    printf("�񩥩�");
	    else if(chessboard[8][i]==2)
	    printf("�𩥩�");
	}
	if(chessboard[8][8]==0)
	printf("��\n");
	else if(chessboard[8][8]==1)
	printf("��\n");
	else if(chessboard[8][8]==2)
	printf("��\n");//��ӡ���̵����һ�У�
	printf("\nx\n"); 
}
bool whether_inboard(int x,int y){
    if(x>=0&&x<9&&y>=0&&y<9)
    return true;
    return false;
}
bool you_qi_or_not(int x,int y){
    have_judged_qi[x][y]=true;
    if(whether_inboard(x,y)){
        if(whether_inboard(x-1,y)&&chessboard[x-1][y]==0)
        return true;
        if(whether_inboard(x+1,y)&&chessboard[x+1][y]==0)
        return true;
        if(whether_inboard(x,y-1)&&chessboard[x][y-1]==0)
        return true;
        if(whether_inboard(x,y+1)&&chessboard[x][y+1]==0)
        return true;//�ж����������Ƿ���ֱ������û�����ӵ����� 
		 //û��ֱ�������������ж��Ƿ��м���� 
        if(chessboard[x-1][y]==chessboard[x][y]&&whether_inboard(x-1,y)&&!have_judged_qi[x-1][y])
        {
            if(you_qi_or_not(x-1,y))
            return true;
        }
        if(chessboard[x+1][y]==chessboard[x][y]&&whether_inboard(x+1,y)&&!have_judged_qi[x-1][y])
        {
            if(you_qi_or_not(x+1,y))
            return true;
        }
        if(chessboard[x][y-1]==chessboard[x][y]&&whether_inboard(x,y-1)&&!have_judged_qi[x-1][y])
        {
            if(you_qi_or_not(x,y-1))
            return true;
        }
        if(chessboard[x][y+1]==chessboard[x][y]&&whether_inboard(x,y+1)&&!have_judged_qi[x-1][y])
        {
            if(you_qi_or_not(x,y+1))
            return true;
        }//ͨ���ݹ�Ѱ�Ҽ���� 
    }
    return false;//һ���ҵ�һ������ֱ����or����������Ϸ���true��ʲô���Ҳ����ͻ�������������false 
}
//�洢״̬+���� save()// 
//�Ծֲ���//
//�ж�x,y���ܷ��°���
bool put_white(int x,int y)
{
	if(chessboard[x][y]!=0)
	return false;	
 	chessboard[x][y]=1;//������x,y��������� ���Ƿ���������
 	memset(have_judged_qi,false,sizeof(have_judged_qi));
 	if(!you_qi_or_not(x,y)){
 		chessboard[x][y]=0;
 		return false;
	}
	for(int l=0;l<=8;l++){
		for(int n=0;n<=8;n++){
			if(chessboard[l][n]!=0)
            {
            	memset(have_judged_qi,false,sizeof(have_judged_qi));
                if(!you_qi_or_not(l,n))   
		    {
			    chessboard[x][y]=0;
			    return false;
		    }        
            }
		}
	}		
    chessboard[x][y]=0;
    return true;	 
}
//�ж�x,y���ܷ��º���

bool put_black(int x,int y)
{
	if(chessboard[x][y]!=0)
	return false;	
 	chessboard[x][y]=2;//������x,y������hei�� ���Ƿ���������
 	memset(have_judged_qi,false,sizeof(have_judged_qi));
 	if(!you_qi_or_not(x,y)){
 		chessboard[x][y]=0;
 		return false;
	}
    for(int l=0;l<=8;l++)
    {
        for(int n=0;n<=8;n++)
        {
            if(chessboard[l][n]!=0)
            {
            	memset(have_judged_qi,false,sizeof(have_judged_qi));
                if(!you_qi_or_not(l,n))
		{
			chessboard[x][y]=0;
			return false;
		}       
                      
            }
        }
    }
    chessboard[x][y]=0;
    return true;	 
}
int available_black()
{
 	int available_for_black=0;
 	int notavailable=0;
 	for(int i=0;i<=8;i++)
 	{
 		for(int j=0;j<=8;j++)
 		{
 			if(chessboard[i][j]==0)
 			{
 				 chessboard[i][j]=2;//������i,j��������� ���Ƿ���������
            available_for_black++;
            for(int l=0;l<=8;l++)
            {
                for(int n=0;n<=8;n++)
                {
                    if(chessboard[l][n]!=0)
                    {
					    memset(have_judged_qi,false,sizeof(have_judged_qi));         
                        if(!you_qi_or_not(l,n))       
                        notavailable=1;       
                    }
                }
            }
        if(notavailable==1)
        available_for_black--;
        notavailable=0;
        chessboard[i][j]=0;
			 }
			
		}
	}
	return available_for_black;
}
int available_white()
{
 	int available_for_white=0;
 	int notavailable=0;
 	for(int i=0;i<=8;i++)
 	{
 		for(int j=0;j<=8;j++)
 		{
 			if(chessboard[i][j]==0)
 			{
 			chessboard[i][j]=1;//������i,j��������� ���Ƿ���������
            available_for_white++;
            for(int l=0;l<=8;l++)
            {
                for(int n=0;n<=8;n++)
                {
                	memset(have_judged_qi,false,sizeof(have_judged_qi));
                    if(chessboard[l][n]!=0)
                    {
                        if(!you_qi_or_not(l,n))       
                        notavailable=1;       
                    }
                }
            }
            if(notavailable==1)
            available_for_white--;
            notavailable=0;
            chessboard[i][j]=0;
			}
	    }
	}
	return available_for_white;
}
// ��ֵ����: ���㵱ǰ������һ������һ��������λ���������
int evaluate(int k)
{
    if(k%2==1) // ��������֮��״̬
    return available_black()-available_white();
    else if(k%2==0) // ��������֮��״̬
    return available_white()-available_black();
}
void loop_traversal(){
	int cur_ceng,if_x,if_y,i;//cur_cengΪĿǰѭ���Ĳ�����if_x,if_yΪ���������������꣬iΪѭ������ 
	for(cur_ceng=1;cur_ceng<=8;cur_ceng++){
		if_x=last_x-cur_ceng;if_y=last_y+cur_ceng;
		if(whether_inboard(if_x,if_y)){
			if(choose_black_or_white==2){
				if(put_white(if_x,if_y)){
					where_allow_x[avail_for_white]=if_x;
					where_allow_y[avail_for_white]=if_y;
					avail_for_white++;
				}
			}
			if(choose_black_or_white==1){
				if(put_black(if_x,if_y)){
					where_allow_x[avail_for_black]=if_x;
					where_allow_y[avail_for_black]=if_y;
					avail_for_black++;
				}
			}
		}//���Ϸ��˵�
		if_x=last_x+cur_ceng,if_y=last_y+cur_ceng;
		if(whether_inboard(if_x,if_y)){
			if(choose_black_or_white==2){
				if(put_white(if_x,if_y)){
					where_allow_x[avail_for_white]=if_x;
					where_allow_y[avail_for_white]=if_y;
					avail_for_white++;
				}
			}
			if(choose_black_or_white==1){
				if(put_black(if_x,if_y)){
					where_allow_x[avail_for_black]=if_x;
					where_allow_y[avail_for_black]=if_y;
					avail_for_black++;
				}
			}
		}//���·��˵�
		if_x=last_x-cur_ceng,if_y=last_y-cur_ceng;
		if(whether_inboard(if_x,if_y)){
			if(choose_black_or_white==2){
				if(put_white(if_x,if_y)){
					where_allow_x[avail_for_white]=if_x;
					where_allow_y[avail_for_white]=if_y;
					avail_for_white++;
				}
			}
			if(choose_black_or_white==1){
				if(put_black(if_x,if_y)){
					where_allow_x[avail_for_black]=if_x;
					where_allow_y[avail_for_black]=if_y;
					avail_for_black++;
				}
			}
		}//���Ϸ��˵�
		if_x=last_x+cur_ceng,if_y=last_y-cur_ceng;
		if(whether_inboard(if_x,if_y)){
			if(choose_black_or_white==2){
				if(put_white(if_x,if_y)){
					where_allow_x[avail_for_white]=if_x;
					where_allow_y[avail_for_white]=if_y;
					avail_for_white++;
				}
			}
			if(choose_black_or_white==1){
				if(put_black(if_x,if_y)){
					where_allow_x[avail_for_black]=if_x;
					where_allow_y[avail_for_black]=if_y;
					avail_for_black++;
				}
			}
		}//���·��˵� 
		for(i=1-cur_ceng;i<=1+cur_ceng;i++){
			if_x=last_x+i;if_y=last_y+cur_ceng;
			if(whether_inboard(if_x,if_y)){
			    if(choose_black_or_white==2){
				    if(put_white(if_x,if_y)){
					    where_allow_x[avail_for_white]=if_x;
					    where_allow_y[avail_for_white]=if_y;
					    avail_for_white++;
				    }
			    }
			    if(choose_black_or_white==1){
				    if(put_black(if_x,if_y)){
					    where_allow_x[avail_for_black]=if_x;
					    where_allow_y[avail_for_black]=if_y;
					    avail_for_black++;
				    }
			    }
		    }
		}//�Ҳ�˵�
		for(i=1-cur_ceng;i<=1+cur_ceng;i++){
			if_x=last_x+i;if_y=last_y-cur_ceng;
			if(whether_inboard(if_x,if_y)){
			    if(choose_black_or_white==2){
				    if(put_white(if_x,if_y)){
					    where_allow_x[avail_for_white]=if_x;
					    where_allow_y[avail_for_white]=if_y;
					    avail_for_white++;
				    }
			    }
			    if(choose_black_or_white==1){
				    if(put_black(if_x,if_y)){
					    where_allow_x[avail_for_black]=if_x;
					    where_allow_y[avail_for_black]=if_y;
					    avail_for_black++;
				    }
			    }
		    }
		}//���˵�
		for(i=1-cur_ceng;i<=1+cur_ceng;i++){
			if_x=last_x+cur_ceng;if_y=last_y+i;
			if(whether_inboard(if_x,if_y)){
			    if(choose_black_or_white){
				    if(put_white(if_x,if_y)){
					    where_allow_x[avail_for_white]=if_x;
					    where_allow_y[avail_for_white]=if_y;
					    avail_for_white++;
				    }
			    }
			    if(choose_black_or_white){
				    if(put_black(if_x,if_y)){
					    where_allow_x[avail_for_black]=if_x;
					    where_allow_y[avail_for_black]=if_y;
					    avail_for_black++;
				    }
			    }
		    }
		}//�·��˵�
		for(i=1-cur_ceng;i<=1+cur_ceng;i++){
			if_x=last_x-cur_ceng;if_y=last_y+i;
			if(whether_inboard(if_x,if_y)){
			    if(choose_black_or_white){
				    if(put_white(if_x,if_y)){
					    where_allow_x[avail_for_white]=if_x;
					    where_allow_y[avail_for_white]=if_y;
					    avail_for_white++;
				    }
			    }
			    if(choose_black_or_white){
				    if(put_black(if_x,if_y)){
					    where_allow_x[avail_for_black]=if_x;
					    where_allow_y[avail_for_black]=if_y;
					    avail_for_black++;
				    }
			    }
		    }
		}//�Ϸ��˵� 
	}
	if(choose_black_or_white==1)
	avail=avail_for_black;
	else if(choose_black_or_white==2)
	avail=avail_for_white;
}
int ABpruning(int step,int steplength,int alpha,int beta,int over_x,int over_y)
{
    int not_avail;
   
	if(choose_black_or_white==2)
    {
        if(steplength==0)
	    {
		    return evaluate(1);
	    }
	    //ʤ���ѷ� ������ֵ
	    if(avail==0)
	    return evaluate(1);  
        if(step%2==0) // ��Сֵ�ڵ�MIN�� ���ѡ����� �����ҷ����� k%2==0 Ϊ��Сֵ�ڵ��
	    {
		int mn=INT_MAX;
		int MN;
		for(int i=0;i<avail;i++)
		{
			int _i=where_allow_x[i],_j=where_allow_y[i];
			chessboard[_i][_j]=1; //������壨�ڵ����ɣ�
			MN=ABpruning(step+1,steplength-1,alpha,beta,_i,_j);//������
			chessboard[_i][_j]=0;//����
			if(MN<mn)
			mn=MN;
			if(mn<beta)
			beta=mn;//ȡ��Сֵ
			if(alpha>=beta)
			break;//��֦
		}
		return beta; //����Сֵ����
	}
	else if(step%2==1) //����ֵ�ڵ�MAX��
	{
		int mx=INT_MIN; 
		int MX;
		for(int i=0;i<avail;i++)
		{
			int _i=where_allow_x[i],_j=where_allow_y[i];
			chessboard[_i][_j]=2;//������壨�ڵ����ɣ�
			MX=ABpruning(step+1,steplength-1,alpha,beta,_i,_j);//������
			chessboard[_i][_j]=0;//����
			if(MX>mx)
			mx=MX;
			if(mx>alpha)
			alpha=mx;
			if(alpha>=beta)
			break;//��֦
		}
		return alpha; //������ֵ����
        }
    }
    //Alpha-beta ��֦
    else if(choose_black_or_white==1)
    {
    if(steplength==0)
	{
		return evaluate(2);
	}
	//ʤ���ѷ� ������ֵ
	if(avail==0)
	return evaluate(2);  

	if(step%2==1) // ��Сֵ�ڵ�MIN�� ���ѡ����� �����ҷ����� k%2==1 Ϊ��Сֵ�ڵ��
	{
		int mn=INT_MAX;
		int MN;
		for(int i=0;i<avail;i++)
		{
			int _i=where_allow_x[i],_j=where_allow_y[i];
			chessboard[_i][_j]=2; //������壨�ڵ����ɣ�
			MN=ABpruning(step+1,steplength-1,alpha,beta,_i,_j);//������
			chessboard[_i][_j]=0;//����
			if(MN<mn)
			mn=MN;
			if(mn<beta)
			beta=mn;//ȡ��Сֵ
			if(alpha>=beta)
			break;//��֦
		}
		return beta; //����Сֵ����
	}
	else if(step%2==0) //����ֵ�ڵ�MAX��
	{
		int mx=INT_MIN; 
		int MX;
		for(int i=0;i<avail;i++)
		{
			int _i=where_allow_x[i],_j=where_allow_y[i];
			chessboard[_i][_j]=1;//������ӣ��ڵ����ɣ�
			MX=ABpruning(step+1,steplength-1,alpha,beta,_i,_j);//������
			chessboard[_i][_j]=0;//����
			if(MX>mx)
			mx=MX;
			if(mx>alpha)
			alpha=mx;
			if(alpha>=beta)
			break;//��֦
		}
		return alpha; //������ֵ����
    }
    }
}
void put_into_storage(){
	storage.push_back(make_pair(last_x,last_y));
	return;
}
void regret_playing(){
	chessboard[last_x][last_y]=0;
	board();
	storage.pop_back();
	k--;
	return;
} 
void replay(){
	memset(chessboard,0,sizeof(chessboard));
	int temp=0;
	while(storage.size()!=0){
		if(temp%2==0)
		chessboard[storage[0].first][storage[0].second]=2;
		else if(temp%2==1)
		chessboard[storage[0].first][storage[0].second]=1;
		temp++;
		board();
		storage.erase(storage.begin());
	}
	if(storage.size()==0)
	return;
}
void RUN(){
	int x,y;
	char n;
	int M_A_X=-999,X_0,Y_0;
	int num=0;
	while(1){
		num=0;
		memset(where_allow_x,0,sizeof(where_allow_x));
		memset(where_allow_y,0,sizeof(where_allow_y));
		avail=0;avail_for_black=0;avail_for_white=0;M_A_X=-999;
		if(choose_black_or_white==1){
			if(k==1) board();
			printf("����������\n");
			cin>>x>>y;
			k++;
			if(x==5&&y==5&&k==1){
				printf("������Ĳ��������������룺");
				k--;
				continue; 
			}
			else if(!put_black(x-1,y-1)){
				printf("�ܱ�Ǹ��������\n");
				break;
		    }
			chessboard[x-1][y-1]=2;
			board();
			last_x=x-1;last_y=y-1;
			put_into_storage();
			num=0;
			for(int i=0;i<=8;i++){
				for(int j=0;j<=8;j++){
					if(put_white(i,j)&&!have_chessed[i][j]){
						num++;
						int temp=ABpruning(k,3,-99999,99999,i,j);
						if(temp>M_A_X){
							M_A_X=temp;
							X_0=i;
							Y_0=j;
						}
					}
				}
			}
			if(num==0){
				printf("��ϲ������ʤ�ˣ�\n");
				break;
			}
				chessboard[X_0][Y_0]=1;
				have_chessed[X_0][Y_0]=1;
				last_x=X_0;last_y=Y_0;
				put_into_storage();
				k++;
				cout<<X_0<<' '<<Y_0<<endl; 
				board();
	    }
	    else if(choose_black_or_white==2){
	    	if(k==1){
	    		chessboard[0][1]=2;
	    		have_chessed[0][1]=1;
	    		board();
	    		k++;
			}
	    	printf("����������\n");
	    	cin>>x>>y;
			k++;
			if(!put_white(x-1,y-1)){
				printf("�ܱ�Ǹ��������\n");
				break;
			}
			else{
				chessboard[x-1][y-1]=1;
				board();
				last_x=x-1;last_y=y-1;
			}
			put_into_storage();
			num=0;
			for(int i=0;i<=8;i++){
				for(int j=0;j<=8;j++){
					if(put_white(i,j)&&!have_chessed[i][j]){
						num++;
						int temp=ABpruning(k,8,-99999,99999,i,j);
						if(temp>M_A_X){
							M_A_X=temp;
							X_0=i;
							Y_0=j;
						}
					}
				}
			}
			if(num==0){
				printf("��ϲ������ʤ�ˣ�\n");
				break;
			}
			else{
				chessboard[X_0][Y_0]=2;
				have_chessed[X_0][Y_0]=1;
				last_x=X_0;last_y=Y_0;
				put_into_storage();
				k++;
				board();
			}
		}
	}
}
void newopen() //�¿�ʼ����
{
//�����ѡ���������
    cout<<"����Ҫѡ�� 1.���� 2.���� ��������Ӧ���"; 
    int kkk;
    cin>>kkk;
    if(kkk==1)
    choose_black_or_white=1; //ѡ�����
    else if(kkk==2)
    choose_black_or_white=2; //ѡ�����
    RUN(); 
}
int main()
{ 
int n=0;
while (n!=3)
{ 
        cout <<"������������������\n";
        cout <<"��--..ѡ��..--�� \n";
        cout <<"��1.  �¿�ʼ    �� \n";
        cout <<"��2.  ����      �� \n";
        cout <<"��3.  �˳���Ϸ  �� \n";
        cout <<"������������������\n";
        cout <<"����������ѡ�� 1/2/3: " << endl;
        cin >> n;
        cin.get();
        switch(n)
        { 
        case 1: cout<<"����Ҫѡ�� 1.���� 2.���� ��������Ӧ���"; 
        int kkk;
        cin>>kkk;
        if(kkk==1)
        choose_black_or_white=1; //ѡ�����
        else if(kkk==2)
        choose_black_or_white=2; //ѡ�����
        RUN();break;
        case 2: replay();break;
        case 3: break;
        default:cout<<"�������"; break;
        }
}
        return 0;
}
