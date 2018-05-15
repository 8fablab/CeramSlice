#include "GL_Renderer.h"

using namespace glm;

GL_Renderer::GL_Renderer(int framesPerSecond, QWidget *parent, char *name) : QOpenGLWidget (parent)
{
    setWindowTitle(QString::fromUtf8(name));
    setWindowFlags(Qt::CustomizeWindowHint);

    if(framesPerSecond == 0)
        t_Timer = NULL;
    else
    {
        int seconde = 1000; // 1 seconde = 1000 ms
        int timerInterval = seconde / framesPerSecond;
        t_Timer = new QTimer(this);
        connect(t_Timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
        t_Timer->start( timerInterval );
    }

    Projection = glm::perspective(70.0, (double) this->size().width() / this->size().height(), 1.0, 100.0);
    ModelView = glm::mat4(1.0);

    Cam = new Camera(vec3(6, 6, 6), vec3(0, 0, 0), vec3(0, 1, 0), 0.5, 0.5);

    setFocusPolicy(Qt::StrongFocus);
}


void GL_Renderer::initializeGL()
{
    initializeOpenGLFunctions();
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void GL_Renderer::resizeGL(int width, int height)
{
    if(height == 0)
        height = 1;
    glViewport(0, 0, width, height);

    Projection = glm::perspective(70.0, (double) width / height, 1.0, 100.0);
    ModelView = glm::mat4(1.0);
}

void GL_Renderer::paintGL()
{
    float vertices[] = {-1.0, -1.0, -1.0,   1.0, -1.0, -1.0,   1.0, 1.0, -1.0,     // Face 1
                        -1.0, -1.0, -1.0,   -1.0, 1.0, -1.0,   1.0, 1.0, -1.0,     // Face 1

                        1.0, -1.0, 1.0,   1.0, -1.0, -1.0,   1.0, 1.0, -1.0,       // Face 2
                        1.0, -1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, -1.0,         // Face 2

                        -1.0, -1.0, 1.0,   1.0, -1.0, 1.0,   1.0, -1.0, -1.0,      // Face 3
                        -1.0, -1.0, 1.0,   -1.0, -1.0, -1.0,   1.0, -1.0, -1.0,    // Face 3

                        -1.0, -1.0, 1.0,   1.0, -1.0, 1.0,   1.0, 1.0, 1.0,        // Face 4
                        -1.0, -1.0, 1.0,   -1.0, 1.0, 1.0,   1.0, 1.0, 1.0,        // Face 4

                        -1.0, -1.0, -1.0,   -1.0, -1.0, 1.0,   -1.0, 1.0, 1.0,     // Face 5
                        -1.0, -1.0, -1.0,   -1.0, 1.0, -1.0,   -1.0, 1.0, 1.0,     // Face 5

                        -1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0, -1.0,         // Face 6
                        -1.0, 1.0, 1.0,   -1.0, 1.0, -1.0,   1.0, 1.0, -1.0};      // Face 6

    float couleurs[] = {1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1
                        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 1

                        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2
                        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 2

                        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3
                        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 3

                        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4
                        1.0, 0.0, 0.0,   1.0, 0.0, 0.0,   1.0, 0.0, 0.0,           // Face 4

                        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5
                        0.0, 1.0, 0.0,   0.0, 1.0, 0.0,   0.0, 1.0, 0.0,           // Face 5

                        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0,           // Face 6
                        0.0, 0.0, 1.0,   0.0, 0.0, 1.0,   0.0, 0.0, 1.0};          // Face 6

    Shader shaderCouleur("View/Shaders/couleur3D.vert", "View/Shaders/couleur3D.frag");
    shaderCouleur.charger();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Cam->lookAt(ModelView);

    // Activation du shader
    glUseProgram(shaderCouleur.getProgramID());

    // Envoi des vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);

    // Envoi des couleurs
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, couleurs);
    glEnableVertexAttribArray(1);

    //Transformations
    //ModelView = translate(ModelView, vec3(-0.4, 0.0, 0.0));
    //ModelView = rotate(ModelView, 45.0f, vec3(0.0, 0.0, 1.0));

    glUniformMatrix4fv(glGetUniformLocation(shaderCouleur.getProgramID(), "modelview"), 1, GL_FALSE, value_ptr(ModelView));
    glUniformMatrix4fv(glGetUniformLocation(shaderCouleur.getProgramID(), "projection"), 1, GL_FALSE, value_ptr(Projection));

    // Affichage du triangle
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Désactivation des tableaux Vertex Attrib
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    // Désactivation du shader
    glUseProgram(0);
}

void GL_Renderer::mousePressEvent(QMouseEvent *mouseEvent)
{
    if(mouseEvent->type() == QEvent::MouseButtonPress)
    {
        MousePos.x = mouseEvent->pos().x();
        MousePos.y = mouseEvent->pos().y();
    }
}

void GL_Renderer::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    Cam->orienter(mouseEvent->pos().x()-MousePos.x, mouseEvent->pos().y()-MousePos.y);
    MousePos.x = mouseEvent->pos().x();
    MousePos.y = mouseEvent->pos().y();
}

void GL_Renderer::keyPressEvent(QKeyEvent *keyEvent)
{
    Cam->deplacer(keyEvent);
    switch(keyEvent->key())
    {
        case Qt::Key_Escape:
            close();
            break;
    }
}

void GL_Renderer::timeOutSlot()
{
    paintGL();
}
