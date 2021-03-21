#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include<windows.h>
#include<GL/glut.h>

#include<vector>

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

    Point operator+(const Point _vector);
    Point operator-(const Point _vector);
    Point operator*(const double scalar);
    Point operator^(const Point _vector);

    ~Point() {
        x = y = z = 0.0;
    }
};

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

Point Point::operator^(const Point _vector) {
    /* vector cross multiplication */
    return Point(y*_vector.z-z*_vector.y, z*_vector.x-x*_vector.z, x*_vector.y-y*_vector.x);
}

/* defining global parameters */

/* draw_axes = true: draw axes; false: do not draw axes; on the screen */
bool draw_axes;

/* pos = position of camera */
Point pos;

/* u = unit vector in up direction of camera */
Point u;

/* r = unit vector in right direction of camera */
Point r;

/* l = unit vector in look direction of camera */
Point l;

/* theta_gun = movement angle of the gun around z-axis in degree */
double theta_gun;

/* theta_barrel_vertical = vertical movement angle of the barrel (with eastern hemisphere) around x-axis in degree */
double theta_barrel_vertical;

/* theta_barrel_vertical_only = vertical movement angle of the barrel (without eastern hemisphere) around x-axis in degree */
double theta_barrel_vertical_only;

/* theta_barrel_rotate = rotation movement angle of the barrel (without eastern hemisphere) around y-axis in degree */
double theta_barrel_rotate;

/* bullets = a vector containing all the bullets that hit the wall after gun firing */
std::vector<Point> bullets;

/* defining drawing functions */
void drawAxes(double axis_length) {
    if(!draw_axes) {
        return ;
    }

	glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    {
        glVertex3f(axis_length, 0.0, 0.0);
        glVertex3f(-axis_length, 0.0, 0.0);

        glVertex3f(0.0, -axis_length, 0.0);
        glVertex3f(0.0, axis_length, 0.0);

        glVertex3f(0.0, 0.0, -axis_length);
        glVertex3f(0.0, 0.0, axis_length);
    }
    glEnd();
}

void drawSquare(double half_side, double distance_from_origin, float r, float g, float b) {
    glColor3f(r, g, b);
	glBegin(GL_QUADS);
	{
		glVertex3f(half_side, distance_from_origin, half_side);
		glVertex3f(half_side, distance_from_origin, -half_side);
		glVertex3f(-half_side, distance_from_origin, -half_side);
		glVertex3f(-half_side, distance_from_origin, half_side);
	}
	glEnd();
}

void drawWesternHemisphere(double radius, int segments, int stacks) {
	Point points[stacks+1][segments+1];
	double height, _radius;

	/* generate points: segments = segments in plane; stacks = segments in hemisphere */
	for(int i=0; i<=stacks; i++) {
		height = radius*sin(((double)i/(double)stacks)*(PI/2));
		_radius = radius*cos(((double)i/(double)stacks)*(PI/2));
		for(int j=0; j<=segments; j++) {
            points[i][j] = Point(_radius*cos(((double)j/(double)segments)*2*PI), -height, _radius*sin(((double)j/(double)segments)*2*PI));
		}
	}

	/* draw quads using generated points */
	for(int i=0; i<stacks; i++) {
		for(int j=0; j<segments; j++) {
            /* ensuring alternating colors in two adjacent segments */
            glColor3f(j%2, j%2, j%2);

            /* drawing western hemisphere */
			glBegin(GL_QUADS);
			{
				glVertex3f(points[i][j].getX(), points[i][j].getY(), points[i][j].getZ());
				glVertex3f(points[i][j+1].getX(), points[i][j+1].getY(), points[i][j+1].getZ());
				glVertex3f(points[i+1][j+1].getX(), points[i+1][j+1].getY(), points[i+1][j+1].getZ());
				glVertex3f(points[i+1][j].getX(), points[i+1][j].getY(), points[i+1][j].getZ());
			}
			glEnd();
		}
	}
}

