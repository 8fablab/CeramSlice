#include "Camera.h"

Camera::Camera() : m_phi(0.0), m_theta(0.0), m_orientation(), m_axeVertical(0, 0, 1), m_deplacementLateral(), m_position(), m_pointCible()
{

}

Camera::Camera(glm::vec3 position, glm::vec3 pointCible, glm::vec3 axeVertical) : m_phi(-35.26), m_theta(-135), m_orientation(), m_axeVertical(axeVertical),
                                                                                  m_deplacementLateral(), m_position(position), m_pointCible(pointCible)
{

}

void Camera::orienter(int xRel, int yRel)
{
    // Récupération des angles
    m_phi += -yRel * 0.5;
    m_theta += -xRel * 0.5;


    // Limitation de l'angle phi
    if(m_phi > 89.0)
        m_phi = 89.0;

    else if(m_phi < -89.0)
        m_phi = -89.0;


    // Conversion des angles en radian
    float phiRadian = m_phi * M_PI / 180;
    float thetaRadian = m_theta * M_PI / 180;


    // Si l'axe vertical est l'axe X
    if(m_axeVertical.x == 1.0)
    {
        // Calcul des coordonnées sphériques

        m_orientation.x = sin(phiRadian);
        m_orientation.y = cos(phiRadian) * cos(thetaRadian);
        m_orientation.z = cos(phiRadian) * sin(thetaRadian);
    }


    // Si c'est l'axe Y
    else if(m_axeVertical.y == 1.0)
    {
        // Calcul des coordonnées sphériques

        m_orientation.x = cos(phiRadian) * sin(thetaRadian);
        m_orientation.y = sin(phiRadian);
        m_orientation.z = cos(phiRadian) * cos(thetaRadian);
    }


    // Sinon c'est l'axe Z
    else
    {
        // Calcul des coordonnées sphériques

        m_orientation.x = cos(phiRadian) * cos(thetaRadian);
        m_orientation.y = cos(phiRadian) * sin(thetaRadian);
        m_orientation.z = sin(phiRadian);
    }


    // Calcul de la normale
    m_deplacementLateral = cross(m_axeVertical, m_orientation);
    m_deplacementLateral = normalize(m_deplacementLateral);

    // Calcul du point ciblé pour OpenGL
    m_pointCible = m_position + m_orientation;
}

void Camera::Move(Input const &input)
{
    // Avancée de la caméra

    if(input.getTouche(SDL_SCANCODE_UP))
    {
        m_position = m_position + m_orientation * 0.5f;
        m_pointCible = m_position + m_orientation;
    }


    // Recul de la caméra

    if(input.getTouche(SDL_SCANCODE_DOWN))
    {
        m_position = m_position - m_orientation * 0.5f;
        m_pointCible = m_position + m_orientation;
    }


    // Déplacement vers la gauche

    if(input.getTouche(SDL_SCANCODE_LEFT))
    {
        m_position = m_position + m_deplacementLateral * 0.5f;
        m_pointCible = m_position + m_orientation;
    }


    // Déplacement vers la droite

    if(input.getTouche(SDL_SCANCODE_RIGHT))
    {
        m_position = m_position - m_deplacementLateral * 0.5f;
        m_pointCible = m_position + m_orientation;
    }
}
