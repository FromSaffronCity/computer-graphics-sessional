#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#include<windows.h>
#include<GL/glut.h>

#define PI 2*acos(0.0)

/* defining Point class */
class Point {
    double x;
    double y;
    double z;

public:
    Point() {
        x = y = z = 0.0;
    }

    Point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    double getZ() const {
        return z;
    }

    double calculateDistanceFromCenter();
    double calculateDistanceBetween(Point point);
    Point operator+(const Point _vector);
    Point operator-(const Point _vector);
    Point operator*(const double scalar);
    Point operator/(const double scalar);
    double operator^(const Point _vector);

    ~Point() {
        x = y = z = 0.0;
    }
};

double Point::calculateDistanceFromCenter() {
    return sqrt(pow(x, 2)+pow(y, 2)+pow(z, 2));
}

double Point::calculateDistanceBetween(Point point) {
    return sqrt(pow(x-point.x, 2)+pow(y-point.y, 2)+pow(z-point.z, 2));
}

Point Point::operator+(const Point _vector) {
    return Point(x+_vector.x, y+_vector.y, z+_vector.z);
}

Point Point::operator-(const Point _vector) {
    return Point(x-_vector.x, y-_vector.y, z-_vector.z);
}

Point Point::operator*(const double scalar) {
    /* vector scalar multiplication */
    return Point(x*scalar, y*scalar, z*scalar);
}

Point Point::operator/(const double scalar) {
    /* vector scalar division */
    return Point(x/scalar, y/scalar, z/scalar);
}

double Point::operator^(const Point _vector) {
    /* vector dot multiplication */
    return x*_vector.x+y*_vector.y+z*_vector.z;
}

/* defining drawing functions */
void drawSquare(double a) {
	glBegin(GL_LINES);
	{
		glVertex3f(-a, a, 0);
		glVertex3f( a, a, 0);

		glVertex3f( a, a, 0);
		glVertex3f( a,-a, 0);

		glVertex3f( a,-a, 0);
		glVertex3f(-a,-a, 0);

		glVertex3f(-a,-a, 0);
		glVertex3f(-a, a, 0);
	}
	glEnd();
}

void drawCircle(double radius, int segments) {
    Point points[segments+1];

    /* generate points */
    for(int i=0; i<=segments; i++) {
        points[i] = Point(radius*cos(((double)i/(double)segments)*2*PI), radius*sin(((double)i/(double)segments)*2*PI), 0);
    }

    /* draw segments using generated points */
    for(int i=0; i<segments; i++) {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].getX(), points[i].getY(), points[i].getZ());
			glVertex3f(points[i+1].getX(), points[i+1].getY(), points[i+1].getZ());
        }
        glEnd();
    }
}

/* defining Bubble class */
class Bubble {
    Point center;
    double radius;
    Point movementVector;
    bool isInsideCircle;

public:
    Bubble() {
        center = Point(0, 0, 0);
        radius = 0.0;
        movementVector = Point(0, 0, 0);
        isInsideCircle = false;
    }

    Bubble(Point center, double radius) {
        this->center = center;
        this->radius = radius;
        movementVector = Point(0, 0, 0);
        isInsideCircle = false;
    }

    Point getCenter() const {
        return center;
    }

    double getRadius() const {
        return radius;
    }

    Point getMovementVector() const {
        return movementVector;
    }

    void setMovementVector(Point movementVector) {
        this->movementVector = movementVector;
    }

    bool getIsInsideCircle() const {
        return isInsideCircle;
    }

    void setIsInsideCircle() {
        this->isInsideCircle = true;
    }

    void updateCenter();
    void drawBubble();

    ~Bubble() {
        center = Point(0, 0, 0);
        radius = 0.0;
        movementVector = Point(0, 0, 0);
        isInsideCircle = false;
    }
};

void Bubble::updateCenter() {
    center = center+movementVector;
}

void Bubble::drawBubble() {
    glTranslatef(center.getX(), center.getY(), center.getZ());
    glColor3f(1, 0.5, 0);  // orange or brown
    drawCircle(radius, 32);
}

/* defining global parameters */

/* border_length = half side of square */
double border_length;

/* border_radius = radius of inner circle */
double border_radius;

/* bubble_radius = radius of each bubble */
double bubble_radius;

/* bubbles = array of 5 Bubble objects */
Bubble bubbles[5];

/* time_interval = counter for creating delay before displaying a new bubble */
int time_interval;

/* velocity_multiplier = multiplier for controlling bubbles' velocity */
double velocity_multiplier;

/* multiplication_counter = counter for keeping track of velocity_multiplier */
int multiplication_counter;

/* mul_upper_bound = upper bound for multiplication_counter */
int mul_upper_bound;

/* mul_lower_bound = lower bound for multiplication_counter */
int mul_lower_bound;

