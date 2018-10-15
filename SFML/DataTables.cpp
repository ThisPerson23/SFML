/**
* @file
* @author
* Justin Lange 2018
* @version 1.0
*
*
* @section DESCRIPTION
* DataTables Class
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

#include "DataTables.h"

namespace GEX
{ 
	std::map<Aircraft::Type, AircraftData> GEX::initializeAircraftData()
	{
		std::map <Aircraft::Type, AircraftData> data;

		data[Aircraft::Type::Eagle].hitpoints = 100;
		data[Aircraft::Type::Eagle].speed = 200.f;
		data[Aircraft::Type::Eagle].texture = TextureID::Eagle;
		data[Aircraft::Type::Eagle].fireInterval = sf::seconds(1);

		data[Aircraft::Type::Raptor].hitpoints = 20;
		data[Aircraft::Type::Raptor].speed = 80.f;
		data[Aircraft::Type::Raptor].texture = TextureID::Raptor;
		data[Aircraft::Type::Raptor].directions.emplace_back(Direction(45.f, 80.f));
		data[Aircraft::Type::Raptor].directions.emplace_back(Direction(-45.f, 160.f));
		data[Aircraft::Type::Raptor].directions.emplace_back(Direction(45.f, 80.f));
		data[Aircraft::Type::Raptor].fireInterval = sf::Time::Zero;

		data[Aircraft::Type::Avenger].hitpoints = 40;
		data[Aircraft::Type::Avenger].speed = 50.f;
		data[Aircraft::Type::Avenger].texture = TextureID::Avenger;
		data[Aircraft::Type::Avenger].fireInterval = sf::seconds(2);

		data[Aircraft::Type::Avenger].directions.emplace_back(Direction(45.f, 50.f));
		data[Aircraft::Type::Avenger].directions.emplace_back(Direction(0.f, 50.f));
		data[Aircraft::Type::Avenger].directions.emplace_back(Direction(-45.f, 100.f));
		data[Aircraft::Type::Avenger].directions.emplace_back(Direction(0.f, 50.f));
		data[Aircraft::Type::Avenger].directions.emplace_back(Direction(45.f, 50.f));

		return data;
	}

	std::map<Projectile::Type, ProjectileData> GEX::initializeProjectileData()
	{
		std::map <Projectile::Type, ProjectileData> data;

		data[Projectile::Type::AlliedBullet].damage = 10;
		data[Projectile::Type::AlliedBullet].speed = 300.f;
		data[Projectile::Type::AlliedBullet].texture = TextureID::Bullet;

		data[Projectile::Type::EnemyBullet].damage = 10;
		data[Projectile::Type::EnemyBullet].speed = 300.f;
		data[Projectile::Type::EnemyBullet].texture = TextureID::Bullet;

		data[Projectile::Type::Missile].damage = 200;
		data[Projectile::Type::Missile].speed = 200.f;
		data[Projectile::Type::Missile].texture = TextureID::Missile;
	}
}
