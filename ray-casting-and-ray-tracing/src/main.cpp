#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>
#include<cstdlib>
#include<vector>
#include<limits>

#include<windows.h>
#include<GL/glut.h>

/*
    You will find "bitmap_image.hpp" from the following link:
    https://drive.google.com/file/d/14eOfsMpwIuh8G_Wy6BSjNX6VXFRA9eSM/view?usp=sharing
*/

#include "bitmap_image.hpp"
#include "header.hpp"

using namespace std;

#define PI 2*acos(0.0)
#define INF numeric_limits<double>::infinity()

/*
    defining global parameters:
        - windowWidth = OpenGL window width
        - windowHeight = OpenGL window height
        - fovY = field of view angle along y-axis

        - bDrawAxes = true: draw axes; false: do not draw axes; on the screen

        - position = position vector of camera
        - u = unit vector in up direction of camera
        - r = unit vector in right direction of camera
        - l = unit vector in look direction of camera

        - recursionlevel = level of recursion for recursive ray tracing
        - imagePixelDimension = dimension of output bitmap images in pixel
        - objectsCount = total objects in scene
        - lightsCount = total light sources in scene

        - objects = vector containing all the objects in the scene
        - lights = vector containing all the lights in the scene

        - bitmapImageCount = counter for captured bitmap images

    reference: https://www.geeksforgeeks.org/understanding-extern-keyword-in-c/
*/

int windowWidth = 500;
int windowHeight = 500;
double fovY;

bool bDrawAxes;

extern Vector position;
Vector u;
Vector r;
Vector l;

extern int recursionLevel;
int imagePixelDimension = 0;
int objectsCount = 0;
int lightsCount = 0;

extern vector<Object*> objects;
extern vector<Light> lights;

int bitmapImageCount;

/*
    defining basic drawing functions:
        - void drawAxes(): for drawing principal axes on the screen
*/

void drawAxes(double axisLength) {
    if(!bDrawAxes) {
        return ;
    }

    /* drawing x-axis */
	glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    {
        glVertex3f(axisLength, 0.0, 0.0);
        glVertex3f(-axisLength, 0.0, 0.0);
    }
    glEnd();

    /* drawing y-axis */
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    {
        glVertex3f(0.0, axisLength, 0.0);
        glVertex3f(0.0, -axisLength, 0.0);
    }
    glEnd();

    /* drawing z-axis */
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    {
        glVertex3f(0.0, 0.0, axisLength);
        glVertex3f(0.0, 0.0, -axisLength);
    }
    glEnd();
}

/*
    defining supplementary ray tracing function:
        - void capture(): for capturing bitmap image of the scene from current position of camera
*/

void capture() {
    cout << position << ": capturing bitmap image" << endl;

    /* initializing bitmap image and setting background color */
    bitmap_image bitmapImage(imagePixelDimension, imagePixelDimension);

    for(int column=0; column<imagePixelDimension; column++) {
        for(int row=0; row<imagePixelDimension; row++) {
            bitmapImage.set_pixel(column, row, 0, 0, 0);  // color = black
        }
    }

    /* computing and setting necessary parameters */
    double planeDistance = windowHeight/(2.0*tan(fovY/2.0*PI/180.0));
    Vector topLeft = position+l*planeDistance-r*(windowWidth/2.0)+u*(windowHeight/2.0);

    double du = ((double) windowWidth/imagePixelDimension);
    double dv = ((double) windowHeight/imagePixelDimension);

    /* choosing middle coordinate of the pixel grid cell */
    topLeft = topLeft+r*(du/2.0)-u*(dv/2.0);

    /* capturing the scene from the current position of camera */

    /*
        reference:
            1) https://www.geeksforgeeks.org/maximum-value-of-int-in-c/
            2) https://en.cppreference.com/w/cpp/types/numeric_limits/max
    */

    for(int column=0; column<imagePixelDimension; column++) {
        for(int row=0; row<imagePixelDimension; row++) {
            /* calculating current pixel and casting ray from camera to (curPixel-camera) direction */
            Vector curPixel = topLeft+r*(column*du)-u*(row*dv);
            Ray ray(position, curPixel-position);

            /* finding nearest intersecting object (if available) */
            int nearest = INT_MAX;
            double t, tMin=INF;

            for(int i=0; i<objects.size(); i++) {
                Color color;  // color = black
                t = objects[i]->intersect(ray, color, 0);

                if(t>0.0 && t<tMin) {
                    tMin = t;
                    nearest = i;
                }
            }

            /* finding color for current pixel */
            if(nearest != INT_MAX) {
                Color color;  // color = black
                tMin = objects[nearest]->intersect(ray, color, 1);
                bitmapImage.set_pixel(column, row, (int) round(color.red*255.0), (int) round(color.green*255.0), (int) round(color.blue*255.0));
            }
        }
    }

    /* saving bitmap image */
    /* reference: https://stackoverflow.com/questions/228005/alternative-to-itoa-for-converting-integer-to-string-c */
    stringstream currentBitmapImageCount;
    currentBitmapImageCount << (++bitmapImageCount);

    bitmapImage.save_image("D:\\Academic 4-1\\CSE410 (Computer Graphics sessional)\\offline-3\\offline3-src\\outputs\\output"+currentBitmapImageCount.str()+".bmp");
    cout << position << ": bitmap image captured" << endl;
}