void drawEasternHemisphere(double radius, int segments, int stacks) {
	Point points[stacks+1][segments+1];
	double height, _radius;

	/* generate points: segments = segments in plane; stacks = segments in hemisphere */
	for(int i=0; i<=stacks; i++) {
		height = radius*sin(((double)i/(double)stacks)*(PI/2));
		_radius = radius*cos(((double)i/(double)stacks)*(PI/2));
		for(int j=0; j<=segments; j++) {
            points[i][j] = Point(_radius*cos(((double)j/(double)segments)*2*PI), -height, _radius*sin(((double)j/(double)segments)*2*PI));
		}
	}

	/* draw quads using generated points */
	for(int i=0; i<stacks; i++) {
		for(int j=0; j<segments; j++) {
            /* ensuring alternating colors in two adjacent segments */
            glColor3f(j%2, j%2, j%2);

            /* drawing eastern hemisphere */
			glBegin(GL_QUADS);
			{
                glVertex3f(points[i][j].getX(), -points[i][j].getY(), points[i][j].getZ());
				glVertex3f(points[i][j+1].getX(), -points[i][j+1].getY(), points[i][j+1].getZ());
				glVertex3f(points[i+1][j+1].getX(), -points[i+1][j+1].getY(), points[i+1][j+1].getZ());
				glVertex3f(points[i+1][j].getX(), -points[i+1][j].getY(), points[i+1][j].getZ());
			}
			glEnd();
		}
	}
}

void drawCylinder(double radius, int segments, double height) {
    Point points[segments+1][2];

    /* generate points */
    for(int i=0; i<=segments; i++) {
        points[i][0] = Point(radius*cos(((double)i/(double)segments)*2*PI), 0, radius*sin(((double)i/(double)segments)*2*PI));
        points[i][1] = Point(radius*cos(((double)i/(double)segments)*2*PI), height, radius*sin(((double)i/(double)segments)*2*PI));
    }

    /* drawing segmented quads using generated points */
    for(int i=0; i<segments; i++) {
        /* ensuring alternating colors in two adjacent segments */
        glColor3f(i%2, i%2, i%2);

        /* drawing segmented quads */
        glBegin(GL_QUADS);
        {
			glVertex3f(points[i][0].getX(), points[i][0].getY(), points[i][0].getZ());
			glVertex3f(points[i][1].getX(), points[i][1].getY(), points[i][1].getZ());
			glVertex3f(points[i+1][1].getX(), points[i+1][1].getY(), points[i+1][1].getZ());
			glVertex3f(points[i+1][0].getX(), points[i+1][0].getY(), points[i+1][0].getZ());
        }
        glEnd();
    }
}

void drawFunnel(double radius, int segments, int stacks) {
	Point points[stacks+1][segments+1];
	double height, _radius;

	/* generate points: segments = segments in plane; stacks = segments in hemisphere */
	for(int i=0; i<=stacks; i++) {
		height = radius*sin(((double)i/(double)stacks)*(PI/2));
		/* modified line from drawEasternHemisphere() below */
		_radius = radius+(radius-radius*cos(((double)i/(double)stacks)*(PI/2)));

		for(int j=0; j<=segments; j++) {
            points[i][j] = Point(_radius*cos(((double)j/(double)segments)*2*PI), -height, _radius*sin(((double)j/(double)segments)*2*PI));
		}
	}

	/* draw quads using generated points */
	for(int i=0; i<stacks; i++) {
		for(int j=0; j<segments; j++) {
            /* ensuring alternating colors in two adjacent segments */
            glColor3f(j%2, j%2, j%2);

            /* drawing funnel */
			glBegin(GL_QUADS);
			{
                glVertex3f(points[i][j].getX(), -points[i][j].getY(), points[i][j].getZ());
				glVertex3f(points[i][j+1].getX(), -points[i][j+1].getY(), points[i][j+1].getZ());
				glVertex3f(points[i+1][j+1].getX(), -points[i+1][j+1].getY(), points[i+1][j+1].getZ());
				glVertex3f(points[i+1][j].getX(), -points[i+1][j].getY(), points[i+1][j].getZ());
			}
			glEnd();
		}
	}
}

