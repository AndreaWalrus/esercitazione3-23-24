#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"

#define M_PI 3.14159265358979323846

// HW1 #2.1
// Image& im: image to L1-normalize
void l1_normalize(Image &im) {
    float val = 1/(float)(im.w*im.h);
    for(int c=0; c<im.c; c++){
        for(int j=0; j<im.h; j++){
            for(int i=0; i<im.w; i++){
                im.set_pixel(i,j,c,val);
            }
        }
    }
}

// HW1 #2.1
// int w: size of filter
// returns the filter Image of size WxW
Image make_box_filter(int w) {
    assert(w % 2); // w needs to be odd
    Image mask(w,w,1);
    for(int j=0; j<mask.h; j++){
        for(int i=0; i<mask.w; i++){
            mask.set_pixel(i,j,0,1);
        }
    }
    l1_normalize(mask);
    return mask;
}

// HW1 #2.2
// const Image&im: input image
// const Image& filter: filter to convolve with
// bool preserve: whether to preserve number of channels
// returns the convolved image
Image convolve_image(const Image &im, const Image &filter, bool preserve) {
    assert(filter.c == 1);
    Image ret;
    
    if(preserve){
        ret = Image(im.w,im.h,im.c);
        for(int c=0; c<ret.c; c++){
            for(int j=0; j<ret.h; j++){
                for(int i=0; i<ret.w; i++){
                    float q=0;
                    int x_s = i-filter.w/2;
                    int y_s = j-filter.h/2;
                    for(int j_m=0; j_m<filter.h; j_m++){
                        for(int i_m=0; i_m<filter.w; i_m++){
                            q += filter.clamped_pixel(i_m,j_m,0)*im.clamped_pixel(x_s+i_m,y_s+j_m,c);
                        }
                    }
                    ret.set_pixel(i,j,c,q);
                }
            }
        }
    }else{
        ret = Image(im.w,im.h,1);
        for(int j=0; j<ret.h; j++){
            for(int i=0; i<ret.w; i++){
                float q=0;
                int x_s = i-filter.w/2;
                int y_s = j-filter.h/2;
                for(int j_m=0; j_m<filter.h; j_m++){
                    for(int i_m=0; i_m<filter.w; i_m++){
                        q += filter.clamped_pixel(i_m,j_m,0)*im.clamped_pixel(x_s+i_m,y_s+j_m,0);
                        if(im.c>1){
                            q += filter.clamped_pixel(i_m,j_m,0)*im.clamped_pixel(x_s+i_m,y_s+j_m,1);
                        }
                        if(im.c>2){
                            q += filter.clamped_pixel(i_m,j_m,0)*im.clamped_pixel(x_s+i_m,y_s+j_m,2);
                        }
                    }
                }
                ret.set_pixel(i,j,0,q);
            }
        }
    }

    return ret;
}

// HW1 #2.2+ Fast convolution
// const Image&im: input image
// const Image& filter: filter to convolve with
// bool preserve: whether to preserve number of channels
// returns the convolved image
Image convolve_image_fast(const Image &im, const Image &filter, bool preserve) {
    assert(filter.c == 1);
    Image ret;
    // This is the case when we need to use the function clamped_pixel(x,y,c).
    // Otherwise you'll have to manually check whether the filter goes out of bounds

    // TODO: Make sure you set the sizes of ret properly. Use ret=Image(w,h,c) to reset ret
    // TODO: Do the fast convolution operator. Remember to use Eigen for matrix operations
    NOT_IMPLEMENTED();

    // Make sure to return ret and not im. This is just a placeholder
    return im;
}


// HW1 #2.3
// returns basic 3x3 high-pass filter
Image make_highpass_filter() {
    Image mask(3,3,1);
    
    mask.set_pixel(0,0,0,0);
    mask.set_pixel(1,0,0,-1);
    mask.set_pixel(2,0,0,0);
    mask.set_pixel(0,1,0,-1);
    mask.set_pixel(1,1,0,4);
    mask.set_pixel(2,1,0,-1);
    mask.set_pixel(0,2,0,0);
    mask.set_pixel(1,2,0,-1);
    mask.set_pixel(2,2,0,0);

    return mask;
}

// HW1 #2.3
// returns basic 3x3 sharpen filter
Image make_sharpen_filter() {
    Image mask(3,3,1);
    
    mask.set_pixel(0,0,0,0);
    mask.set_pixel(1,0,0,-1);
    mask.set_pixel(2,0,0,0);
    mask.set_pixel(0,1,0,-1);
    mask.set_pixel(1,1,0,5);
    mask.set_pixel(2,1,0,-1);
    mask.set_pixel(0,2,0,0);
    mask.set_pixel(1,2,0,-1);
    mask.set_pixel(2,2,0,0);
    
    return mask;
}

