#include "Camera2D.h"

#include "Cyclopes/Core/Window.h"
#include "Cyclopes/Core/Assert.h"

namespace cyc {

	Camera2D::Camera2D()
	{
	}

	glm::mat4 Camera2D::GetOrthoProjectionMat() const
	{
		glm::mat4 orthoMat = glm::ortho(0.0f, (float)Display::GetWidth(), (float)Display::GetHeight(), 0.0f);
		return orthoMat;
	}
}
