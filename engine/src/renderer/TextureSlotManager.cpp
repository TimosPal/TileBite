#include "renderer/TextureSlotManager.hpp"

#include "utilities/assertions.hpp"

namespace Engine {

void TextureSlotManager::setNumberOfSlots(uint8_t numberOfSlots)
{
    m_slotData.resize(numberOfSlots);
}

uint8_t TextureSlotManager::getLeastUsedSlot() const
{
    ASSERT(!m_slotData.empty(), "Zero slots found");

    uint32_t minCount = std::numeric_limits<uint32_t>::max();
    uint8_t minSlot = 0;

    // NOTE: O(N) complexity, but if texture draw calls are sorted this
    // shouldnt be called many times either way (ALso array is small ~32)
    for (uint8_t i = 0; i < m_slotData.size(); i++)
    {
        if (!m_slotData[i].isEnabled)
        {
            return i;
        }
        if (m_slotData[i].Counter < minCount)
        {
            minCount = m_slotData[i].Counter;
            minSlot = i;
        }
    }

    return minSlot;
}

void TextureSlotManager::addSlot(uint8_t slot, ID textureID)
{
    ASSERT(slot < m_slotData.size(), "Slot index out of range");
    
    if (m_slotData[slot].AssignedTexture == textureID)
    {
		// Texture already assigned to this slot, no need to change anything
        // Just enable it.
        m_slotData[slot].isEnabled = true;
    }
    else
    {
        auto& oldTex = m_slotData[slot].AssignedTexture;
        m_textureIDToSlot.erase(oldTex);

        m_textureIDToSlot[textureID] = slot;

        m_slotData[slot].AssignedTexture = textureID;
        m_slotData[slot].isEnabled = true;
        m_slotData[slot].Counter = 0;
    }
}

bool TextureSlotManager::isSlotActive(uint8_t slot)
{
    return m_slotData[slot].isEnabled;
}

bool TextureSlotManager::isTextureAssigned(ID textureID)
{
    auto it = m_textureIDToSlot.find(textureID);
    return it != m_textureIDToSlot.end();
}

uint8_t TextureSlotManager::getTextureToSlotID(ID textureID)
{
    auto it = m_textureIDToSlot.find(textureID);
    if (it != m_textureIDToSlot.end())
    {
        return it->second;
    }

    ASSERT_FALSE("TextureID not found");
}


void TextureSlotManager::incrementSlotCounter(uint8_t slot)
{
	// NOTE: if this counter exceed the maximum value, it will wrap around to 0
    // which is fine because it will sort itself out. If more control is needed
    // we can reset all counters for eg, every X frames.
    ASSERT(slot < m_slotData.size(), "Slot index out of range");
    m_slotData[slot].Counter++;
}

void TextureSlotManager::reset()
{
    for (auto& slot : m_slotData) slot = SlotData{};
    m_textureIDToSlot.clear();
}

void TextureSlotManager::makeDisabled()
{
    for (auto& slot : m_slotData) slot.isEnabled = false;
}

} // Engine