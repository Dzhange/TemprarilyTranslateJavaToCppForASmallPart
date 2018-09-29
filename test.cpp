#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <math.h>
#include <iterator>
using namespace std;
class RectangularVectors
{
public:
    static std::vector<std::vector<int>> ReturnRectangularIntVector(int size1, int size2)
    {
        std::vector<std::vector<int>> newVector(size1);
        for (int vector1 = 0; vector1 < size1; vector1++)
        {
            newVector[vector1] = std::vector<int>(size2);
        }

        return newVector;
    }
};
int main(){
    float output_tensor[57*46*46] = {0};
    ifstream infile("out_tensor");
    int count = 0;
    while(!infile.eof()){
        infile>>output_tensor[count];
        count++;
    }
    infile.close();
    for (int j = 0;j<sizeof(output_tensor)/sizeof(output_tensor[0]);j++){
        cout<<output_tensor[j]<<endl;
    }
    float NMS_Threshold = static_cast<float>(0.15);
float Local_PAF_Threshold = static_cast<float>(0.2);
int PAF_Count_Threshold = 5;
int Part_Count_Threshold = 4;
      float Part_Score_Threshold = static_cast<float>(4.5);
      int MapHeight = 46;
      int MapWidth = 46;
      int HeatMapCount = 19;
      int MaxPairCount = 19;
      int PafMapCount = 38;
      int MaximumFilterSize = 5;
      int NumPafIter = 10;
      std::vector<std::vector<int>> CocoPairs =
      {
          std::vector<int> {1, 2},
          std::vector<int> {1, 5},
          std::vector<int> {2, 3},
          std::vector<int> {3, 4},
          std::vector<int> {5, 6},
          std::vector<int> {6, 7},
          std::vector<int> {1, 8},
          std::vector<int> {8, 9},
          std::vector<int> {9, 10},
          std::vector<int> {1, 11},
          std::vector<int> {11, 12},
          std::vector<int> {12, 13},
          std::vector<int> {1, 0},
          std::vector<int> {0, 14},
          std::vector<int> {14, 16},
          std::vector<int> {0, 15},
          std::vector<int> {15, 17},
          std::vector<int> {2, 16},
          std::vector<int> {5, 17}
      };
      std::vector<std::vector<int>> CocoPairsNetwork =
      {
          std::vector<int> {12, 13},
          std::vector<int> {20, 21},
          std::vector<int> {14, 15},
          std::vector<int> {16, 17},
          std::vector<int> {22, 23},
          std::vector<int> {24, 25},
          std::vector<int> {0, 1},
          std::vector<int> {2, 3},
          std::vector<int> {4, 5},
          std::vector<int> {6, 7},
          std::vector<int> {8, 9},
          std::vector<int> {10, 11},
          std::vector<int> {28, 29},
          std::vector<int> {30, 31},
          std::vector<int> {34, 35},
          std::vector<int> {32, 33},
          std::vector<int> {36, 37},
          std::vector<int> {18, 19},
          std::vector<int> {26, 27}
      };
     int inputSize_W = 368;
     int inputSize_H = 368;
     int imageMean = 128;
     float imageStd = 128.0f;
     std::vector<int> raw_input_image;
     std::vector<float> rgb_input_image;
     // std::vector<float> output_tensor;
     std::vector<std::wstring> outputNames;
//JAVA TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in Java:
//ORIGINAL LINE: int final_pairs[][] = new int[18][2];
    std::vector<std::vector<int>> final_pairs = RectangularVectors::ReturnRectangularIntVector(18, 2);

//----------------------------------------------------------------------------------------
//  Copyright © 2007 - 2018 Tangible Software Solutions, Inc.
//  This class can be used by anyone provided that the copyright notice remains intact.
//
//  This class includes methods to convert multidimensional arrays to C++ vectors.
//----------------------------------------------------------------------------------------


// cout<< "begin" << endl;

std::vector<std::vector<std::vector<int>>> coords;
coords.reserve(HeatMapCount-1);
coords.resize(HeatMapCount-1);

        // 用最大滤波和非极大值抑制来过滤重复的点，尽量使得某一个人的某一个部位只会被取到一次
        for (int i = 0; i < (HeatMapCount - 1); i++)
        {
          // cout<<"before coords[i]"<<endl;
            coords[i] = std::vector<std::vector<int>>();
            // cout<<"after"<<endl;
            for (int j = 0; j < MapHeight; j++)
            {
                for (int k = 0; k < MapWidth; k++)
                {
                    std::vector<int> coord = {j, k};
                    // cout<< j << endl;
                    float max_value = 0;
                    for (int dj = -(MaximumFilterSize - 1) / 2; dj < (MaximumFilterSize + 1) / 2; dj++)
                    {
                        if ((j + dj) >= MapHeight || (j + dj) < 0)
                        {
                            break;
                        }
                        for (int dk = -(MaximumFilterSize - 1) / 2; dk < (MaximumFilterSize + 1) / 2; dk++)
                        {
                            if ((k + dk) >= MapWidth || (k + dk) < 0)
                            {
                                break;
                            }
                            float value = output_tensor[(HeatMapCount + PafMapCount) * MapWidth * (j + dj) + (HeatMapCount + PafMapCount) * (k + dk) + i];
                            if (value > max_value)
                            {
                                max_value = value;
                            }
                        }
                    }
                    if (max_value > NMS_Threshold)
                    {
                      // cout<<"here"<<endl;
                        if (max_value == output_tensor[(HeatMapCount + PafMapCount) * MapWidth * j + (HeatMapCount + PafMapCount) * k + i])
                        {   
                            // cout<< "test"<<endl;
                            coords[i].push_back(coord);
                            // cout<< "max value" << endl;
                        }
                    }
                }
            }
        }
cout<< "begin" <<endl;
        // 用paf算分数，并用贪心法来剔除不合理或者重复的连线
        std::vector<std::vector<std::vector<int>>> pairs;
        pairs.reserve(MaxPairCount);
        pairs.resize(MaxPairCount);
        std::vector<std::vector<std::vector<int>>> pairs_final ;
        pairs_final.reserve(MaxPairCount);
        pairs_final.resize(MaxPairCount);
        std::vector<std::vector<float>> pairs_scores;
        pairs_scores.reserve(MaxPairCount);
        pairs_scores.resize(MaxPairCount);
        std::vector<std::vector<float>> pairs_scores_final ;
        pairs_scores_final.reserve(MaxPairCount);
        pairs_scores_final.resize(MaxPairCount);
        for (int i = 0; i < MaxPairCount; i++)
        {
            pairs[i] = std::vector<std::vector<int>>();
            pairs_scores[i] = std::vector<float>();
            pairs_final[i] = std::vector<std::vector<int>>();
            pairs_scores_final[i] = std::vector<float>();
            std::vector<int> part_set;
            for (int p1 = 0; p1 < coords[CocoPairs[i][0]].size(); p1++)
            {
                for (int p2 = 0; p2 < coords[CocoPairs[i][1]].size(); p2++)
                {
                    int count = 0;
                    float score = 0.0f;
                    std::vector<float> scores(10);
                    int p1x = coords[CocoPairs[i][0]][p1][0];
                    int p1y = coords[CocoPairs[i][0]][p1][1];
                    int p2x = coords[CocoPairs[i][1]][p2][0];
                    int p2y = coords[CocoPairs[i][1]][p2][1];
                    float dx = p2x - p1x;
                    float dy = p2y - p1y;
                    float normVec = static_cast<float>(std::sqrt(std::pow(dx, 2) + std::pow(dy, 2)));

                    if (normVec < 0.0001f)
                    {
                        break;
                    }
                    float vx = dx / normVec;
                    float vy = dy / normVec;
                    for (int t = 0; t < 10; t++)
                    {
                        int tx = static_cast<int>(static_cast<float>(p1x) + (t * (static_cast<float>(dx)) / 9) + 0.5);
                        int ty = static_cast<int>(static_cast<float>(p1y) + (t * (static_cast<float>(dy)) / 9) + 0.5);
                        int location = tx * (HeatMapCount + PafMapCount) * MapWidth + ty * (HeatMapCount + PafMapCount) + HeatMapCount;
                        scores[t] = vy * output_tensor[location + CocoPairsNetwork[i][0]];
                        scores[t] += vx * output_tensor[location + CocoPairsNetwork[i][1]];
                    }
                    for (int h = 0;h < 10;h++)
                    {
                        if (scores[h] > Local_PAF_Threshold)
                        {
                            count += 1;
                            score += scores[h];
                        }
                    }
                    if (score > 0.0f && count >= PAF_Count_Threshold)
                    {
                        bool inserted = false;
                        std::vector<int> pair = {p1, p2};
                        for (int l = 0;l < pairs[i].size();l++)
                        {
                            if (score > pairs_scores[i][l])
                            {
                                pairs[i].insert(pairs[i].begin() + l,pair);
                                pairs_scores[i].insert(pairs_scores[i].begin() + l,score);
                                inserted = true;
                                break;
                            }
                        }
                        if (!inserted)
                        {
                            pairs[i].push_back(pair);
                            pairs_scores[i].push_back(score);
                        }
                    }
                }
            }
            for (int m = 0;m < pairs[i].size();m++)
            {
                bool conflict = false;
                for (int n = 0;n < part_set.size();n++)
                {
                    if (pairs[i][m][0] == part_set[n] || pairs[i][m][1] == part_set[n])
                    {
                        conflict = true;
                        break;
                    }
                }
                if (!conflict)
                {
                  cout<<"here"<<endl;
                    pairs_final[i].push_back(pairs[i][m]);
                    pairs_scores_final[i].push_back(pairs_scores[i][m]);
                    part_set.push_back(pairs[i][m][0]);
                    part_set.push_back(pairs[i][m][1]);
                }
            }
        }

        // 得到所有的连线集合后，用并查集算法，尽可能合并所有的连线，无法合并的多个部分即为多个人
//JAVA TO C++ CONVERTER TODO TASK: Local classes are not converted by Java to C++ Converter:
//      class Human
//      {
//          public int parts_coords[][] = new int[18][2];
//          // not important
//          public int part_count=0;
//          public int coords_index_set[] = new int[18];
//          public boolean coords_index_asigned[] = new boolean[18];
//      }

        typedef struct _
        {   
            std::vector<std::vector<int>> parts_coords;
            // int parts_coords[18][2];
            int part_count=0;
            int coords_index_set[18];
            bool coords_index_asigned[18];
        }Human;
        cout<< "ok" << endl;
        std::vector<Human> humans;
        std::vector<Human> humans_final;
        for (int i = 0;i < MaxPairCount;i++)
{
    cout<<"size "<<pairs_final[i].size()<<endl;
            for (int j = 0;j < pairs_final[i].size();j++)
            { 
                bool merged = false;
                int p1 = CocoPairs[i][0];
                int p2 = CocoPairs[i][1];
                int ip1 = pairs_final[i][j][0];
                int ip2 = pairs_final[i][j][1];
                for (int k = 0;k < humans.size();k++)
                {
                    Human human = humans[k];
                    if ((ip1 == human.coords_index_set[p1] && human.coords_index_asigned[p1]) || (ip2 == human.coords_index_set[p2] && human.coords_index_asigned[p2]))
                    {
                        human.parts_coords[p1] = coords[p1][ip1];
                        human.parts_coords[p2] = coords[p2][ip2];
                        human.coords_index_set[p1] = ip1;
                        human.coords_index_set[p2] = ip2;
                        human.coords_index_asigned[p1] = true;
                        human.coords_index_asigned[p2] = true;
                        merged = true;
                        break;
                    }
                }
                if (!merged)
                {
                    Human human;
                    human.parts_coords[p1] = coords[p1][ip1];
                    human.parts_coords[p2] = coords[p2][ip2];
                    human.coords_index_set[p1] = ip1;
                    human.coords_index_set[p2] = ip2;
                    human.coords_index_asigned[p1] = true;
                    human.coords_index_asigned[p2] = true;
                    humans.push_back(human);
                }
            }
}

cout<<'seg2'<<endl;
        // 去掉部位数量过少的人
        for (int i = 0;i < humans.size();i++)
        {
            int human_part_count = 0;
            for (int j = 0;j < HeatMapCount - 1;j++)
            {
                if (humans[i].coords_index_asigned[j])
                {
                    human_part_count += 1;
                }
            }
            if (human_part_count > Part_Count_Threshold)
            {
                humans_final.push_back(humans[i]);
            }
        }


for (int i = 0;i < humans_final.size();i++)
        {
            // has_people = true;
            for (int j = 0;j < HeatMapCount - 1;j++)
            {
                int x1 = humans_final[i].parts_coords[j][0];
                int y1 = humans_final[i].parts_coords[j][1];
                if (x1 != 0)
                {
                    final_pairs[j][0] = x1;
                    final_pairs[j][1] = y1;
                    cout<< x1 << endl;
                    cout<< y1 << endl;
                }
            }
        }

    return 0;
}


