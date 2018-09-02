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

    VBO_Id = 0;
    ReloadVertices = false;
    ModelSize = 0;

    Projection = glm::perspective(70.0, (double) this->size().width() / this->size().height(), 1.0, 100.0);
    ModelView = glm::mat4(1.0);

    Cam = new Camera(vec3(10.0, 10.0, 10.0), vec3(6.0, 6.0, 0.0), vec3(0, 1, 0), 0.05, 0.05);

    setFocusPolicy(Qt::StrongFocus);
}

void GL_Renderer::UpdateModel(double ViewPercentage)
{
    /*glGenBuffers(1, &VBO_Id);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Id);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */

    Vertices = new float[Data.size()*3];
    Colors = new float[Data.size()*3];
    ModelSize = Data.size();

    MeanPosition_X = 0;
    MeanPosition_Y = 0;

    int j = 0;
    int MaxLayer = Data.size()*ViewPercentage;

    for(int i = 0; i < MaxLayer; i++)
    {
        Vertices[j] = Data[i].x() /60.0;
        Vertices[j+1] = Data[i].y()/60.0;
        Vertices[j+2] = Data[i].z()/60.0;

        Colors[j] = 1.0-((float)i/(float)Data.size());
        Colors[j+1] = ((float)i/(float)Data.size());
        Colors[j+2] = 1.0-((float)i/(float)Data.size());

        MeanPosition_X += Data[i].x() / Data.size();
        MeanPosition_Y += Data[i].y() / Data.size();

        j += 3;
    }

    qDebug() << MeanPosition_X << MeanPosition_Y;

    //Cam->orienter(MeanPosition_X, MeanPosition_Y);

    ReloadVertices = true;
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

    Projection = glm::perspective(30.0, (double) width / height, 1.0, 100.0);
    ModelView = glm::mat4(1.0);
}

void GL_Renderer::paintGL()
{

    float Grid[] = {-60.0, 0.0, 0.0,    60.0, 0.0, 0.0,
                    0.0, 0.0, 0.0,      0.0, 60.0, 0.0,
                    0.0, 0.0, 0.0,      0.0, -60.0, 0.0};
    float GridColor[] = {1.0, 0.0, 0.0,    1.0, 0.0, 0.0,
                        0.0, 1.0, 0.0,      0.0, 1.0, 0.0,
                        0.0, 1.0, 0.0,      0.0, 1.0, 0.0};

    Shader shaderCouleur("View/Shaders/couleur3D.vert", "View/Shaders/couleur3D.frag");
    shaderCouleur.charger();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Cam->lookAt(ModelView);

    // Activation du shader
    glUseProgram(shaderCouleur.getProgramID());

    // Envoi des vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, Vertices);
    glEnableVertexAttribArray(0);

    // Envoi des couleurs
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, Colors);
    glEnableVertexAttribArray(1);

    glUniformMatrix4fv(glGetUniformLocation(shaderCouleur.getProgramID(), "modelview"), 1, GL_FALSE, value_ptr(ModelView));
    glUniformMatrix4fv(glGetUniformLocation(shaderCouleur.getProgramID(), "projection"), 1, GL_FALSE, value_ptr(Projection));

    // Affichage du triangle
    glDrawArrays(GL_LINE_STRIP, 0, ModelSize);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    // Envoi des vertices
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, Grid);
    glEnableVertexAttribArray(2);

    // Envoi des couleurs
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, GridColor);
    glEnableVertexAttribArray(3);


    glUniformMatrix4fv(glGetUniformLocation(shaderCouleur.getProgramID(), "modelview"), 1, GL_FALSE, value_ptr(ModelView));
    glUniformMatrix4fv(glGetUniformLocation(shaderCouleur.getProgramID(), "projection"), 1, GL_FALSE, value_ptr(Projection));

    glDrawArrays(GL_LINE, 0, 6);

    // Désactivation des tableaux Vertex Attrib
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(2);

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
