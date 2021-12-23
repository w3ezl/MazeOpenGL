#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <math.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

HWND hwnd;

float brick[] = {0,0,0, 0,1,0, 1,1,0, 1,0,0, 0,0,1, 0,1,1, 1,1,1, 1,0,1};
GLuint brickInd[] = {0,1,2, 2,3,0, 4,5,6, 6,7,4, 3,6,2, 6,7,3,
0,1,5, 5,4,0, 1,2,6, 6,5,1, 0,3,7, 7,4,0};

typedef struct {
    float r,g,b;
} TColor;

typedef struct {
    TColor clr;
} TCell;

#define pW 21
#define pH 21
TCell map[pW][pH];

void Map_Init()
{
    for (int i = 0; i < pW; i++)
        for (int j = 0; j < pH; j++)
    {
        if (i == 2 & j == 2) {
                map[i][j].clr.b = 0.4;
                map[i][j].clr.g = 0.1;
                map[i][j].clr.r = 0.1;
        }
        else if ((i == 18 & j == 18) | (i == 18 & j == 19) | (i == 19 & j == 19) | (i == 19 & j == 18)) {
                map[i][j].clr.b = 0.1;
                map[i][j].clr.g = 0.1;
                map[i][j].clr.r = 0.4;
        }
        else {
        float dc = (rand() % 20) * 0.002;
        map[i][j].clr.r = 0.85+dc;
        map[i][j].clr.g = 0.72+dc;
        map[i][j].clr.b = 0.47+dc;
        }
    }
}

struct {
    float x,y,z;
    float Xrot,Zrot;
} camera = {2.5,2.5,1.7,70,-40};

void Camera_Apply()
{
    glRotatef(-camera.Xrot, 1,0,0);
    glRotatef(-camera.Zrot, 0,0,1);
    glTranslatef(-camera.x, -camera.y, -camera.z);
}

void Camera_Rotation(float xAngle, float zAngle)
{
    camera.Zrot += zAngle;
    if (camera.Zrot < 0) camera.Zrot += 360;
    if (camera.Zrot > 360) camera.Zrot -= 360;
    camera.Xrot += xAngle;
    if (camera.Xrot < 0) camera.Zrot = 0;
    if (camera.Xrot > 360) camera.Zrot = 180;
}

void Player_Move()
{
    if (GetForegroundWindow() != hwnd) return;

    float ugol = -camera.Zrot / 180 * M_PI;
    float speed = 0;
    if (GetKeyState('W') < 0) speed = 0.1;
    if (GetKeyState('S') < 0) speed = -0.1;
    if (GetKeyState('A') < 0) {speed = 0.1; ugol -= M_PI*0.5;}
    if (GetKeyState('D') < 0) {speed = 0.1; ugol += M_PI*0.5;}
    if (camera.x < 1 | camera.y < 1)
    {
            speed = 0;
            camera.x = camera.x + 0.1;
            camera.y = camera.y + 0.1;
    }
    if (camera.x > pH-1 | camera.y > pW-1)
    {
            speed = 0;
            camera.x = camera.x - 0.1;
            camera.y = camera.y - 0.1;
    }
    if (camera.y > 3.9 & camera.y < 5.1){
        if (camera.x < 15){
            speed = 0;
            if (camera.y > 5) camera.y += 0.1;
            if (camera.y < 4) camera.y -= 0.1;
        }
    }
    if (camera.y > 7.9 & camera.y < 9.1){
        if (camera.x > 4){
            speed = 0;
            if (camera.y > 9) camera.y += 0.1;
            if (camera.y < 8) camera.y -= 0.1;
        }
    }
    if (camera.y > 11.9 & camera.y < 13.1){
        if (camera.x < 15){
            speed = 0;
            if (camera.y > 13) camera.y += 0.1;
            if (camera.y < 12) camera.y -= 0.1;
        }
    }
    if (camera.y > 15.9 & camera.y < 17.1){
        if (camera.x > 4){
            speed = 0;
            if (camera.y > 17) camera.y += 0.1;
            if (camera.y < 16) camera.y -= 0.1;
        }
    }

    if (speed != 0)
    {
        camera.x += sin(ugol)*speed;
        camera.y += cos(ugol)*speed;
    }

    POINT cur;
    static POINT base = {400, 300};
    GetCursorPos(&cur);
    Camera_Rotation((base.y - cur.y)/5.0, (base.x - cur.x)/5.0);
    SetCursorPos(base.x, base.y);
}

void WndResize(int x, int y);

void Game_Move()
{
    Player_Move();
}

void Game_Init()
{
    glEnable(GL_DEPTH_TEST);
    Map_Init();

    RECT rct;
    GetClientRect(hwnd, &rct);
    WndResize(rct.right, rct.bottom);
}

void Game_Show()
{
    glClearColor(0.6, 0.8, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
        Camera_Apply();

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, brick);
        for (int i=0; i<pW; i++)
            for (int j=0; j<pH; j++)
        {
            glPushMatrix();
                glTranslatef(i,j,0);
                glColor3f(map[i][j].clr.r, map[i][j].clr.g, map[i][j].clr.b);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, brickInd);
            glPopMatrix();
        }
        for (int i=0; i<pW; i++)
            for (int z=0; z<3; z++)
            {
                glPushMatrix();
                    glTranslatef(i,0,z);
                    glColor3f(0.8, 0.8, 0.6);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
                glPopMatrix();
            }
        for (int i=0; i<pW; i++)
            for (int z=0; z<3; z++)
            {
                glPushMatrix();
                    glTranslatef(0,i,z);
                    glColor3f(0.8, 0.8, 0.6);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
                glPopMatrix();
            }
        for (int i=0; i<pH; i++)
            for (int z=0; z<3; z++)
            {
                glPushMatrix();
                    glTranslatef(i,pH-1,z);
                    glColor3f(0.8, 0.8, 0.6);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
                glPopMatrix();
            }
        for (int i=0; i<pH; i++)
            for (int z=0; z<3; z++)
            {
                glPushMatrix();
                    glTranslatef(pH-1,i,z);
                    glColor3f(0.8, 0.8, 0.6);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
                glPopMatrix();
            }

        /*Стены лабиринта*/
        for (int j=4; j<pH; j+=4){
                int startWall, endWall;
                if (j==4) {startWall = 1; endWall = 15;}
                if (j==8) {startWall = 4; endWall = 20;}
                if (j==12) {startWall = 1; endWall = 15;}
                if (j==16) {startWall = 4; endWall = 20;}
            for (int i=startWall; i<endWall; i++)
                for (int z=0; z<3; z++)
                {
                    glPushMatrix();
                        glTranslatef(i,j,z);
                        glColor3f(0.8, 0.8, 0.6);
                        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, brickInd);
                    glPopMatrix();
                }
            }

        glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}

void WndResize(int x, int y)
{
    glViewport(0,0, x,y);
    float k = x / (float)y;
    float sz = 0.1;
    glLoadIdentity();
    glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 100);
}

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    Game_Init();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            Game_Move();
            Game_Show();

            SwapBuffers(hDC);

            Sleep(1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_SIZE:
            WndResize(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_SETCURSOR:
            ShowCursor(FALSE);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}


