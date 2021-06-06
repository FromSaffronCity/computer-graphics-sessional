#include<iostream>
#include<fstream>
#include<iomanip>
#include<cmath>
#include<string>
#include<cstdlib>
#include<stack>
#include<ctime>
#include<limits>

/*
    You will find "bitmap_image.hpp" from the following link:
    https://drive.google.com/file/d/14eOfsMpwIuh8G_Wy6BSjNX6VXFRA9eSM/view?usp=sharing
*/

#include "bitmap_image.hpp"

using namespace std;

#define PI 2.0*acos(0.0)
#define INF numeric_limits<double>::infinity()

class Point {
    double x;
    double y;
    double z;
    double w;

public:
    Point() {
        x = y = z = 0.0;
        w = 1.0;
    }

    Point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
        w = 1.0;
    }

    Point(double x, double y, double z, double w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    double getX() const {
        return x;
    }

    void setX(double x) {
        this->x = x;
    }

    double getY() const {
        return y;
    }

    double getZ() const {
        return z;
    }

    double getW() const {
        return w;
    }

    void normalize();
    void scale();

    Point operator+(const Point);
    Point operator-(const Point);
    Point operator*(const double);
    double operator*(const Point);
    Point operator^(const Point);

    friend ostream& operator<<(ostream&, Point&);
    friend ifstream& operator>>(ifstream&, Point&);
    friend ofstream& operator<<(ofstream&, Point&);

    ~Point() {
        x = y = z = 0.0;
        w = 1.0;
    }
};

void Point::normalize() {
    double magnitude = sqrt(pow(x, 2.0)+pow(y, 2.0)+pow(z, 2.0));
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

void Point::scale() {
    x /= w;
    y /= w;
    z /= w;
    w /= w;
}

Point Point::operator+(const Point point) {
    return Point(x+point.x, y+point.y, z+point.z);
}

Point Point::operator-(const Point point) {
    return Point(x-point.x, y-point.y, z-point.z);
}

Point Point::operator*(const double scalar) {
    /* vector scalar multiplication */
    return Point(x*scalar, y*scalar, z*scalar);
}

double Point::operator*(const Point point) {
    /* vector dot multiplication */
    return x*point.x+y*point.y+z*point.z;
}

Point Point::operator^(const Point point) {
    /* vector cross multiplication */
    return Point(y*point.z-z*point.y, z*point.x-x*point.z, x*point.y-y*point.x);
}

ostream& operator<<(ostream &output, Point &point) {
    output << '[' << point.x << ", " << point.y << ", " << point.z << ", " << point.w << ']';
    return output;
}

ifstream& operator>>(ifstream &input, Point &point) {
    input >> point.x >> point.y >> point.z;
    return input;
}

ofstream& operator<<(ofstream &output, Point &point) {
    output << fixed << setprecision(7) << point.x << ' ' << point.y << ' '  << point.z;
    return output;
}

class Transformation {
    double matrix[4][4];

    void generateIdentityMatrix();
    Point applyRodriguesFormula(Point, Point, double);
    void multiplyMatrices(Transformation);

public:
    Transformation() {
        generateIdentityMatrix();
    }

    void generateTranslationMatrix(double, double, double);
    void generateScalingMatrix(double, double, double);
    void generateRotationMatrix(double, double, double, double);
    void generateViewMatrix(Point, Point, Point);
    void generateProjectionMatrix(double, double, double, double);

    Transformation operator*(const Transformation);
    Point operator*(const Point);

    ~Transformation() {
        generateIdentityMatrix();
    }
};

void Transformation::generateIdentityMatrix() {
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            matrix[i][j] = (i == j)? 1.0: 0.0;
        }
    }
}

Point Transformation::applyRodriguesFormula(Point x, Point a, double theta) {
    return x*cos(theta*PI/180.0)+a*(a*x)*(1-cos(theta*PI/180.0))+(a^x)*sin(theta*PI/180.0);
}

