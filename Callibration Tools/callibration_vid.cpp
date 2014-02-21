#include"highgui.h"
#include"cv.h"

int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
//names that will appear at the top of each window
char trackbarWindowName[15] = "Trackbars";
IplImage* input;                    //keeps the captured frames. 
   IplImage* temp_blob;                //contains the blobs of 'temp_dilate'.
   IplImage* red;  
   IplImage* botfilter;  
   IplImage* black;
   IplImage* cyan;
   IplImage* filter;
   IplImage* temp;
   IplImage* liftfilter;
   IplImage* temp_erode;
   IplImage* output;
   int h,s,v;
void on_trackbar( int )
{//This function gets called whenever a
	// trackbar position is changed
    cvReleaseImage( &filter );
   
    temp = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,3);
     output = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
     filter= cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
      //conver input to 'HSV' and store in temp.
     cvCvtColor(input,temp,CV_BGR2HSV);
      //erode temp into temp_erode.
     temp_erode=cvCreateImage(cvGetSize(temp),8,3);
     cvErode(temp,temp_erode,NULL,1);
         //prepare pointers for 'eroded temp' and 'output'.
     uchar *ptemp  =  (uchar*)temp_erode->imageData;
     uchar *poutput  =  (uchar*)output->imageData;
     
     for(int i=0;i<input->height;i++)
     {
          for(int j=0;j<input->width;j++)
            {      h=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+0];
                   s=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+1];
                   v=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+2];
                   if((h<H_MAX)&&(h>H_MIN)&& (s>S_MIN) && (s<S_MAX) && (v>V_MIN) &&(v<V_MAX) )
                                     poutput[i*output->widthStep + j]=255;
                   else poutput[i*output->widthStep + j]=0;
            }
     }
            
     filter = cvCloneImage( output );
   cvReleaseImage( &output );
   cvReleaseImage( &temp );
   cvReleaseImage( &temp_erode );
   cvShowImage("img1",filter);
                     

}
void on_trackbar_vid()
{//This function gets called whenever a
	// trackbar position is changed
    cvReleaseImage( &filter );
   
    temp = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,3);
     output = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
     filter= cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
      //conver input to 'HSV' and store in temp.
     cvCvtColor(input,temp,CV_BGR2HSV);
      //erode temp into temp_erode.
     temp_erode=cvCreateImage(cvGetSize(temp),8,3);
     cvErode(temp,temp_erode,NULL,1);
         //prepare pointers for 'eroded temp' and 'output'.
     uchar *ptemp  =  (uchar*)temp_erode->imageData;
     uchar *poutput  =  (uchar*)output->imageData;
     
     for(int i=0;i<input->height;i++)
     {
          for(int j=0;j<input->width;j++)
            {      h=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+0];
                   s=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+1];
                   v=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+2];
                   if((h<H_MAX)&&(h>H_MIN)&& (s>S_MIN) && (s<S_MAX) && (v>V_MIN) &&(s<V_MAX) )
                                     poutput[i*output->widthStep + j]=255;
                   else poutput[i*output->widthStep + j]=0;
            }
     }
            
     filter = cvCloneImage( output );
   cvReleaseImage( &output );
   cvReleaseImage( &temp );
   cvReleaseImage( &temp_erode );
   cvShowImage("img1",filter);
                     

}

void createTrackbars()
{
	//create window for trackbars


    cvNamedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
    cvCreateTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
    cvCreateTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
    cvCreateTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
    cvCreateTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
    cvCreateTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
    cvCreateTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );


}

int main()
{
    CvCapture *capture=cvCreateCameraCapture(1);//initiate camera 
    for(long int i=0;i<1000000000&& capture!=NULL ;i++);//starting camera takes some time and we can take any frame only after camera starts or frame value will be  black
    
   cvNamedWindow("img1");
    int c;
    createTrackbars();
                     
    
    if(capture!=NULL)//camera has begun starting itself
    {
                     //input=cvLoadImage("picture014e.jpg");
                     while(1)
                     {input=cvQueryFrame(capture);//take current image in camera and give it to frame pointer
                     cvNamedWindow("img");
                     cvShowImage("img",input);
                     on_trackbar_vid();
                     c=cvWaitKey(5);
                     if(c>0&&c<225)
                     {
                      cvDestroyAllWindows();
                      cvReleaseImage( &input ); 
                      break;
                     }//cvReleaseCapture(&capture);
                     }
    }

return 0;
    
}
