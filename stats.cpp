
#include "stats.h"

stats::stats(PNG & im){

    auto width = im.width();
    auto height = im.height();
    HSLAPixel * curr_pix_ptr;

    // initialize cumulative sum vectors and histogram
    // treat vertical direction first
    for (int i = 0; i < width; ++i)
    {
        double X_sum = 0.0;
        double Y_sum = 0.0;
        double Sat_sum = 0.0;
        double Lum_sum = 0.0;

        vector<double> temp_vec_X;
        vector<double> temp_vec_Y;
        vector<double> temp_vec_Sat;
        vector<double> temp_vec_Lum;
        vector< vector <int> > temp_vec_hist;

        for (int j = 0; j < height; ++j)
        {
            curr_pix_ptr = im.getPixel(i,j);

            double hue_rad = (curr_pix_ptr->h)*PI/180.0;
            double X = cos(hue_rad);
            double Y = sin(hue_rad);

            X_sum += X;
            Y_sum += Y;
            Sat_sum += curr_pix_ptr->s;
            Lum_sum += curr_pix_ptr->l;

            temp_vec_X.push_back(X_sum);
            temp_vec_Y.push_back(Y_sum);
            temp_vec_Sat.push_back(Sat_sum);
            temp_vec_Lum.push_back(Lum_sum);

            // histogram
            vector<int> curr_hist(36,0);
            if (j != 0) 
            {
                // base current count on upper neighbor
                vector<int> upp_neigh = temp_vec_hist[j-1];
                std::transform(curr_hist.begin(), curr_hist.end(), upp_neigh.begin(), curr_hist.begin(), std::plus<int>());
            }
            int idx = curr_pix_ptr->h / 10;
            if(idx > 35) cout << "idx is larger than 35, segFault!" << endl;
            curr_hist[idx] += 1;
            temp_vec_hist.push_back(curr_hist);
        }

        sumHueX.push_back(temp_vec_X);
        sumHueY.push_back(temp_vec_Y);
        sumSat.push_back(temp_vec_Sat);
        sumLum.push_back(temp_vec_Lum);
        hist.push_back(temp_vec_hist);
    }

    // now iterate over the horizontal direction
    for (int j = 0; j < height; ++j)
    {
        double X_sum = 0.0;
        double Y_sum = 0.0;
        double Sat_sum = 0.0;
        double Lum_sum = 0.0;

        for (int i = 0; i < width; ++i)
        {
            X_sum += sumHueX[i][j];
            Y_sum += sumHueY[i][j];
            Sat_sum += sumSat[i][j];
            Lum_sum += sumLum[i][j];

            sumHueX[i][j] = X_sum;
            sumHueY[i][j] = Y_sum;
            sumSat[i][j] = Sat_sum;
            sumLum[i][j] = Lum_sum;

            // histogram
            if (i != 0)
            {
                // copy all values from left neighbor
                vector<int> left_neigh = hist[i-1][j];
                vector<int> curr_hist = hist[i][j];
                std::transform(curr_hist.begin(), curr_hist.end(), left_neigh.begin(), curr_hist.begin(), std::plus<int>());
                hist[i][j] = curr_hist;
            }
        }
    }
    
    // cout << "Successfully initialized cumulative sum vectors." << endl;

}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){

    long dx = abs(ul.first - lr.first) + 1;
    long dy = abs(ul.second - lr.second) + 1;
    return ( dx * dy );
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){

    long num_pix = rectArea(ul, lr);
    HSLAPixel ret_pix;
    ret_pix.a = 1.0;
    double hueX;
    double hueY;

    if (ul.first < 0 || ul.second < 0)
    {
        cout << "negative coords for ul" << endl;
    }

    // get relevant coords of regions
    // equation: largest region - left region - upper region + leftupper region
    long dx = abs(ul.first - lr.first) + 1;
    long dy = abs(ul.second - lr.second) + 1;

    int x_lar = lr.first;
    int y_lar = lr.second;

    int x_left = x_lar - dx;
    int y_left = y_lar;

    int x_upp = x_lar;
    int y_upp = y_lar - dy;

    int x_left_upp = x_left;
    int y_left_upp = y_upp;

    // fill ret_pix with cumulative sum values
    // case 1: ul is (0,0)
    if (ul.first == 0 && ul.second == 0)
    {
        hueX = sumHueX[x_lar][y_lar];
        hueY = sumHueY[x_lar][y_lar];
        ret_pix.s = sumSat[x_lar][y_lar];
        ret_pix.l = sumLum[x_lar][y_lar];
    }
    // case 2: ul is (0,x)
    else if (ul.first == 0 && ul.second != 0)
    {
        hueX = sumHueX[x_lar][y_lar] - sumHueX[x_upp][y_upp];
        hueY = sumHueY[x_lar][y_lar] - sumHueY[x_upp][y_upp];
        ret_pix.s = sumSat[x_lar][y_lar] - sumSat[x_upp][y_upp];
        ret_pix.l = sumLum[x_lar][y_lar] - sumLum[x_upp][y_upp];
    }
    // case 3: ul is (x,0)
    else if (ul.first != 0 && ul.second == 0)
    {
        hueX = sumHueX[x_lar][y_lar] - sumHueX[x_left][y_left];
        hueY = sumHueY[x_lar][y_lar] - sumHueY[x_left][y_left];
        ret_pix.s = sumSat[x_lar][y_lar] - sumSat[x_left][y_left];
        ret_pix.l = sumLum[x_lar][y_lar] - sumLum[x_left][y_left];
    }
    // case 4: ul is (x,y), x and y > 0
    else
    {
        hueX = sumHueX[x_lar][y_lar] - sumHueX[x_left][y_left] - sumHueX[x_upp][y_upp] + sumHueX[x_left_upp][y_left_upp];
        hueY = sumHueY[x_lar][y_lar] - sumHueY[x_left][y_left] - sumHueY[x_upp][y_upp] + sumHueY[x_left_upp][y_left_upp];
        ret_pix.s = sumSat[x_lar][y_lar] - sumSat[x_left][y_left] - sumSat[x_upp][y_upp] + sumSat[x_left_upp][y_left_upp];
        ret_pix.l = sumLum[x_lar][y_lar] - sumLum[x_left][y_left] - sumLum[x_upp][y_upp] + sumLum[x_left_upp][y_left_upp];
    }

    // calculate averages
    hueX /= num_pix;
    hueY /= num_pix;
    ret_pix.s /= num_pix;
    ret_pix.l /= num_pix;

    // transform cartesian hue values back to polar (degree)
    ret_pix.h = atan2(hueY, hueX) * 180 / PI;
    // cout << "atan2 before adjustment: " << ret_pix.h << endl;
    ret_pix.h += 360.0;
    ret_pix.h = fmod(ret_pix.h, 360.0);
    // cout << "atan2 after adjustment: " << ret_pix.h << endl;

    return ret_pix;
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){

    // get relevant coords of regions
    // equation: largest region - left region - upper region + leftupper region
    int x_lar = lr.first;
    int y_lar = lr.second;

    int x_left = ul.first - 1;
    int y_left = y_lar;

    int x_upp = x_lar;
    int y_upp = ul.second - 1;

    int x_left_upp = x_left;
    int y_left_upp = y_upp;

    // get histogram of desired region
    // case 1: ul is (0,0)
    if (ul.first == 0 && ul.second == 0)
    {
        return hist[x_lar][y_lar];
    }
    // case 2: ul is (0,y)
    else if (ul.first == 0 && ul.second != 0)
    {
        vector<int> ret_vec = hist[x_lar][y_lar];
        vector<int> upp_vec = hist[x_upp][y_upp];
        std::transform(ret_vec.begin(), ret_vec.end(), upp_vec.begin(), ret_vec.begin(), std::minus<int>());
        return ret_vec;
    }
    // case 3: ul is (x,0)
    else if (ul.first != 0 && ul.second == 0)
    {
        vector<int> ret_vec = hist[x_lar][y_lar];
        vector<int> left_vec = hist[x_left][y_left];
        std::transform(ret_vec.begin(), ret_vec.end(), left_vec.begin(), ret_vec.begin(), std::minus<int>());
        return ret_vec;  
    }
    // case 4: ul is (x,y), x and y > 0
    else
    {
        vector<int> ret_vec = hist[x_lar][y_lar];
        vector<int> left_vec = hist[x_left][y_left];
        vector<int> upp_vec = hist[x_upp][y_upp];
        vector<int> left_upp_vec = hist[x_left_upp][y_left_upp];
        std::transform(ret_vec.begin(), ret_vec.end(), left_vec.begin(), ret_vec.begin(), std::minus<int>());
        std::transform(ret_vec.begin(), ret_vec.end(), upp_vec.begin(), ret_vec.begin(), std::minus<int>());
        std::transform(ret_vec.begin(), ret_vec.end(), left_upp_vec.begin(), ret_vec.begin(), std::plus<int>());
        return ret_vec; 
    }
}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn, int area){

    double entropy = 0.;

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;
}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

    int x1 = ul.first;
    int x2 = lr.first;
    int y1 = ul.second;
    int y2 = lr.second;
    int patch_size_x = hist.size();
    int patch_size_y = hist[0].size();

    // case 1: ul <= lr -> 1 region
    if (x1 <= x2 && y1 <= y2)
    {
        vector<int> distn = buildHist(ul, lr);
        int area = rectArea(ul, lr);
        return entropy(distn, area);
    }
    // case 2: ul.x > lr.x but ul.y < lr.y -> 2 horizontal regions
    if (x1 > x2 && y1 <= y2)
    {
        // left part of region
        pair<int, int> ul1(0, y1);
        pair<int, int> lr1 = lr;
        vector<int> distn1 = buildHist(ul1, lr1);
        int area1 = rectArea(ul1, lr1);

        // right part of region
        pair<int, int> ul2 = ul;
        pair<int, int> lr2(patch_size_x - 1, y2);
        vector<int> distn2 = buildHist(ul2, lr2);
        int area2 = rectArea(ul2, lr2);

        // combine the 2 halves
        std::transform(distn1.begin(), distn1.end(), distn2.begin(), distn1.begin(), std::plus<int>());
        return entropy(distn1, area1 + area2);
    }
    // case 3: ul.y > lr.y but ul.x < lr.x -> 2 vertical regions
    if (y1 > y2 && x1 <= x2)
    {
        // upper part of region
        pair<int, int> ul1(x1, 0);
        pair<int, int> lr1 = lr;
        vector<int> distn1 = buildHist(ul1, lr1);
        int area1 = rectArea(ul1, lr1);

        // lower part of region
        pair<int, int> ul2 = ul;
        pair<int, int> lr2(x2, patch_size_y - 1);
        vector<int> distn2 = buildHist(ul2, lr2);
        int area2 = rectArea(ul2, lr2);

        // combine the 2 halves
        std::transform(distn1.begin(), distn1.end(), distn2.begin(), distn1.begin(), std::plus<int>());
        return entropy(distn1, area1 + area2);
    }
    // case 4: both ul.x > lr.x and ul.y > lr.y -> 4 regions
    if (y1 > y2 && x1 > x2)
    {
        // upper-left part of region
        pair<int, int> ul1(0, 0);
        pair<int, int> lr1 = lr;
        vector<int> distn1 = buildHist(ul1, lr1);
        int area1 = rectArea(ul1, lr1);

        // upper-right part of region
        pair<int, int> ul2(x1, 0);
        pair<int, int> lr2(patch_size_x - 1, y2);
        vector<int> distn2 = buildHist(ul2, lr2);
        int area2 = rectArea(ul2, lr2);

        // lower-left part of region
        pair<int, int> ul3(0, y1);
        pair<int, int> lr3(x2, patch_size_y - 1);
        vector<int> distn3 = buildHist(ul3, lr3);
        int area3 = rectArea(ul3, lr3);

        // lower-right part of region
        pair<int, int> ul4 = ul;
        pair<int, int> lr4(patch_size_x - 1, patch_size_y - 1);
        vector<int> distn4 = buildHist(ul4, lr4);
        int area4 = rectArea(ul4, lr4);

        // combine the 4 quarters
        std::transform(distn1.begin(), distn1.end(), distn2.begin(), distn1.begin(), std::plus<int>());
        std::transform(distn1.begin(), distn1.end(), distn3.begin(), distn1.begin(), std::plus<int>());
        std::transform(distn1.begin(), distn1.end(), distn4.begin(), distn1.begin(), std::plus<int>());
        return entropy(distn1, area1 + area2 + area3 + area4);
    }

}
