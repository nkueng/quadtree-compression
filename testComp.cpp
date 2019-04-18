#define CATCH_CONFIG_MAIN
#include "cs221util/catch.hpp"
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include "cs221util/PNG.h"
#include "cs221util/HSLAPixel.h"
#include "stats.h"
#include "toqutree.h"

using namespace std;
using namespace cs221util;


// TEST_CASE("stats::basic rectArea","[weight=1][part=stats]"){

//     PNG data; data.resize(2,2);

//     stats s(data);
//     pair<int,int> ul(0,0);
//     pair<int,int> lr(1,1);
//     long result = s.rectArea(ul,lr);

//     REQUIRE(result == 4);
    
// }

// TEST_CASE("stats::basic getAvg","[weight=1][part=stats]"){
//     PNG data; data.resize(2,2);
//     for (int i = 0; i < 2; i ++){
//         for (int j = 0; j < 2; j++){
//             HSLAPixel * p = data.getPixel(i,j);
//             p->h = 135*j + i * 90;
//             p->s = 1.0;
//             p->l = 0.5;
//             p->a = 1.0;
//         }
//     }
//     stats s(data);
//     pair<int,int> ul(0,0);
//     pair<int,int> lr(1,1);
//     HSLAPixel result = s.getAvg(ul,lr);
//     HSLAPixel expected(112.5,1.0, 0.5);

//     REQUIRE(result == expected);
// }

// // maybe TODO: expand to 3x3
// TEST_CASE("stats::advanced getAvg","[weight=1][part=stats]"){
//     PNG data; data.resize(2,2);
//     for (int i = 0; i < 2; i ++){
//         for (int j = 0; j < 2; j++){
//             HSLAPixel * p = data.getPixel(i,j);
//             p->h = 135*j + i * 90;
//             cout << "hue at (" << i << ", " << j << "): " << p->h << endl; 
//             p->s = 1.0;
//             p->l = 0.5;
//             p->a = 1.0;
//         }
//     }
//     stats s(data);
//     pair<int,int> ul(1,1);
//     pair<int,int> lr(1,1);
//     HSLAPixel result = s.getAvg(ul,lr);
//     HSLAPixel expected(225,1.0, 0.5);

//     REQUIRE(result == expected);
// }

// TEST_CASE("stats:: histogram","[weight=1][part=stats]"){
//     PNG data; data.resize(2,2);
//     for (int i = 0; i < 2; i ++){
//         for (int j = 0; j < 2; j++){
//             HSLAPixel * p = data.getPixel(i,j);
//             p->h = 135*j + i * 90;
//             cout << "hue at (" << i << ", " << j << "): " << p->h << endl; 
//             p->s = 1.0;
//             p->l = 0.5;
//             p->a = 1.0;
//         }
//     }
//     stats s(data);
//     for (int i = 0; i < 2; i ++){
//         for (int j = 0; j < 2; j++){
//             cout << "hist[" << i << "][" << j << "]: ";
//             for (int k = 0; k < 36; ++k)
//             {
//                 cout << k << ": " << s.hist[i][j][k] << ", ";
//             }
//             cout << endl;
//         }
//     }
//     pair<int,int> ul(1,1);
//     pair<int,int> lr(1,1);
//     HSLAPixel result = s.getAvg(ul,lr);
//     HSLAPixel expected(225,1.0, 0.5);

//     REQUIRE(result == expected);
// }

// TEST_CASE("stats:: single histogram","[weight=1][part=stats]"){
//     PNG data; data.resize(2,2);
//     for (int i = 0; i < 2; i ++){
//         for (int j = 0; j < 2; j++){
//             HSLAPixel * p = data.getPixel(i,j);
//             p->h = 135*j + i * 90;
//             cout << "hue at (" << i << ", " << j << "): " << p->h << endl; 
//             p->s = 1.0;
//             p->l = 0.5;
//             p->a = 1.0;
//         }
//     }
//     stats s(data);
//     pair<int,int> ul(0,1);
//     pair<int,int> lr(1,1);
//     vector<int> hist = s.buildHist(ul, lr);
//     for (int k = 0; k < 36; ++k)
//     {
//         cout << k << ": " << hist[k] << ", ";
//     }
// }

