
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 
/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */

	int sub_width = pow(2,k);
	// cout << "center of " << imIn.width() << "x" << imIn.height() << " at (" << center_x << ", " << center_y << ")" << endl; 
	PNG * dyn_img = new PNG(sub_width, sub_width);

	// copy sub-image only for inputs larger than k=1
	if (k <= 1)
	{
		* dyn_img = imIn;
	}
	else
	{
		HSLAPixel * read_ptr;
		HSLAPixel * write_ptr;
		int center_x = imIn.width() / 2;
		int center_y = imIn.height() / 2;

		// (i,j) are coordinates in imIn
		for (int i = center_x - sub_width/2; i < center_x + sub_width/2; ++i)
		{
			for (int j = center_y - sub_width/2; j < center_y + sub_width/2; ++j)
			{
				// (x,y) are coordinates in dyn_img
				int x = i - center_x + sub_width/2;
				int y = j - center_y + sub_width/2;

				// cout << "i: " << i << ", j: " << j << ", x: " << x << ", y: " << y << endl;
				read_ptr = imIn.getPixel(i,j);
				write_ptr = dyn_img->getPixel(x,y);
				* write_ptr = * read_ptr;
			}
		}
	}

	root = buildTree(dyn_img, k);
	delete dyn_img;

	// cout << "toqutree::toqutree success" << endl;
}

int toqutree::size() {

	return size(root);
}

int toqutree::size(Node * const node) {

	// base case: node is a leaf
	if (node->SE == NULL)
	{
		return 1;
	}

	int sum = 0;

	sum += size(node->SE);
	sum += size(node->NE);
	sum += size(node->NW);
	sum += size(node->SW);

	return sum + 1;
}


toqutree::Node * toqutree::buildTree(PNG * im, int k) {
// Note that you will want to practice careful memory use
// In this function. We pass the dynamically allocated image
// via pointer so that it may be released after it is used .
// similarly, at each level of the tree you will want to 
// declare a dynamically allocated stats object, and free it
// once you've used it to choose a split point, and calculate
// an average.

	// cout << "call to buildTree with k = " << k << endl;

	// initialize pointer to current node
	Node * curr_node;

	// base case k=0: single pixels without leaves
	if (k == 0)
	{	
		// center is just (0,0)
		pair<int, int> ctr(0, 0);

		// average corresponds to pixel
		HSLAPixel * avg_ptr = im->getPixel(0, 0);

		// cout << "average hue at k = 0: " <<  avg_ptr->h << endl;

		// dynamically allocate current node
		curr_node = new Node(ctr, k, * avg_ptr);

		// no leaves
		curr_node->NW = NULL;
		curr_node->NE = NULL;
		curr_node->SW = NULL;
		curr_node->SE = NULL;

		// cout << "toqutree::buildTree success for k = 0" << endl;

		return curr_node;
	}

	// dynamically allocate stats object 
	// why dynamically: because of recursive calls to lower degree sub-images
	// memory would accumulate very quickly. deleting the object before the next 
	// recursive call fixes this problem
	stats * curr_stats_ptr = new stats(*im);

	// cout << "stats success" << endl;

	// calculate average (over square region)
	pair<int, int> ul(0, 0);
	int img_width = pow(2, k);
	pair<int, int> lr(img_width - 1, img_width - 1);
	HSLAPixel avg = curr_stats_ptr->getAvg(ul, lr);

	// cout << "average hue at k = " << k << ": " << avg.h << endl;

	// initialize 4 pointers to sub-images
	PNG * NW_im;
	PNG * NE_im;
	PNG * SW_im;
	PNG * SE_im;

	// base case 1: no findSplit() for k = 1
	if (k == 1)
	{
		// center is always SE
		pair<int, int> ctr(1, 1);

		// dynamically allocate current node
		curr_node = new Node(ctr, k, avg);

		// split image into 4 pixels
		NW_im = new PNG(1, 1);
		NE_im = new PNG(1, 1);
		SW_im = new PNG(1, 1);
		SE_im = new PNG(1, 1);

		HSLAPixel * read_ptr;
		HSLAPixel * write_ptr;

		// NW
		read_ptr = im->getPixel(0, 0);
		write_ptr = NW_im->getPixel(0, 0);
		* write_ptr = * read_ptr;

		// NE
		read_ptr = im->getPixel(1, 0);
		write_ptr = NE_im->getPixel(0, 0);
		* write_ptr = * read_ptr;

		// SW
		read_ptr = im->getPixel(0, 1);
		write_ptr = SW_im->getPixel(0, 0);
		* write_ptr = * read_ptr;

		// SE
		read_ptr = im->getPixel(1, 1);
		write_ptr = SE_im->getPixel(0, 0);
		* write_ptr = * read_ptr;
	}
	else 
	{
		// find split point (center)
		vector< pair<int, int> > split_coords = findSplit(im, k - 1, curr_stats_ptr);
		pair<int, int> ctr = split_coords[0];

		// cout << "split_coords at: " << endl;
		// for (int i = 0; i < 4; ++i)
		// {
		// 	pair<int, int> curr_coords = split_coords[i];
		// 	pair<int, int> next_coords = split_coords[i+1];
		// 	cout << "from (" << curr_coords.first << "," << curr_coords.second << ")" << " to (" << next_coords.first << "," << next_coords.second << ")" << endl;
		// }

		// dynamically allocate current node
		curr_node = new Node(ctr, k, avg);

		// split image into 4 sub-images
		SE_im = splitIm(im, k - 1, split_coords[0], split_coords[1]);
		SW_im = splitIm(im, k - 1, split_coords[2], split_coords[3]);
		NE_im = splitIm(im, k - 1, split_coords[4], split_coords[5]);
		NW_im = splitIm(im, k - 1, split_coords[6], split_coords[7]);
	}

	// delete stats object once we don't need it anymore
	delete curr_stats_ptr;

	// call buildTree() on all sub-images with d = k - 1
	curr_node->NW = buildTree(NW_im, k - 1);
	curr_node->NE = buildTree(NE_im, k - 1);
	curr_node->SW = buildTree(SW_im, k - 1);
	curr_node->SE = buildTree(SE_im, k - 1);

	// delete sub images
	delete SE_im;
	delete SW_im;
	delete NE_im;
	delete NW_im;

	// cout << "toqutree::buildTree success for k = " << k << endl;

	// return current node pointer
	return curr_node;

}

