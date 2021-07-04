# ray-casting-and-ray-tracing  
This repository contains all the programs coded for assignment on ray casting and ray tracing **(Offline-3)**.  

In this assignment, realistic images are generated for geometric shapes like *sphere*, *triangle*, *plane*, and *general quadric surface* using ray casting as well as ray tracing with **Phong Illumination Model** technique. **OpenGL** and **OpenGL Utility Toolkit (GLUT)** have been used to visualize the provided scenario in input file.  

## navigation  
- `./exe` contains dynamic-link library file `./exe/glut32.dll` along with executable file `./exe/offline3-src.exe`    
- `./inputs` contains sample input files  
- `./res` contains sample output bitmap images for input files in `./inputs`  
- `./spec` contains specification for this assignment  
- `./src` contains two files -  
	- `./src/header.hpp` is header file with definitions of structures and classes  
	- `./src/main.cpp` is main file with **OpenGL**, input processing, and image capturing functions  

## guidelines  
### getting started  
- just run `./exe/offline3-src.exe` inside `./exe` directory  

### operating fully controllable camera  
| Key         	| Function      	| key 	| Function           	|
|-------------	|---------------	|-----	|--------------------	|
| up arrow    	| move forward  	| 1   	| look left          	|
| down arrow  	| move backward 	| 2   	| look right         	|
| right arrow 	| move right    	| 3   	| look upward        	|
| left arrow  	| move left     	| 4   	| look downward      	|
| page up     	| move upward   	| 5   	| tilt clockwise     	|
| page down   	| move downward 	| 6   	| tilt anticlockwise 	|  

### input file interpretation  
```
4                       // level of recursion
768                     // pixel dimension of output bitmap image

3                       // number of objects in the scene
sphere
40.0 0.0 10.0           // center
10.0                    // radius
0.0 1.0 0.0             // rgb values
0.4 0.2 0.2 0.2         // ambient, diffuse, specular & reflection coefficients
10                      // shininess

triangle
30 60 0                 // coordinates of first corner
50 30 0                 // coordinates of second corner
50 45 50                // coordinates of third corner
0.0 0.0 1.0             // rgb values
0.4 0.2 0.1 0.3         // ambient, diffuse, specular & reflection coefficients
5                       // shininess

general
1 1 1 0 0 0 0 0 0 -100  // coefficients of general quadratic equations
0 0 0 0 0 20            // cube reference point coordinates, length, width, height
0.0 1.0 0.0             // rgb values
0.4 0.2 0.1 0.3         // ambient, diffuse, specular & reflection coefficients
10                      // shininess

1                       // number of lights in the scene
70.0 70.0 70.0          // coordinates of light source
1.0 0.0 0.0             // rgb values
```  

### capturing bitmap image  
| Key 	| Function             	|
|-----	|----------------------	|
| 0   	| capture bitmap image 	|  

## references  
**OpenGL documentation:** https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/  
**GLUT API documentation:** https://www.opengl.org/resources/libraries/glut/spec3/spec3.html  
**Download `bitmap_image.hpp` from:** [https://drive.google.com/file/d/14eOfsMpwIuh8G_Wy6BSjNX6VXFRA9eSM/view?usp=sharing](https://drive.google.com/file/d/14eOfsMpwIuh8G_Wy6BSjNX6VXFRA9eSM/view?usp=sharing)  

## bitmap images  
### bitmap image captured from above the floor  
![above-floor](https://github.com/FromSaffronCity/computer-graphics-sessional/blob/main/ray-casting-and-ray-tracing/res/above-floor.bmp?raw=true)  

### bitmap image captured from below the floor  
![below-floor](https://github.com/FromSaffronCity/computer-graphics-sessional/blob/main/ray-casting-and-ray-tracing/res/below-floor.bmp?raw=true)

### national memorial  
![national-memorial](https://github.com/FromSaffronCity/computer-graphics-sessional/blob/main/ray-casting-and-ray-tracing/res/national-memorial.bmp?raw=true)