/*
    defining listener functions:
        - void keyboardListener(): for listening to inputs from keyboard
        - void specialKeyListener(): for listening to inputs from special keys
        - void mouseListener(): for listening to inputs from mouse
*/

void keyboardListener(unsigned char key, int x, int y) {
	switch(key) {
        case '0':
            capture();
            break;
		case '1':
		    /* rotate/look left */
		    r = r*cos(PI/180)+(u%r)*sin(PI/180);
		    l = l*cos(PI/180)+(u%l)*sin(PI/180);
			break;
        case '2':
            /* rotate/look right */
		    r = r*cos(-PI/180)+(u%r)*sin(-PI/180);
		    l = l*cos(-PI/180)+(u%l)*sin(-PI/180);
			break;
        case '3':
            /* look up */
		    l = l*cos(PI/180)+(r%l)*sin(PI/180);
		    u = u*cos(PI/180)+(r%u)*sin(PI/180);
			break;
        case '4':
            /* look down */
		    l = l*cos(-PI/180)+(r%l)*sin(-PI/180);
		    u = u*cos(-PI/180)+(r%u)*sin(-PI/180);
			break;
        case '5':
            /* tilt clockwise */
		    u = u*cos(PI/180)+(l%u)*sin(PI/180);
		    r = r*cos(PI/180)+(l%r)*sin(PI/180);
			break;
        case '6':
            /* tilt counter-clockwise */
		    u = u*cos(-PI/180)+(l%u)*sin(-PI/180);
		    r = r*cos(-PI/180)+(l%r)*sin(-PI/180);
			break;
		default:
		    break;
	}
}

void specialKeyListener(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_DOWN:
            /* down arrow key: move backward */
            position = position-l;
			break;
		case GLUT_KEY_UP:
            /* up arrow key: move forward */
            position = position+l;
			break;
		case GLUT_KEY_RIGHT:
		    /* right arrow key: move right */
		    position = position+r;
			break;
		case GLUT_KEY_LEFT:
		    /* left arrow key: move left */
		    position = position-r;
			break;
		case GLUT_KEY_PAGE_UP:
		    /* page up key: move upward */
		    position = position+u;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    /* page down key: move downward */
		    position = position-u;
			break;
		default:
			break;
	}
}

void mouseListener(int button, int state, int x, int y) {
	switch(button) {
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN) {
                bDrawAxes = !bDrawAxes;
			}
			break;
        case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN) {

			}
			break;
		default:
			break;
	}
}

/*
    defining supplementary OpenGL functions:
        - void display(): for displaying objects on the screen
        - void animate(): for animation purpose
        - void init(): for initialization purpose
*/

void display() {
	/* clearing the display */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);  // color = black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* setting up the camera here */
	/* loading the correct matrix: MODEL-VIEW matrix */
	glMatrixMode(GL_MODELVIEW);

	/* initializing the matrix */
	glLoadIdentity();

	/* setting the camera by providing necessary parameters */
	gluLookAt(position.getX(), position.getY(), position.getZ(), (position+l).getX(), (position+l).getY(), (position+l).getZ(), u.getX(), u.getY(), u.getZ());

	/* again, selecting MODEL-VIEW matrix */
	glMatrixMode(GL_MODELVIEW);

    /* adding objects from here */
	/* adding axes */
	drawAxes(300.0);

	/* adding objects */
	for(int i=0; i<objects.size(); i++) {
        objects[i]->draw();
	}

	/* adding lights */
	for(int i=0; i<lights.size(); i++) {
        lights[i].draw();
	}

	/* ADD this line in the end: if you use double buffer (i.e. GL_DOUBLE) */
	glutSwapBuffers();
}

void animate() {
	/* codes for any changes in Models, Camera */
	glutPostRedisplay();
}