PNG * toqutree::splitIm(PNG * im, int k, pair<int, int> ul, pair<int, int> lr) {

	// initialize relevant coordinates of region
	int x1 = ul.first;
    int x2 = lr.first;
    int y1 = ul.second;
    int y2 = lr.second;
    int patch_width = pow(2, k);
    int image_width = im->width();

    // initialize dynamic sub image to return
    PNG * ret_im = new PNG(patch_width, patch_width);
    HSLAPixel * read_ptr;
	HSLAPixel * write_ptr;

    // (i,j) are coordinates in im; (x,y) in ret_im
    int i = x1;
    int x = 0;

    // loop over input image possibly wrapping around edges
    while(i != (x2 + 1) % image_width)
    {
    	int j = y1;
        int y = 0;

    	while(j != (y2 + 1) % image_width)
    	{

    		// cout << "writing (" << i << "," << j << ") to (" << x << "," << y << ")" << endl;

    		read_ptr = im->getPixel(i,j);
			write_ptr = ret_im->getPixel(x,y);
			* write_ptr = * read_ptr;

    		y++;
    		j = (j + 1) % image_width;
    	}

    	x++;
    	i = (i + 1) % image_width;
    }

    return ret_im;
}

vector< pair<int, int> > toqutree::findSplit(PNG * im, int k, stats * curr_stats_ptr) {

	// initialize container to keep track of averaged entropy for each point
	int sear_reg_wdth = pow(2, k);
	vector<double> avg_entrs(sear_reg_wdth*sear_reg_wdth, 0.0);

	// initialize container to save all 8 coord pairs per split candidate
	vector< vector< vector< pair<int, int> > > > split_coord_mat;

	// iterate over middle region of dim 2^k
	int img_width = im->width();
	int center_x = img_width / 2;
	int center_y = center_x;

	// (i,j) are coordinates in im
	for (int i = center_x - sear_reg_wdth/2; i < center_x + sear_reg_wdth/2; ++i)
	{
		// initialize vector to be pushed back on matrix
		vector< vector< pair<int, int> > > split_coord_vec;

		for (int j = center_y - sear_reg_wdth/2; j < center_y + sear_reg_wdth/2; ++j)
		{
			// initialize container for 8 coords
			vector< pair<int, int> > split_coords;

			// find ul and lr for all 4 subregions
			// SE
			pair<int, int> SE_ul(i, j);
			pair<int, int> SE_lr;
			SE_lr.first = (i + sear_reg_wdth - 1) % img_width;
			SE_lr.second = (j + sear_reg_wdth - 1) % img_width;
			split_coords.push_back(SE_ul);
			split_coords.push_back(SE_lr);

			// SW
			pair<int, int> SW_ul(i, j);
			SW_ul.first = (i + sear_reg_wdth) % img_width;
			pair<int, int> SW_lr = SE_lr;
			SW_lr.first = (SE_lr.first + sear_reg_wdth) % img_width;
			split_coords.push_back(SW_ul);
			split_coords.push_back(SW_lr);

			// NE
			pair<int, int> NE_ul(i, j);
			NE_ul.second = (j + sear_reg_wdth) % img_width;
			pair<int, int> NE_lr = SE_lr;
			NE_lr.second = (SE_lr.second + sear_reg_wdth) % img_width;
			split_coords.push_back(NE_ul);
			split_coords.push_back(NE_lr);

			// NW
			pair<int, int> NW_ul = NE_ul;
			NW_ul.first = SW_ul.first;
			pair<int, int> NW_lr = NE_lr;
			NW_lr.first = SW_lr.first;
			split_coords.push_back(NW_ul);
			split_coords.push_back(NW_lr);

			// push 8 coordinates back on vertical vector
			split_coord_vec.push_back(split_coords);

			// calculate entropies of 4 subregions
			double SE_entr = curr_stats_ptr->entropy(SE_ul, SE_lr);
			double SW_entr = curr_stats_ptr->entropy(SW_ul, SW_lr);
			double NE_entr = curr_stats_ptr->entropy(NE_ul, NE_lr);
			double NW_entr = curr_stats_ptr->entropy(NW_ul, NW_lr);

			// (x,y) are coordinates in search region used for indexing
			int x = i - center_x + sear_reg_wdth/2;
			int y = j - center_y + sear_reg_wdth/2;

			// average the 4 entropies and store in container
			double avg_entr = (SE_entr + SW_entr + NE_entr + NW_entr) / 4.0;
			avg_entrs[ x * sear_reg_wdth + y ] = avg_entr;
		}
		// push vertical vector back on horizontal vector (matrix)
		split_coord_mat.push_back(split_coord_vec);
	}

	// return min in container (first occurrence)
	auto min_it = std::min_element(avg_entrs.begin(), avg_entrs.end());
	auto min_idx = std::distance(avg_entrs.begin(), min_it);

	// convert vector index back to (x,y) coordinates
	int min_x = min_idx / sear_reg_wdth;
	int min_y = min_idx % sear_reg_wdth;

	// return vector containing the 8 relevant split coords
	return split_coord_mat[min_x][min_y];
}

