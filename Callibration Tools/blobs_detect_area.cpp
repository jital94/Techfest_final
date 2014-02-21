/* inclusion of the 'HEADER' files..!! */
/*------------------------------------ */
   //for OpenCV features
   #include <cv.h>
   #include <highgui.h>
   #include "cxcore.h"
   #include <conio.h>
    //for UART
    #include "Tserial.cpp"
    #ifdef __BORLANDC__
    #pragma hdrstop            // borland specific
    #include <condefs.h>
    #pragma argsused
    USEUNIT("Tserial.cpp");
    #endif
   
   //for blobs extraction
   #include "cvblob.h"
   #include "cvblob.cpp"
   #include "cvlabel.cpp"
   using namespace cvb;
   //for string entry 
   #include "string.h"
   #include<time.h>
   #include <Math.h>
/* Some 'color' definitions..!!        */
/*----------------------------------   */
   //Blocks
   #define redhuehigh 12
   #define redhuelow 0
   #define cyanhuehigh 98
   #define cyanhuelow 65
   #define yellowhuehigh 40
   #define yellowhuelow 30
   #define magnetahuehigh 181
   #define magnetahuelow 160
   #define fronthuehigh 102
   #define fronthuelow 70
   #define backhuehigh 14
   #define backhuelow 0
   
   //saturation
    #define redsathigh 210
    #define redsatlow 128
    #define yellowsathigh 250
    #define yellowsatlow 120
    #define cyansathigh 255
    #define cyansatlow 240
    #define magnetasathigh 202
    #define magnetasatlow 98
    #define backsathigh  256
    #define backsatlow  198
    #define frontsathigh 249
    #define frontsatlow 0
    
    #define redvarhigh 255
    #define redvarlow 0
    #define yellowvarhigh 255
    #define yellowvarlow 130
    #define cyanvarhigh 255
    #define cyanvarlow 0
    #define magnetavarhigh 255
    #define magnetavarlow 0
    #define backvarhigh 255
    #define backvarlow 0
    #define frontvarhigh 256
    #define frontvarlow 0
    
    //number of boxes in the puzzle zone
    #define num_boxes 6
    #define depmaxx 609
    #define depmaxy 107
    #define depminx 500
    #define depminy 17
    #define puzzlemaxx 100
    #define puzzlemaxy 96
    #define puzzleminx 6
    #define puzzleminy 8
    #define intermediatex 400
    #define intermediatey 50
    #define boundarymaxx 640
    #define boundarymaxy 480
    #define boundaryminx 0
    #define boundaryminy 0
    #define boundarydist 60


/*------------------------------------ */


/* Declaration of global variables..!! */
/*------------------------------------ */
   //Image frames
   IplImage* input;                    //keeps the captured frames. 
   IplImage* temp_blob;                //contains the blobs of 'temp_dilate'.
   IplImage* red;  
   IplImage* botfilter;  
   IplImage* magenta;
   IplImage* cyan;
   IplImage* yellow;
   IplImage* temp;
   IplImage* liftfilter;
   IplImage* temp_erode;
   IplImage* output;
   
     
   //integer variables
   int k=0;                  //for running the loops.
   int h,s,v,area=0;                    //for storing h,s and v.
   int grabbed=0;
   int depo_time=0;
   float x,y;               //x,y contain centroid;area calculates max cube.
   float angle;                    //stores calculated angle.
   //for UART
   Tserial *com;
   //structure for x and y coordinate
   struct coordinate
   {float x,y;}cube[3],bot_front,bot_back,bot_centre;
   //structure to store properties of a blob
   struct cube
   {
          coordinate centroid;
          char placed;
          char color;
          float distance;
   }puzzle[num_boxes],liftbox,botfront,botback;
   
      
/*-------------PROTOTYPES----------------------- */

IplImage* prepare_filter(char ch);   
int zoneDecider(float x,float y);
float find_Distance(float x1,float y1);
void sort_distance();
void blobs(IplImage* filter,char ch);  
void blobs_arena(IplImage* filter,char ch);  
void blobsBlack(IplImage* filter); 
void get_bot_cordinates();
int calculate(); 
int Send(char head);


/*------------------------------------ */

