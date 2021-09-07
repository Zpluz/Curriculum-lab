#include "sudoku.hpp"
#include <bitset>
#include <thread>

//header files
using namespace std;
bool res = false;
vector<thread> ths;
vector<vector<char>> *final_mat = new vector<vector<char>>(9, vector<char>(9));

namespace sudoku
{
    vector<bitset<9>> rows;
    vector<bitset<9>> cols;
    vector<vector<bitset<9>>> cells;

    //通过取或再取反，值为1对应的位号+1即为可填数字
    bitset<9> candidateNum(int r, int c, vector<bitset<9>> &rows, vector<bitset<9>> &cols, vector<vector<bitset<9>>> &cells)
    {
        return ~(rows[r] | cols[c] | cells[r / 3][c / 3]);
    }

    //更新矩阵以及相关信息
    void updateMatrix(int r, int c, int bit, int flag, vector<vector<char>> &matrix, vector<bitset<9>> &rows, vector<bitset<9>> &cols, vector<vector<bitset<9>>> &cells)
    {
        rows[r][bit] = flag ? 1 : 0;
        cols[c][bit] = flag ? 1 : 0;
        cells[r / 3][c / 3][bit] = flag ? 1 : 0;
        matrix[r][c] = flag ? (bit + '1') : '-';
    }

    //选取可能数目最小的空开始递归，即candidateNum返回值中1的数量最小
    pair<int, int> selectLeastPossible(vector<vector<char>> &matrix, vector<bitset<9>> &rows, vector<bitset<9>> &cols, vector<vector<bitset<9>>> &cells)
    {
        pair<int, int> indices;
        int minCandNum = 9;
        for (int r = 0; r < 9; ++r)
        {
            for (int c = 0; c < 9; ++c)
            {
                if (matrix[r][c] == '-')
                {
                    bitset<9> candState = candidateNum(r, c, rows, cols, cells);
                    if (candState.count() < minCandNum)
                    {
                        pair<int, int> tmp(r, c);
                        indices = tmp;
                        minCandNum = candState.count();
                    }
                }
            }
        }
        return indices;
    }

    //开始递归
    void dfs(vector<vector<char>> &matrix, int empty, int lev, vector<bitset<9>> &rows, vector<bitset<9>> &cols, vector<vector<bitset<9>>> &cells)
    {
        if (!empty)
        {
            res = true;
            for (int i = 0; i < 9; ++i)
            {
                for (int j = 0; j < 9; ++j)
                {
                    (*final_mat)[i][j] = matrix[i][j];
                }
            }
            return;
        }

        pair<int, int> selected = selectLeastPossible(matrix, rows, cols, cells);               //选择进行填的空的坐标
        bitset<9> candState = candidateNum(selected.first, selected.second, rows, cols, cells); //该空的可能值
        for (int b = 0; b < 9; ++b)
        {
            if (candState[b] == 1)
            {
                updateMatrix(selected.first, selected.second, b, 1, matrix, rows, cols, cells);
                if (candState.count() > 2)
                {
                    vector<vector<char>> *th_mat = new vector<vector<char>>(9, vector<char>(9));
                    vector<bitset<9>> *th_rows = new vector<bitset<9>>(9, bitset<9>());
                    vector<bitset<9>> *th_cols = new vector<bitset<9>>(9, bitset<9>());
                    vector<vector<bitset<9>>> *th_cells = new vector<vector<bitset<9>>>(3, vector<bitset<9>>(3, bitset<9>()));

                    for (int i = 0; i < 9; ++i)
                    {
                        for (int j = 0; j < 9; ++j)
                        {
                            (*th_mat)[i][j] = matrix[i][j];
                            (*th_rows)[i][j] = rows[i][j];
                            (*th_cols)[i][j] = cols[i][j];
                        }
                    }

                    for (int i = 0; i < 3; ++i)
                    {
                        for (int j = 0; j < 3; ++j)
                        {
                            for (int k = 0; k < 9; ++k)
                            {
                                (*th_cells)[i][j][k] = cells[i][j][k];
                            }
                        }
                    }
                    ths.push_back(thread(dfs, ref(*th_mat), empty - 1, lev + 1, ref(*th_rows), ref(*th_cols), ref(*th_cells)));
                }
                else
                {
                    dfs(matrix, empty - 1, lev + 1, rows, cols, cells);
                    if (res)
                        return;
                }
                updateMatrix(selected.first, selected.second, b, 0, matrix, rows, cols, cells);
            }
        }
        return;
    }

    void solveSudoku(vector<vector<char>> &matrix)
    {
        int empty = 0; //记录未填空数
        //用9位二进制(第0~8位)记录每一行(列，块)数字的使用情况(0，1不是指对应下标是否填写数字，而是记录对应数字是否已经使用)，譬如若第8位为1，则该行(列，块)某个位置已经填写数字8+1=9(数字范围1～9)
        //因此matrix的盘面状态也需要一直维护
        rows = vector<bitset<9>>(9, bitset<9>());
        cols = vector<bitset<9>>(9, bitset<9>());
        cells = vector<vector<bitset<9>>>(3, vector<bitset<9>>(3, bitset<9>()));

        for (int r = 0; r < 9; ++r)
        {
            for (int c = 0; c < 9; ++c)
            {
                char cur = matrix[r][c];
                if (cur == '-')
                    empty++;
                else
                {
                    int bit = cur - '1';
                    rows[r] |= 1 << bit;
                    cols[c] |= 1 << bit;
                    cells[r / 3][c / 3] |= 1 << bit;
                }
            }
        }

        // 首先通过枚举，填写有唯一可能的空，即对应位上的candidateNum返回值中只有一个位上为1
        while (true)
        {
            int modified = 0;
            for (int r = 0; r < 9; ++r)
            {
                for (int c = 0; c < 9; ++c)
                {
                    if (matrix[r][c] == '-')
                    {
                        bitset<9> candState = candidateNum(r, c, rows, cols, cells);
                        if (candState.count() == 1)
                        {
                            for (int b = 0; b < 9; ++b)
                            {
                                if (candState[b] == 1)
                                {
                                    modified = 1;
                                    //为1的最低位对应的位号
                                    rows[r][b] = 1;
                                    cols[c][b] = 1;
                                    cells[r / 3][c / 3][b] = 1;
                                    matrix[r][c] = b + '1';
                                    empty--;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (modified == 0)
                break;
        }

        if (empty > 0)
        {
            dfs(matrix, empty, 0, rows, cols, cells);

            for (auto &th : ths)
            {
                th.join();
            }

            for (int i = 0; i < 9; ++i)
            {
                for (int j = 0; j < 9; ++j)
                {
                    matrix[i][j] = (*final_mat)[i][j];
                }
            }
        }
    }
}

int main()
{
    freopen("test.txt", "r", stdin);
    vector<vector<char>> matrix = vector<vector<char>>(9, vector<char>(9, '-'));
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            cin >> matrix[i][j];
        }
    }
    sudoku::solveSudoku(matrix);

    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            cout << matrix[i][j] << ' ';
        }
        cout << endl;
    }
}