PNG toqutree::render(){

	return renderNode(root);
}

PNG toqutree::renderNode(Node * node) {

	// base case: node is a leaf
	if (node->NE == NULL)
	{
		int img_width = pow(2, node->dimension);
		PNG ret_im(img_width, img_width);
		for (int i = 0; i < img_width; ++i)
		{
			for (int j = 0; j < img_width; ++j)
			{
				HSLAPixel * write_ptr = ret_im.getPixel(i, j);
				* write_ptr = node->avg;
			}
		}
		
		return ret_im;
	}

	// get 4 sub-images from children
	PNG NE_im = renderNode(node->NE);
	PNG SE_im = renderNode(node->SE);
	PNG SW_im = renderNode(node->SW);
	PNG NW_im = renderNode(node->NW);


	// combine sub-images to image of current node
	return combine4Ims(&NW_im, &NE_im, &SW_im, &SE_im, node->dimension, node->center);

}

PNG toqutree::combine4Ims(PNG * NW_im, PNG * NE_im, PNG * SW_im, PNG * SE_im, int k, pair<int, int> ctr) {

	int img_width = pow(2, k);
	int sub_img_width = img_width/2;
	PNG ret_im(img_width, img_width);

	// SE
	overlayIm(&ret_im, SE_im, ctr);

	// SW
	pair<int, int> SW_ul;
	SW_ul.first = (ctr.first + sub_img_width) % img_width;
	SW_ul.second = ctr.second;
	overlayIm(&ret_im, SW_im, SW_ul);

	// NE
	pair<int, int> NE_ul;
	NE_ul.first = ctr.first;
	NE_ul.second = (ctr.second + sub_img_width) % img_width;
	overlayIm(&ret_im, NE_im, NE_ul);

	// NW
	pair<int, int> NW_ul;
	NW_ul.first = (ctr.first + sub_img_width) % img_width;
	NW_ul.second = (ctr.second + sub_img_width) % img_width;
	overlayIm(&ret_im, NW_im, NW_ul);

	return ret_im;
}

