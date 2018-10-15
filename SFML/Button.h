/**
* @file
* @author
* Justin Lange 2018
* @version 1.0
*
*
* @section DESCRIPTION
* Button Class
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

#include "Component.h"
#include "TextureManager.h"

#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Graphics\Text.hpp>

#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace GUI
{
	class Button : public Component
	{
	public:
		typedef std::shared_ptr<Button>		Ptr;
		typedef	std::function<void()>			Callback;

	public:
		Button(const sf::Font& font, const GEX::TextureManager& textures);

		void				setCallback(Callback callback);
		void				setText(const std::string& text);
		void				setToggle(bool flag);

		virtual bool		isSelectable() const;
		virtual void		select();
		virtual void		deselect();

		virtual void		activate();
		virtual void		deactivate();

		virtual void		handleEvent(const sf::Event& event);

	private:
		virtual void		draw(sf::RenderTarget& target, sf::RenderStates& states) const;

	private:
		Callback			callback_;
		const sf::Texture&	normalTexture_;
		const sf::Texture&	selectedTexture_;
		const sf::Texture&	pressedTexture_;
		sf::Sprite			sprite_;
		sf::Text			text_;
		bool				isToggle_;
	};
}
