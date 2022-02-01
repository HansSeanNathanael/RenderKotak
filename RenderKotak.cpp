#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "windows.h"
#include "wingdi.h"
#include "thread"
#include "future"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

double rad = 3.141593 / 180.0;

class Camera
{

private:
    double x;
    double y;
    double z;

    double xSpace;
    double ySpace;
    double zSpace;

    double **matrixProjection;

public:
    Camera(double x, double y, double z, double xSpace, double ySpace, double zSpace)
    {
        this->x = x;
        this->y = y;
        this->z = z;

        this->xSpace = xSpace;
        this->ySpace = ySpace;
        this->zSpace = zSpace;

        this->matrixProjection = new double*[3];
        for (int i = 0; i < 3; i++)
        {
            matrixProjection[i] = new double[3];
        }

        matrixProjection[0][0] = 1; matrixProjection[0][1] = 0; matrixProjection[0][2] = 0;
        matrixProjection[1][0] = 0; matrixProjection[1][1] = 1; matrixProjection[1][2] = 0;
        matrixProjection[2][0] = 0; matrixProjection[2][1] = 0; matrixProjection[2][2] = 1;
    }

    ~Camera()
    {
        for (int i = 0; i < 3; i++)
        {
            delete matrixProjection[i];
        }

        delete matrixProjection;
    }

    double **getMatrixProjection()
    {
        return this->matrixProjection;
    }
};

class Point
{
    double x;
    double y;
    double z;

public:

    Point()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void transform(Point *p, double **mt)
    {
        double x = p->x;
        double y = p->y;
        double z = p->z;

        this->x = x * mt[0][0] + y * mt[0][1] + z * mt[0][2];
        this->y = x * mt[1][0] + y * mt[1][1] + z * mt[1][2];
        this->z = x * mt[2][0] + y * mt[2][1] + z * mt[2][2];

        double distance = 240.00;
        this->x = this->x / (log(z) / log(distance));
        this->y = this->y / (log(z) / log(distance));
//        printf("%lf %lf\n", this->x, this->y);
    }

    void rotatePoint(double xCenter, double yCenter, double zCenter, int xyDegree = 0, int xzDegree = 0, int yzDegree = 0)
    {
//        printf("%lf %lf %lf\n", this->x, this->y, this->z);
        this->x = this->x - xCenter;
        this->y = this->y - yCenter;
        this->z = this->z - zCenter;

        int x, y, z;
        x = this->x;
        y = this->y;
        z = this->z;

        this->x = cos(rad * xyDegree) * x - sin(rad * xyDegree) * y;
        this->y = sin(rad * xyDegree) * x + cos(rad * xyDegree) * y;

        x = this->x;
        y = this->y;
        z = this->z;

        this->x = cos(rad * xzDegree) * x + sin(rad * xzDegree) * z;
        this->z = -1 * sin(rad * xzDegree) * x + cos(rad * xzDegree) * z;

        x = this->x;
        y = this->y;
        z = this->z;

        this->y = cos(rad * yzDegree) * y - sin(rad * yzDegree) * z;
        this->z = sin(rad * yzDegree) * y + cos(rad * yzDegree) * z;

        this->x = this->x + xCenter;
        this->y = this->y + yCenter;
        this->z = this->z + zCenter;
//        printf("%lf %lf %lf\n\n", this->x, this->y, this->z);
    }

    void setPoint(Point *point)
    {
        this->x = point->getX();
        this->y = point->getY();
        this->z = point->getZ();
    }

    double getX()
    {
        return this->x;
    }

    double getY()
    {
        return this->y;
    }

    double getZ()
    {
        return this->z;
    }
};

class Pane
{

private:
    Point *point[4];

    Point *rotatedPoint[4];
    Point *projection[4];

public:
    Pane(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double x4, double y4, double z4)
    {
        this->point[0] = new Point(x1, y1, z1);
        this->point[1] = new Point(x2, y2, z2);
        this->point[2] = new Point(x3, y3, z3);
        this->point[3] = new Point(x4, y4, z4);


        this->rotatedPoint[0] = new Point(x1, y1, z1);
        this->rotatedPoint[1] = new Point(x2, y2, z2);
        this->rotatedPoint[2] = new Point(x3, y3, z3);
        this->rotatedPoint[3] = new Point(x4, y4, z4);

        this->projection[0] = new Point(x1, y1, z1);
        this->projection[1] = new Point(x2, y2, z2);
        this->projection[2] = new Point(x3, y3, z3);
        this->projection[3] = new Point(x4, y4, z4);
    }

