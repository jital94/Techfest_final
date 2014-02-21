/* inclusion of the 'HEADER' files..!! */
/*------------------------------------ */
   //for OpenCV features
   #include <cv.h>
   #include <highgui.h>
   #include "cxcore.h"
   #include <conio.h>
    //for UART
    #include "Header_Files/Tserial.cpp"
    #ifdef __BORLANDC__
    #pragma hdrstop            // borland specific
    #include <condefs.h>
    #pragma argsused
    USEUNIT("Tserial.cpp");
    #endif
   
   //for blobs extraction
   #include "Header_Files/cvblob.h"
   #include "Header_Files/cvblob.cpp"
   #include "Header_Files/cvlabel.cpp"
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
int zoneDecider(float x,float y)
{
        if( x <puzzlemaxx && x>puzzleminx && y<puzzlemaxy && y>puzzleminy)
             return 1;                                                                //in the puzzle zone
        else if(x<depmaxx && x>depminx && y<depmaxy && y>depminy)
             return 0;                                                                //in the deposit zone
        else
             return -1;                                                               //in the arena
}
   
//to find the distance between two points
float find_Distance(float x1,float y1)
{
          float d=sqrt((x1-puzzlemaxx)*(x1-puzzlemaxx)+(y1-puzzleminy)*(y1-puzzleminy));
          return d;
}
float find_Distance_pit(float x1,float y1)
{
          float d=sqrt((x1-depmaxx)*(x1-depmaxx)+(y1-depminy)*(y1-depminy));
          return d;
}

//sorting the two distances
void sort_distance()
{
        int pos=0;
        struct cube temp;
        for(int f=0;f<num_boxes;f++ )
                puzzle[f].distance=find_Distance(puzzle[f].centroid.x,puzzle[f].centroid.y);
        for(int i=0;i<num_boxes-1;i++)
        {
                pos=i;
                for(int j=i+1;j<num_boxes;j++)
                {
                        if(puzzle[pos].centroid.x<puzzle[j].centroid.x)
                             pos=j;
                                  
                             
                }
                temp=puzzle[pos];
                puzzle[pos]=puzzle[i];
                puzzle[i]=temp;
        }
        for(int i=0;i<num_boxes-1;i++)
        {
                pos=i;
                for(int j=i+1;j<num_boxes;j++)
                {
                        if(puzzle[pos].centroid.x<puzzle[j].centroid.x)
                             pos=j;
                                  
                             
                }
                temp=puzzle[pos];
                puzzle[pos]=puzzle[i];
                puzzle[i]=temp;
        }

}        
        
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
    if(ch=='p' ||ch== 'o')
    {
        cvFilterByArea(blobs, 400,3000);
        for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
        {
              area= it->second->area;
               x= it->second->centroid.x;
               y= it->second->centroid.y;
               //printf("\n Blob # %d : Area= %d , Centroid=(%f,%f) (x,y):(%d,%d)",it->second->label, it->second->area, it->second->centroid.x, it->second->centroid.y,x,y);
        }
     }
    
    else
    {
        cvFilterByArea(blobs, 100,700);
        for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
        {
              area= it->second->area;
               x= it->second->centroid.x;
               y= it->second->centroid.y;
               if(zoneDecider(it->second->centroid.x,it->second->centroid.y)==1)
               {                 
                    puzzle[k].centroid.x=x;
                    puzzle[k].centroid.y=y;
                    puzzle[k].color=ch;
                    k++;
                printf("\n Blob # %d : Area= %d , Centroid=(%f,%f) \n",it->second->label, it->second->area, it->second->centroid.x, it->second->centroid.y);
                }
        }
     }
    cvReleaseBlobs(blobs);
    cvReleaseImage( &temp_blob );
}
    
