#pragma once
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cyc {

	class Camera2D
	{
	public:
		Camera2D();

	public:
		glm::mat4 GetOrthoProjectionMat() const;

	public:
		glm::vec2 Origin{ 0.0f, 0.0f };
		glm::vec2 Position{ 0.0f, 0.0f };
		glm::vec2 Scale{ 1.0f, 1.0f };
		float Rotation = 0.0f;
	};

}