void init() {
    fovY = 80.0;

	bDrawAxes = true;

	position = Vector(100.0, 100.0, 0.0);
	u = Vector(0.0, 0.0, 1.0);
	r = Vector(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0.0);
	l = Vector(-1.0/sqrt(2.0), -1.0/sqrt(2.0), 0.0);

	bitmapImageCount = 0;

	/* clearing the screen */
	glClearColor(0, 0, 0, 0);  // color = black

	/* setting up projection here */
	/* loading the PROJECTION matrix */
	glMatrixMode(GL_PROJECTION);

	/* initializing the matrix */
	glLoadIdentity();

    /* setting the camera perspective by providing necessary parameters */
	gluPerspective(fovY, 1.0, 1.0, 1000.0);
}

/*
    defining complementary functions:
        - void clearObjects(): for clearing objects vector in a proper manner
        - void clearLights(): for clearing lights vector in a proper manner
        - void loadData(): for loading & extracting inputs from input file
*/

void clearObjects() {
    /* deallocating dynamically allocated memory for object instances */
    for(int i=0; i<objects.size(); i++) {
        delete objects[i];
    }

    /* clearing objects vector */
    /* reference: https://www.geeksforgeeks.org/vector-erase-and-clear-in-cpp/ */
    objects.clear();
}

void clearLights() {
    /* clearing lights vector */
    lights.clear();
}

void loadData() {
    ifstream input;

    /* preparing input for extracting values from input file */
    input.open("D:\\Academic 4-1\\CSE410 (Computer Graphics sessional)\\offline-3\\offline3-src\\inputs\\scene.txt");
    if(!input.is_open()) {
        cout << "input.is_open(): failed to open input file" << endl;
        exit(EXIT_FAILURE);
    }

    /* extracting recursion level & image pixel dimension from input file */
    input >> recursionLevel >> imagePixelDimension;

    /* extracting objects information from input file */
    input >> objectsCount;

    string objectShape;
    bool bInvalidObjectShapeFound = false;

    Object* object = NULL;

    for(int i=0; i<objectsCount; i++) {
        input >> objectShape;

        if(objectShape.compare("sphere") == 0) {
            Vector center;
            double radius;

            input >> center;
            input >> radius;

            object = new Sphere(center, radius, 72, 24);
        } else if(objectShape.compare("triangle") == 0) {
            Vector a, b, c;

            input >> a;
            input >> b;
            input >> c;

            object = new Triangle(a, b, c);
        } else if(objectShape.compare("general") == 0) {
            GeneralQuadricSurfaceCoefficient coefficient;
            Vector cubeReferencePoint;
            double length, width, height;

            input >> coefficient;
            input >> cubeReferencePoint;
            input >> length >> width >> height;

            object = new GeneralQuadricSurface(coefficient, cubeReferencePoint, length, width, height);
        } else {
            cout << objectShape << ": invalid object shape found" << endl;
            bInvalidObjectShapeFound = true;
            break;
        }

        Color color;
        ReflectionCoefficient reflectionCoefficient;
        int shininess;

        input >> color;
        input >> reflectionCoefficient;
        input >> shininess;

        object->setColor(color);
        object->setReflectionCoefficient(reflectionCoefficient);
        object->setShininess(shininess);

        objects.push_back(object);
    }
    object = NULL;

    if(bInvalidObjectShapeFound) {
        clearObjects();
        input.close();
        exit(EXIT_FAILURE);
    }

    /* extracting light sources information from input file */
    input >> lightsCount;

    for(int i=0; i<lightsCount; i++) {
        Vector position;
        Color color;

        input >> position;
        input >> color;

        lights.push_back(Light(position, color, 1.0, 12, 4));
    }
    input.close();

    /* creating a floor object and pushing it to objects vector */
    object = new Floor(1000.0, 20.0, Color());  // color = black

    object->setColor(Color(1.0, 1.0, 1.0));  // color = white
    object->setReflectionCoefficient(ReflectionCoefficient(0.25, 0.25, 0.25, 0.25));
    object->setShininess(15);

    objects.push_back(object);
    object = NULL;
}

/* defining main function */
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);  // depth, double buffer, rgb color

	glutCreateWindow("Assignment3: Ray Casting & Ray Tracing");

	init();  // initialization

	glEnable(GL_DEPTH_TEST);  // enable depth testing

	glutDisplayFunc(display);  // display(): callback function
	glutIdleFunc(animate);  // animate(): what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	/* carrying out atexit() function registration */
	/* reference: https://www.geeksforgeeks.org/atexit-function-in-c-c/ */
	if((atexit(clearLights) != 0) || (atexit(clearObjects) != 0)) {
        cout << "atexit(): atexit() function registration failed" << endl;
        exit(EXIT_FAILURE);
	}

	/* loading and extracting inputs from input file */
	loadData();

	glutMainLoop();  // the main loop of OpenGL
	return 0;
}