void Transformation::multiplyMatrices(Transformation transformation) {
    double temp[4][4];

    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            temp[i][j] = 0.0;

            for(int k=0; k<4; k++) {
                temp[i][j] += matrix[i][k]*transformation.matrix[k][j];
            }
        }
    }
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            matrix[i][j] = temp[i][j];
        }
    }
}

void Transformation::generateTranslationMatrix(double tx, double ty, double tz) {
    matrix[0][3] = tx;
    matrix[1][3] = ty;
    matrix[2][3] = tz;
}

void Transformation::generateScalingMatrix(double sx, double sy, double sz) {
    matrix[0][0] = sx;
    matrix[1][1] = sy;
    matrix[2][2] = sz;
}

void Transformation::generateRotationMatrix(double angle, double ax, double ay, double az) {
    Point a(ax, ay, az);
    a.normalize();

    /* applying vector form of Rodrigues formula to produce column vectors */
    Point c1 = applyRodriguesFormula(Point(1.0, 0.0, 0.0), a, angle);
    Point c2 = applyRodriguesFormula(Point(0.0, 1.0, 0.0), a, angle);
    Point c3 = applyRodriguesFormula(Point(0.0, 0.0, 1.0), a, angle);

    /* generating corresponding rotation matrix R */
    matrix[0][0] = c1.getX();
    matrix[1][0] = c1.getY();
    matrix[2][0] = c1.getZ();

    matrix[0][1] = c2.getX();
    matrix[1][1] = c2.getY();
    matrix[2][1] = c2.getZ();

    matrix[0][2] = c3.getX();
    matrix[1][2] = c3.getY();
    matrix[2][2] = c3.getZ();
}

void Transformation::generateViewMatrix(Point eye, Point look, Point up) {
    /* determining mutually perpendicular unit vectors l, r & u */
    Point l = look-eye;
    l.normalize();
    Point r = l^up;
    r.normalize();
    Point u = r^l;

    /* generating translation matrix T */
    Transformation translationTransformation;
    translationTransformation.generateTranslationMatrix(-eye.getX(), -eye.getY(), -eye.getZ());

    /* generating rotation matrix R */
    matrix[0][0] = r.getX();
    matrix[0][1] = r.getY();
    matrix[0][2] = r.getZ();

    matrix[1][0] = u.getX();
    matrix[1][1] = u.getY();
    matrix[1][2] = u.getZ();

    matrix[2][0] = -l.getX();
    matrix[2][1] = -l.getY();
    matrix[2][2] = -l.getZ();

    /* generating corresponding view matrix V */
    multiplyMatrices(translationTransformation);
}

void Transformation::generateProjectionMatrix(double fovY, double aspectRatio, double near, double far) {
    /* determining parameters fovX, r & t */
    double fovX = fovY*aspectRatio;
    double r = near*tan(fovX/2.0*PI/180.0);
    double t = near*tan(fovY/2.0*PI/180.0);

    /* generating corresponding projection matrix P */
    matrix[0][0] = near/r;
    matrix[1][1] = near/t;
    matrix[2][2] = -(far+near)/(far-near);
    matrix[2][3] = -(2.0*far*near)/(far-near);
    matrix[3][2] = -1.0;
    matrix[3][3] = 0.0;
}

Transformation Transformation::operator*(const Transformation transformation) {
    /* two 4x4 matrices multiplication */
    Transformation temp;
    temp.multiplyMatrices(*this);
    temp.multiplyMatrices(transformation);
    return temp;
}

Point Transformation::operator*(const Point point) {
    /* 4x4 & 4x1 matrices multiplication */
    double temp[4];

    for(int i=0; i<4; i++) {
        temp[i] = 0.0;

        for(int j=0; j<4; j++) {
            temp[i] += matrix[i][j]*((j == 0)? point.getX(): ((j == 1)? point.getY(): ((j == 2)? point.getZ(): point.getW())));
        }
    }
    return Point(temp[0], temp[1], temp[2], temp[3]);
}

