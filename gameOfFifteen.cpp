#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <tuple>
#include <cmath>


#define WIDTH 1920
#define HEIGHT 1008


#define INIT_MAX_ITER 100
#define STEP_ITER 50
#define MANUAL_STEP_ITER 50


#define LEFT -2.4
#define RIGHT 0.6
#define TOP 0.7875
#define BOTTOM -0.7875
#define POW 2


#define SCALE_FACTOR 2
#define MOVE_SENS 0.25


const int N3 = (int) pow(256,3) ;



//  We need to create a structure which we will call a Frame
//  This will represent the information that needs to be shown on the screen
//  While rendering the image, we will pick the coordinates of the point which will be shown in the centre
//  and the other dimensions like the frame width and height which are capturing the extent to which we want to plot the set

class Frame{

    public:


        //Class variables
        
        /*  
            Storing the centre of the frame
            These contain the coordinates of the centre which will be used for calculating colours, and not the actual centre of the screen
        */
        double centre_x ;
        double centre_y ;


        /*  
            Storing the extent to which we want to display our plot
            These contain the lengths of range around the centre, in which we want to plot the set
            In essence, these also represent the current magnification
        */
        double frame_width ;
        double frame_height ;
    

        /*
            We need to keep track of the limit of number of iterations the system can go through for each point
            As we zoom in, we will keep increasing this count as per our convenience :
                As we zoom in, the points in view are actually much more closer, so we need to increase the number of iterations
                for differentiating between those points
            and on zooming out, we will decrease this count so as to reduce the load on the computer
        */
        int max_iter ;

        /*
            The main texture which will hold the image which needs to be displayed
            This texture is updated whenever a parameter of the frame is changed
        */
        sf::Texture texture ;

    //  ----------------------------------------------------------------------------------//
    //  Member functions
        
    //  Declaring constructors
        Frame(double argleft,double argright,double argtop,double argbottom) ;
        Frame() ;


        /*
            Functions for zooming in and zooming out of the image
        */    
        void zoom_in(int x,int y) ;
        void zoom_out(int x,int y) ;

        /*
            Functions which will just move the view to show above the current frame, and below the current frame
        */
        void move_up() ;
        void move_left() ;

        /*
            Similar functions for moving right and left, basically shifts the centre
        */
        void move_right() ;
        void move_down() ;

        /*
            For returning back to the initial condition
        */
        void reset() ;

    
};


//  irst constructor for the frame
Frame::Frame(double argleft,double argright,double argtop,double argbottom){
        
        
    centre_x = (argleft+argright)/2 ;
    centre_y = (argtop+argbottom)/2 ;
        
        
    frame_width = argright-argleft ;
    frame_height = argtop-argbottom ;
        
    max_iter = INIT_MAX_ITER ;
        
        
    sf::Image image ;
    image.create(WIDTH,HEIGHT,sf::Color::Green) ;
    texture.loadFromImage(image) ;
    
    
}


//  Another constructor for the frame
Frame::Frame(){
    
    
    centre_x = (LEFT+RIGHT)/2 ;
    centre_y = (TOP+BOTTOM)/2 ;
        
        
    frame_width = RIGHT-LEFT ;
    frame_height = TOP-BOTTOM ;
        
    max_iter = INIT_MAX_ITER ;
       
    sf::Image image ;
    image.create(WIDTH,HEIGHT,sf::Color::Green) ;
    texture.loadFromImage(image) ;


}


/*
    This function helps in zooming in to the image
    The new image will be centred around the point where mouse pointer was pointing to, while zooming in
    One has to increase the resolution as we move in, since the points are getting closer,
    so, we also increment the max_iter parameter for this frame
*/
void Frame::zoom_in(int x,int y){ // x and y are cursor positions
    

    /*
        Assigning coordinates to the central point for the new image to rendered
        x and y are arguments which contain the x and y coordinate of the mouse pointer when zoom_in was called
    */
    centre_x = centre_x+(x-WIDTH/2)*frame_width/WIDTH ;
    centre_y = centre_y+(y-HEIGHT/2)*frame_height/HEIGHT ;
    
    /*
        Scale down the frame lengths, and fit the remaining part on the screen
    */
    frame_width = frame_width/SCALE_FACTOR ;
    frame_height = frame_height/SCALE_FACTOR ;
    
    /*
        Incrementing the max_iter for better resolution while zooming in
    */
    max_iter+=STEP_ITER ;


}