// HW1 #2.3
// returns basic 3x3 emboss filter
Image make_emboss_filter() {
    Image mask(3,3,1);
    
    mask.set_pixel(0,0,0,-2);
    mask.set_pixel(1,0,0,-1);
    mask.set_pixel(2,0,0,0);
    mask.set_pixel(0,1,0,-1);
    mask.set_pixel(1,1,0,1);
    mask.set_pixel(2,1,0,1);
    mask.set_pixel(0,2,0,0);
    mask.set_pixel(1,2,0,1);
    mask.set_pixel(2,2,0,2);
    
    return mask;
}

// HW1 #2.4
// float sigma: sigma for the gaussian filter
// returns basic gaussian filter
Image make_gaussian_filter(float sigma) {
    int size = 6*sigma+1;
    Image mask(size,size,1);
    for(int j=0; j<mask.h; j++){
        for(int i=0; i<mask.w; i++){
            float val = (1/(2*M_PI*sigma*sigma))*exp(-(pow((i-size/2),2)+pow((j-size/2),2))/(2*sigma*sigma)); 
            mask.set_pixel(i,j,0,val);
        }
    }
    return mask;
}


// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their sum
Image add_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size

    Image ret(a.w,a.h,a.c);
    for(int c=0; c<ret.c; c++){
        for(int j=0; j<ret.h; j++){
            for(int i=0; i<ret.w; i++){
                ret.set_pixel(i,j,c,a.clamped_pixel(i,j,c)+b.clamped_pixel(i,j,c));
            }
        }
    }
    return ret;
}

// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their difference res=a-b
Image sub_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size

    Image ret(a.w,a.h,a.c);
    for(int c=0; c<ret.c; c++){
        for(int j=0; j<ret.h; j++){
            for(int i=0; i<ret.w; i++){
                ret.set_pixel(i,j,c,a.clamped_pixel(i,j,c)-b.clamped_pixel(i,j,c));
            }
        }
    }
    return ret;
}

// HW1 #4.1
// returns basic GX filter
Image make_gx_filter() {
   Image mask(3,3,1);
    
    mask.set_pixel(0,0,0,-1);
    mask.set_pixel(1,0,0,0);
    mask.set_pixel(2,0,0,1);
    mask.set_pixel(0,1,0,-2);
    mask.set_pixel(1,1,0,0);
    mask.set_pixel(2,1,0,2);
    mask.set_pixel(0,2,0,-1);
    mask.set_pixel(1,2,0,0);
    mask.set_pixel(2,2,0,1);
    
    return mask;
}

// HW1 #4.1
// returns basic GY filter
Image make_gy_filter() {
    Image mask(3,3,1);
    
    mask.set_pixel(0,0,0,-1);
    mask.set_pixel(1,0,0,-2);
    mask.set_pixel(2,0,0,-1);
    mask.set_pixel(0,1,0,0);
    mask.set_pixel(1,1,0,0);
    mask.set_pixel(2,1,0,0);
    mask.set_pixel(0,2,0,1);
    mask.set_pixel(1,2,0,2);
    mask.set_pixel(2,2,0,1);
    
    return mask;
}

// HW1 #4.2
// Image& im: input image
void feature_normalize(Image &im) {
    assert(im.w * im.h); // assure we have non-empty image

    float max=0;
    float min=1;
    for(int c=0; c<im.c; c++){
        for(int j=0; j<im.h; j++){
            for(int i=0; i<im.w; i++){
                float val = im.clamped_pixel(i,j,c);
                if(val>max) max=val;
                if(val<min) min=val;
            }
        }
    }
    for(int c=0; c<im.c; c++){
        for(int j=0; j<im.h; j++){
            for(int i=0; i<im.w; i++){
                if(max==min) im.set_pixel(i,j,c,0);
                else{
                    float val = (im.clamped_pixel(i,j,c)-min)/(max-min);
                    im.set_pixel(i,j,c,val);
                }
            }
        }
    }
}


// Normalizes features across all channels
void feature_normalize_total(Image &im) {
    assert(im.w * im.h * im.c); // assure we have non-empty image

    int nc = im.c;
    im.c = 1;
    im.w *= nc;

    feature_normalize(im);

    im.w /= nc;
    im.c = nc;

}


