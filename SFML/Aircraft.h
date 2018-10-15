/**
* @file
* @author
* Justin Lange 2018
* @version 1.0
*
*
* @section DESCRIPTION
* Aircraft Class
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

#pragma once

#include <SFML\Graphics\Sprite.hpp>

#include "Entity.h"
#include "Command.h"
#include "ResourceIdentifiers.h"
#include "TextureManager.h"
#include "Projectile.h"
#include "TextNode.h"

namespace GEX
{
	class Aircraft : public Entity
	{
	public:
		enum class Type
		{
			Eagle,
			Raptor,
			Avenger
		};

	public:
								Aircraft(Aircraft::Type type, const TextureManager& textures);
		
		void					drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
		unsigned int			getCategory() const override;

		void					updateTexts(); //update mini health and missile display

		void					fire();
		void					launchMissile();

		bool					isAllied() const;

	protected:
		void					updateCurrent(sf::Time dt, CommandQueue& commands) override;

	private:
		void					updateMovementPattern(sf::Time dt);
		float					getMaxSpeed() const;

		void					createBullets(SceneNode& node, const TextureManager& textures);
		void					createProjectile(SceneNode& node, Projectile::Type type, 
												 float xoffset, float yoffset, const TextureManager& textures);

		void					checkProjectileLaunch(sf::Time dt, CommandQueue& commands);

	private:
		Type					type_;
		sf::Sprite				sprite_;
		TextNode*				healthDisplay_;
		TextNode*				missileDisplay_;

		float					travelDistance_;
		std::size_t				directionIndex_;

		bool					isFiring_;
		bool					isLaunchingMissiles_;

		int						fireRateLevel_;
		int						fireSpreadLevel_;
		int						missileAmmo_;

		sf::Time				fireCountDown_;

		Command					fireCommand_;
		Command					launchMissileCommand_;
		Command					dropPickupCommand_;
	};
}