
#include "Intellivoice.h"

Intellivoice::Intellivoice() {}

Intellivoice::Intellivoice(AudioOutputLine* aol) {
    init(aol);
}

void Intellivoice::init(AudioOutputLine* aol) {
    hasIvoiceImage = FALSE;
    lpc12.init(aol);
    microSequencer.init(&lpc12);
}

void Intellivoice::setIntellivoiceImage(UINT16* image) {
    microSequencer.ivoiceROM.loadImage(image);
    hasIvoiceImage = TRUE;
}

BOOL Intellivoice::hasIntellivoiceImage() {
    return hasIvoiceImage;
}