struct Color {
    int redValue;
    int greenValue;
    int blueValue;
};

struct Triangle {
    Point corners[3];
    Color rgb;
};

int main(int argc, char** argv) {
    ifstream input;
    ofstream output;

    /* setting test case directory name */
    string testCaseDir = "1";

    /* preparing input for extracting values from scene.txt */
    input.open("./test-cases/"+testCaseDir+"/scene.txt");
    if(!input.is_open()) {
        exit(EXIT_FAILURE);
    }

    /* extracting gluLookAt & gluPerspective function parameters from scene.txt */
    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovY, aspectRatio, near, far;

    input >> eyeX >> eyeY >> eyeZ;
    input >> lookX >> lookY >> lookZ;
    input >> upX >> upY >> upZ;
    input >> fovY >> aspectRatio >> near >> far;

    /* stage1: modeling transformation by analyzing display code */
    string command;

    stack<Transformation> transformationMatrixStack;
    transformationMatrixStack.push(Transformation());

    output.open("./test-cases/"+testCaseDir+"/stage1.txt");
    if(!output.is_open()) {
        exit(EXIT_FAILURE);
    }

    bool bInvalidCommandEncountered, bPopOnEmptyStack;
    bInvalidCommandEncountered = bPopOnEmptyStack = false;

    int triangleCounter, pushCounter;
    triangleCounter = pushCounter = 0;

    while(true) {
        input >> command;

        if(command.compare("triangle") == 0) {
            Point p1, p2, p3;

            input >> p1;
            input >> p2;
            input >> p3;

            p1 = transformationMatrixStack.top()*p1;
            p1.scale();
            p2 = transformationMatrixStack.top()*p2;
            p2.scale();
            p3 = transformationMatrixStack.top()*p3;
            p3.scale();

            output << p1 << endl;
            output << p2 << endl;
            output << p3 << endl;
            output << endl;

            triangleCounter++;
        } else if(command.compare("translate") == 0) {
            double tx, ty, tz;
            input >> tx >> ty >> tz;

            Transformation translationTransformation;
            translationTransformation.generateTranslationMatrix(tx, ty, tz);

            Transformation temp = transformationMatrixStack.top()*translationTransformation;
            transformationMatrixStack.pop();
            transformationMatrixStack.push(temp);
        } else if(command.compare("scale") == 0) {
            double sx, sy, sz;
            input >> sx >> sy >> sz;

            Transformation scalingTransformation;
            scalingTransformation.generateScalingMatrix(sx, sy, sz);

            Transformation temp = transformationMatrixStack.top()*scalingTransformation;
            transformationMatrixStack.pop();
            transformationMatrixStack.push(temp);
        } else if(command.compare("rotate") == 0) {
            double angle, ax, ay, az;
            input >> angle >> ax >> ay >> az;

            Transformation rotationTransformation;
            rotationTransformation.generateRotationMatrix(angle, ax, ay, az);

            Transformation temp = transformationMatrixStack.top()*rotationTransformation;
            transformationMatrixStack.pop();
            transformationMatrixStack.push(temp);
        } else if(command.compare("push") == 0) {
            Transformation temp;
            temp = temp*transformationMatrixStack.top();
            transformationMatrixStack.push(temp);
            pushCounter++;
        } else if(command.compare("pop") == 0) {
            if(pushCounter == 0) {
                cout << command << ": pop on empty stack" << endl;
                bPopOnEmptyStack = true;
                break;
            }

            transformationMatrixStack.pop();
            pushCounter--;
        } else if(command.compare("end") == 0) {
            break;
        } else {
            cout << command << ": invalid command encountered" << endl;
            bInvalidCommandEncountered = true;
            break;
        }
    }
    input.close();
    output.close();

    if(bInvalidCommandEncountered) {
        exit(EXIT_FAILURE);
    }

    if(bPopOnEmptyStack) {
        exit(EXIT_FAILURE);
    }

    /* stage2: view transformation */
    Transformation viewTransformation;
    viewTransformation.generateViewMatrix(Point(eyeX, eyeY, eyeZ), Point(lookX, lookY, lookZ), Point(upX, upY, upZ));

    input.open("./test-cases/"+testCaseDir+"/stage1.txt");
    if(!input.is_open()) {
        exit(EXIT_FAILURE);
    }

    output.open("./test-cases/"+testCaseDir+"/stage2.txt");
    if(!output.is_open()) {
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<triangleCounter; i++) {
        Point p1, p2, p3;

        input >> p1;
        input >> p2;
        input >> p3;

        p1 = viewTransformation*p1;
        p1.scale();
        p2 = viewTransformation*p2;
        p2.scale();
        p3 = viewTransformation*p3;
        p3.scale();

        output << p1 << endl;
        output << p2 << endl;
        output << p3 << endl;
        output << endl;
    }
    input.close();
    output.close();

    /* stage3: projection transformation */
    Transformation projectionTransformation;
    projectionTransformation.generateProjectionMatrix(fovY, aspectRatio, near, far);

    input.open("./test-cases/"+testCaseDir+"/stage2.txt");
    if(!input.is_open()) {
        exit(EXIT_FAILURE);
    }

    output.open("./test-cases/"+testCaseDir+"/stage3.txt");
    if(!output.is_open()) {
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<triangleCounter; i++) {
        Point p1, p2, p3;

        input >> p1;
        input >> p2;
        input >> p3;

        p1 = projectionTransformation*p1;
        p1.scale();
        p2 = projectionTransformation*p2;
        p2.scale();
        p3 = projectionTransformation*p3;
        p3.scale();

        output << p1 << endl;
        output << p2 << endl;
        output << p3 << endl;
        output << endl;
    }
    input.close();
    output.close();

    /* stage4: clipping & scan conversion using z-buffer algorithm */

    /* reading values from config.txt */
    input.open("./test-cases/"+testCaseDir+"/config.txt");
    if(!input.is_open()) {
        exit(EXIT_FAILURE);
    }

    int screenWidth, screenHeight;
    double leftLimitX, rightLimitX, bottomLimitY, topLimitY, frontLimitZ, rearLimitZ;

    input >> screenWidth >> screenHeight;
    input >> leftLimitX;
    input >> bottomLimitY;
    input >> frontLimitZ >> rearLimitZ;

    input.close();

    rightLimitX = -leftLimitX;
    topLimitY = -bottomLimitY;

    /* reading values from stage3.txt */
    input.open("./test-cases/"+testCaseDir+"/stage3.txt");
    if(!input.is_open()) {
        exit(EXIT_FAILURE);
    }

    Triangle triangles[triangleCounter];
    srand(time(0));

    for(int i=0; i<triangleCounter; i++) {
        input >> triangles[i].corners[0];
        input >> triangles[i].corners[1];
        input >> triangles[i].corners[2];

        triangles[i].rgb.redValue = rand()%256;
        triangles[i].rgb.greenValue = rand()%256;
        triangles[i].rgb.blueValue = rand()%256;
    }
    input.close();

    /* initializing z-buffer & frame buffer */
    double dx, dy, topY, bottomY, leftX, rightX;

    dx = (rightLimitX - leftLimitX)/screenWidth;
    dy = (topLimitY - bottomLimitY)/screenHeight;
    topY = topLimitY - dy/2.0;
    bottomY = bottomLimitY + dy/2.0;
    leftX = leftLimitX + dx/2.0;
    rightX = rightLimitX - dx/2.0;

    /*
        How do I declare a 2d array in c++ using new?
        Link: https://stackoverflow.com/questions/936687/how-do-i-declare-a-2d-array-in-c-using-new/
    */

    double** zBuffer = new double*[screenHeight];
    for(int i=0; i<screenHeight; i++) {
        zBuffer[i] = new double[screenWidth];
    }

    for(int row=0; row<screenHeight; row++) {
        for(int column=0; column<screenWidth; column++) {
            zBuffer[row][column] = rearLimitZ;
        }
    }

    Color** frameBuffer = new Color*[screenHeight];
    for(int i=0; i<screenHeight; i++) {
        frameBuffer[i] = new Color[screenWidth];
    }

    for(int row=0; row<screenHeight; row++) {
        for(int column=0; column<screenWidth; column++) {
            frameBuffer[row][column].redValue = 0;
            frameBuffer[row][column].greenValue = 0;
            frameBuffer[row][column].blueValue = 0;
        }
    }

    /* applying procedure */
    for(int i=0, topScanline, bottomScanline; i<triangleCounter; i++) {
        /* finding topScanline & bottomScanline after necessary clipping */
        double maxY = max(triangles[i].corners[0].getY(), max(triangles[i].corners[1].getY(), triangles[i].corners[2].getY()));
        double minY = min(triangles[i].corners[0].getY(), min(triangles[i].corners[1].getY(), triangles[i].corners[2].getY()));

        if(maxY >= topY) {
            topScanline = 0;
        } else {
            topScanline = (int) round((topY - maxY)/dy);
        }
        if(minY <= bottomY) {
            bottomScanline = screenHeight - 1;
        } else {
            bottomScanline = screenHeight - (1 + ((int) round((minY - bottomY)/dy)));
        }

        /* scanning from topScanline to bottomScanline (inclusive) */
        for(int row=topScanline, leftIntersectingColumn, rightIntersectingColumn; row<=bottomScanline; row++) {
            /* defining three intersecting points on triangle's three sides */
            double ys = topY - row*dy;

            Point intersectingPoints[3];
            intersectingPoints[0] = Point(INF, ys, 0, 1);
            intersectingPoints[1] = Point(INF, ys, 1, 2);
            intersectingPoints[2] = Point(INF, ys, 2, 0);

            /* determining intersecting points' x value (if any) */
            for(int j=0; j<3; j++) {
                Point p1 = triangles[i].corners[(int) intersectingPoints[j].getZ()];
                Point p2 = triangles[i].corners[(int) intersectingPoints[j].getW()];

                if(p1.getY() != p2.getY()) {
                    intersectingPoints[j].setX(p1.getX() + (ys - p1.getY())*(p1.getX() - p2.getX())/(p1.getY() - p2.getY()));
                }
            }

            /* filtering out all invalid points (if any) */
            for(int j=0; j<3; j++) {
                Point p1 = triangles[i].corners[(int) intersectingPoints[j].getZ()];
                Point p2 = triangles[i].corners[(int) intersectingPoints[j].getW()];

                if(intersectingPoints[j].getX() != INF) {
                    if(intersectingPoints[j].getX()>max(p1.getX(), p2.getX()) || intersectingPoints[j].getX()<min(p1.getX(), p2.getX()) || intersectingPoints[j].getY()>max(p1.getY(), p2.getY()) || intersectingPoints[j].getY()<min(p1.getY(), p2.getY())) {
                        intersectingPoints[j].setX(INF);
                    }
                }
            }

            /* finding out leftIntersecting & rightIntersecting points */
            int maxIndex, minIndex;
            maxIndex = minIndex = -1;

            double maxX, minX;

            for(int j=0; j<3; j++) {
                if(maxIndex==-1 && minIndex==-1) {
                    if(intersectingPoints[j].getX() != INF) {
                        maxIndex = minIndex = j;
                        maxX = minX = intersectingPoints[j].getX();
                    }
                } else {
                    if(intersectingPoints[j].getX() != INF) {
                        if(intersectingPoints[j].getX() < minX) {
                            minIndex = j;
                            minX = intersectingPoints[j].getX();
                        }
                        if(intersectingPoints[j].getX() > maxX) {
                            maxIndex = j;
                            maxX = intersectingPoints[j].getX();
                        }
                    }
                }
            }

            /* finding leftIntersectingColumn & rightIntersectingColumn after necessary clipping */
            if(intersectingPoints[minIndex].getX() <= leftX) {
                leftIntersectingColumn = 0;
            } else {
                leftIntersectingColumn = (int) round((intersectingPoints[minIndex].getX() - leftX)/dx);
            }
            if(intersectingPoints[maxIndex].getX() >= rightX) {
                rightIntersectingColumn = screenWidth - 1;
            } else {
                rightIntersectingColumn = screenWidth - (1 + ((int) round((rightX - intersectingPoints[maxIndex].getX())/dx)));
            }

            /* determining za & zb values */
            Point p1 = triangles[i].corners[(int) intersectingPoints[minIndex].getZ()];
            Point p2 = triangles[i].corners[(int) intersectingPoints[minIndex].getW()];

            double za = p1.getZ() + (intersectingPoints[minIndex].getY() - p1.getY())*(p2.getZ() - p1.getZ())/(p2.getY() - p1.getY());

            p1 = triangles[i].corners[(int) intersectingPoints[maxIndex].getZ()];
            p2 = triangles[i].corners[(int) intersectingPoints[maxIndex].getW()];

            double zb = p1.getZ() + (intersectingPoints[maxIndex].getY() - p1.getY())*(p2.getZ() - p1.getZ())/(p2.getY() - p1.getY());

            /* scanning from leftIntersectingColumn to rightIntersectingColumn (inclusive) */
            double zp;
            double constantTerm = dx*(zb - za)/(intersectingPoints[maxIndex].getX() - intersectingPoints[minIndex].getX());

            for(int column=leftIntersectingColumn; column<=rightIntersectingColumn; column++) {
                /* calculating z value */
                if(column == leftIntersectingColumn) {
                    zp = za + ((leftX + leftIntersectingColumn*dx) - intersectingPoints[minIndex].getX())*(zb - za)/(intersectingPoints[maxIndex].getX()- intersectingPoints[minIndex].getX());
                } else {
                    zp = zp + constantTerm;
                }

                /* comparing computed z value with current value in zBuffer[row][column] & frontLimitZ and updating zBuffer[row][column] & frameBuffer[row][column] if necessary */
                if(zp>frontLimitZ && zp<zBuffer[row][column]) {
                    zBuffer[row][column] = zp;

                    frameBuffer[row][column].redValue = triangles[i].rgb.redValue;
                    frameBuffer[row][column].greenValue = triangles[i].rgb.greenValue;
                    frameBuffer[row][column].blueValue = triangles[i].rgb.blueValue;
                }
            }
        }
    }

    /* saving outputs */
    bitmap_image bitmapImage(screenWidth, screenHeight);

    for(int row=0; row<screenHeight; row++) {
        for(int column=0; column<screenWidth; column++) {
            bitmapImage.set_pixel(column, row, frameBuffer[row][column].redValue, frameBuffer[row][column].greenValue, frameBuffer[row][column].blueValue);
        }
    }
    bitmapImage.save_image("./test-cases/"+testCaseDir+"/out.bmp");

    output.open("./test-cases/"+testCaseDir+"/z-buffer.txt");
    if(!output.is_open()) {
        exit(EXIT_FAILURE);
    }

    for(int row=0; row<screenHeight; row++) {
        for(int column=0; column<screenWidth; column++) {
            if(zBuffer[row][column] < rearLimitZ) {
                output << zBuffer[row][column] << '\t';
            }
        }
        output << endl;
    }
    output.close();

    /* freeing memory */
    for(int i=0; i<screenHeight; i++) {
        delete[] zBuffer[i];
    }
    delete[] zBuffer;

    for(int i=0; i<screenHeight; i++) {
        delete[] frameBuffer[i];
    }
    delete[] frameBuffer;

    return 0;
}
