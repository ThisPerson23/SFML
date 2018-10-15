/**
* @file
* @author
* Justin Lange 2018
* @version 1.0
*
*
* @section DESCRIPTION
* Aircraft class
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

#include "Aircraft.h"
#include "DataTables.h"
#include "Utility.h"
#include "Category.h"
#include "TextNode.h"
#include "CommandQueue.h"

#include <string>



namespace GEX
{ 
	namespace
	{
		const std::map<Aircraft::Type, AircraftData> TABLE = initializeAircraftData();
	}
	
	Aircraft::Aircraft(Aircraft::Type type, const TextureManager& textures)
		: Entity(1)
		, type_(type)
		, sprite_(textures.get(TABLE.at(type).texture))
		, healthDisplay_(nullptr)
		, missileDisplay_(nullptr)
		, travelDistance_(0.f)
		, directionIndex_(0)
		, isFiring_(false)
		, isLaunchingMissiles_(false)
		, fireRateLevel_(1)
		, fireSpreadLevel_(1)
		, missileAmmo_(10)
		, fireCountDown_(sf::Time::Zero)
		, fireCommand_()
		, launchMissileCommand_()
		, dropPickupCommand_()
	{

		centerOrigin(sprite_);

		//Set up Commands
		fireCommand_.category = Category::AirSceneLayer;
		fireCommand_.action = [this, &textures] (SceneNode& node, sf::Time dt) 
		{
			createBullets(node, textures);
		};

		launchMissileCommand_.category = Category::AirSceneLayer;
		launchMissileCommand_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			createProjectile(node, Projectile::Type::Missile, 0.f, 0.5f, textures);
		};

		dropPickupCommand_.category = Category::AirSceneLayer;
		dropPickupCommand_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			//createPicku(node, textures);
		};

		//set up text for health and missiles
		std::unique_ptr<TextNode> health(new TextNode(""));
		healthDisplay_ = health.get();
		attachChild(std::move(health));
		
	}

	unsigned int Aircraft::getCategory() const
	{
		switch (type_)
		{
		case Type::Eagle:
			return Category::PlayerAircraft;
		default:
			return Category::EnemyAircraft;
		}
	}

	void Aircraft::updateTexts()
	{
		healthDisplay_->setString(std::to_string(getHitpoints()) + "HP");
		healthDisplay_->setPosition(0.f, 50.f);
		healthDisplay_->setRotation(-getRotation());
	}

	void Aircraft::fire()
	{
		if (TABLE.at(type_).fireInterval != sf::Time::Zero)
			isFiring_ = true;
	}

	void Aircraft::launchMissile()
	{
			isLaunchingMissiles_ = true;
	}

	bool Aircraft::isAllied() const
	{
		return type_ == Type::Eagle;
	}

	void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
	{
		updateMovementPattern(dt);
		Entity::updateCurrent(dt, commands);
		updateTexts();
		checkProjectileLaunch(dt, commands);
	}

	void Aircraft::updateMovementPattern(sf::Time dt)
	{
		// movement pattern
		const std::vector<Direction>& directions = TABLE.at(type_).directions;

		if (!directions.empty())
		{
			if (travelDistance_ > directions[directionIndex_].distance)
			{
				directionIndex_ = (++directionIndex_) % directions.size();
				travelDistance_ = 0;
			}

			float radians = toRadian(directions[directionIndex_].angle + 90.f);
			float vx = getMaxSpeed() * std::cos(radians);
			float vy = getMaxSpeed() * std::sin(radians);

			setVelocity(vx, vy);
			travelDistance_ += getMaxSpeed() * dt.asSeconds();
		}
	}

	float Aircraft::getMaxSpeed() const
	{
		return TABLE.at(type_).speed;
	}

	void Aircraft::createBullets(SceneNode & node, const TextureManager & textures)
	{
		Projectile::Type type = isAllied() ? Projectile::Type::AlliedBullet : Projectile::Type::EnemyBullet;

		switch (fireSpreadLevel_)
		{
		case 1:
			createProjectile(node, type, 0.f, 0.5f, textures);
			break;
		case 2:
			createProjectile(node, type, -0.33f, 0.5f, textures);
			createProjectile(node, type, +0.33f, 0.5f, textures);
			break;
		case 3:
			createProjectile(node, type, -0.5f, 0.5f, textures);
			createProjectile(node, type, 0.f, 0.5f, textures);
			createProjectile(node, type, +0.5f, 0.5f, textures);
			break;
		}
	}

	void Aircraft::createProjectile(SceneNode & node, Projectile::Type type, float xoffset, float yoffset, const TextureManager & textures)
	{
		std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

		sf::Vector2f offset(xoffset * sprite_.getGlobalBounds().width, yoffset * sprite_.getGlobalBounds().height);
		sf::Vector2f velocity(0, projectile->getMaxSpeed());
		float sign = isAllied() ? -1.f : 1.f;

		projectile->setPosition(getWorldPosition() + offset * sign);
		projectile->setVelocity(velocity * sign);
		node.attachChild(std::move(projectile));
	}

	void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue & commands)
	{
		//enemies always fire
		if (!isAllied())
			fire();

		//Bullets
		if (isFiring_ && fireCountDown_ <= sf::Time::Zero)
		{
			commands.push(fireCommand_);
			isFiring_ = false;
			fireCountDown_ = TABLE.at(type_).fireInterval / (fireRateLevel_ + 1.f);
		}
		else if (fireCountDown_ > sf::Time::Zero)
		{
			fireCountDown_ -= dt;
		}

		//Missiles
		if (isLaunchingMissiles_)
		{ 
			if(missileAmmo_ > 0)
			{ 
				commands.push(launchMissileCommand_);
				isLaunchingMissiles_ = false;
				--missileAmmo_;
			}
		}
	}

	void Aircraft::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
	{
		target.draw(sprite_, states);
	}
}