void toqutree::overlayIm(PNG * im, PNG * const sub_im, pair<int, int> ul) {
	
	HSLAPixel * read_ptr;
	HSLAPixel * write_ptr;
	int x = ul.first;

	for (int i = 0; i < sub_im->width(); ++i)
	{
		int y = ul.second;

		for (int j = 0; j < sub_im->width(); ++j)
		{
			// (i,j) in sub_im; (x,y) in im
			read_ptr = sub_im->getPixel(i, j);
			write_ptr = im->getPixel(x, y);
			* write_ptr = * read_ptr;

			y = (y + 1) % im->width();
		}

		x = (x + 1) % im->width();
	}
}

void toqutree::prune(double tol){

	prune(root,tol);

}

void toqutree::prune(Node * node, double tol) {

	// base case: node is a leaf
	if (node->SE == NULL)
	{
		return;
	}

	// check if averages of subtree are within distance
	if (withinTol(node, node->avg, tol))
	{

		// cout << "node with k = " << node->dimension << " is ready for pruning" << endl;

		// if so, prune it off
		clear(node->SE);
		clear(node->SW);
		clear(node->NE);
		clear(node->NW);
	}
	else 
	{
		// else, recurse on children
		prune(node->SE, tol);
		prune(node->SW, tol);
		prune(node->NE, tol);
		prune(node->NW, tol);
	}
}

bool toqutree::withinTol(Node * node, HSLAPixel avg, double tol) {

	// base case: node is a leaf
	if (node->SE == NULL)
	{
		return true;
	}

	bool isWithinTol = true;
	
	// check children of current node
	isWithinTol &= (tol >= abs(avg.dist(node->SE->avg)));
	// cout << "tol: " << tol << ", dist: " << avg.dist(node->SE->avg) << ", isWithinTol: " << isWithinTol << endl;
	isWithinTol &= (tol >= abs(avg.dist(node->NE->avg)));
	// cout << "tol: " << tol << ", dist: " << avg.dist(node->SE->avg) << ", isWithinTol: " << isWithinTol << endl;
	isWithinTol &= (tol >= abs(avg.dist(node->SW->avg)));
	// cout << "tol: " << tol << ", dist: " << avg.dist(node->SE->avg) << ", isWithinTol: " << isWithinTol << endl;	
	isWithinTol &= (tol >= abs(avg.dist(node->NW->avg)));
	// cout << "tol: " << tol << ", dist: " << avg.dist(node->SE->avg) << ", isWithinTol: " << isWithinTol << endl;

	// recurse on their children if still within tolerance
	if (isWithinTol)
	{
		isWithinTol &= withinTol(node->SE, avg, tol);
		isWithinTol &= withinTol(node->NE, avg, tol);
		isWithinTol &= withinTol(node->SW, avg, tol);
		isWithinTol &= withinTol(node->NW, avg, tol);
	}

	return isWithinTol;
}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){

	// navigate down tree until reaching a leaf
	if (curr->SE != NULL)
	{
		clear(curr->SE);
		clear(curr->SW);
		clear(curr->NE);
		clear(curr->NW);
	}
	
	// free memory of this node
	delete curr;

	// set its pointer to NULL
	curr = NULL;
}

/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {

	// copy current node
	Node * node_copy = new Node(other->center, other->dimension, other->avg);

	// recurse until reaching leaves
	if (other->SE != NULL)
	{
		node_copy->NW = copy(other->NW);
		node_copy->NE = copy(other->NE);
		node_copy->SW = copy(other->SW);
		node_copy->SE = copy(other->SE);
	}
	
	return node_copy;
}