    ~Pane()
    {
        for (int i = 0; i < 4; i++)
        {
            delete this->point[i];
            delete this->projection[i];
        }
    }

    Point* getProjection(int point, double **matrixProjection)
    {
        this->projection[point]->transform(this->rotatedPoint[point], matrixProjection);

        return this->projection[point];
    }

    void rotatePane(double xCenter, double yCenter, double zCenter, double xyDegree, double xzDegree, double yzDegree)
    {
        for (int i = 0; i < 4; i++)
        {
//            printf("%lf %lf %lf\n", this->point[i]->getX(), this->point[i]->getY(), this->point[i]->getZ());
            this->rotatedPoint[i]->setPoint(this->point[i]);
            this->rotatedPoint[i]->rotatePoint(xCenter, yCenter, zCenter, xyDegree, xzDegree, yzDegree);
//            printf("%lf %lf %lf\n", this->point[i]->getX(), this->point[i]->getY(), this->point[i]->getZ());
        }
//        printf("\n");
    }

    void getAllCornerPointProjection(double **matrixProjection, Point arrayCornerPoint[], int *width, int *height, int *offsetX, int *offsetY)
    {
        int upLeft, upRight, downLeft, downRight;
        int groupByLowestFirstThenHighest[4];

        for (int i = 0; i < 4; i++)
        {
            this->projection[i]->transform(this->rotatedPoint[i], matrixProjection);
//            printf("%lf %lf\n", this->projection[i]->getX(), this->projection[i]->getY());

            groupByLowestFirstThenHighest[i] = i;
            for (int j = i; j >= 1; j--)
            {
                if (this->projection[groupByLowestFirstThenHighest[j - 1]]->getY() < this->projection[groupByLowestFirstThenHighest[j]]->getY())
                {
                    int temp = groupByLowestFirstThenHighest[j - 1];
                    groupByLowestFirstThenHighest[j - 1] = groupByLowestFirstThenHighest[j];
                    groupByLowestFirstThenHighest[j] = temp;
                }
            }
        }

        if (this->projection[groupByLowestFirstThenHighest[0]]->getX() > this->projection[groupByLowestFirstThenHighest[1]]->getX())
        {
            downRight = groupByLowestFirstThenHighest[0];
            downLeft = groupByLowestFirstThenHighest[1];
        }
        else
        {
            downRight = groupByLowestFirstThenHighest[1];
            downLeft = groupByLowestFirstThenHighest[0];
        }

        if (this->projection[groupByLowestFirstThenHighest[2]]->getX() > this->projection[groupByLowestFirstThenHighest[3]]->getX())
        {
            upRight = groupByLowestFirstThenHighest[2];
            upLeft = groupByLowestFirstThenHighest[3];
        }
        else
        {
            upRight = groupByLowestFirstThenHighest[3];
            upLeft = groupByLowestFirstThenHighest[2];
        }

        for (int i = 0; i < 4; i++)
        {
            arrayCornerPoint[i].setPoint(this->projection[i]);
        }

//        printf("%lf %lf\n", upperLeft->getX(), upperLeft->getY());
//        printf("%lf %lf\n", upperRight->getX(), upperRight->getY());
//        printf("%lf %lf\n", lowerLeft->getX(), lowerLeft->getY());
//        printf("%lf %lf\n", lowerRight->getX(), lowerRight->getY());

        *height = this->projection[groupByLowestFirstThenHighest[0]]->getY() - this->projection[groupByLowestFirstThenHighest[3]]->getY() + 8;
        *width = (this->projection[upRight]->getX() > this->projection[downRight]->getX() ? this->projection[upRight]->getX() : this->projection[downRight]->getX()) -
            (this->projection[upLeft]->getX() < this->projection[downLeft]->getX() ? this->projection[upLeft]->getX() : this->projection[downLeft]->getX()) + 8;

        *offsetY = this->projection[groupByLowestFirstThenHighest[3]]->getY();
        *offsetX = this->projection[upLeft]->getX() < this->projection[downLeft]->getX() ? this->projection[upLeft]->getX() : this->projection[downLeft]->getX();
    }
};

class Light
{

private:
    double x;
    double y;
    double z;

public:
    Light(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

};

class Pixel
{

private:
    int color = RGB(127, 127, 127);
    double depth = -1;

public:

    void setPixel(int color, double depth)
    {
        this->color = color;
        this->depth = depth;
    }

    void setPixel(int color)
    {
        this->color = color;
    }