/* should_pause = true: pause; false: resume; the movement of bubbles */
bool should_pause;

/* defining listener functions */
void keyboardListener(unsigned char key, int x, int y) {
	switch(key) {
		case 'p':
		    should_pause = !should_pause;
			break;
		default:
		    break;
	}
}

void specialKeyListener(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_DOWN:
            /* down arrow key */
            if(multiplication_counter > mul_lower_bound) {
                multiplication_counter--;
                for(int i=0; i<5; i++) {
                    bubbles[i].setMovementVector(bubbles[i].getMovementVector()*(1.0/velocity_multiplier));
                }
            }
			break;
		case GLUT_KEY_UP:
            /* up arrow key */
            if(multiplication_counter < mul_upper_bound) {
                multiplication_counter++;
                for(int i=0; i<5; i++) {
                    bubbles[i].setMovementVector(bubbles[i].getMovementVector()*velocity_multiplier);
                }
            }
			break;
		default:
			break;
	}
}

void mouseListener(int button, int state, int x, int y) {
    /* x, y is the x-y of the screen (2D) */
	switch(button) {
		default:
			break;
	}
}

/* defining auxiliary functions */
void display() {
	/* clear the display */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);  //color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* setting up camera here */

	/* load the correct matrix -- MODEL-VIEW matrix */
	glMatrixMode(GL_MODELVIEW);

	/* initialize the matrix */
	glLoadIdentity();

	/*
        now give three info:
            1. where is the camera (viewer)?
            2. where is the camera looking?
            3. Which direction is the camera's UP direction?
	*/

	/* set the camera by providing aforementioned parameters */
	gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);

	/* again select MODEL-VIEW */
	glMatrixMode(GL_MODELVIEW);

    /* adding objects from here */

	/* adding boundaries */
	glColor3f(0, 1, 0);
    drawSquare(border_length);
    glColor3f(1, 0, 0);
    drawCircle(border_radius, 32);

    /* adding bubbles */
    for(int i=0; i<5; i++) {
        if(i*5000 <= time_interval) {
            glPushMatrix();
            {
                bubbles[i].drawBubble();
            }
            glPopMatrix();
        }
	}

	/* ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE) */
	glutSwapBuffers();
}

