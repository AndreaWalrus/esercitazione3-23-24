#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"

#define M_PI 3.14159265358979323846


/*
Smooths a grayscale image by convolving it with a Gaussian kernel of standard deviation sigma.
Input:
    Image im: the input image
    float sigma: the standard deviation of the Gaussian kernel
Output:
    Image: the smoothed image (im.w, im.h, 1)
*/
Image smooth_image(const Image& im, float sigma)
{
    Image mask = make_gaussian_filter(sigma);
    Image ret = convolve_image(im, mask, true);
    return ret;
}


/*
Computes the magnitude and direction of the gradient of an image.
Input:
    Image im: the input image
Output:
    pair<Image,Image>: the magnitude and direction of the gradient of the image
                       with magnitude in [0,1] and direction in [-pi,pi]
*/
pair<Image,Image> compute_gradient(const Image& im)
{
    pair<Image,Image> sobel = sobel_image(im);

    feature_normalize(sobel.first);

    //Normalize direction in [-pi,pi]
    float max=-4;
    float min=4;

    for(int j=0; j<sobel.second.h; j++){
        for(int i=0; i<sobel.second.w; i++){
            float val = sobel.second.clamped_pixel(i,j,0);
            if(val>max) max=val;
            if(val<min) min=val;
        }
    }


    for(int j=0; j<sobel.second.h; j++){
        for(int i=0; i<sobel.second.w; i++){
            if(max==min) sobel.second.set_pixel(i,j,0,0);
            else{
                float val = ((sobel.second.clamped_pixel(i,j,0)-min)*2*M_PI)/(max-min)-M_PI;
                sobel.second.set_pixel(i,j,0,val);
            }
        }
    }


    return sobel;
}


/*
Performs non-maximum suppression on an image.
Input:
    Image mag: the magnitude of the gradient of the image [0,1]
    Image dir: the direction of the gradient of the image [-pi,pi]
Output:
    Image: the image after non-maximum suppression
*/
Image non_maximum_suppression(const Image& mag, const Image& dir)
{
    Image nms(mag.w, mag.h, 1);
    float neighbor1, neighbor2, val;

    // Iterate through the image and perform non-maximum suppression
    for (int y = 0; y < mag.h; y++) {
        for (int x = 0; x < mag.w; x++) {
            
            int theta = round(dir.clamped_pixel(x,y,0)/(M_PI/4));

            if(theta==1 || theta==-3){ //i+1,j+1/i-1,j-1
                val = mag.clamped_pixel(x,y,0);
                neighbor1 = mag.clamped_pixel(x+1,y+1,0);
                neighbor2 = mag.clamped_pixel(x-1,y-1,0);
            }else if(theta==2 || theta==-2){ //i,j+1/i,j-1
                val = mag.clamped_pixel(x,y,0);
                neighbor1 = mag.clamped_pixel(x,y+1,0);
                neighbor2 = mag.clamped_pixel(x,y-1,0);
            }else if(theta==3 || theta==-1){ //i-1,j+1/i+1,j-1
                val = mag.clamped_pixel(x,y,0);
                neighbor1 = mag.clamped_pixel(x-1,y+1,0);
                neighbor2 = mag.clamped_pixel(x+1,y-1,0);
            }else{ //i-1,j/i+1,j
                val = mag.clamped_pixel(x,y,0);
                neighbor1 = mag.clamped_pixel(x-1,y,0);
                neighbor2 = mag.clamped_pixel(x+1,y,0);
            }

            if(neighbor1>val || neighbor2>val){
                nms.set_pixel(x,y,0,0);
            }else{
                nms.set_pixel(x,y,0,val);
            }
        }
    }

    return nms;
}



/*
    Applies double thresholding to an image.
    Input:
        Image im: the input image
        float lowThreshold: the low threshold value
        float highThreshold: the high threshold value
        float strongVal: the value to use for strong edges
        float weakVal: the value to use for weak edges
    Output:
        Image: the thresholded image
*/
Image double_thresholding(const Image& im, float lowThreshold, float highThreshold, float strongVal, float weakVal)
{
    Image res(im.w, im.h, im.c);

    for(int c=0; c<im.c; c++){
        for(int j=0; j<im.h; j++){
            for(int i=0; i<im.w; i++){
                float val = im.clamped_pixel(i,j,c);
                if(val>highThreshold){
                    res.set_pixel(i,j,c,strongVal);
                }else if(val<lowThreshold){
                    res.set_pixel(i,j,c,0);
                }else{
                    res.set_pixel(i,j,c,weakVal);
                }
            }
        }
    }

    return res;
}


/*
    Applies hysteresis thresholding to an image.
    Input:
        Image im: the input image
        float weak: the value of the weak edges
        float strong: the value of the strong edges
    Output:
        Image: the image after hysteresis thresholding, with only strong edges
*/
Image edge_tracking(const Image& im, float weak, float strong)
{
    Image res(im.w, im.h, im.c);

    for(int c=0; c<im.c; c++){
        for (int y=0; y < im.h; ++y) {
            for (int x=0; x < im.w; ++x) {
                if(im.clamped_pixel(x,y,c)==weak){
                    bool check1 = im.clamped_pixel(x-1,y-1,c)==strong || im.clamped_pixel(x,y-1,c)==strong || im.clamped_pixel(x+1,y-1,c)==strong;
                    bool check2 = im.clamped_pixel(x-1,y,c)==strong || im.clamped_pixel(x+1,y,c)==strong;
                    bool check3 = im.clamped_pixel(x-1,y+1,c)==strong || im.clamped_pixel(x,y+1,c)==strong || im.clamped_pixel(x+1,y+1,c)==strong;
                    if(check1 || check2 || check3){
                        res.set_pixel(x,y,c,strong);
                    }else{
                        res.set_pixel(x,y,c,0);
                    }
                }else{
                    res.set_pixel(x,y,c,im.clamped_pixel(x,y,c));
                }
            }
        }
    }
    return res;

}
