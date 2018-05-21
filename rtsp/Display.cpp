#include <functional>
#include <process.h>
#include "Display.h"
#define GLUT_DISABLE_ATEXIT_HACK  
#include "glew/glew.h"  
#include "freeglut\freeglut.h"  


PATTERN_SINGLETON_IMPLEMENT(Display)
void OnReshape(int w,int h)  
{  
    GLfloat aspect =(GLfloat)w/(GLfloat)h;  
    GLfloat nRange=100.0f;  
  
    glViewport(0,0,w,h);  
  
    glMatrixMode(GL_PROJECTION); //����ǰ����ָ��ΪͶӰģʽ  
    glLoadIdentity();  
  
    //������άͶӰ��  
  
    if (w<=h)  
        glOrtho(-nRange,nRange,-nRange*aspect,nRange*aspect,-nRange,nRange);  
    else  
        glOrtho(-nRange,nRange,-nRange/aspect,nRange/aspect,-nRange,nRange);  
}  
void Display::pushBackPict(MyPicture &pict)
{
	if (mPictList.size() > 200)
	{
		return;
	}
	WaitForSingleObject(mMutex, INFINITE);
	mPictList.push_back(std::move(pict));
	ReleaseMutex(mMutex);
}

void Display::onTimer()
{
    //glClear(GL_COLOR_BUFFER_BIT);  
  
    //glRasterPos2i(-50, -50); //ָ����ǰ��դλ��  
	MyPicture pict;
	int ret = Decode::getInstance()->pop(pict);
	if (ret != 0)
	{
		return;
	}
	//glDrawPixels(pict.width, pict.height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pict.data.get());
	mTexID = loadTexture(pict.data.get(), pict.width, pict.height);
	showTexture();
	//mPictList.pop_front();
	//printf("leng:%d %d\n", mPictList.size(), pict.width * pict.height * 3);
  
    //glutSwapBuffers();
	//glutPostRedisplay();
}

void timerFunc(int val)
{
	Display::getInstance()->onTimer();
	glutTimerFunc(30, timerFunc,1);
}

Display::Display()
{
	mMutex = CreateMutex(NULL, FALSE, NULL);
	mTexID = 0;
}

void OnDisPlay(void)  
{  
	Display::getInstance()->showTexture();
	/*
    glClear(GL_COLOR_BUFFER_BIT);  
  
    glColor3f(1,0,0);  
  
    //glRasterPos2i(-3,-3); //ָ����ǰ��դλ��  
    //glBitmap(g_w, g_h, 0, 0, 0, 0, (GLubyte *)ptr);  
    glPointSize(5);  
    glBegin(GL_POINTS);  
    glColor3f(0,1,0);  
    glVertex2i(0,0);  
    glEnd();  
  
    glutSwapBuffers();  */
}  

void glutMainFunc(void *arg)
{
	int argc = 0;
    glutInit(&argc, NULL);  
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  
    glutInitWindowSize(600,480);  
    glutCreateWindow("EXAM601");     
    glEnable(GL_DEPTH_TEST);      
    glEnable(GL_TEXTURE_2D);
  

    glutReshapeFunc(OnReshape);  
    glutDisplayFunc(OnDisPlay);  
	glutTimerFunc(50, timerFunc,1);
    glClearColor(1,1,1,1);  
    glutMainLoop(); 
}

void Display::init()
{
    //Initialization();  
	
    //glutMainLoop();
	_beginthread(glutMainFunc, 0, NULL);
}

Display::~Display()
{
	printf("~Display\n");
	CloseHandle(mMutex);
}

UINT32 Display::loadTexture(UINT8 *pixels, int width, int height)  
{  
    GLint total_bytes;  
    UINT32 last_texture_ID = 0, texture_ID = 0;  
  
	total_bytes = width * height * 3;
  
    // ����һ���µ�������  
    glGenTextures(1, &texture_ID);  
    if( texture_ID == 0 )  
    {  
        return 0;  
    }  
  
    // ���µ������������������������  
    // �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�  
    GLint lastTextureID = last_texture_ID;  
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);  
    glBindTexture(GL_TEXTURE_2D, texture_ID);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);  
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,  
        GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);  
    glBindTexture(GL_TEXTURE_2D, lastTextureID);  //�ָ�֮ǰ�������  
    return texture_ID;  
}  

void Display::showTexture()
{  
    // �����Ļ  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    // �����ӽ�  
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();  
    gluPerspective(90, 1, 1, 21);  
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity();  
    gluLookAt(0, 0,-1, 0, 0, 0, 0, 1, 0);  
  
    glRotatef(180, 0.0f, 0.0f, 1.0f); //��ת  
  
    // ���Ƶ����Լ�����  
    glBindTexture(GL_TEXTURE_2D, mTexID);  
    glBegin(GL_QUADS);  
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);  
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);  
    glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);  
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);  
    glEnd();    
  
    glutSwapBuffers();    
}  

void Initialization(void)  
{  
    glClearColor(1,1,1,1);  
}  
void testDisplay()
{
	Display::getInstance()->init();
	getchar();
	/*int argc = 0;
    glutInit(&argc,NULL);  
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  
    glutInitWindowSize(600,480);  
    glutCreateWindow("EXAM601");  
  
	
	glutTimerFunc(50, timerFunc,1);
    glutReshapeFunc(OnReshape);  
    glutDisplayFunc(OnDisPlay);  
  
    glutMainLoop();  */
}