#include "almanach.hpp"

EtatHydrant etatVanneHydrant()
{
    EtatHydrant result;
    if (digitalRead(ILS_PIN_OF) == LOW)
    {
        result = OUVERT;
        // msgConsole = "Ouvert"; // IHM
    }
    else
    {
        result = FERME;
        // msgConsole = "Fermé"; // IHM
    }
    return result;
}

float renversementHydrant(ADXL345 adxl, float prmInitialX, float prmInitialY)
{
    int x, y, z;
    adxl.readAccel(&x, &y, &z); // lecture des valeurs d'accélération

    float angleX = ((0.36) * (x - prmInitialX)); // calcule angle X
    float angleY = ((0.36) * (y - prmInitialY)); // calcule angle Y
                                                 // float angleProportion = 90 / 250; // (90 / 250) = 0.36°/LSB

    // valeur absolue
    angleX = sqrt(angleX * angleX);
    angleY = sqrt(angleY * angleY);

    float valangle = angleX;
    if ((angleX) < (angleY))
    {
        valangle = angleY;
    }
    return valangle;
}