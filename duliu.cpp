#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <climits>
#include <vector>
#include <utility>
using namespace std;
int last_x,last_y,k=1;
int chessboard[9][9];//记录是黑白子或者是空棋盘的数组，0为空；1为白棋；2为黑棋
int have_chessed[9][9];
int g;
int choose_black_or_white;
bool have_judged_qi[9][9]={false};
int where_allow_x[81],where_allow_y[81];
int avail_for_white=0,avail_for_black=0,avail;
vector < pair<int,int> > storage;
 //棋盘棋子+判断是否有气
void board(){ //board是打印棋盘和棋子的函数 
	int i,j,k;
	printf("   1  2  3  4  5  6  7  8  9  y\n");
	printf("1 "); 
	if(chessboard[0][0]==0)
	printf("┏━━");
	else if(chessboard[0][0]==1)
	printf("●━━");
	else if(chessboard[0][0]==2)
	printf("○━━");
	for(i=1;i<=7;i++){
		if(chessboard[0][i]==0)
	    printf("┳━━");
	    else if(chessboard[0][i]==1)
	    printf("●━━");
	    else if(chessboard[0][i]==2)
	    printf("○━━");
	}
	if(chessboard[0][8]==0)
	printf("┓\n");
	else if(chessboard[0][8]==1)
	printf("●\n");
	else if(chessboard[0][8]==2)
	printf("○\n");
	printf("  ");
	for(i=0;i<=7;i++){
		printf("┃  ");
	}
	printf("┃\n");//打印棋盘的第一行； 
	for(i=1;i<=7;i++){
		printf("%d ",i+1);
		if(chessboard[i][0]==0)
		printf("┣━━");
		else if(chessboard[i][0]==1)
		printf("●━━");
		else if(chessboard[i][0]==2)
		printf("○━━");
		for(j=1;j<=7;j++){
			if(chessboard[i][j]==0)
			printf("╋━━");
			else if(chessboard[i][j]==1)
			printf("●━━");
			else if(chessboard[i][j]==2)
		    printf("○━━");
		}
		if(chessboard[i][8]==0)
		printf("┫\n");
		else if(chessboard[i][8]==1)
		printf("●\n");
		else if(chessboard[i][8]==2)
		printf("○\n");
		printf("  ");
		for(k=0;k<=7;k++){
			printf("┃  ");
		}
		printf("┃\n");
	}//打印棋盘的中间部分；
	printf("9 "); 
	if(chessboard[8][0]==0)
	printf("┗━━");
	else if(chessboard[8][0]==1)
	printf("●━━");
	else if(chessboard[8][0]==2)
	printf("○━━");
	for(i=1;i<=7;i++){
		if(chessboard[8][i]==0)
	    printf("┻━━");
	    else if(chessboard[8][i]==1)
	    printf("●━━");
	    else if(chessboard[8][i]==2)
	    printf("○━━");
	}
	if(chessboard[8][8]==0)
	printf("┛\n");
	else if(chessboard[8][8]==1)
	printf("●\n");
	else if(chessboard[8][8]==2)
	printf("○\n");//打印棋盘的最后一行；
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
        return true;//判断上下左右是否有直接气（没有棋子的气） 
		 //没有直接气，接下来判断是否有间接气 
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
        }//通过递归寻找间接气 
    }
    return false;//一旦找到一口气（直接气or间接气）马上返回true，什么都找不到就会跳出来，返回false 
}
//存储状态+存盘 save()// 
//对局策略//
//判断x,y处能否下白棋
bool put_white(int x,int y)
{
	if(chessboard[x][y]!=0)
	return false;	
 	chessboard[x][y]=1;//假设在x,y处放入白棋 看是否满足条件
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
//判断x,y处能否下黑棋

bool put_black(int x,int y)
{
	if(chessboard[x][y]!=0)
	return false;	
 	chessboard[x][y]=2;//假设在x,y处放入hei棋 看是否满足条件
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
 				 chessboard[i][j]=2;//假设在i,j处放入黑棋 看是否满足条件
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
 			chessboard[i][j]=1;//假设在i,j处放入黑棋 看是否满足条件
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
// 估值函数: 计算当前局面下一方比另一方可落子位置数多多少
int evaluate(int k)
{
    if(k%2==1) // 黑子落棋之后状态
    return available_black()-available_white();
    else if(k%2==0) // 白子落棋之后状态
    return available_white()-available_black();
}
void loop_traversal(){
	int cur_ceng,if_x,if_y,i;//cur_ceng为目前循环的层数，if_x,if_y为接下来遍历的坐标，i为循环因子 
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
		}//右上方端点
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
		}//右下方端点
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
		}//左上方端点
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
		}//左下方端点 
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
		}//右侧端点
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
		}//左侧端点
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
		}//下方端点
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
		}//上方端点 
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
	    //胜负已分 返回数值
	    if(avail==0)
	    return evaluate(1);  
        if(step%2==0) // 极小值节点MIN层 玩家选择白棋 对于我方而言 k%2==0 为极小值节点层
	    {
		int mn=INT_MAX;
		int MN;
		for(int i=0;i<avail;i++)
		{
			int _i=where_allow_x[i],_j=where_allow_y[i];
			chessboard[_i][_j]=1; //放入白棋（节点生成）
			MN=ABpruning(step+1,steplength-1,alpha,beta,_i,_j);//树延伸
			chessboard[_i][_j]=0;//回溯
			if(MN<mn)
			mn=MN;
			if(mn<beta)
			beta=mn;//取极小值
			if(alpha>=beta)
			break;//剪枝
		}
		return beta; //将极小值返回
	}
	else if(step%2==1) //极大值节点MAX层
	{
		int mx=INT_MIN; 
		int MX;
		for(int i=0;i<avail;i++)
		{
			int _i=where_allow_x[i],_j=where_allow_y[i];
			chessboard[_i][_j]=2;//放入黑棋（节点生成）
			MX=ABpruning(step+1,steplength-1,alpha,beta,_i,_j);//树延伸
			chessboard[_i][_j]=0;//回溯
			if(MX>mx)
			mx=MX;
			if(mx>alpha)
			alpha=mx;
			if(alpha>=beta)
			break;//剪枝
		}
		return alpha; //将极大值返回
        }
    }
    //Alpha-beta 剪枝
    else if(choose_black_or_white==1)
    {
    if(steplength==0)
	{
		return evaluate(2);
	}
	//胜负已分 返回数值
	if(avail==0)
	return evaluate(2);  

	if(step%2==1) // 极小值节点MIN层 玩家选择黑棋 对于我方而言 k%2==1 为极小值节点层
	{
		int mn=INT_MAX;
		int MN;
		for(int i=0;i<avail;i++)
		{
			int _i=where_allow_x[i],_j=where_allow_y[i];
			chessboard[_i][_j]=2; //放入黑棋（节点生成）
			MN=ABpruning(step+1,steplength-1,alpha,beta,_i,_j);//树延伸
			chessboard[_i][_j]=0;//回溯
			if(MN<mn)
			mn=MN;
			if(mn<beta)
			beta=mn;//取极小值
			if(alpha>=beta)
			break;//剪枝
		}
		return beta; //将极小值返回
	}
	else if(step%2==0) //极大值节点MAX层
	{
		int mx=INT_MIN; 
		int MX;
		for(int i=0;i<avail;i++)
		{
			int _i=where_allow_x[i],_j=where_allow_y[i];
			chessboard[_i][_j]=1;//放入白子（节点生成）
			MX=ABpruning(step+1,steplength-1,alpha,beta,_i,_j);//树延伸
			chessboard[_i][_j]=0;//回溯
			if(MX>mx)
			mx=MX;
			if(mx>alpha)
			alpha=mx;
			if(alpha>=beta)
			break;//剪枝
		}
		return alpha; //将极大值返回
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
			printf("请输入坐标\n");
			cin>>x>>y;
			k++;
			if(x==5&&y==5&&k==1){
				printf("不允许的操作，请重新输入：");
				k--;
				continue; 
			}
			else if(!put_black(x-1,y-1)){
				printf("很抱歉，您输了\n");
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
				printf("恭喜您，获胜了！\n");
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
	    	printf("请输入坐标\n");
	    	cin>>x>>y;
			k++;
			if(!put_white(x-1,y-1)){
				printf("很抱歉，您输了\n");
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
				printf("恭喜您，获胜了！\n");
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
void newopen() //新开始程序
{
//让玩家选择黑棋或白棋
    cout<<"您想要选择 1.黑棋 2.白棋 请输入相应序号"; 
    int kkk;
    cin>>kkk;
    if(kkk==1)
    choose_black_or_white=1; //选择黑棋
    else if(kkk==2)
    choose_black_or_white=2; //选择白棋
    RUN(); 
}
int main()
{ 
int n=0;
while (n!=3)
{ 
        cout <<"┍━━━━━━━┓\n";
        cout <<"┃--..选择..--┃ \n";
        cout <<"┃1.  新开始    ┃ \n";
        cout <<"┃2.  读盘      ┃ \n";
        cout <<"┃3.  退出游戏  ┃ \n";
        cout <<"┕━━━━━━━┛\n";
        cout <<"请输入数据选择 1/2/3: " << endl;
        cin >> n;
        cin.get();
        switch(n)
        { 
        case 1: cout<<"您想要选择 1.黑棋 2.白棋 请输入相应序号"; 
        int kkk;
        cin>>kkk;
        if(kkk==1)
        choose_black_or_white=1; //选择黑棋
        else if(kkk==2)
        choose_black_or_white=2; //选择白棋
        RUN();break;
        case 2: replay();break;
        case 3: break;
        default:cout<<"输入错误"; break;
        }
}
        return 0;
}
