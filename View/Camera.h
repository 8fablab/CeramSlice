#ifndef DEF_CAMERA
#define DEF_CAMERA


// Includes GLM

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Classe

class Camera
{
    public:

        Camera();
        Camera(glm::vec3 position, glm::vec3 pointCible, glm::vec3 axeVertical);
        ~Camera();


    private:

        float m_phi;
        float m_theta;
        glm::vec3 m_orientation;

        glm::vec3 m_axeVertical;
        glm::vec3 m_deplacementLateral;

        glm::vec3 m_position;
        glm::vec3 m_pointCible;
};

#endif