void animate() {
    /* checking whether two bubbles inside the circle border collide with each other */
    for(int i=0; i<5; i++) {
        if(!bubbles[i].getIsInsideCircle()) {
            continue;
        }

        for(int j=0; j<5; j++) {
            if(i==j || !bubbles[j].getIsInsideCircle()) {
                continue;
            }

            Point iCenter = bubbles[i].getCenter()+bubbles[i].getMovementVector();
            Point jCenter = bubbles[j].getCenter()+bubbles[j].getMovementVector();

            if(iCenter.calculateDistanceBetween(jCenter) <= bubbles[i].getRadius()+bubbles[j].getRadius()) {
                Point normalVector = (iCenter-jCenter)/(iCenter-jCenter).calculateDistanceFromCenter();
                bubbles[i].setMovementVector(bubbles[i].getMovementVector()-normalVector*(bubbles[i].getMovementVector()^normalVector)*2.0);

                normalVector = (jCenter-iCenter)/(jCenter-iCenter).calculateDistanceFromCenter();
                bubbles[j].setMovementVector(bubbles[j].getMovementVector()-normalVector*(bubbles[j].getMovementVector()^normalVector)*2.0);
            }
        }
    }

    /* checking whether a bubble goes outside the circle border */
    for(int i=0; i<5; i++) {
        if(!bubbles[i].getIsInsideCircle()) {
            continue;
        }

        Point reflectionPoint = bubbles[i].getCenter()+bubbles[i].getMovementVector();
        if(reflectionPoint.calculateDistanceFromCenter() > border_radius-bubbles[i].getRadius()) {
            Point normalVector = (Point(0, 0, 0)-reflectionPoint)/reflectionPoint.calculateDistanceFromCenter();
            bubbles[i].setMovementVector(bubbles[i].getMovementVector()-normalVector*(bubbles[i].getMovementVector()^normalVector)*2.0);
        }
    }

    /* checking whether a bubble goes outside the square border */
    for(int i=0; i<5; i++) {
        if(bubbles[i].getCenter().getX()+bubbles[i].getMovementVector().getX()+bubbles[i].getRadius() > border_length) {
            if(bubbles[i].getCenter().getY()+bubbles[i].getMovementVector().getY()+bubbles[i].getRadius() > border_length) {
                bubbles[i].setMovementVector(Point(-bubbles[i].getMovementVector().getX(), -bubbles[i].getMovementVector().getY(), bubbles[i].getMovementVector().getZ()));
            } else if(bubbles[i].getCenter().getY()+bubbles[i].getMovementVector().getY()-bubbles[i].getRadius() < -border_length) {
                bubbles[i].setMovementVector(Point(-bubbles[i].getMovementVector().getX(), -bubbles[i].getMovementVector().getY(), bubbles[i].getMovementVector().getZ()));
            } else {
                bubbles[i].setMovementVector(Point(-bubbles[i].getMovementVector().getX(), bubbles[i].getMovementVector().getY(), bubbles[i].getMovementVector().getZ()));
            }
        } else if(bubbles[i].getCenter().getX()+bubbles[i].getMovementVector().getX()-bubbles[i].getRadius() < -border_length) {
            if(bubbles[i].getCenter().getY()+bubbles[i].getMovementVector().getY()+bubbles[i].getRadius() > border_length) {
                bubbles[i].setMovementVector(Point(-bubbles[i].getMovementVector().getX(), -bubbles[i].getMovementVector().getY(), bubbles[i].getMovementVector().getZ()));
            } else if(bubbles[i].getCenter().getY()+bubbles[i].getMovementVector().getY()-bubbles[i].getRadius() < -border_length) {
                bubbles[i].setMovementVector(Point(-bubbles[i].getMovementVector().getX(), -bubbles[i].getMovementVector().getY(), bubbles[i].getMovementVector().getZ()));
            } else {
                bubbles[i].setMovementVector(Point(-bubbles[i].getMovementVector().getX(), bubbles[i].getMovementVector().getY(), bubbles[i].getMovementVector().getZ()));
            }
        } else if(bubbles[i].getCenter().getY()+bubbles[i].getMovementVector().getY()+bubbles[i].getRadius() > border_length) {
            bubbles[i].setMovementVector(Point(bubbles[i].getMovementVector().getX(), -bubbles[i].getMovementVector().getY(), bubbles[i].getMovementVector().getZ()));
        } else if(bubbles[i].getCenter().getY()+bubbles[i].getMovementVector().getY()-bubbles[i].getRadius() < -border_length) {
            bubbles[i].setMovementVector(Point(bubbles[i].getMovementVector().getX(), -bubbles[i].getMovementVector().getY(), bubbles[i].getMovementVector().getZ()));
        }
    }

    /* changing bubbles position by updating center */
    for(int i=0; i<5 && i*5000<=time_interval && !should_pause; i++) {
        bubbles[i].updateCenter();
	}

	/* checking whether a bubble is inside the circle border */
	for(int i=0; i<5; i++) {
        if(!bubbles[i].getIsInsideCircle() && bubbles[i].getCenter().calculateDistanceFromCenter()<=border_radius-bubbles[i].getRadius()) {
            bubbles[i].setIsInsideCircle();
        }
	}

	/* incrementing time_interval counter */
	if(time_interval<20000 && !should_pause) {
        time_interval++;
	}

	/* codes for any changes in Models, Camera */
	glutPostRedisplay();
}

void init() {
	/* codes for initialization */
	border_length = 65.0;
	border_radius = 45.0;
	bubble_radius = 6;

	for(int i=0; i<5; i++) {
        bubbles[i] = Bubble(Point(-border_length+bubble_radius, -border_length+bubble_radius, 0.0), bubble_radius);
	}

	time_interval = 0;
	velocity_multiplier = 2.0;
	multiplication_counter = 0;
	mul_upper_bound = 5;
	mul_lower_bound = -5;
	should_pause = false;

	/* seeding pseudo-random number generator */
	srand(time(0));

	/* setting movementVector for each bubble */
	for(int i=0; i<5; i++) {
        double vel_x = (double) rand()/(double) RAND_MAX;
        double vel_y = (double) rand()/(double) RAND_MAX;
        bubbles[i].setMovementVector(Point(0.003*vel_x/sqrt(pow(vel_x, 2)+pow(vel_y, 2)), 0.003*vel_y/sqrt(pow(vel_x, 2)+pow(vel_y, 2)), 0));
	}

	/* clear the screen */
	glClearColor(0, 0, 0, 0);

	/* setting up projection here */

	/* load the PROJECTION matrix */
	glMatrixMode(GL_PROJECTION);

	/* initialize the matrix */
	glLoadIdentity();

	/*
        give PERSPECTIVE parameters:
            1. field of view in the Y (vertically)
            2. aspect ratio that determines the field of view in the X direction (horizontally)
            3. near distance
            4. far distance
    */
	gluPerspective(80, 1, 1.0, 1000.0);
}

/* defining main function */
int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);  //Depth, Double buffer, RGB color

	glutCreateWindow("Problem-2: Bubbles");

	init();  // initialization

	glEnable(GL_DEPTH_TEST);  //enable Depth Testing

	glutDisplayFunc(display);  //display callback function
	glutIdleFunc(animate);  //what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();  //The main loop of OpenGL

	return 0;
}
