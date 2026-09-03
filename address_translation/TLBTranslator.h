#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

class TLBTranslator {
private:
    static constexpr int ADDR_SIZE{ 32 };

    int m_offsetBits;
    uint32_t m_offsetMask;
    std::unordered_map<uint32_t, uint32_t> m_translations;
public:
    TLBTranslator(int vpn_bits) : m_offsetBits{ ADDR_SIZE - vpn_bits }, m_offsetMask{ static_cast<uint32_t>(std::pow(2, ADDR_SIZE - vpn_bits) - 1) } {}

    void add_entry(uint32_t vpn, uint32_t pfn) {
        m_translations[vpn] = pfn;
    }

    uint32_t translate(uint32_t virtual_addr) const {
        uint32_t vpn{ (virtual_addr & (~m_offsetMask)) >> m_offsetBits };
        if (!m_translations.contains(vpn))
            throw std::out_of_range("TLB miss");
        return (m_translations.at(vpn) << m_offsetBits) | (virtual_addr & m_offsetMask);
    }
};