IplImage* prepare_filter(char ch)   
{//initialise 'temp' and 'output'.
     temp = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,3);
     output = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
     IplImage* filter= cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
      //conver input to 'HSV' and store in temp.
     cvCvtColor(input,temp,CV_BGR2HSV);
      //erode temp into temp_erode.
     temp_erode=cvCreateImage(cvGetSize(temp),8,3);
     cvErode(temp,temp_erode,NULL,1);
         //prepare pointers for 'eroded temp' and 'output'.
     uchar *ptemp  =  (uchar*)temp_erode->imageData;
     uchar *poutput  =  (uchar*)output->imageData;
      //the loop to go thru the image and prepare filter based on a switch's value.
      for(int i=0;i<input->height;i++)
          {for(int j=0;j<input->width;j++)
            {      h=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+0];
                   s=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+1];
                   v=ptemp[i*temp_erode->widthStep + j*temp_erode->nChannels+2];
                               switch (ch)
                               
                               {case 'r': //Place your condtion here for red
                                   {if((h<redhuehigh)&&(h>redhuelow)&& (s<redsathigh)&&(s>redsatlow) &&(h<redvarhigh)&&(h>redvarlow) )
                                     poutput[i*output->widthStep + j]=255;
                                     else poutput[i*output->widthStep + j]=0;}
                                     break;
                                case 'c': //Place your condtion here for blue
                                     {if((h<cyanhuehigh)&&(h>cyanhuelow) && (s>cyansatlow) && (s<cyansathigh)&& (s>cyanvarlow) && (s<cyanvarhigh))
                                     poutput[i*output->widthStep + j]=255;
                                     else poutput[i*output->widthStep +j]=0;}
                                     break;  
                                case 'y': //Place your condtion here for yellow
                                     {if((h<yellowhuehigh)&&(h>yellowhuelow)&&(s>yellowsatlow)&&(s<yellowsathigh)&&(s>yellowvarlow)&&(s<yellowvarhigh))
                                     poutput[i*output->widthStep + j]=255;
                                     else poutput[i*output->widthStep +j]=0;}
                                     break;  
                               case 'm': //Place your condtion here for yellow
                                     {if((h<magnetahuehigh)&&(h>magnetahuelow)&&(s>magnetasatlow)&&(s<magnetavarhigh)&&(s>magnetavarlow)&&(s<magnetavarhigh))
                                     poutput[i*output->widthStep + j]=255;
                                     else poutput[i*output->widthStep +j]=0;}
                                     break;  
                               case 'p': //Place your condtion here for yellow
                                     {if((h<backhuehigh)&&(h>backhuelow)&&(s<backsathigh)&&s >backsatlow &&(s<backvarhigh)&&s>backvarlow)
                                     poutput[i*output->widthStep + j]=255;
                                     else poutput[i*output->widthStep +j]=0;}
                                     break;  
                               case 'o': //Place your condtion here for yellow
                                     {if((h<fronthuehigh)&&(h>fronthuelow)&&(s>frontsatlow&&s<frontsathigh)&&(s>frontvarlow&&s<frontvarhigh))
                                     poutput[i*output->widthStep + j]=255;
                                     else poutput[i*output->widthStep +j]=0;}
                                     break;  
                               }
                   }}
   filter = cvCloneImage( output );
   cvReleaseImage( &output );
   cvReleaseImage( &temp );
   cvReleaseImage( &temp_erode );
   return filter;
}
//to decide the zone of the box
   
//to find the distance between two points
        
//general blobs in arena and bot        
void blobs(IplImage* filter,char ch)  
{
    //temp_blob to store the blobs.
    temp_blob=cvCreateImage(cvGetSize(filter),IPL_DEPTH_LABEL,1);
    //working with the blobs.
    CvBlobs blobs;
    cvLabel(filter, temp_blob, blobs);
    area=0;
    x=0;
    y=0;
    //cvRenderBlobs(temp_blob,blobs,input,input, CV_BLOB_RENDER_BOUNDING_BOX);
        cvFilterByArea(blobs, 100,3000);
        for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
        {
              area= it->second->area;
               x= it->second->centroid.x;
               y= it->second->centroid.y;
               printf("\n Blob # %d : Area= %d , Centroid=(%f,%f) colour (%c)",it->second->label, it->second->area, it->second->centroid.x, it->second->centroid.y,ch);
        }
     
    
    cvReleaseBlobs(blobs);
    cvReleaseImage( &temp_blob );
}
    
int main()
{
    int c,check=1;
    CvCapture *capture=cvCreateCameraCapture(1);//initiate camera 
    //cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH , 1000);
    //cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT , 750);
    
    cvNamedWindow("input",1);
                                  
                             
                             //input=cvQueryFrame(capture);
                            //cvShowImage("input",input);
                            input = cvLoadImage("output1.jpg");
    
                            // Puzzle Detection and Sorting
    //blobsBlack(black);
    {   
    red=prepare_filter('r');
    cvNamedWindow("filterr",1);
    cvShowImage("filterr",red);
    cvReleaseImage(&red);
    botfilter=prepare_filter('p');
    cvNamedWindow("back",1);
    cvShowImage("back",botfilter);
    blobs(botfilter,'p');
    cout<<"\n\n\n";
    cvReleaseImage(&botfilter);
    botfilter=prepare_filter('o');
    cvNamedWindow("front",1);
    cvShowImage("front",botfilter);
    blobs(botfilter,'o');cout<<"\n\n\n";
    cvReleaseImage(&botfilter);
    cvShowImage("input",input);
    red=prepare_filter('r');
    blobs(red,'r');cout<<"\n\n\n";
    yellow=prepare_filter('y');
    cvNamedWindow("filtery",1);
    cvShowImage("filtery",yellow);
    blobs(yellow,'y');
    cout<<"\n\n\n";
    cyan=prepare_filter('c');
    blobs(cyan,'c');
    cout<<"\n\n\n";
    cvNamedWindow("cyan",1);
    cvShowImage("cyan",cyan);
    magenta=prepare_filter('m');
    cvNamedWindow("mag",1);
    cvShowImage("mag",magenta);
    blobs(magenta,'m');
    cout<<"\n\n\n";
    cvReleaseImage(&red);
    cvReleaseImage(&yellow);
    cvReleaseImage(&cyan);
    cvReleaseImage(&magenta);
    }        
            
    cvWaitKey(0);
    cvDestroyAllWindows();
     
    cvReleaseImage(&input);
    cvReleaseCapture(&capture);
                   
    return 0;
} 