/*
    This function helps in zooming out of the image
    The new image will be centred around the point where mouse pointer was pointing to, while zooming out
    We can decrease the resolution while we zoom out, and we can reduce pressure on the cpu this way
    This helps in faster image rendering
*/
void Frame::zoom_out(int x,int y){
    

    /*
        Assigning coordinates to the central point for the new image to rendered
        x and y are arguments which contain the x and y coordinate of the mouse pointer when zoom_in was called
    */    
    centre_x = centre_x+(x-WIDTH/2)*frame_width/WIDTH ;
    centre_y = centre_y+(y-HEIGHT/2)*frame_height/HEIGHT ;
    
    /*
        Scale up the frame lengths, and fit the remaining part on the screen
    */    
    frame_width = frame_width*SCALE_FACTOR ;
    frame_height = frame_height*SCALE_FACTOR ;

    /*
        Decrementing the max_iter for decreasing the resolution, helps in faster processing
    */    
    if(max_iter>=100){
        max_iter-=STEP_ITER ;
    }

}


//  Moving the view towards top of the image
//  Scale remains the same, but centre is moved upwards
void Frame::move_up(){
    
    //  Changing y because vertical movement is required
    centre_y -= frame_height*MOVE_SENS ;

}

//  Moving the view towards left of the image
//  Scale remains the same, but centre is moved toward left
void Frame::move_left(){

    //  Changing x because horizontal movement is required
    centre_x -= frame_width*MOVE_SENS ;

}

//  Moving the view towards right of the image
//  Scale remains the same, but centre is moved toward right
void Frame::move_right(){

    //  Changing x because horizontal movement is required
    centre_x += frame_width*MOVE_SENS ;

}

//  Moving the view towards bottom of the image
//  Scale remains the same, but centre is moved toward the bottom
void Frame::move_down(){

    //  Changing y because vertical movement is required
    centre_y += frame_height*MOVE_SENS ;

}


/*
    Basically reset the frame to its initial condition
    Changes the centre and frame lengths back to initial values
    max_iter is also reinitialised
*/
void Frame::reset(){
    
    
    centre_x = (LEFT+RIGHT)/2 ;
    centre_y = (TOP+BOTTOM)/2 ;
    
    
    frame_width = RIGHT-LEFT ;
    frame_height = TOP-BOTTOM ;
    
    
    max_iter = INIT_MAX_ITER ;


}

//  Class definition complete....
//________________________________________________________________________________________________________________________________________________


/*
    here is the algorithm for generating colour according to the iter value at which iteration stopped,
    along with the max_iter value at the moment
    Colours tend to vary as max_iter value is varied, since the ratio of iter to max_iter changes
*/
std::tuple<int, int, int> iter_to_rgb(int iter,int max_iter){


    long double iter_n = (long double)iter / max_iter ;
    int N = 256 ;

    int n = (int) (iter_n * (long double) N3) ;

    // Assigning colours
    int r = n/(N*N) ;
    int nn2 = n - r*N*N ;
    int b = nn2 / N ;
    int g = nn2 - b*N ;


    return std::tuple<int, int, int> (r,g,b) ;


}


