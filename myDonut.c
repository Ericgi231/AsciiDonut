#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define PI 3.14159265358979323846

const int default_screen_width = 80;
const int default_frame_delay = 30000;
const float default_R1 = 1;
const float default_R2 = 2;
const float default_K2 = 5;
const float default_theta_spacing = 0.07;
const float default_phi_spacing = .02;

int main(int argc, char** argv) {
    //set extra vars
    int setARot = 0;
    int setBRot = 0;

    //set screen size and properties default
    int screen_width = default_screen_width; //width of display area
    int screen_height = default_screen_width / 10 * 3; //height of display area
    int frame_delay = default_frame_delay; //time between frames

    //set mathimatical cosntants default
    float R1 = default_R1; //circle radius
    float R2 = default_R2; //torus radius
    float K2 = default_K2; //distnace from eyes to donut
    float ARot = 0;
    float BRot = 0;

    //set dot spacing deafult
    float theta_spacing = default_theta_spacing; //how often to create dots around the torus
    float phi_spacing = default_phi_spacing; //how often to create dots around the circle

    //set custom flags from command line
    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-s") == 0)
            {
                screen_width = atoi(argv[i+1]);
                screen_height = atoi(argv[i+1]) / 10 * 3;
            } 
            else if (strcmp(argv[i], "-r1") == 0) 
            {
                R1 = atof(argv[i+1]);
            }
            else if (strcmp(argv[i], "-r2") == 0) 
            {
                R2 = atof(argv[i+1]);
            }
            else if (strcmp(argv[i], "-k") == 0) 
            {
                K2 = atof(argv[i+1]);
            }
            else if (strcmp(argv[i], "-f") == 0) 
            {
                frame_delay = atoi(argv[i+1]);
            }
            else if (strcmp(argv[i], "-t") == 0) 
            {
                theta_spacing = atof(argv[i+1]);
            }
            else if (strcmp(argv[i], "-p") == 0) 
            {
                phi_spacing = atof(argv[i+1]);
            }
            else if (strcmp(argv[i], "-a") == 0) 
            {
                ARot = atof(argv[i+1]);
                setARot = 1;
            }
            else if (strcmp(argv[i], "-b") == 0) 
            {
                BRot = atof(argv[i+1]);
                setBRot = 1;
            }
        }
    }

    //set final mathimatical cosntants
    const float K1 = screen_height * K2 * 3 / ( 8 * (R1+R2)); //distance from eyes to screen

    //set random rotation if not set
    srand(time(0));
    if (setARot == 0)
    {
        ARot = (float)(rand() % 5 + 1) / 100000;
    }
    if (setBRot == 0)
    {
        BRot = (float)(rand() % 5 + 1) / 100000;
    }
    float A = 0;
    float B = 0;

    //create dot and zbuffer arrays
    char output[screen_width][screen_height];
    float zbuffer[screen_width][screen_height];

    printf("\x1b[2J"); //clear screen

    //run until escaped
    //each loop represents a frame
    for (;;)
    {
        //clear memory at start of each frame
        memset(output, 32, screen_width*screen_height*sizeof(char)); //fill output with blank space
        memset(zbuffer, 0, screen_width*screen_height*sizeof(float)); //fill zbuffer with 0

        //precompute trig values
        float cosA = cos(A);
        float sinA = sin(A);
        float cosB = cos(B);
        float sinB = sin(B);

        //theta goes around the circle that is rotated to form the donut
        for (float theta = 0; theta < 2*PI; theta += theta_spacing)
        {
            //precompute trig values
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            //phi goes around revolution of donut
            for (float phi = 0; phi < 2*PI; phi += phi_spacing)
            {
                //precompute trig values
                float cosPhi = cos(phi);
                float sinPhi = sin(phi);

                //x y of circle
                float circleX = R2 + (R1 * cosTheta);
                float circleY = R1 * sinTheta;

                //3d coordinates
                float x = circleX * ((cosB * cosPhi) + (sinA * sinB * sinPhi)) - (circleY * cosA * sinB);
                float y = circleX * ((sinB * cosPhi) - (sinA * cosB * sinPhi)) + (circleY * cosA * cosB);
                float z = K2 + (cosA * circleX * sinPhi) + (circleY * sinA);
                float ooz = 1/z;

                //x and y projections onto screen
                int xp = (int) ((screen_width/2) + ((2*K1)*ooz*x));
                int yp = (int) ((screen_height/2) - (K1*ooz*y));

                //lumination
                float L = (cosPhi * cosTheta * sinB) - (cosA * cosTheta * sinPhi) - (sinA * sinTheta) + (cosB * (cosA * sinTheta - cosTheta * sinA * sinPhi));
                //ranges from -sqrt2 to sqrt2
                //if less than 0 can not be seen

                if (L > 0)
                {
                    if (ooz > zbuffer[xp][yp])
                    {
                        zbuffer[xp][yp] = ooz;
                        //get index between 0 and 11
                        //sqrt2 * 8
                        int luminance_index = L*8;

                        //set visual output
                        output[xp][yp] = ".,-~:;=~*#$@"[luminance_index];
                    }
                }
            }
        }

        //display
        printf("\x1b[H");
        for (int j = 0; j < screen_height; j++)
        {
            for (int i = 0; i < screen_width; i++)
            {
                putchar(output[i][j]);
                A += ARot;
                B += BRot;
                if (A > 32000 || B > 32000)
                {
                    A = 0;
                    B = 0;
                }
            }
            putchar('\n');
        }

        usleep(frame_delay);
    }
    return 0;
}