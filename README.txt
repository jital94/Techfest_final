This a project done to control a robot 's movement using an ovehead camera. 
The aim of bot is to recognize pattern and reconstruct it using boxes in pit
Problem statement is available in file "polaroid.pdf"

The final running code is present in file name "check_grip_ADV_final_edit_sort/cpp"
The part of code you need to change according to conditions are specified
in beginning of code.

other codes are previous versions of same code. but with lesser accuracies.


About the code
>it is built using "OpenCV 2.1" and a 3rd paty library "cvBlob"
>It uses arduino to drive the bot. The messages are sent to arduino using USB port
>The code also takes into various aspects like 
	inaccuracy in exact turning
	Boundary colisions and picking boxes on boundary
	Checking if box is still gripped at various stages
	Perfection in keeping the box
	two coloured plates to get orientation of bot 
>Instructions are given to bot using "Tserial" which can either communicate via any port in your computer
>messages sent are 1 letter instructions eac assigned a specific movement


Changes you have to do in code before running it
>set the hsv high-low values so as to filter a specific colour. Use callbration tools as required
>U need to set values for bot's both colours
>set the COM port in Send().(COM port can be seen from device manager)
>Set the filter blob area in blobs if want to cancel small noises
>set the distance for gripping of box in calculate() function
>Set camera index in main() function cvCaputre declaration.


Callibration Tools:-
>HSV test_camera.cpp:- it captures an image from webcam and use mouse pointer to find HSV values of specific points
>send_test_bot:- It sends a single letter to bot(Use to test com port and bot motors)
>callibration:- It captures an image from webcam and use sliders to find filter values of colours
>callibration_vid:- Same as callibration but it captures image continuously.(used to test filter when bot is moving)



In case of any Queries:-
mail to:- jital1994@gmail.com