// class Human
// {
// public:
// 	int parts_coords[18][2];
//             // not important
//    	int part_count=0;
//     int coords_index_set[18];
//     bool coords_index_asigned[18];
// };

// std::vector<Human> humans;
// std::vector<human> humans_final;

// for(int i = 0; i < MaxPairCount;i++){
// 	for(int j=0;j<pairs_final[i].size();j++){
// 		bool merged = false;
// 		int p1=CocoPairs[i][0];
//         int p2=CocoPairs[i][1];
//         int ip1=pairs_final[i][j][0];
//         int ip2=pairs_final[i][j][1];
//         for(int k=0;k<humans.size();k++){
//         	Human human = humans[k];
//         	if((ip1 == human.coords_index_set[p1] && human.coords_index_asigned[p1]) || (ip2 == human.coords_index_set[p2] && human.coords_index_asigned[p2]))
//             {
//                 human.parts_coords[p1]=coords[p1].[ip1];
//                 human.parts_coords[p2]=coords[p2].[ip2];
//                 human.coords_index_set[p1]=ip1;
//                 human.coords_index_set[p2]=ip2;
//                 human.coords_index_asigned[p1]=true;
//                 human.coords_index_asigned[p2]=true;
//                 merged=true;
//                 break;
//             }
//         }
//         if(!merged){
//         	Human human=new Human();
//             human.parts_coords[p1]=coords[p1].[ip1];
//             human.parts_coords[p2]=coords[p2].[ip2];
//             human.coords_index_set[p1]=ip1;
//             human.coords_index_set[p2]=ip2;
//             human.coords_index_asigned[p1]=true;
//             human.coords_index_asigned[p2]=true;
//             humans.push_back(human);
//         }
// 	}
// }

// for(int i=0;i<humans.size();i++)
// {
//     int human_part_count=0;
//     for(int j=0;j<HeatMapCount-1;j++)
//     {
//         if(humans[i].coords_index_asigned[j])
//         {
//             human_part_count+=1;
//         }
//     }
//     if (human_part_count>Part_Count_Threshold)
//     {
//         humans_final.push_back(humans.[i]);
//     }
// }

// for(int i = 0;i<humans_final.size();i++){
//     has_people = true;
//     for(int j=0;j<HeatMapCount-1;j++){
//         int x1 =humans_final[i].parts_coords[j][0];
//         int y1 =humans_final[i].parts_coords[j][1];
//         if(x1 !=0){
//             final_pairs[j][0] = x1;
//             final_pairs[j][1] = y1;
//         }
//     }
// }