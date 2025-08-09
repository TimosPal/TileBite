#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include "renderer/Camera/OrthographicCamera.hpp"

// TODO: refactor into camera interface (Usefull if we need a perspective camera later on)

namespace Engine {

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
			m_isDirty = false;
		}
	}

	void setZoom(float zoom, bool forceReapply = false) {
		if (zoomLevel != zoom || forceReapply) {
			zoomLevel = zoom;
			m_isDirty = true;

			float invZoom = 1.0f / zoomLevel;
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
private:
	OrthographicCamera m_camera;
	bool m_isDirty;

	float m_left, m_right, m_bottom, m_top;
	float zoomLevel = 1.0f;
};

} // Engine

#endif // !CAMERA_CONTROLLER_HPP