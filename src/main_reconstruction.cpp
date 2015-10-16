#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "patch_match_provider/RandomizedPatchMatch.h"
#include "TrivialReconstruction.h"
#include "VotedReconstruction.h"
#include "util.h"

#include <iostream>

using cv::addWeighted;
using cv::Mat;
using cv::Size;
using cv::namedWindow;
using cv::imread;
using cv::Rect;
using cv::Scalar;
using cv::String;
using cv::split;
using pmutil::convert_for_computation;
using pmutil::ssd;
using std::cout;
using std::endl;

const float RESIZE_FACTOR = 0.5;
const int PATCH_SIZE = 7;

/**
 * Tries to reconstruct the second image with patches from the first images.
 */
int main( int argc, char** argv )
{
    /// Load image and template
    Mat source = imread( argv[1], 1 );
    Mat target = imread( argv[2], 1);

    if (!source.data || !target.data) {
        printf("Need two pictures as arguments.\n");
        return -1;
    }
    // For later comparison.
    Mat original;
    resize(target, original, Size(), RESIZE_FACTOR, RESIZE_FACTOR);
    original.convertTo(original, CV_32FC3, 1 / 255.f);

    // For fast testing, make it tiny
    convert_for_computation(source, RESIZE_FACTOR);
    convert_for_computation(target, RESIZE_FACTOR);

    RandomizedPatchMatch rpm(source, target, PATCH_SIZE);

    Mat minDistImg = rpm.match();

    TrivialReconstruction tr(minDistImg, source, PATCH_SIZE);
    Mat reconstructed = tr.reconstruct();
    cvtColor(reconstructed, reconstructed, CV_Lab2BGR);
    imwrite("reconstructed.exr", reconstructed);

    VotedReconstruction vr(minDistImg, source, PATCH_SIZE);
    Mat reconstructed2 = vr.reconstruct();
    cvtColor(reconstructed2, reconstructed2, CV_Lab2BGR);
    imwrite("reconstructed_voted.exr", reconstructed2);

    cout << "SSD trivial reconstruction: " << ssd(reconstructed, original) << endl;
    cout << "SSD voted reconstruction: " << ssd(reconstructed2, original) << endl;
    return 0;
}