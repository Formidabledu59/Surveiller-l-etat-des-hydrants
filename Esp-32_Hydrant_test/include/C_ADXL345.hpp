/**
 * Fichier Interface de la classe
 * Nom du fichier :
 * Nom de la classe :
 * Description :
 * Auteur :
 * Date :
 **/

#pragma once

class C_ADXL345
{
private:
	int x, y, z = 0;
	float angleInclianaison = 0;

public:
	C_ADXL345();
	~C_ADXL345();
};