// HW1 #4.3
// Image& im: input image
// return a pair of images of the same size
pair<Image, Image> sobel_image(const Image &im) {
    Image mask_x = make_gx_filter();
    Image mask_y = make_gy_filter();
    Image G_x = convolve_image(im,mask_x,false);
    Image G_y = convolve_image(im,mask_y,false);

    Image mag(im.w,im.h,1);
    Image theta(im.w,im.h,1);

    for(int j=0; j<im.h; j++){
        for(int i=0; i<im.w; i++){
            float x_val = G_x.clamped_pixel(i,j,0);
            float y_val = G_y.clamped_pixel(i,j,0);
            float mag_val = sqrt(x_val*x_val+y_val*y_val);
            float theta_val = atan2(y_val,x_val);
            mag.set_pixel(i,j,0,mag_val);
            theta.set_pixel(i,j,0,theta_val);
        }
    }
    

    return {mag, theta};
}


// HW1 #4.4
// const Image& im: input image
// returns the colorized Sobel image of the same size
Image colorize_sobel(const Image &im) {
    Image mask = make_gaussian_filter(4);
    Image res = convolve_image(im,mask,true);

    pair<Image,Image> ret = sobel_image(res);
    Image mag = ret.first;
    Image theta = ret.second;
    
    feature_normalize(mag);

    for(int j=0; j<theta.h; j++){
        for(int i=0; i<theta.w; i++){
            float val = (theta.clamped_pixel(i,j,0)/(2*M_PI))+0.5;
            theta.set_pixel(i,j,0,val);
        }
    }

    Image color(im.w,im.h,im.c);

    for(int j=0; j<im.h; j++){
        for(int i=0; i<im.w; i++){
            float mag_val = mag.clamped_pixel(i,j,0);
            float theta_val = theta.clamped_pixel(i,j,0);
            color.set_pixel(i,j,0,theta_val);
            color.set_pixel(i,j,1,mag_val);
            color.set_pixel(i,j,2,mag_val);
        }
    }

    hsv_to_rgb(color);

    return color;
}


// HW1 #4.5
// const Image& im: input image
// float sigma1,sigma2: the two sigmas for bilateral filter
// returns the result of applying bilateral filtering to im
Image bilateral_filter(const Image &im, float sigma1, float sigma2) {
    int size = 6*sigma1+1;

    return Image();
}

// HW1 #4.5+ Fast bilateral filter
// const Image& im: input image
// float sigma1,sigma2: the two sigmas for bilateral filter
// returns the result of applying bilateral filtering to im
Image bilateral_filter_fast(const Image &im, float sigma1, float sigma2) {
    Image bf = im;

    // TODO: Your fast bilateral code
    NOT_IMPLEMENTED();

    return bf;
}

// HM #5
//
float *compute_histogram(const Image &im, int ch, int num_bins) {
    float *hist = (float *) malloc(sizeof(float) * num_bins);
    for (int i = 0; i < num_bins; ++i) {
        hist[i] = 0;
    }

    for(int j=0; j<im.h; j++){
        for(int i=0; i<im.w; i++){
            int index = im.clamped_pixel(i,j,ch)*num_bins-1;
            hist[index]=hist[index]+1;
        }
    }

    for(int i=0; i<num_bins; i++){
        int size = im.w*im.h;
        hist[i]=hist[i]/size;
    }
    
    return hist;
}

float *compute_CDF(float *hist, int num_bins) {
    float *cdf = (float *) malloc(sizeof(float) * num_bins);

    cdf[0] = hist[0];

    for(int i=1; i<num_bins; i++){
        cdf[i]=cdf[i-1]+hist[i];
    }

    return cdf;
}

Image histogram_equalization_hsv(const Image &im, int num_bins) {
    Image new_im(im);
    rgb_to_hsv(new_im);

    float* cdf = compute_CDF(compute_histogram(new_im, 2, num_bins), num_bins);
    for(int j=0; j<im.h; j++){
        for(int i=0; i<im.w; i++){
            int index = new_im.clamped_pixel(i,j,2)*num_bins-1;
            new_im.set_pixel(i,j,2,cdf[index]);
        }
    }
    delete(cdf);

    hsv_to_rgb(new_im);
    return new_im;
}

Image histogram_equalization_rgb(const Image &im, int num_bins) {
    Image new_im(im);

    for (int c = 0; c < im.c; ++c) {
        float* cdf = compute_CDF(compute_histogram(im, c, num_bins), num_bins);
        for(int j=0; j<im.h; j++){
            for(int i=0; i<im.w; i++){
                int index = im.clamped_pixel(i,j,c)*num_bins-1;
                new_im.set_pixel(i,j,c,cdf[index]);
            }
        }
        delete(cdf);
    }
    
    return new_im;
}


// HELPER MEMBER FXNS

void Image::feature_normalize(void) { ::feature_normalize(*this); }

void Image::feature_normalize_total(void) { ::feature_normalize_total(*this); }

void Image::l1_normalize(void) { ::l1_normalize(*this); }

Image operator-(const Image &a, const Image &b) { return sub_image(a, b); }

Image operator+(const Image &a, const Image &b) { return add_image(a, b); }
