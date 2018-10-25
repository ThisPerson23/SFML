/**
* @file
* @author
* Justin Lange 2018
* @version 1.0
*
*
* @section DESCRIPTION
* World Class
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

#include "World.h"
#include "Aircraft.h"
#include "Pickup.h"
#include "Projectile.h"
#include "ParticleNode.h"

namespace GEX
{ 
	World::World(sf::RenderWindow& window)
	: window_(window)
	, worldView_(window.getView())
	, textures_()
	, sceneGraph_()
	, sceneLayers_()
	, worldBounds_(0.f, 0.f, worldView_.getSize().x, 5000.f)
	, spawnPosition_(worldView_.getSize().x / 2.f, worldBounds_.height - worldView_.getSize().y / 2.f)
	, scrollSpeed_(-50.f)
	, playerAircraft_(nullptr)
	{
		loadTextures();
		buildScene();

		//prepare the view
		worldView_.setCenter(spawnPosition_);
	}

	void World::update(sf::Time dt, CommandQueue& commands)
	{
		// Scroll screen and reset player velocity
		worldView_.move(0.f, scrollSpeed_ * dt.asSeconds());
		playerAircraft_->setVelocity(0.f, 0.f);

		// Destroy all entities that leave the battlefield
		destroyEntitiesOutOfView();

		// Guide missiles
		guideMissiles();

		// Run all the commands in the command queue
		while (!commandQueue_.isEmpty())
		{ 
			sceneGraph_.onCommand(commandQueue_.pop(), dt);
		}
		adaptPlayerVelocity();

		// Handle collisions
		handleCollision();

		// Destroy all wrecks on the battlefield
		sceneGraph_.removeWrecks();

		// Spawn enemies
		spawnEnemies();

		// Regular update step, and adapt position of aircraft
		sceneGraph_.update(dt, getCommandQueue());
		adaptPlayerPosition();

		
	}

	void World::adaptPlayerVelocity()
	{
		sf::Vector2f velocity = playerAircraft_->getVelocity();

		if (velocity.x != 0.f && velocity.y != 0.f)
			playerAircraft_->setVelocity(velocity / std::sqrt(2.f));
	}

	void World::adaptPlayerPosition()
	{
		const float BORDER_DISTANCE = 40.f;
		sf::FloatRect viewBounds(worldView_.getCenter() - worldView_.getSize() / 2.f, worldView_.getSize());

		sf::Vector2f position = playerAircraft_->getPosition();
		position.x = std::max(position.x, viewBounds.left + BORDER_DISTANCE);
		position.x = std::min(position.x, viewBounds.left + viewBounds.width - BORDER_DISTANCE);

		position.y = std::max(position.y, viewBounds.top + BORDER_DISTANCE);
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - BORDER_DISTANCE);

		playerAircraft_->setPosition(position);
	}

		// Add spawn points for enemies outside the local view
	void World::addEnemies()
	{
		addEnemy(Aircraft::Type::Raptor, 0.f, 500.f);
		addEnemy(Aircraft::Type::Raptor, 0.f, 1000.f);
		addEnemy(Aircraft::Type::Raptor, +100.f, 1150.f);
		addEnemy(Aircraft::Type::Raptor, -100.f, 1150.f);
		addEnemy(Aircraft::Type::Avenger, 70.f, 1500.f);
		addEnemy(Aircraft::Type::Avenger, -70.f, 1500.f);
		addEnemy(Aircraft::Type::Avenger, -70.f, 1710.f);
		addEnemy(Aircraft::Type::Avenger, 70.f, 1700.f);
		addEnemy(Aircraft::Type::Avenger, 30.f, 1850.f);
		addEnemy(Aircraft::Type::Raptor, 300.f, 2200.f);
		addEnemy(Aircraft::Type::Raptor, -300.f, 2200.f);
		addEnemy(Aircraft::Type::Raptor, 0.f, 2200.f);
		addEnemy(Aircraft::Type::Raptor, 0.f, 2500.f);
		addEnemy(Aircraft::Type::Avenger, -300.f, 2700.f);
		addEnemy(Aircraft::Type::Avenger, -300.f, 2700.f);
		addEnemy(Aircraft::Type::Raptor, 0.f, 3000.f);
		addEnemy(Aircraft::Type::Raptor, 250.f, 3250.f);
		addEnemy(Aircraft::Type::Raptor, -250.f, 3250.f);
		addEnemy(Aircraft::Type::Avenger, 0.f, 3500.f);
		addEnemy(Aircraft::Type::Avenger, 0.f, 3700.f);
		addEnemy(Aircraft::Type::Raptor, 0.f, 3800.f);
		addEnemy(Aircraft::Type::Avenger, 0.f, 4000.f);
		addEnemy(Aircraft::Type::Avenger, -200.f, 4200.f);
		addEnemy(Aircraft::Type::Raptor, 200.f, 4200.f);
		addEnemy(Aircraft::Type::Raptor, 0.f, 4400.f);

		std::sort(enemySpawnPoints_.begin(), enemySpawnPoints_.end(), 
			[](Spawnpoint lhs, Spawnpoint rhs)
		{
			return lhs.y < rhs.y;
		});
	}

	void World::addEnemy(Aircraft::Type type, float relX, float relY)
	{
		Spawnpoint spawnpoint(type, spawnPosition_.x + relX, spawnPosition_.y - relY);
		enemySpawnPoints_.push_back(spawnpoint);
	}

	void World::spawnEnemies()
	{
		while (!enemySpawnPoints_.empty() && enemySpawnPoints_.back().y > getBattlefieldBounds().top)
		{
			auto spawnpoint = enemySpawnPoints_.back();
			std::unique_ptr<Aircraft> enemy(new Aircraft(spawnpoint.type, textures_));
			enemy->setPosition(spawnpoint.x, spawnpoint.y);
			enemy->setRotation(180);
			sceneLayers_[UpperAir]->attachChild(std::move(enemy));
			enemySpawnPoints_.pop_back();
		}
	}

	sf::FloatRect World::getViewBounds() const
	{
		return sf::FloatRect(worldView_.getCenter() - worldView_.getSize() / 2.f, worldView_.getSize());
	}

	sf::FloatRect World::getBattlefieldBounds() const
	{
		sf::FloatRect bounds = getViewBounds();
		bounds.top -= 100.f;
		bounds.height += 100.f;
		return bounds;
	}

	void World::guideMissiles()
	{
		// build a list of active Enemies
		Command enemyCollector;
		enemyCollector.category = Category::EnemyAircraft;
		enemyCollector.action = derivedAction<Aircraft>([this](Aircraft& enemy, sf::Time dt)
		{
			if (!enemy.isDestroyed())
				activeEnemies_.push_back(&enemy);
		});

		Command missileGuider;
		missileGuider.category = Category::Type::AlliedProjectile;
		missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time dt)
		{
			//ignore bullets
			if (!missile.isGuided())
				return;

			float minDistance = std::numeric_limits<float>::max();
			Aircraft* closestEnemy = nullptr;

			for (Aircraft* e : activeEnemies_)
			{
				auto d = distance(missile, *e);
				if (d < minDistance)
				{
					minDistance = d;
					closestEnemy = e;
				}
			}

			if (closestEnemy)
				missile.guidedTowards(closestEnemy->getWorldPosition());
		});

		commandQueue_.push(enemyCollector);
		commandQueue_.push(missileGuider);
		activeEnemies_.clear();
	}

	bool matchesCategory(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
	{
		unsigned int category1 = colliders.first->getCategory();
		unsigned int category2 = colliders.second->getCategory();

		// Make sure pair entry has category type1 and second has type2
		if (type1 & category1 && type2 & category2)
		{
			return true;
		}
		else if (type1 & category2 && type2 & category1)
		{
			std::swap(colliders.first, colliders.second);
			return true;
		}
		else
		{
			return false;
		}
	}

	void World::handleCollision()
	{
		//build a list of colliding pairs of SceneNodes
		std::set<SceneNode::Pair> collisionPairs;
		sceneGraph_.checkSceneCollision(sceneGraph_, collisionPairs);

		for (SceneNode::Pair pair : collisionPairs)
		{
			if (matchesCategory(pair, Category::Type::PlayerAircraft, Category::Type::EnemyAircraft))
			{
				auto& player = static_cast<Aircraft&>(*pair.first);
				auto& enemy = static_cast<Aircraft&>(*pair.second);

				player.damage(enemy.getHitpoints());
				enemy.destroy();
			}
			else if (matchesCategory(pair, Category::Type::PlayerAircraft, Category::Type::Pickup))
			{
				auto& player = static_cast<Aircraft&>(*pair.first);
				auto& pickup = static_cast<Pickup&>(*pair.second);

				pickup.apply(player);
				pickup.destroy();
			}
			else if (matchesCategory(pair, Category::Type::PlayerAircraft, Category::Type::EnemyProjectile) ||
				     matchesCategory(pair, Category::Type::EnemyAircraft, Category::Type::AlliedProjectile))
			{
				auto& aircraft = static_cast<Aircraft&>(*pair.first);
				auto& projectile = static_cast<Projectile&>(*pair.second);

				aircraft.damage(projectile.getDamage());
				projectile.destroy();
			}
		}
	}

	void World::destroyEntitiesOutOfView()
	{
		Command command;
		command.category = Category::Type::Projectile | Category::Type::EnemyAircraft;
		command.action = derivedAction<Entity>([this](Entity& e, sf::Time dt)
		{
			if (!getBattlefieldBounds().intersects(e.getBoundingBox()))
				e.remove();
		});

		commandQueue_.push(command);
	}

	void World::draw()
	{
		window_.setView(worldView_);
		window_.draw(sceneGraph_);
	}

	CommandQueue& World::getCommandQueue()
	{
		return commandQueue_;
	}

	bool World::hasAlivePlayer() const
	{
		return playerAircraft_->isMarkedForRemoval();
	}

	bool World::hasPlayerReachedEnd() const
	{
		return !worldBounds_.contains(playerAircraft_->getPosition());
	}

	void World::loadTextures()
	{
		textures_.load(GEX::TextureID::Entities, "Media/Textures/Entities.png");
		textures_.load(GEX::TextureID::Jungle, "Media/Textures/JungleBig.png");
		textures_.load(GEX::TextureID::Particle, "Media/Textures/Particle.png");
		textures_.load(GEX::TextureID::Explosion, "Media/Textures/Explosion.png");
		textures_.load(GEX::TextureID::FinishLine, "Media/Textures/FinishLine.png");
	}

	void World::buildScene()
	{
		// Initialize layers
		for (int i = 0; i < LayerCount; i++)
		{
			auto category = i == UpperAir ? Category::Type::SceneAirLayer : Category::Type::None;
			SceneNode::Ptr layer(new SceneNode(category));
			sceneLayers_.push_back(layer.get());
			sceneGraph_.attachChild(std::move(layer));
		}

		// Particle System
		std::unique_ptr<ParticleNode> smoke(new ParticleNode(Particle::Type::Smoke, textures_));
		sceneLayers_[LowerAir]->attachChild(std::move(smoke));

		std::unique_ptr<ParticleNode> fire(new ParticleNode(Particle::Type::Propellant, textures_));
		sceneLayers_[LowerAir]->attachChild(std::move(fire));

		// draw background
		sf::Texture& texture = textures_.get(TextureID::Jungle);
		sf::IntRect textureRect(worldBounds_);
		texture.setRepeated(true);

		std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
		backgroundSprite->setPosition(worldBounds_.left, worldBounds_.top);
		sceneLayers_[Background]->attachChild(std::move(backgroundSprite));

		// add player aircraft & game objects
			//player
		std::unique_ptr<Aircraft> leader(new Aircraft(Aircraft::Type::Eagle, textures_));
		leader->setPosition(spawnPosition_);
		leader->setVelocity(50.f, scrollSpeed_);
		playerAircraft_ = leader.get();
		sceneLayers_[UpperAir]->attachChild(std::move(leader));

		// add enemy planes
		addEnemies();
	}
}