/*
    Linear interpolation of colours is important to reduce the bands of colour seen in plot
    This helps in merging the colours better and gives a more continuous look
    Done by linearly interpolating individual r g b components and then returning the processed tuple
*/
std::tuple<int, int, int> interpolate(std::tuple<int, int, int> temp1,std::tuple<int, int, int> temp2,long double iter_d){

    // Getting interpolated values
    int a1 = std::get<0>(temp1)*(1-iter_d) + std::get<0>(temp2)*(iter_d) ;
    int a2 = std::get<1>(temp1)*(1-iter_d) + std::get<1>(temp2)*(iter_d) ;
    int a3 = std::get<2>(temp1)*(1-iter_d) + std::get<2>(temp2)*(iter_d) ;


    std::tuple<int, int, int> temp (a1, a2, a3) ;
    return temp ;


}


/*
    The main function which works for assigning colours to different pixels in the image
    Takes the coordinates of the point for which colour has to be calculated
*/
sf::Color getColor(double c_x,double c_y,int pow,int max_iter){
    
    //  Initialising iteration counters
    //  iter_d will be used in interpolation whenever it is required
    int iter = 0 ;
    long double iter_d = 0 ;
    
    // Setting the initial point for getting the Mandelbrot
    double x = 0 ;
    double y = 0 ;
    double x2 = 0 ;
    double y2 = 0 ;
    double w = 0 ;
    
    // Slightly faster escape time algorithm due to lesser number of multiplications
    while(x2+y2<=(1<<pow) && iter < max_iter){
    
    
        x = x2 - y2 + c_x ;
        y = w - x2 - y2 + c_y ;
        x2 = x*x ;
        y2 = y*y ;
        w = (x+y)*(x+y) ;
    
    
        iter++ ;
    
    
    }
    
    // Calculating the bias of the point toward a colour out of two nearest colours
    if(iter<max_iter){

    
        long double log_zn = log(x2+y2)/2 ;
        long double nu = log(log_zn/log(2)) ;
        iter_d = iter + 1 - nu ;

    
    }
    
    //  temp1 holds first colour
    std::tuple<int, int, int> temp1 = iter_to_rgb((int)iter_d,max_iter) ;
    

    if(iter<max_iter){
    
        // temp2 holds the second nearest colour
        std::tuple<int, int, int> temp2 = iter_to_rgb((int)iter_d+1,max_iter) ;
        iter_d -= (int)iter_d ;

        // Interpolate according to bias, that is fractional part(iter_d)
        temp1 = interpolate(temp1,temp2,iter_d) ;
    
    
    }
    
    
    sf::Color retColor(std::get<0>(temp1),std::get<1>(temp1),std::get<2>(temp1),255) ;
    
    //return sf::Color::Green ;
    //sf::Color retColor(5,5,5,255) ;

    return retColor ;


}


/*
    This plots the complete frame pixel by pixel
    Utilises the getColor function to get a colour for each pixel
*/
void plot(Frame& frame){
    
    //  Extract the image from texture for further manipulation
    sf::Image image = frame.texture.copyToImage() ;
    

    //  Get the dimensions of the image
    int width = image.getSize().x ;
    int height = image.getSize().y ;


    // Colour each pixel one by one
    for(int i=0;i<width;i++){
    
        for(int j=0;j<height;j++){
        
            
            sf::Color pixelColor = getColor(frame.centre_x+(i-width/2)*frame.frame_width/width,frame.centre_y+(j-height/2)*frame.frame_height/height,POW,frame.max_iter) ;
            image.setPixel(i,j,pixelColor) ;
            //image.setPixel(i,j,sf::Color::Green) ;
        
        
        }
    
    }
    
    //  Now paste the modified image onto the frame texture
    frame.texture.loadFromImage(image) ;


}


// Implementation complete....

//_______________________________________________________________________________________________________________________________




