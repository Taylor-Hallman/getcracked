#include "address_translation/TLBTranslator.h"
#include <cassert>

int main() {
    TLBTranslator tlb(20);
    tlb.add_entry(1,   5);   // VPN 0x001 → PFN 5
    tlb.add_entry(2,  10);   // VPN 0x002 → PFN 10  (0xA)
    tlb.add_entry(100, 50);  // VPN 0x064 → PFN 50  (0x32)

    // VPN=1, offset=0x000 → (5 << 12) | 0x000 = 0x5000
    assert(tlb.translate(0x00001000) == 0x5000u);
}