// TEST_CASE("stats::basic entropy","[weight=1][part=stats]"){
//     PNG data; data.resize(2,2);
//     for (int i = 0; i < 2; i ++){
//         for (int j = 0; j < 2; j++){
//             HSLAPixel * p = data.getPixel(i,j);
//             p->h = 135*j + i * 90;
//             p->s = 1.0;
//             p->l = 0.5;
//             p->a = 1.0;
//         }
//     }
//     stats s(data);
//     pair<int,int> ul(0,0);
//     pair<int,int> lr(1,1);
//     long result = s.entropy(ul,lr);

//     REQUIRE(result == 2);
// }

// TEST_CASE("toqutree::basic splitting","[weight=1][part=toqutree]"){
    
//     // // create simple img of k=2 with 4 different color regions to check splitting  
//     // PNG img(4, 4);
//     // for (int i = 0; i < 2; ++i)
//     // {
//     //     for (int j = 0; j < 2; ++j)
//     //     {
//     //         HSLAPixel write_pxl( ( i + 2 * j ) * 90, 1.0, 0.5);

//     //         // ul
//     //         HSLAPixel * read_ptr = img.getPixel(2*i, 2*j);
//     //         * read_ptr = write_pxl;

//     //         // ur
//     //         read_ptr = img.getPixel(2*i+1, 2*j);
//     //         * read_ptr = write_pxl;

//     //         // ll
//     //         read_ptr = img.getPixel(2*i, 2*j+1);
//     //         * read_ptr = write_pxl;

//     //         // lr
//     //         read_ptr = img.getPixel(2*i+1, 2*j+1);
//     //         * read_ptr = write_pxl;
//     //     }
//     // }

//     // img.writeToFile("images/test_patch.png");

//     PNG img;
//     img.readFromFile("images/oneSquare.png");

//     toqutree t1(img,4);

//     PNG out = t1.render();
//     out.convert();
//     out.writeToFile("images/oneSquare_rendered.png");
//     REQUIRE(out==img);
// }

// TEST_CASE("toqutree::basic ctor render","[weight=1][part=toqutree]"){
//     PNG img;
//     img.readFromFile("images/stanleySquare.png");

//     toqutree t1(img,9);

//     PNG out = t1.render();
//     out.writeToFile("images/stanleySquare_rendered.png");
//     out.convert();

//     REQUIRE(out==img);
// }

// TEST_CASE("toqutree::basic copy","[weight=1][part=toqutree]"){
//     PNG img;
//     img.readFromFile("images/geo.png");

//     toqutree t1(img,5);
//     toqutree t1copy(t1);

//     PNG out = t1copy.render();

//     REQUIRE(out==img);
// }

// TEST_CASE("toqutree::very basic prune and size","[weight=1][part=toqutree]"){
//     PNG img;
//     img.readFromFile("images/smB.png");
    
//     toqutree t1(img,3);
//     cout << "size before pruning: " << t1.size() << endl;

//     t1.prune(0.05);
//     cout << "size after pruning: " << t1.size() << endl;
//     PNG result = t1.render();
//     result.writeToFile("images/smB_rendered.png");

//     // PNG expected; expected.readFromFile("images/adaPrune.05.png");
//     // result.convert();

//     // REQUIRE(expected==result);
// }

TEST_CASE("toqutree::basic prune","[weight=1][part=toqutree]"){
    PNG img;
    img.readFromFile("images/ada.png");
    
    toqutree t1(img,9);

    t1.prune(0.05);
    PNG result = t1.render();
    result.writeToFile("images/ada_rendered.png");

    PNG expected; expected.readFromFile("images/adaPrune.05.png");
    result.convert();

    REQUIRE(expected==result);
}

