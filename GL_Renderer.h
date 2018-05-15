#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <QOpenGLWidget>
#include <QTimer>
#include <QKeyEvent>

#include <QOpenGLFunctions>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glu.h>

#include "View/Shader.h"
#include "View/Camera.h"

class GL_Renderer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GL_Renderer(int framesPerSecond = 0, QWidget *parent = 0, char *name = 0);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void keyPressEvent( QKeyEvent *keyEvent);

public slots:
    virtual void timeOutSlot();

private:
    QTimer *t_Timer;
    glm::mat4 Projection;
    glm::mat4 ModelView;

    glm::vec2 MousePos;

    Camera *Cam;

};

#endif // GL_RENDERER_Hs
