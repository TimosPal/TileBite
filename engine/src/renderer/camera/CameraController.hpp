#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include "renderer/Camera/OrthographicCamera.hpp"

// TODO: refactor into camera interface (Usefull if we need a perspective camera later on)

namespace TileBite {

class CameraController {
public:
	// Init values shall be overriden by the window resize callback.
	CameraController()
		: m_camera(-1, 1, -1, 1), m_isDirty(false),
		m_left(-1), m_right(1), m_bottom(-1), m_top(1), zoomLevel(1.0f)
	{}

	void setPosition(const glm::vec2& position) 
	{ 
		m_isDirty = true;
		m_camera.setPosition(position);
	}
	const glm::vec2& getPosition() const { return m_camera.getPosition(); }

	float getRotation() const { return m_camera.getRotation(); }
	void setRotation(float rotation)
	{
		m_isDirty = true;
		m_camera.setRotation(rotation);
	}

	void recalculate()
	{
		if (m_isDirty)
		{
			m_camera.recalculate();
			updateFrustum();
			m_isDirty = false;
		}
	}

	void setZoom(float zoom, bool forceReapply = false) {
		if (zoomLevel != zoom || forceReapply) {
			zoomLevel = zoom;
			m_isDirty = true;

			float invZoom = 1.0f / zoomLevel;
			appliedZoomModifier = invZoom; // Store the inverse zoom modifier for later use
			float scaledLeft = m_left * invZoom;
			float scaledRight = m_right * invZoom;
			float scaledBottom = m_bottom * invZoom;
			float scaledTop = m_top * invZoom;

			m_camera.setProjection(scaledLeft, scaledRight, scaledBottom, scaledTop);
		}
	}

	void setProjection(float left, float right, float bottom, float top)
	{
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;

		setZoom(zoomLevel, true); // Reapply zoom to update projection
	}

	float getZoom() const { return zoomLevel; }

	const glm::mat4& getViewProjectionMatrix() const { return m_camera.getViewProjectionMatrix(); }

	bool isInsideFrustum(AABB collider) const {
		// TODO: consider rotation?
		return m_cameraFrustum.intersects(collider);
	}

	glm::vec2 screenToWorld(const glm::vec2& screenPos, float screenWidth, float screenHeight) const {
		// Convert screen coordinates to clip space coordinates
		glm::vec2 ndx = glm::vec2(
			(screenPos.x / screenWidth) * 2.0f - 1.0f,
			1.0f - (screenPos.y / screenHeight) * 2.0f
		);
		// Apply inverse view projection to convert to world coordinates
		glm::vec4 clipSpacePos(ndx, 0.0f, 1.0f);
		glm::vec4 worldSpacePos = m_camera.getInverseViewProjectionMatrix() * clipSpacePos;
		return glm::vec2(worldSpacePos);
	}
private:
	OrthographicCamera m_camera;
	AABB m_cameraFrustum;
	bool m_isDirty;

	float m_left, m_right, m_bottom, m_top;
	float zoomLevel = 1.0f;
	float appliedZoomModifier = 1.0f;

	void updateFrustum() {
		// Calculate the camera frustum based on the current position, zoom, and projection
		glm::vec2 min = m_camera.getPosition() + glm::vec2(m_left * appliedZoomModifier, m_bottom * appliedZoomModifier);
		glm::vec2 max = m_camera.getPosition() + glm::vec2(m_right * appliedZoomModifier, m_top * appliedZoomModifier);
		m_cameraFrustum = AABB{ min, max };
	}
};

} // TileBite

#endif // !CAMERA_CONTROLLER_HPP