void blobs_arena(IplImage* filter,char ch)  
{
    //temp_blob to store the blobs.
    temp_blob=cvCreateImage(cvGetSize(filter),IPL_DEPTH_LABEL,1);
    //working with the blobs.
    CvBlobs blobs;
    cvLabel(filter, temp_blob, blobs);
    area=0;
    x=0;
    y=0;
    liftbox.centroid.x=10000;
    liftbox.centroid.y=10000;
    if(ch=='y')            
    cvFilterByArea(blobs, 250,650);
    else
    cvFilterByArea(blobs, 200,500);
    //cvRenderBlobs(temp_blob,blobs,input,input, CV_BLOB_RENDER_BOUNDING_BOX);
    for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
    {
           area= it->second->area;
           x= it->second->centroid.x;
           y= it->second->centroid.y;
           printf("\n arenaBlob # %d : Area= %d , Centroid=(%f,%f) \n",it->second->label, it->second->area, it->second->centroid.x, it->second->centroid.y);
           if( zoneDecider(it->second->centroid.x,it->second->centroid.y)==-1)
               if(find_Distance_pit( liftbox.centroid.x,liftbox.centroid.y) >  find_Distance_pit(x,y))
               {                 
                                 liftbox.centroid.x=x;
                                 liftbox.centroid.y=y;
                                 liftbox.color=ch;
            //cout<<"\ncentroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
               }
     }
    cvReleaseBlobs(blobs);
    cvReleaseImage( &temp_blob );
}

//black blobs
/*
void blobsBlack(IplImage* filter)  
{
     //temp_blob to store the blobs.
    temp_blob= cvCreateImage(cvGetSize(filter),IPL_DEPTH_LABEL,1);
    //working with the blobs.
    CvBlobs blobs;
    cvLabel(filter, temp_blob, blobs);
    area=0;
    x=0.0;
    y=0.0;
    cvFilterByArea(blobs, 500,10000);
    cvRenderBlobs(temp_blob,blobs,input,input, CV_BLOB_RENDER_BOUNDING_BOX);
    for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
    {
       area= it->second->area;
       x= it->second->centroid.x;
       y= it->second->centroid.y;
       //printf("\n Blob # %d : Area= %d , Centroid=(%f,%f) (x,y):(%d,%d)",it->second->label, it->second->area, it->second->centroid.x, it->second->centroid.y,x,y);
       if(x>320 && y<240)
       {
             puzzleBlob=*(it->second);
             printf("\n puzllBlob # %d : Area= %d , Centroid=(%f,%f) (x,y):(%d,%d)",it->second->label, it->second->area, it->second->centroid.x, it->second->centroid.y,x,y);
       }
       if(x<320 && y<240)
       {
                depBlob=*(it->second);
                printf("\n depBlob # %d : Area= %d , Centroid=(%f,%f) (x,y):(%d,%d)",it->second->label, it->second->area, it->second->centroid.x, it->second->centroid.y,x,y);
       }
                
     } 
    cvReleaseBlobs(blobs);
    cvReleaseImage( &temp_blob );
   
} 
*/

//Update bot coordinates
void get_bot_cordinates()
{
            cvReleaseImage(&botfilter);
            botfilter=prepare_filter(botfront.color);
            blobs(botfilter,botfront.color);
            botfront.centroid.x=x;
            botfront.centroid.y=y;
            cout<<"\nBot Front centroid.x "<<botfront.centroid.x<<"\t centroid.y "<<botfront.centroid.y<<"\tcolor "<<botfront.color;
            
            cvReleaseImage(&botfilter);
            botfilter=prepare_filter(botback.color);
            blobs(botfilter,botback.color);
            botback.centroid.x=x;
            botback.centroid.y=y;
            cout<<"\nBotBack: centroid.x "<<botback.centroid.x<<"\t centroid.y "<<botback.centroid.y<<"\tcolor "<<botback.color;
}

