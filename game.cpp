#pragma GCC optimize(3)
#include <iostream>
#include <iomanip>
#include <thread>
#define simulated_round 2 //模拟?轮
#define inf 900000000

using namespace std;

//void initialize_chessboard(); //初始化棋盘
void change_sim_aera(int x, int y);
void display_chessboard(); //输出棋盘
int change_chessboard(int, int, int); //改变棋盘
int evaluation(); //评估函数入口
int get_analysising_list(int x, int y); //评估子函数
int analysis_pattern(); //评估子函数
int simulation(int round, int alpha, int beta); //模拟函数入口
void if_success();

int chessboard[15][15] = {}; //主棋盘
int simulating_aera[25][25] = {};  //设定需要模拟的区域
int best_x, best_y;
int round_counter = 0;



//棋形列表(16个棋形)
char pattern_list[16][8] = {
    {'*', '*', '*', 'O', 'O', 'O', 'O', 'O'},
    {'*', '*', '+', 'O', 'O', 'O', 'O', '+'},
    {'*', '*', '+', 'O', 'O', 'O', '+', '+'},
    {'*', '+', '+', 'O', 'O', 'O', '+', '*'},
    {'*', '*', '+', 'O', 'O', '+', 'O', '+'},
    {'*', '*', '+', 'O', '+', 'O', 'O', '+'},
    {'*', '*', '*', 'O', 'O', 'O', 'O', '+'},
    {'*', '*', '+', 'O', 'O', 'O', 'O', '*'},
    {'*', '*', '*', 'O', 'O', '+', 'O', 'O'},
    {'*', '*', '*', 'O', '+', 'O', 'O', 'O'},
    {'*', '*', '*', 'O', 'O', 'O', '+', 'O'},
    {'*', '+', '+', 'O', 'O', '+', '+', '*'},
    {'*', '+', '+', 'O', '+', 'O', '+', '*'},
    {'*', '*', '+', 'O', '+', 'O', '+', '+'},
    {'+', '+', '+', 'O', '+', '+', '*', '*'},
    {'*', '+', '+', 'O', '+', '+', '+', '*'},
};

//如果不是本棋型则跳转（优化加速）
int jump_list[16][8] = {
    {14, 3, 2, 16, 5, 4, 2, 1},       //0
    {14, 3, 6, 16, 5, 4, 2, 3},       //1
    {14, 3, 6, 16, 5, 4, 4, 3},       //2
    {14, 4, 6, 16, 5, 4, 4, 4},       //3
    {14, 11, 6, 16, 5, 5, 10, 7},     //4
    {14, 11, 6, 16, 6, 8, 10, 7},     //5
    {14, 11, 7, 16, 9, 8, 10, 7},     //6
    {14, 11, 8, 16, 9, 8, 10, 8},     //7
    {14, 11, 11, 16, 9, 9, 10, 11},   //8
    {14, 11, 11, 16, 10, 11, 10, 11}, //9
    {14, 11, 11, 16, 12, 11, 14, 11}, //10
    {14, 13, 16, 16, 12, 12, 14, 13}, //11
    {14, 13, 16, 16, 16, 14, 14, 13}, //12
    {14, 14, 16, 16, 16, 14, 14, 14}, //13
    {15, 16, 16, 16, 16, 16, 15, 16}, //14
    {16, 16, 16, 16, 16, 16, 16, 16}, //15
};

//棋形所对应的分数
int pattern_score[16] = {inf, 4320, 720, 720, 720, 720, 720, 720, 720, 720, 720, 120, 120, 120, 20, 20};




int main()
{
    //initialize_chessboard();//初始化棋盘
    display_chessboard();//输出棋盘

    while (1)
    {

        //if (round_counter == 0)
          //  change_chessboard(7, 7, 2);

        int err_code = 0;
        int x, y, score;
        do
        {
            cin >> x >> y;
            err_code = change_chessboard(x - 1, y - 1, 1);
        }while (err_code);
        display_chessboard();
        if_success();
        
        cout << "Please wait..." << endl;
        simulation(1, -inf, inf);
        change_chessboard(best_x, best_y, 2);
        display_chessboard();
        cout << "Computer put a chess at (" << best_x + 1 << ", " << best_y + 1 << ")." << endl;
        if_success();

        round_counter++;

    }

    return 0;
}