int main()
{
    
    
    int width = 1920 ;
    int height = 1008 ;
    
    
    //  Create the main window
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");
    
    
    //  Load the font for text
    sf::Font font ;
    font.loadFromFile("arial.ttf");

    
    //  text1 will display the Max Iteration count in the window
    sf::Text text1 ;
    

    text1.setFont(font) ;
    text1.setFillColor(sf::Color::White) ;
    text1.setCharacterSize(30) ;
    text1.setString("nwbefjlanlkjlklfdjawgasdf");


    sf::Text text2 ;
    

    text2.setFont(font) ;
    text2.setFillColor(sf::Color::White) ;
    text2.setCharacterSize(30) ;
    text2.setString("nwbefjlanlkjlklfdjawgasdf");
    text2.move(0,30) ;


    // Load a sprite to display


//    sf::Texture texture;
//    sf::Image image ;
//    image.create(width,height,sf::Color::Green) ;
//    texture.loadFromImage(image) ;

    // The all powerful frame is initialised to hold all the required variables for the image
    Frame frame ;
    

    sf::Sprite sprite;
    sprite.setTexture(frame.texture) ;

    
    while (window.isOpen())
    {
    
    
        // Process events
        sf::Event event;
    
    
        while (window.pollEvent(event))
        {
            

            /*
                MouseWheelScroll event is being used for zooming in and zooming out of the image
                Scrolling up results in zooming in, while scrolling down results in zooming out
            */

            if (event.type == sf::Event::MouseWheelScrolled){
                //std::cout << event.mouseWheelScroll.x << std::endl ;
                

                if(event.mouseWheelScroll.delta>0){

                    frame.zoom_in(event.mouseWheelScroll.x,event.mouseWheelScroll.y) ;

                }


                else{

                    frame.zoom_out(event.mouseWheelScroll.x,event.mouseWheelScroll.y) ;

                }


            }


            /*
                Keyboard presses (WASD) are for navigating through the plot, and R key is for resetting the plot to its initial condition
            */

            if (event.type == sf::Event::KeyPressed){
                //std::cout << event.key.code << std::endl ;

                
                if(event.key.code == sf::Keyboard::Key::W){
                
                    frame.move_up() ;
                
                }

                
                if(event.key.code == sf::Keyboard::Key::A){
                
                    frame.move_left() ;
                
                }


                if(event.key.code == sf::Keyboard::Key::S){

                    frame.move_down() ;

                }


                if(event.key.code == sf::Keyboard::Key::D){

                    frame.move_right() ;

                }


                if(event.key.code == sf::Keyboard::Key::R){

                    frame.reset() ;

                }

            }


            /*
                MouseButton presses will be used for increasing and decreasing the resolution
                This is done by increasing or decreasing the max_iter count inside the frame
                Changing max_iter also changes the colours of the plot, since colouring is dependent on max_iter of the frame
            */

            if (event.type == sf::Event::MouseButtonPressed){

                
                if(event.mouseButton.button == sf::Mouse::Button::Left){
                
                    frame.max_iter+=MANUAL_STEP_ITER ;
                
                }
                
                if(event.mouseButton.button == sf::Mouse::Button::Right){
                
                    if(frame.max_iter>=INIT_MAX_ITER+50){
                
                        frame.max_iter-=MANUAL_STEP_ITER ;
                
                    }
                }

            
            }


            // Close window: exit
            if (event.type == sf::Event::Closed)
            
                window.close();


        }
        
        
        // Clear screen
        window.clear() ;

        
        //Colour the frame
        //This takes into account the x and y aspects of the frame
        plot(frame) ;

        //setPixelColor(texture,sf::Color::Green) ;
        

        /*
            Generate fresh text for displaying on the window
            Take in the values of Max iterations and Zoom to show on the screen
        */
        char t1[32];
        char t2[32];
        sprintf(t1,"Max Iterations : %d",frame.max_iter) ;
        
        
        long double zoom = RIGHT-LEFT/(frame.frame_width) ;
        sprintf(t2,"Zoom : %Lfx",zoom) ;
        
        
        // Set the texts equal to their respective strings
        text1.setString(t1) ;
        text2.setString(t2) ;


        // Draw the sprite
        window.draw(sprite) ;
        

        /*
            Draw the text over the sprite
            I was initially drawing text before the sprite, due to which the text was not being displayed at all
        */
        window.draw(text1) ;
        window.draw(text2) ;
        
        
        // Update the window
        window.display() ;
    
    
    }
    
    
    return EXIT_SUCCESS;


}