//Calculate angle
int calculate() 
{
            float distance1,distance2;
            float centrex,centrey,x1,y1,x2,y2,d1,d2;
            float sine;
            centrex=(botfront.centroid.x + botback.centroid.x)/2;
            centrey=(botfront.centroid.y + botback.centroid.y)/2;
            x1=liftbox.centroid.x-centrex;
            y1=liftbox.centroid.y-centrey;
            x2=botfront.centroid.x-centrex;
            y2=botfront.centroid.y-centrey;
            distance1=sqrt(x1*x1 + y1*y1);
            distance2=sqrt(x2*x2 + y2*y2);
            sine=(-x2*y1 + x1*y2)/(distance1*distance2);    // positive means rotate left, else right
            angle=((asin(sine)*180)/(3.142));
            
            d1=sqrt((liftbox.centroid.x-botfront.centroid.x)*(liftbox.centroid.x-botfront.centroid.x) + (liftbox.centroid.y-botfront.centroid.y)*(liftbox.centroid.y-botfront.centroid.y));
            d2=sqrt((liftbox.centroid.x-botback.centroid.x)*(liftbox.centroid.x-botback.centroid.x) + (liftbox.centroid.y-botback.centroid.y)*(liftbox.centroid.y-botback.centroid.y));
            
            if(d2<d1 && angle>0)
                angle=180-angle;
            else if(d2<d1 && angle<0)
                angle=-180-angle;
            printf("\n calculated angle: %f",angle);
            printf("\n calculated distance:   %f",distance1);
            
            if(grabbed)
            {
                       if(abs(angle)<10 && distance1<45)
                       {               
                            Send('s');
                            return 0; 
                       }
                       else return 1 ;  
            }
            else 
            {
                       if(abs(angle)<10 && distance1<90)
                       {               
                            Send('s');
                            return 0; 
                       }
                       else return 1 ;  
            }                                           
            
            
}
void check_grab()
{
            float distance1,distance2;
            float centrex,centrey,x1,y1,x2,y2,d1,d2;
            float sine;
            centrex=(botfront.centroid.x + botback.centroid.x)/2;
            centrey=(botfront.centroid.y + botback.centroid.y)/2;
            x1=liftbox.centroid.x-centrex;
            y1=liftbox.centroid.y-centrey;
            x2=botfront.centroid.x-centrex;
            y2=botfront.centroid.y-centrey;
            distance1=sqrt(x1*x1 + y1*y1);
            distance2=sqrt(x2*x2 + y2*y2);
            sine=(-x2*y1 + x1*y2)/(distance1*distance2);    // positive means rotate left, else right
            angle=((asin(sine)*180)/(3.142));
            
            d1=sqrt((liftbox.centroid.x-botfront.centroid.x)*(liftbox.centroid.x-botfront.centroid.x) + (liftbox.centroid.y-botfront.centroid.y)*(liftbox.centroid.y-botfront.centroid.y));
            d2=sqrt((liftbox.centroid.x-botback.centroid.x)*(liftbox.centroid.x-botback.centroid.x) + (liftbox.centroid.y-botback.centroid.y)*(liftbox.centroid.y-botback.centroid.y));
            
            if(d2<d1 && angle>0)
                angle=180-angle;
            else if(d2<d1 && angle<0)
                angle=-180-angle;
            printf("\n calculated angle: %f",angle);
            printf("\n calculated distance:   %f",distance1);
            
                       if(distance1<50 && abs(angle)<20)
                       {               
                            grabbed=1;
                       }
                       else grabbed=0 ;
                       cout<<"\n Grabbed = "<<grabbed;



}  
                                                       
     
// Send Character
int Send(char head)
{
    cout<<"\n"<<head<<'\t';
    com = new Tserial();
    com->connect("\\\\.\\COM4", 9600, spNONE);
    com->sendChar(head);
    com->disconnect();
    return 0;

}

void orient()
{
     if (abs(angle)<13)
        if(depo_time)
                   Send('f'); //Move forward
         else
                   Send('F'); //Move forward
                        
     else
        if(angle<0)
                       //if(abs(angle)>30)
                                        Send('l');//fast Right Rotate
                       
            else 
                       //if(abs(angle)>30)
                                        Send('r');//Fast Left Rotate
                       
}
int check_boundary()
{
                if( liftbox.centroid.x < boundarymaxx && liftbox.centroid.x > (boundarymaxx-boundarydist) )
                {
                    liftbox.centroid.x= (boundarymaxx - boundaryminx)/2;
                    liftbox.centroid.y=  liftbox.centroid.y;
                    return 1;
                }
                else if( liftbox.centroid.x > boundaryminx && liftbox.centroid.x < (boundaryminx+boundarydist) )
                {
                    liftbox.centroid.x= (boundarymaxx - boundaryminx)/2;
                    liftbox.centroid.y=  liftbox.centroid.y;
                    return 1;
                }
                else if( liftbox.centroid.y > boundaryminy && liftbox.centroid.y < (boundaryminy+boundarydist) )
                {
                    liftbox.centroid.y= (boundarymaxy - boundaryminy)/2;
                    liftbox.centroid.x=  liftbox.centroid.x;
                    return 1;
                }
                else  if( liftbox.centroid.y < boundarymaxy && liftbox.centroid.y > (boundarymaxy-boundarydist) )
                {
                    liftbox.centroid.y= (boundarymaxy - boundaryminy)/2;
                    liftbox.centroid.x=  liftbox.centroid.x;
                    return 1;
                }
                else return 0;
}


