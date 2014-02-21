#include "Tserial.cpp"
    #ifdef __BORLANDC__
    #pragma hdrstop            // borland specific
    #include <condefs.h>
    #pragma argsused
    USEUNIT("Tserial.cpp");
    #endif

   
   Tserial *com;
int Send(char head)
    {com = new Tserial();
    com->connect("COM4", 9600, spNONE);
    com->sendChar(head);
    com->disconnect();
    return 0;}
    
int main()
{
    char ch;
    ch=getchar();
    while(ch != '0')
    {
             Send(ch);
             ch=getchar();
    }
}