/*
void initialize_chessboard()//初始化棋盘
{
     for (int y = 0; y < 15; y++)
    {
        for (int x = 0; x < 15; x++)
        {
            chessboard[x][y] = 0;
        }
    }
}
*/



void display_chessboard()//输出棋盘
{
    char output;
    for (int y = 14; y >= 0; y--)
    {
        cout << setw(2) << y + 1 << " ";
        for (int x = 0; x < 15; x++)
        {
            if (chessboard[x][y] == 0)
                output = '.';
            if (chessboard[x][y] == 1)
                output = 'X';
            if (chessboard[x][y] == 2)
                output = 'O';
            cout << output << "  ";
        }
        cout << endl;
    }
    cout << "   1  2  3  4  5  6  7  8  9  10 11 12 13 14 15" << endl;
}



int change_chessboard(int x, int y, int chess_type)  //改变棋盘
{
    if (x > 14 || x < 0 || y > 14 || y < 0)
    {
        cerr << "Illegal location. (" << x + 1 << ", " << y + 1 << ") is out of range." << endl;
        return 1;
    }
    else if (chessboard[x][y] != 0)
    {
        cerr << "There has been a chess at (" << x + 1 << ", " << y + 1 << ")." << endl;
        return 2;
    }
    else
    {
        chessboard[x][y] = chess_type;
        change_sim_aera(x, y);
        return 0;
    }
}


void if_success()
{
    int score = evaluation();
    if (score >= 500000000)
    {
        cout << "You lose." << endl;
        cout << "Rounds count:" << round_counter;
        exit(1);
    }
    else if (score <= -500000000)
    {
        cout << "You win!" << endl;
        cout << "Rounds count:" << round_counter;
        exit(1);
    }
    else
    {
        cout << "Current score:" << -score << endl;
    }
    
}


void change_sim_aera(int x, int y)
{
    x += 5;
    y += 5; // FIXME: 防止越界报错，使用时记得也+5
    simulating_aera[x + 1][y] = simulating_aera[x + 2][y] = simulating_aera[x + 3][y] = 1;
    simulating_aera[x - 1][y] = simulating_aera[x - 2][y] = simulating_aera[x - 3][y] = 1;
    simulating_aera[x][y + 1] = simulating_aera[x][y + 2] = simulating_aera[x][y + 3] = 1;
    simulating_aera[x][y - 1] = simulating_aera[x][y - 2] = simulating_aera[x][y - 3] = 1;
    simulating_aera[x + 1][y + 1] = simulating_aera[x + 2][y + 2] = simulating_aera[x + 3][y + 3] = 1;
    simulating_aera[x - 1][y - 1] = simulating_aera[x - 2][y - 2] = simulating_aera[x - 3][y - 3] = 1;
    simulating_aera[x - 1][y + 1] = simulating_aera[x - 2][y + 2] = simulating_aera[x - 3][y + 3] = 1;
    simulating_aera[x + 1][y - 1] = simulating_aera[x + 2][y - 2] = simulating_aera[x + 3][y - 3] = 1;
}


int analysising_list[8];
int evaluation()//评估函数入口
{
    //评估函数（包括 evaluation, get_analysising_list, analysis_pattern）的入口
    //获取跟踪棋子位置
    int score = 0;  //score(分数)
    for (int y = 0; y < 15; y++)
    {
        for (int x = 0; x < 15; x++)
        {
            if (chessboard[x][y] != 0)//当当前棋子位置不是空位的时候
            {
                score += get_analysising_list(x, y);  //将棋子位置输入子函数，并合并每个被跟踪棋子分数
                //get_analysising_list返回当前分数
            }
        }
    }
    return score;//返回总分数
}


