/**
* @file
* @author
* Justin Lange 2018
* @version 1.0
*
*
* @section DESCRIPTION
* Projectile class
*
*
*
*
* @section LICENSE
*
*
* Copyright 2018
* Permission to use, copy, modify, and/or distribute this software for
* any purpose with or without fee is hereby granted, provided that the
* above copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
* @section Academic Integrity
* I certify that this work is solely my own and complies with
* NBCC Academic Integrity Policy (policy 1111)
*/

#include "Projectile.h"
#include "Utility.h"
#include "Category.h"
#include "DataTables.h"

namespace GEX
{ 
	namespace
	{
		const std::map<Projectile::Type, ProjectileData> TABLE = initializeProjectileData();
	}

	GEX::Projectile::Projectile(Type type, const TextureManager & textures)
		: Entity(1)
		, type_(type)
		, sprite_(textures.get(TABLE.at(type).texture))
	{
		centerOrigin(sprite_);
	}

	unsigned int GEX::Projectile::getCategory() const
	{
		if (type_ == Type::EnemyBullet)
			return Category::EnemyProjectile;
		else
			return Category::AlliedProjectile;
	}

	float GEX::Projectile::getMaxSpeed() const
	{
		return TABLE.at(type_).speed;
	}

	int GEX::Projectile::getDamage() const
	{
		return TABLE.at(type_).damage;
	}

	void GEX::Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
	{
		Entity::updateCurrent(dt, commands);
	}

	void GEX::Projectile::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(sprite_, states);
	}
}