    void setPixel(const Pixel &pixel)
    {
        if (((pixel.depth < this->depth) && pixel.depth >= 0) || this->depth == -1)
        {
            *this = pixel;
        }
    }

    COLORREF getColor()
    {
        return (COLORREF) this->color;
    }

    void setDepth(double depth)
    {
        this->depth = depth;
    }

    double getDepth()
    {
        return this->depth;
    }
};

Camera *camera = new Camera(0, 360, 0, 1, 1, 0);

Pane *pane[] = {

    new Pane(-128, -128, 240, 128, -128, 240, 128, 128, 240, -128, 128, 240),
    new Pane(-128, -128, 496, 128, -128, 496, 128, 128, 496, -128, 128, 496),

    new Pane(-128, -128, 240, -128, -128, 496, -128, 128, 496, -128, 128, 240),
    new Pane(128, -128, 240, 128, -128, 496, 128, 128, 496, 128, 128, 240),

    new Pane(-128, 128, 240, 128, 128, 240, 128, 128, 496, -128, 128, 496),
    new Pane(-128, -128, 240, 128, -128, 240, 128, -128, 496, -128, -128, 496)

};

Light *light = new Light(192, 168, 176);

Pixel pixel[720][720];

COLORREF paneColor[] = {
    RGB(83, 255, 0), RGB(250, 255, 45), RGB(255, 45, 45),
    RGB(45, 255, 255), RGB(45, 45, 255), RGB(255, 45, 255)
};

struct RotateBlock
{
    void run(HWND hwnd, const RECT* rect)
    {
        int degree1 = 10;
        int degree2 = 5;
        while(true)
        {
            for (int i = 0; i < 6; i++)
            {
                pane[i]->rotatePane(0, 0, 368, 0, degree1, degree2);
            }
            InvalidateRect(hwnd, rect, true);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            degree1 += 12 % 360;
            degree2 += 8 % 360;
//            printf("TES\n");
        }
    }
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"Sample Window Class";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Learn to Program Windows",
        WS_OVERLAPPEDWINDOW,

        // Size and position
        GetSystemMetrics(SM_CXSCREEN) / 2 - 360, GetSystemMetrics(SM_CYSCREEN) / 2 - 360, 720, 720,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    for (int i = 0; i < 720; i++)
    {
        for (int j = 0; j < 720 ; j++)
        {
            pixel[i][j] = Pixel();
        }
    }
    RECT screenRect;
    screenRect.top = 0;
    screenRect.left = 0;
    screenRect.right = GetSystemMetrics(SM_CXSCREEN);
    screenRect.bottom = GetSystemMetrics(SM_CYSCREEN);

    RotateBlock rotateBlock;
    auto tes = std::async(std::launch::async, &RotateBlock::run, &rotateBlock, hwnd, &screenRect);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    delete light;
    for (int i = 0; i < 6; i++)
    {
        delete pane[i];
    }
    delete camera;

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            {

                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);

                for (int i = 0; i < 720; i++)
                {
                    for (int j = 0; j < 720 ; j++)
                    {
                        pixel[i][j] = Pixel();
                    }
                }