int main()
{
    int c,check=1;
    CvCapture *capture=cvCreateCameraCapture(1);//initiate camera 
    //cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH , 1000);
    //cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT , 750);
    
    for(long int i=0;i<100000000&& capture!=NULL ;i++);//starting camera takes some time and we can take any frame only after camera starts or frame value will be  black
    
    input=cvQueryFrame(capture);//take current image in camera and give it to frame pointer
    cvNamedWindow("input",1);
                                  
                             
                             input=cvQueryFrame(capture);
                            //cvShowImage("input",input);
                            //input = cvLoadImage("arenatest.png");
    
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
    cvReleaseImage(&botfilter);
    botfilter=prepare_filter('o');
    cvNamedWindow("front",1);
    cvShowImage("front",botfilter);
    cvReleaseImage(&botfilter);
    cvShowImage("input",input);
    red=prepare_filter('r');
    blobs(red,'r');
    yellow=prepare_filter('y');
    cvNamedWindow("filtery",1);
    cvShowImage("filtery",yellow);
    blobs(yellow,'y');
    cyan=prepare_filter('c');
    blobs(cyan,'c');
    cvNamedWindow("cyan",1);
    cvShowImage("cyan",cyan);
    magenta=prepare_filter('m');
    cvNamedWindow("mag",1);
    cvShowImage("mag",magenta);
    blobs(magenta,'m');
    cvReleaseImage(&red);
    cvReleaseImage(&yellow);
    cvReleaseImage(&cyan);
    cvReleaseImage(&magenta);
    }        
    sort_distance();
    for(int l=0;l<num_boxes;l++)
               cout<<"\ncentroid.x "<<puzzle[l].centroid.x<<"\t centroid.y "<<puzzle[l].centroid.y<<"\tcolor "<<puzzle[l].color<<"   distance  "<<puzzle[l].distance;
    
    botfront.color='o';
    botback.color='p';
    //puzzle[0].color='r';
    get_bot_cordinates();
            
    cvWaitKey(0);
    cvDestroyAllWindows();
     
     cvNamedWindow("input",1);
     cvShowImage("input",input);
     cout<<"\nlift order\n";
     input=cvQueryFrame(capture);
     for(int l=0;l<num_boxes;l++)
     {
            //int l=0;
            //Choose which box to pick
            grabbed=0;
            liftfilter= prepare_filter(puzzle[l].color);
            blobs_arena(liftfilter,puzzle[l].color);
            cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
            cvReleaseImage(&liftfilter);
            get_bot_cordinates();
            check=calculate();
            //cvWaitKey(0);
            if(liftbox.centroid.x==10000)
            continue;          
            //movement of bot
            // bot coordinates reach upto the box
            // add a condition for boundaries if it is extremes reach according coordinates 
            do{
                      
                      liftfilter= prepare_filter(puzzle[l].color);
                      blobs_arena(liftfilter,puzzle[l].color);
                      cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
                      cvReleaseImage(&liftfilter);
                      
                      Send('g');//close grip and move forward
                      cvWaitKey(800);
                      Send('s');
                      grabbed=0;
                      if(check_boundary())
                      {
                       check=calculate();
                       cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
                       //cvWaitKey(0);
                       while(check)
                       {
                           input=cvQueryFrame(capture);
                           get_bot_cordinates();
                           orient();
                           check=calculate();
                           cout<<"angle  "<<angle<<"\n";;
                       }
                       //cvWaitKey(0);
                       
                      }         
                      
                      liftfilter= prepare_filter(puzzle[l].color);
                      blobs_arena(liftfilter,puzzle[l].color);
                      cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
                      cvReleaseImage(&liftfilter);
                      check=calculate();
                      while(check)
                      {
                           liftfilter= prepare_filter(puzzle[l].color);
                           blobs_arena(liftfilter,puzzle[l].color);
                           cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
                           cvReleaseImage(&liftfilter);
                           input=cvQueryFrame(capture);
                           get_bot_cordinates();
                           orient();
                           check=calculate();
                           cout<<"angle  "<<angle<<"\n";;
                      }
            
                      //OPEN
                   grabbed=1;
                   Send('d');
                   cvWaitKey(800);
                   Send('s');
                   //get box within
                   check=calculate();
                   while(check)
                      {
                           input=cvQueryFrame(capture);
                           liftfilter= prepare_filter(puzzle[l].color);
                           blobs_arena(liftfilter,puzzle[l].color);
                           cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
                           cvReleaseImage(&liftfilter);
                           get_bot_cordinates();
                           orient();
                           check=calculate();
                           cout<<"angle  "<<angle<<"\n";;
                      }
            
                    //hold box
                   Send('g');
                   cvWaitKey(800);
                   Send('s');
                   
                   //check if box is gripped
                   input=cvQueryFrame(capture);
                   liftfilter= prepare_filter(puzzle[l].color);
                   blobs_arena(liftfilter,puzzle[l].color);
                   cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
                   cvReleaseImage(&liftfilter);
                   get_bot_cordinates();
                   check_grab();                    
                   
                   if(!grabbed)//If grabbed is still move move back
                   {
                    Send('b');
                    cvWaitKey(600);
                    Send('s');
                   }
                   else
                   {
                    Send('b');
                    cvWaitKey(500);
                    Send('s');
                    input=cvQueryFrame(capture);
                   liftfilter= prepare_filter(puzzle[l].color);
                   blobs_arena(liftfilter,puzzle[l].color);
                   cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
                   cvReleaseImage(&liftfilter);
                   get_bot_cordinates();
                   check_grab();                    
                   
                   }
                   
                   
                   
            }while(!grabbed);
            
            //Need to add movement to a point
            //Intermediate point           
            liftbox.centroid.x = intermediatex;
            liftbox.centroid.y= intermediatey;
            cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
            grabbed=0;
            check=calculate();
            while(check)
            {
                           input=cvQueryFrame(capture);
                           get_bot_cordinates();
                           orient();
                           check=calculate();
                           cout<<"check  "<<check<<"\n";            
                           cout<<"angle  "<<angle<<"\n";;
                                       
            }
                   
            //Check box
            liftfilter= prepare_filter(puzzle[l].color);
            blobs_arena(liftfilter,puzzle[l].color);
            cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
            cvReleaseImage(&liftfilter);
            get_bot_cordinates();
            check_grab();
            //grabbed=1;
            if(!grabbed)
            {
                        l--;
                        Send('d');//drop box
                        cvWaitKey(600);
                        Send('s');
                        cvWaitKey(500);
            
                        Send('b');//move back
                        cvWaitKey(600);
                        Send('s');
                        continue;
            }
            
            Send('g');//Tighten Grip
            cvWaitKey(200);
            Send('s');
            
            grabbed=0;
            //final Point
            liftbox.centroid.x = depmaxx - ( puzzlemaxx - puzzle[l].centroid.x);
            liftbox.centroid.y= depmaxy - ( puzzlemaxy - puzzle[l].centroid.y);
            cout<<"\nPicked: Centroid.x "<<liftbox.centroid.x<<"\t centroid.y "<<liftbox.centroid.y<<"\tcolor "<<liftbox.color<<"   distance  "<<liftbox.distance;
            check=calculate();
                   
            while(check)
            {
                           input=cvQueryFrame(capture);
                           get_bot_cordinates();
                           orient();
                           check=calculate();
                           cout<<"check  "<<check<<"\n";            
                           cout<<"angle  "<<angle<<"\n";;
            }
            grabbed=1;
            depo_time=1;
            check=calculate();
            while(check)
            {
                           input=cvQueryFrame(capture);
                           get_bot_cordinates();
                           orient();
                           check=calculate();
                           cout<<"check  "<<check<<"\n";            
                           cout<<"angle  "<<angle<<"\n";;
            }
            depo_time=0;
            Send('d');//drop box
            cvWaitKey(100);
            Send('s');
            cvWaitKey(500);
            
            Send('b');//move back
            cvWaitKey(1500);
            Send('s');
            cvWaitKey(400);
            
     } 
        
    
    cvShowImage("input",input);
    cvWaitKey(0); 
    cvDestroyAllWindows();
    cvReleaseImage(&input);
    cvReleaseCapture(&capture);
                   
    return 0;
} 

