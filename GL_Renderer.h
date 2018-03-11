#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <QOpenGLWidget>
#include <QTimer>
#include <QKeyEvent>

#include <GL/glu.h>

class GL_Renderer : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GL_Renderer(int framesPerSecond = 0, QWidget *parent = 0, char *name = 0);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void keyPressEvent( QKeyEvent *keyEvent);

public slots:
    virtual void timeOutSlot();

private:
    QTimer *t_Timer;

};

#endif // GL_RENDERER_Hs
