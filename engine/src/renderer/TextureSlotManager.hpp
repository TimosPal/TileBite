#ifndef TEXTURE_SLOT_MANAGER_HPP
#define TEXTURE_SLOT_MANAGER_HPP

#include "core/pch.hpp"
#include "utilities/IDGenerator.hpp"

namespace Engine {

struct SlotData {
	uint32_t Counter = 0;
	ID AssignedTexture = 0;
	bool isEnabled = false;
};

class TextureSlotManager {
public:
	void setNumberOfSlots(uint8_t numberOfSlots);
	void reset();
	void makeDisabled();
	
	void addSlot(uint8_t slot, ID textureID);
	uint8_t getLeastUsedSlot() const;

	bool isSlotActive(uint8_t slot);
	bool isTextureAssigned(ID textureID);
	uint8_t getTextureToSlotID(ID textureID, bool& isAssigned);

	uint8_t getNumberOfSlots() const { return m_slotData.size(); }
	void incrementSlotCounter(uint8_t slot);
private:

	std::vector<SlotData> m_slotData;
	std::unordered_map<ID, uint8_t> m_textureIDToSlot;
};

} // Engine

#endif // !TEXTURE_SLOT_MANAGER_HPP