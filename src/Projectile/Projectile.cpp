#include "Projectile.h"

Projectile::Projectile(float xa, float ya, float rot)
{
	this->xa = xa;
	this->ya = ya;
	this->rot = (PI / 180) * rot;

	length = 50;

	lifeTime = lifeTime * 60;
	setSpeed(speed, rot);
}

void Projectile::update(float xOffset, float yOffset) {
	xr = xa - xOffset;
	yr = ya - yOffset;

	counter++;
	if (counter > lifeTime) {
		remove();
		counter = 0;
	}
	move();
}

void Projectile::move() {
	xa += velX;
	ya += velY;
}

void Projectile::draw() {
	ofSetColor(255, 0, 0);

	float xr2 = (cosf(rot) * length) + xr;
	float yr2 = (sin(rot) * length) + yr;

	ofSetLineWidth(5);
	ofDrawLine(xr, yr, xr2, yr2);
}

float Projectile::getLength() {
	return length;
}