                for (int i = 0; i < 6; i++)
                {
//                    printf("TES\n");
                    Point rectPoint[4];
                    int width;
                    int height;

                    int offsetX;
                    int offsetY;

                    pane[i]->getAllCornerPointProjection(camera->getMatrixProjection(), rectPoint, &width, &height, &offsetX, &offsetY);

//                    printf("%d %d\n", height, width);
                    Pixel panePixel[height + 1][width + 1];

                    for (int b = 0; b <= height; b++)
                    {
                        for (int e = 0; e <= width; e++)
                        {
                            panePixel[b][e] = Pixel();
                        }
                    }


//                    printf("offsetY compare: %lf %lf\n", rectPoint[1].getY(), rectPoint[1].getY());
//                    printf("%d %d\n", offsetX, offsetY);

//                    for (int r = 0; r < 4; r++)
//                    {
////                        printf("CORNER: \n");
////                        printf("%d %d\n", (int)(rectPoint[r].getY()) - offsetY, (int)(rectPoint[r].getX() - offsetX));
////                        printf("%d %d\n", (int)(rectPoint[r].getX()), (int)(rectPoint[r].getY()));
////                        panePixel[(int)(rectPoint[r].getY()) - offsetY][(int)(rectPoint[r].getX() - offsetX)].setPixel(RGB(0, 0, 0), rectPoint[r].getZ());
//                    }
////                    printf("\n");


                    for (int h = 0; h < 4; h++)
                    {
                        int next = h+1 < 4 ? h+1 : 0;

                        int startX = rectPoint[h].getX() - offsetX;
                        int startY = rectPoint[h].getY() - offsetY;
                        int finishX = rectPoint[next].getX() - offsetX;
                        int finishY = rectPoint[next].getY() - offsetY;

                        double startZ = rectPoint[h].getZ();
                        double finishZ = rectPoint[next].getZ();

                        int dx = finishX - startX;
                        int dy = finishY - startY;
                        double dz = finishZ - startZ;

                        int length = sqrt(dx*dx + dy*dy);

                        double ddz = dz/length;

                        int prevY = startY;
                        int finalY = rectPoint[next].getY() - offsetY;

//                        printf("%lf %lf\n", dx, dy);
                        int q = 0;
                        while(prevY != finalY)
                        {
                            if (prevY != (int)(startY + (dy * q /length)))
                            {
                                prevY = (int)(startY + (dy * q /length));
                                panePixel[(int)(startY + (dy * q /length))][(int)(startX + (dx * q /length))].setPixel(RGB(0, 0, 0), startZ + ddz * q);
                            }

                            q++;
                        }
                    }

                    for (int b = 0; b <= height; b++)
                    {
                        bool opaque = false;

                        double z1, z2;
                        int x1, x2;

                        for (int e = 0; e <= width; e++)
                        {

                            if (panePixel[b][e].getDepth() != -1)
                            {
                                opaque = !opaque;
                                z1 = z2;
                                z2 = panePixel[b][e].getDepth();

                                x1 = x2;
                                x2 = e;

                                if (opaque == false)
                                {
                                    double ddz = (z2 - z1) / (x2 - x1);

                                    for (int v = x1 + 1; v < x2; v++)
                                    {
                                        panePixel[b][v].setPixel(paneColor[i], z1 + ddz * ((double)(v - x1)));
                                    }
                                }
                            }
                        }
                    }

                    for (int h = 0; h < 4; h++)
                    {
//                        int next = h+1 < 4 ? h+1 : 0;
//
//                        double dx = rectPoint[next].getX() - rectPoint[h].getX();
//                        double dy = rectPoint[next].getY() - rectPoint[h].getY();
//                        double dz = rectPoint[next].getZ() - rectPoint[h].getZ();
//
//                        double itx = rectPoint[h].getX() - ((double)offsetX);
//                        double ity = rectPoint[h].getY() - ((double)offsetY);
//                        double itz = rectPoint[h].getZ();
//
//                        double length = sqrt(dx*dx + dy*dy);
//
//                        double ddx = dx/length;
//                        double ddy = dy/length;
//                        double ddz = dz/length;
//
////                        printf("%lf %lf\n", dx, dy);
//                        for (int q = 0; q < length; q++)
//                        {
//                            panePixel[(int)(ity + ddy * q)][(int)(itx + ddx * q)].setPixel(RGB(0, 0, 0), itz + ddz * q);
//                        }



                        int next = h+1 < 4 ? h+1 : 0;

                        int startX = rectPoint[h].getX() - offsetX;
                        int startY = rectPoint[h].getY() - offsetY;
                        int finishX = rectPoint[next].getX() - offsetX;
                        int finishY = rectPoint[next].getY() - offsetY;

                        double startZ = rectPoint[h].getZ();
                        double finishZ = rectPoint[next].getZ();

                        int dx = finishX - startX;
                        int dy = finishY - startY;
                        double dz = finishZ - startZ;

                        int length = sqrt(dx*dx + dy*dy);

                        double ddz = dz/length;

                        int prevY = startY;
                        int finalY = rectPoint[next].getY() - offsetY;

//                        printf("%lf %lf\n", dx, dy);
                        for (int q = 0; q < length; q++)
                        {
                            panePixel[(int)(startY + (dy * q /length))][(int)(startX + (dx * q /length))].setPixel(RGB(0, 0, 0), startZ + ddz * q);
                        }
                    }


                    for (int b = 0; b <= height; b++)
                    {
                        for (int e = 0; e <= width; e++)
                        {
//                            printf("%d %d\n",  offsetY + b, offsetX + e);
                            pixel[360 + offsetY + b][360 + offsetX + e].setPixel(panePixel[b][e]);
                        }
                    }
                }


                for (int i = 0; i < 720; i++)
                {
                    for (int j = 0; j < 720 ; j++)
                    {
                        SetPixel(hdc, j, i, pixel[i][j].getColor());
                    }
                }

                EndPaint(hwnd, &ps);
            }
            return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