/* defining listener functions */
void keyboardListener(unsigned char key, int x, int y) {
	switch(key) {
		case '1':
		    r = r*cos(PI/180)+(u^r)*sin(PI/180);
		    l = l*cos(PI/180)+(u^l)*sin(PI/180);
			break;
        case '2':
		    r = r*cos(-PI/180)+(u^r)*sin(-PI/180);
		    l = l*cos(-PI/180)+(u^l)*sin(-PI/180);
			break;
        case '3':
		    l = l*cos(PI/180)+(r^l)*sin(PI/180);
		    u = u*cos(PI/180)+(r^u)*sin(PI/180);
			break;
        case '4':
		    l = l*cos(-PI/180)+(r^l)*sin(-PI/180);
		    u = u*cos(-PI/180)+(r^u)*sin(-PI/180);
			break;
        case '5':
		    u = u*cos(PI/180)+(l^u)*sin(PI/180);
		    r = r*cos(PI/180)+(l^r)*sin(PI/180);
			break;
        case '6':
		    u = u*cos(-PI/180)+(l^u)*sin(-PI/180);
		    r = r*cos(-PI/180)+(l^r)*sin(-PI/180);
			break;
        case 'q':
            theta_gun = (theta_gun<45? theta_gun+1: theta_gun);
            break;
        case 'w':
            theta_gun = (theta_gun>-45? theta_gun-1: theta_gun);
            break;
        case 'e':
            theta_barrel_vertical = (theta_barrel_vertical<45? theta_barrel_vertical+1: theta_barrel_vertical);
            break;
        case 'r':
            theta_barrel_vertical = (theta_barrel_vertical>-45? theta_barrel_vertical-1: theta_barrel_vertical);
            break;
        case 'a':
            theta_barrel_vertical_only = (theta_barrel_vertical_only<45? theta_barrel_vertical_only+1: theta_barrel_vertical_only);
            break;
        case 's':
            theta_barrel_vertical_only = (theta_barrel_vertical_only>-45? theta_barrel_vertical_only-1: theta_barrel_vertical_only);
            break;
        case 'd':
            theta_barrel_rotate = (theta_barrel_rotate<45? theta_barrel_rotate+1: theta_barrel_rotate);
            break;
        case 'f':
            theta_barrel_rotate = (theta_barrel_rotate>-45? theta_barrel_rotate-1: theta_barrel_rotate);
            break;
		default:
		    break;
	}
}

void specialKeyListener(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_DOWN:
            /* down arrow key */
            pos = pos-l;
			break;
		case GLUT_KEY_UP:
            /* up arrow key */
            pos = pos+l;
			break;
		case GLUT_KEY_RIGHT:
		    /* right arrow key */
		    pos = pos+r;
			break;
		case GLUT_KEY_LEFT:
		    /* left arrow key */
		    pos = pos-r;
			break;
		case GLUT_KEY_PAGE_UP:
		    pos = pos+u;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos = pos-u;
			break;
		default:
			break;
	}
}