int get_analysising_list(int x, int y)//评估子函数
//用于获取被跟踪棋子周围4个方向的棋子，将每个方向跟踪棋子前3个棋子和后4个棋子（加本身共8个元素）写入数组 analysising_list[8]
//输入被跟踪棋子的 x, y，返回被跟踪棋子的分数
{
    int score = 0;
    int pos_x, pos_y;

    //横向
    for (int i = 0; i < 8; i++)
    {
        pos_x = x - 3 + i;
        if (pos_x < 0 || pos_x > 14)
        {
            analysising_list[i] = -1;
        }
        else
        {
            analysising_list[i] = chessboard[pos_x][y];
        }
    }
    score += analysis_pattern();  //调用子函数分析获取的 analysising_list[8]，并合并每一数组分数



    //纵向
    for (int i = 0; i < 8; i++)
    {
        pos_y = y - 3 + i;
        if (pos_y < 0 || pos_y > 14)
        {
            analysising_list[i] = -1;
        }
        else
        {
            analysising_list[i] = chessboard[x][pos_y];
        }
    }
    score += analysis_pattern();



    //左下到右上
    for (int i = 0; i < 8; i++)
    {
        pos_x = x - 3 + i;
        pos_y = y - 3 + i;
        if (pos_y < 0 || pos_y > 14 || pos_x < 0 || pos_x > 14)
        {
            analysising_list[i] = -1;
        }
        else
        {
            analysising_list[i] = chessboard[pos_x][pos_y];
        }
    }
    score += analysis_pattern();



    //左上到右下
    for (int i = 0; i < 8; i++)
    {
        pos_x = x - 3 + i;
        pos_y = y + 3 - i;
        if (pos_y < 0 || pos_y > 14 || pos_x < 0 || pos_x > 14)
        {
            analysising_list[i] = -1;
        }
        else
        {
            analysising_list[i] = chessboard[pos_x][pos_y];
        }
    }
    score += analysis_pattern();

    return score;
}



int analysis_pattern()//评估子函数
//用于识别棋形，评估核心函数
//输入 数组 analysising_list[8]，返回 本数组分数

/*
    analysising_list[0][1][2] 为被跟踪棋子前三个棋子
    analysising_list[3] 为被跟踪棋子
    analysising_list[4][5][6][7] 为被跟踪棋子后三个棋子

    analysising_list[i] = -1(边界外) / 0(无棋子) / 1(玩家棋子) / 2(Ai棋子)
*/

{
    char comparator;
    for (int i = 0; i < 16; i++)  //16个棋形
    {
        for (int k = 0; k < 9; k++)  //加上分数共9项
        {
            if (k == 8) //判断是否为在读取分数项
            {
                if (analysising_list[3] == 2)
                    return pattern_score[i];   //Ai棋子
                else
                    return -pattern_score[i];  //玩家棋子
            }
            else
            {
                comparator = pattern_list[i][k];

                if (comparator == '+' && analysising_list[k] != 0)  //判断空位
                {
                    i = jump_list[i][k] - 1;
                    break;
                }

                else if (comparator == 'O' && analysising_list[k] != analysising_list[3])  //判断棋子
                {
                    i = jump_list[i][k] - 1;
                    break;
                }

                //'*'为空位，不必判断
            }
        }
    }
    return 0;
}



int simulation(int round, int alpha, int beta) //模拟函数
//TODO: 注意：必须使用1 (1为开始轮数)

//返回最佳下棋点
{
    if (round > simulated_round) //检测是否模拟完毕最后一轮，调用评估
    {
        return evaluation();
    }
    else
    {
        int chess_type = round % 2 + 1;  //round为奇数时模拟Ai(max)下子，偶数数时模拟玩家(min)下子

        for (int y = 0; y < 15; y++)
        {

            for (int x = 0; x < 15; x++)
            {
                if (alpha >= beta)
                    return alpha;  //剪枝，返回任意一不影响母节点的值（alpha到beta之间的任意值）
                
                if (simulating_aera[x + 5][y + 5] == 0)  //定义时+5防止越界，此处调用时也+5
                    continue;

                if (chessboard[x][y] == 0) //空位判断
                {
                    chessboard[x][y] = chess_type;
                    int score = simulation(round + 1, alpha, beta);
                    chessboard[x][y] = 0;

                    if (chess_type == 2) //奇数时模拟Ai取max
                    {
                        if (score > alpha)
                        {
                            alpha = score;
                            best_x = x; //如果是第一轮需返回的最好下棋位置x
                            best_y = y; //如果是第一轮需返回的最好下棋位置y
                        }
                    }
                    else //if (chess_type == 1)  偶数时模拟玩家取min
                    {
                        if (score < beta)
                        {
                            beta = score;
                        }
                    }

                }

            }
        }

        if (chess_type == 2)
            return alpha;
        else //if (chess_type == 1)
            return beta;



    }


}
