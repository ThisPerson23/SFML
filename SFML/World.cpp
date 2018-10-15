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
		// update the world
		worldView_.move(0.f, scrollSpeed_ * dt.asSeconds());

		playerAircraft_->setVelocity(0.f, 0.f);

		// run all the commands in the command queue
		while (!commandQueue_.isEmpty())
			sceneGraph_.onCommand(commandQueue_.pop(), dt);

		adaptPlayerVelocity();
		sceneGraph_.update(dt, getCommandQueue());
		adaptPlayerPosition();

		spawnEnemies();
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
		addEnemy(Aircraft::Type::Raptor, -250.f, 200.f);
		addEnemy(Aircraft::Type::Raptor, 0.f, 200.f);
		addEnemy(Aircraft::Type::Raptor, 250.f, 200.f);

		addEnemy(Aircraft::Type::Avenger, -70.f, 400.f);
		addEnemy(Aircraft::Type::Avenger, 70.f, 400.f);

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
			sceneLayers_[Air]->attachChild(std::move(enemy));
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

	void World::draw()
	{
		window_.setView(worldView_);
		window_.draw(sceneGraph_);
	}

	CommandQueue& World::getCommandQueue()
	{
		return commandQueue_;
	}

	void World::loadTextures()
	{
		textures_.load(GEX::TextureID::Eagle, "Media/Textures/Eagle.png");
		textures_.load(GEX::TextureID::Raptor, "Media/Textures/Raptor.png");
		textures_.load(GEX::TextureID::Landscape, "Media/Textures/Desert.png");
		textures_.load(GEX::TextureID::Avenger, "Media/Textures/Avenger.png");
		textures_.load(GEX::TextureID::Bullet, "Media/Textures/Bullet.png");
		textures_.load(GEX::TextureID::Missile, "Media/Textures/Missile.png");
	}

	void World::buildScene()
	{
		// Initialize layers
		for (int i = 0; i < LayerCount; i++)
		{
			auto category = i == Air ? Category::Type::AirSceneLayer : Category::Type::None;
			SceneNode::Ptr layer(new SceneNode(category));
			sceneLayers_.push_back(layer.get());
			sceneGraph_.attachChild(std::move(layer));
		}

		// draw background
		sf::Texture& texture = textures_.get(TextureID::Landscape);
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
		sceneLayers_[Air]->attachChild(std::move(leader));

		// add enemy planes
		addEnemies();

	}
}