void mouseListener(int button, int state, int x, int y) {
    /* x, y is the x-y of the screen (2D) */
	switch(button) {
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN) {
                /* NOTICE: gun firing task */

                /* finding out `p` point */
                Point p = Point(30.0*cos(PI*theta_barrel_vertical/180.0)*cos(PI*(90+theta_gun)/180.0), 30.0*cos(PI*theta_barrel_vertical/180.0)*sin(PI*(90+theta_gun)/180.0), 30.0*sin(PI*theta_barrel_vertical/180.0));

                /* finding out `q` point */
                Point pq = Point(120.0*cos(PI*(theta_barrel_vertical+theta_barrel_vertical_only)/180.0)*cos(PI*(90+theta_gun)/180.0), 120.0*cos(PI*(theta_barrel_vertical+theta_barrel_vertical_only)/180.0)*sin(PI*(90+theta_gun)/180.0), 120.0*sin(PI*(theta_barrel_vertical+theta_barrel_vertical_only)/180.0));
                /* new point = old point + vector from old point to new point */
                Point q = p + pq;

                /* finding out intersection point between pq line of barrel and wall */
                if(q.getY()-p.getY() == 0) {
                    /* there is no intersection point, pq line of barrel and wall are parallel to each other */
                } else {
                    /* there may be an intersection point between pq line of barrel and wall */
                    double t = (350.0-p.getY())/(q.getY()-p.getY());

                    /* determining the probable intersection point `r` */
                    Point r = Point(p.getX()+t*(q.getX()-p.getX()), p.getY()+t*(q.getY()-p.getY()), p.getZ()+t*(q.getZ()-p.getZ()));

                    /* checking whether `r` point lies on the wall */
                    bool isOnWall = true;

                    if((r.getX()>150.0 || r.getX()<-150.0) || (r.getZ()>150.0 || r.getZ()<-150.0)) {
                        isOnWall = false;
                    }

                    /* adding bullet on wall if applicable */
                    if(isOnWall) {
                        bullets.push_back(Point(r.getX(), r.getY()-1.0, r.getZ()));
                    }
                }
			}
			break;
        case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN) {
                draw_axes = !draw_axes;
			}
			break;
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
	gluLookAt(pos.getX(), pos.getY(), pos.getZ(), pos.getX()+l.getX(), pos.getY()+l.getY(), pos.getZ()+l.getZ(), u.getX(), u.getY(), u.getZ());

	/* again select MODEL-VIEW */
	glMatrixMode(GL_MODELVIEW);

    /* adding objects from here */

	/* adding axes and wall */
	drawAxes(300.0);
    drawSquare(150.0, 350.0, 0.5, 0.5, 0.5);

    /* adding geometric objects with appropriate transformation */
    glPushMatrix();
    {
        glRotatef(theta_gun, 0, 0, 1);
        drawWesternHemisphere(30.0, 96, 32);

        glRotatef(theta_barrel_vertical, 1, 0, 0);
        drawEasternHemisphere(30.0, 96, 32);

        glTranslatef(0.0, 30.0, 0.0);

        glRotatef(theta_barrel_vertical_only, 1, 0, 0);
        glRotatef(theta_barrel_rotate, 0, 1, 0);

        glTranslatef(0.0, 10.0, 0.0);
        drawWesternHemisphere(10.0, 64, 32);
        drawCylinder(10.0, 64, 100.0);

        glTranslatef(0.0, 100.0, 0.0);
        drawFunnel(10.0, 64, 32);
    }
    glPopMatrix();

    /* adding bullets on wall */
    for(int i=0; i<bullets.size(); i++) {
        glPushMatrix();
        {
            glTranslatef(bullets[i].getX(), bullets[i].getY(), bullets[i].getZ());
            drawSquare(5.0, 0.0, 1.0, 0.0, 0.0);
        }
        glPopMatrix();
    }

	/* ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE) */
	glutSwapBuffers();
}

void animate() {
	/* codes for any changes in Models, Camera */
	glutPostRedisplay();
}

void init() {
	/* codes for initialization */
	draw_axes = true;

	pos = Point(100.0, 100.0, 0.0);
	u = Point(0.0, 0.0, 1.0);
	r = Point(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0.0);
	l = Point(-1.0/sqrt(2.0), -1.0/sqrt(2.0), 0.0);

	theta_gun = theta_barrel_vertical = theta_barrel_vertical_only = theta_barrel_rotate = 0.0;

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

	glutCreateWindow("Problem-1: Fully Controllable Camera & Gun");

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
