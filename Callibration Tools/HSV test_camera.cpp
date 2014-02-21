#include "cv.h"
#include "highgui.h"
#include <stdio.h>

IplImage* temp;


void my_mouse_callback( int event, int x, int y, int flags, void* param )
{
   uchar *ptemp  =  (uchar*)temp->imageData;
printf("\nx=%d(%d)\t y=%d(%d)\t\th=%d \ts=%d \tv=%d\n",x,temp->width,y,temp->height,ptemp[y*temp->widthStep + x*temp->nChannels+0],ptemp[y*temp->widthStep + x*temp->nChannels+1],ptemp[y*temp->widthStep + x*temp->nChannels+2]);
} 


int main()
{
   int i,j,h,s,v;
   IplImage* input;
   IplImage* output;
   CvCapture *capture=cvCreateCameraCapture(1);//initiate camera 
   for(long int i=0;i<1000000000&& capture!=NULL ;i++);//starting camera takes some time and we can take any frame only after camera starts or frame value will be  black
        input=cvQueryFrame(capture);//take current image in camera and give it to frame pointer
    
   //input=cvLoadImage("Picture 13.jpg",1); 
   
    temp=cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,3);
    output=cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
    
    cvCvtColor(input,temp,CV_BGR2HSV);
    uchar *ptemp  =  (uchar*)temp->imageData;
    uchar *poutput  =  (uchar*)output->imageData;
    
     for(i=0;i<input->height;i++)
            for(j=0;j<input->width;j++)
            {
                                       h=ptemp[i*temp->widthStep + j*temp->nChannels+0];
                                       s=ptemp[i*temp->widthStep + j*temp->nChannels+1];
                                       v=ptemp[i*temp->widthStep + j*temp->nChannels+2];
                                       
                                       if(s>150&&h<10)//Place your condtion here
                                                                         poutput[i*output->widthStep + j]=255;
                                       else
                                                                          poutput[i*output->widthStep + j]=0;
            }
            
    cvNamedWindow("input",1);
    //cvNamedWindow("output",1);
    
    cvShowImage("input",input);
    //cvSaveImage("Picture 13.jpg",input);
    
    //cvShowImage("output",output);
    cvSetMouseCallback("input", my_mouse_callback, NULL);
    
    
    cvWaitKey(0); 
    
    cvDestroyWindow( "output" );
    cvReleaseImage( &output ); 
    cvReleaseImage( &input );  
    cvReleaseImage( &temp );  
    return 0;
}
