#include<bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <climits>
#include <vector>
#include <utility>
#include<fstream>
using namespace std;
bool visChildrenAction[10][10];
int ksave=1;
int x_regret_player,y_regret_player;
int x_regret_computer,y_regret_computer;
int chessboardsave[9][9]={0};
int last_x,last_y,k=1;
int chessboard[9][9];
int step=1;
int choose_black_or_white;
bool have_judged_qi[9][9]={false};
int where_allow_x[81],where_allow_y[81];
int avail_for_white=0,avail_for_black=0,avail;
vector < pair<int,int> > storage;
int dx[4] = {-1, 0, 1, 0}; //��λ��
int dy[4] = {0, -1, 0, 1}; //��λ��
void board(){ //board�Ǵ�ӡ���̺����ӵĺ��� 
	int i,j,k;
	printf("   1  2  3  4  5  6  7  8  9  x\n");
	printf("1 "); 
	if(chessboard[0][0]==0)
	printf("������");
	else if(chessboard[0][0]==1)
	printf("��");
	else if(chessboard[0][0]==-1)
	printf("��");
	for(i=1;i<=7;i++){
		if(chessboard[0][i]==0)
	    printf("�ש���");
	    else if(chessboard[0][i]==1)
	    printf("��");
	    else if(chessboard[0][i]==-1)
	    printf("��");
	}
	if(chessboard[0][8]==0)
	printf("��\n");
	else if(chessboard[0][8]==1)
	printf("��\n");
	else if(chessboard[0][8]==-1)
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
		printf("��");
		else if(chessboard[i][0]==-1)
		printf("��");
		for(j=1;j<=7;j++){
			if(chessboard[i][j]==0)
			printf("�贈��");
			else if(chessboard[i][j]==1)
			printf("��");
			else if(chessboard[i][j]==-1)
		    printf("��");
		}
		if(chessboard[i][8]==0)
		printf("��\n");
		else if(chessboard[i][8]==1)
		printf("��\n");
		else if(chessboard[i][8]==-1)
		printf("��\n");
		printf("  ");
		for(k=0;k<=7;k++){
			printf("��  ");
		}
		printf("��\n");
	}
	printf("9 "); 
	if(chessboard[8][0]==0)
	printf("������");
	else if(chessboard[8][0]==1)
	printf("��");
	else if(chessboard[8][0]==-1)
	printf("��");
	for(i=1;i<=7;i++){
		if(chessboard[8][i]==0)
	    printf("�ߩ���");
	    else if(chessboard[8][i]==1)
	    printf("��");
	    else if(chessboard[8][i]==-1)
	    printf("��");
	}
	if(chessboard[8][8]==0)
	printf("��\n");
	else if(chessboard[8][8]==1)
	printf("��\n");
	else if(chessboard[8][8]==-1)
	printf("��\n");//��ӡ���̵����һ�У�
	printf("\ny\n"); 
}
bool whether_inboard(int x,int y){
    if(x>=0&&x<9&&y>=0&&y<9)
    return true;
    return false;
}
bool you_qi_or_not(int x, int y)
{
    have_judged_qi[x][y] = true; //����Ѿ���������
    bool flag=false;
    for (int i=0;i<4;i++)
    {
        int _dx=x+dx[i],_dy=y+dy[i];
        if(whether_inboard(_dx,_dy))//�ж��Ƿ��ڽ���
        {
            if(chessboard[_dx][_dy]==0)//ֱ����
                flag=true;
            if(chessboard[_dx][_dy]==chessboard[x][y]&&!have_judged_qi[_dx][_dy])//�����
                if(you_qi_or_not(_dx,_dy))
                flag=true;
        }
    }
    return flag;
}

