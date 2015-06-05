/*
/ This class was supposed to represent a lightning bolt (the name "Flash" was used to avoid confusion with "Lighting"),
/ but the idea was scrapped because the result didn't look good. The idea was to use a pseudorandom fractal pattern to
/ create a lightning bolt at random times, and along with it, a flash of (by default) light which decays at some rate.
/ As mentioned, the lightning bolt is not included here, but the flash effect is used at various places in the project,
/ such as flashing a texture of a demon in front of the player's eyes.
*/


#ifndef FLASH_H
#define FLASH_H

#include "Constants.h"
#include "Material.h"

class Flash
{
private:

	float alpha;	// the alpha-value of the flash. Decreases over time.
	float decreaseRate;	// the rate at which the alpha-value decreases

	bool done;	// is set to true when the lightning is totally over

	Material * mat;
	float * emission;

	// most of the time, the flash will not have a texture to it, but will only be white. However,
	// sometimes we might to have an image flash in front of the viewer... creepy!
	unsigned int textureID;
public:
	Flash(unsigned int _texID = 0, bool _lightning = true, float _decreaseRate = 0.01, float _initAlpha = 1.0);
	~Flash();

	void update();

	// Draws the flash. Should be called last in the render sequence, to make the blending work properly
	void drawFlash();

	// Draws the lightning
	void drawLightning();

	// returns true if the lightning is completely over
	bool isDone();
};

#endif