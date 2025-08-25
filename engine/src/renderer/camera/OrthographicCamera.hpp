#ifndef ORTHOGRAPHIC_CAMERA_HPP
#define ORTHOGRAPHIC_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace TileBite {

class OrthographicCamera {
public:
	OrthographicCamera(float left, float right, float bottom, float top)
		: m_position(0.0f, 0.0f), m_rotation(0.0f),
		m_projectionMatrix(glm::ortho(left, right, bottom, top)),
		m_viewMatrix(1.0f), m_viewProjectionMatrix(m_projectionMatrix * m_viewMatrix),
		m_inverseViewProjectionMatrix(glm::inverse(m_viewProjectionMatrix))
	{}

	void setPosition(const glm::vec2& position) { m_position = position; }
	const glm::vec2& getPosition() const { return m_position; }

	float getRotation() const { return m_rotation; }
	void setRotation(float rotation) { m_rotation = rotation; }

	void setProjection(float left, float right, float bottom, float top)
	{
		m_projectionMatrix = glm::ortho(left, right, bottom, top);
	}

	// Retrieve this for rendering.
	const glm::mat4& getViewProjectionMatrix() const { return m_viewProjectionMatrix; }
	const glm::mat4& getInverseViewProjectionMatrix() const { return m_inverseViewProjectionMatrix; }

	// Recalculate if position or rotation changes.
	void recalculate()
	{
		// Recalculate the view matrix based on position and rotation
		// Applying inverse values is the same as finding T inverse.
		// (View matrix is inverse of camera transform)
		m_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-m_position, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(-m_rotation), glm::vec3(0, 0, 1));

		// Update the view projection matrix with the new view
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
		m_inverseViewProjectionMatrix = glm::inverse(m_viewProjectionMatrix);
	}

private:
	glm::vec2 m_position;
	float m_rotation;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_viewProjectionMatrix;
	glm::mat4 m_inverseViewProjectionMatrix;
};

} // TileBite

#endif // !ORTHOGRAPHIC_CAMERA_HPP