bool put_black_or_white(int x,int y,int color)
{
    if(!whether_inboard(x,y))
    return false;
    if(chessboard[x][y]!=0) 
    return false;
    chessboard[x][y]=color;
    memset(have_judged_qi,false,sizeof(have_judged_qi)); 
    if(!you_qi_or_not(x,y)) 
    {
        chessboard[x][y]=0;
        return false;
    }
    for(int i=0;i<4;++i) 
    {
        int x_dx=x+dx[i],y_dy=y+dy[i];
        if(whether_inboard(x_dx,y_dy)) 
        {
            if(chessboard[x_dx][y_dy]&&!have_judged_qi[x_dx][y_dy]){
            	memset(have_judged_qi,false,sizeof(have_judged_qi));
            	if(!you_qi_or_not(x_dx,y_dy)){
                    chessboard[x][y]=0; 
                    return false;
                }
			}        
        }
    }
    chessboard[x][y]=0; 
    return true;
}
int available_sum(int chessboard[9][9],int result[9][9])
{
    memset(result,0,81*4);
    int right=0;
    for(int x=0;x<9;++x)
    {
        for (int y=0;y<9;++y)
        {
            if (put_black_or_white(x,y,1))
            {
                right++;
                result[x][y]=1;
            }
        }
    }
    return right;
}
bool next_available(int color){
    int temp=0;
    for(int x=0;x<9;x++){
        for(int y=0;y<9;y++){
            if(put_black_or_white(x,y,color)){
                temp++;
                break;
            }
        }
    }
    return temp;
}
void put_into_storage(){
	storage.push_back(make_pair(last_x,last_y));
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
class treeNode
{
public:
    treeNode *parent;                 
    treeNode *children[81]; 
    int chessboard[9][9];
    int childrenAction[81][2];
    int childrenCount;
    int childrenCountMax;
    double value;      
    int n;             
    double UCB;        
    int *countPointer; 
    treeNode(int parentBoard[9][9],int opp_action[2],treeNode *parentPointer,int *countp) 
    {
        for (int i=0;i<9;++i) 
        {
            for (int j=0;j<9;++j)
            {
                chessboard[i][j]=-parentBoard[i][j];
            }
        }
        if (opp_action[0]>=0&&opp_action[0]<9&&opp_action[1]>=0&&opp_action[1]<9)
            chessboard[opp_action[0]][opp_action[1]]=-1;
        parent=parentPointer;
        value=0;
        n=0;
        childrenCount=0;     
        countPointer=countp; 
        evaluate();            
    }
    treeNode *treeRules() 
    {
        if(childrenCountMax==0)
        {
            return this; 
        }
        if(childrenCountMax>childrenCount)
        {
            treeNode *newNode=new treeNode(chessboard,childrenAction[childrenCount],this,countPointer); 
            children[childrenCount]=newNode;
            childrenCount++; 
            return newNode;
        }
        for (int i=0;i<childrenCount;++i)
        {
            children[i]->UCB=children[i]->value/double(children[i]->n)+0.2*sqrt(log(double(*countPointer))/double(children[i]->n)); 
        }
        int bestChild=0;
        double maxUCB=0;
        for(int i=0;i<childrenCount;++i)
        {
            if(maxUCB<children[i]->UCB)
            {
                bestChild=i;
                maxUCB=children[i]->UCB;
            }
        }
        return children[bestChild]->treeRules(); 
    }    
    double simulation()
    {
        int chessboard_opp[9][9]; 
        int res[9][9];
        for(int i=0;i<9;++i)
        {
            for(int j=0;j<9;++j)
            {
                chessboard_opp[i][j]=-chessboard[i][j];
            }
        }
        int x=available_sum(chessboard,res);     
        int y=available_sum(chessboard_opp,res);
        return x-y;
    }
    void backup(double deltaValue) 
    {
        treeNode *node=this;
        int side=0;
        while(node!=NULL) 
        {
            if(side==1) 
            {
                node->value+=deltaValue;
                side--;
            }
            else 
            {
                node->value-=deltaValue;
                side++;
            }
            node->n++; 
            node=node->parent;
        }
    }
private:
    void evaluate() 
    {
        int result[9][9];
        int validPositionCount=available_sum(chessboard,result);
        int validPositions[81];                         
        int availableNum=0;
        for (int i=0;i<9;++i)
        {
            for (int j=0;j<9;++j)
            {
                if (result[i][j] && !visChildrenAction[i][j])
                {
                    validPositions[availableNum]=i*9+j; 
                    availableNum++;                          
                }
            }
        }
        validPositionCount = availableNum;
        childrenCountMax=validPositionCount; 
        for(int i=0;i<validPositionCount;++i)
        {
            childrenAction[i][0]=validPositions[i]/9;
            childrenAction[i][1]=validPositions[i]%9;
        }
    }
};
void RUN()
{
    int count=0;    
    int timeout=(int)(0.98*(double)CLOCKS_PER_SEC);
    int t1,t2;
    int x,y;
    if(choose_black_or_white==1){
        t1=10,t2=10;
        cout<<endl;
        goto Start;
    } 
    else if(choose_black_or_white==-1){
        cout<<endl;
        if(step==1)board(); 
        printf("����������:\n"); 
    }
    while(cin>>x>>y){
        if(x==-2&&y==-2)
		{
			chessboard[x_regret_player][y_regret_player]=0;
			chessboard[x_regret_computer][y_regret_computer]=0;
			step=step-2;
			system("cls");
			board();
		}
		if(x==-3&&y==-3){
			memset(chessboardsave,0,sizeof(chessboardsave));
			step=1;
			storage.clear();
			break;
		}
		else{
			x_regret_player=y-1;
			y_regret_player=x-1;
			if(x==-1&&y==-1)
			{
				ksave=step;
				for(int ii=0;ii<=8;ii++)
				{
					for(int jj=0;jj<=8;jj++)
					{
						chessboardsave[ii][jj]=chessboard[ii][jj];
					}
				}
				return;
			}
			t1=x-1;t2=y-1;
			step++;
			if(!put_black_or_white(t2,t1,choose_black_or_white)){
				last_x=t2;last_y=t1;
				put_into_storage();
				printf("����λ�ô��󣬺ܱ�Ǹ��������\n");
				break;
			}
			if(!next_available(-choose_black_or_white)){
				last_x=t2;last_y=t1;
				put_into_storage();
				printf("��ϲ������ʤ��!\n");
				break;
			}
            visChildrenAction[t1][t2] = 1;
			Start:
            srand(clock());
            int start=clock();
            int opp_action[2]={t2,t1};
            treeNode rootNode(chessboard,opp_action,NULL,&count);
            while(clock()-start<5*timeout)
            {
                count++;                                
                treeNode *node=rootNode.treeRules(); 
                double result=node->simulation();     
                node->backup(result);
            }
            int bestChildren[81]={0}; 
            int bestChildrenNum=0;              
            int maxValue=INT_MIN;              
            for (int i=0;i<rootNode.childrenCount;++i)
            {
                if(maxValue<rootNode.children[i]->value)
                {            
                    memset(bestChildren,0,sizeof(bestChildren));
                    bestChildrenNum=0;
                    bestChildren[bestChildrenNum++]=i;
                    maxValue=rootNode.children[i]->value;
                }
                else if (maxValue==rootNode.children[i]->value)
                {
                    bestChildren[bestChildrenNum++]=i;
                }
            }
			int random = rand() % bestChildrenNum;                
            int *bestAction = rootNode.childrenAction[bestChildren[random]];
            cout << endl << "�Է����ӣ���"<<bestAction[0]+1<<','<<bestAction[1]+1<<')'<<endl;
            visChildrenAction[bestAction[0]][bestAction[1]];
            if(choose_black_or_white==-1){
                chessboard[t2][t1]=-1;
                chessboard[bestAction[1]][bestAction[0]]=1;
            }
            else{
                chessboard[t2][t1]=1;
                chessboard[bestAction[1]][bestAction[0]]=-1;
            }
            board();
            printf("����������:\n");
            printf("������Ҫ���� ������ '-1 -1'\n");
		    printf("���������� '-2 -2' ���޻�һ����\n");
		    printf("��;ֹͣ��Ϸ������'-3 -3'��������������̲��ᱻ���棬Ҳ���ܸ���\n");
        }
	} 
}
void read_last_game()
{
	for(int i=0;i<=8;i++)
	{
		for(int j=0;j<=8;j++)
		{
			chessboard[i][j]=chessboardsave[i][j];
		}
	}
	k=ksave;
	system("cls");
	board();
	RUN();
}
int main()
{ 
int n=0;
while (n!=5)
{ 
        cout <<"��������������������������������\n";
        cout <<"��---..ѡ��..---�� \n";
        cout <<"��1.  �¿�ʼ    �� \n";
        cout <<"��2.  ����      �� \n";
        cout <<"��3.  ����      �� \n";
        cout <<"��4.  ��մ���  �� \n"; 
        cout <<"��5.  �˳���Ϸ  �� \n";
        cout <<"��������������������������������\n";
        cout <<"����������ѡ�� 1/2/3/4/5: " << endl;
        cin>>n;
        cin.get();
        switch(n)
        { 
        case 1: cout<<"����Ҫѡ�� 1.���� 2.���� ��������Ӧ���";
        memset(chessboard,0,sizeof(chessboard));
        k=1;
        storage.clear();
        int kkk;
        cin>>kkk;
        if(kkk==1)
        choose_black_or_white=-1;//ѡ�����
        else if(kkk==2)
        choose_black_or_white=1;//ѡ�����
        RUN();break;
        case 2:replay();storage.clear();break;
        case 3:read_last_game();break;
        case 4:memset(chessboardsave,0,sizeof(chessboardsave));break;
        case 5:break;
        default:cout<<"�������"; break;
        }
}
    return 0;
}

