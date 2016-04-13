/*
 * Copyright (c) 2016 AudioTools - All Rights Reserved
 *
 * This Software may not be distributed in parts or its entirety
 * without prior written agreement by AutioTools.
 *
 * Neither the name of the AudioTools nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY AUDIOTOOLS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AUDIOTOOLS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Written by Alexandre Arsenault <alx.arsenault@gmail.com>
 */
 
#ifndef mdiWidgetMenu_hpp
#define mdiWidgetMenu_hpp

#include <OpenAX/OpenAX.h>

#include <OpenAX/Button.h>
#include <OpenAX/ScrollBar.h>

namespace at {
namespace editor {
	class WidgetMenuSeparator : public ax::Window::Backbone {
	public:
		WidgetMenuSeparator(const ax::Rect& rect, const std::string& name);

	private:
		ax::Font _font;
		std::string _name;

		void OnMouseLeftDown(const ax::Point& pos);
		void OnMouseLeftDragging(const ax::Point& pos);
		void OnMouseLeftUp(const ax::Point& pos);
		void OnPaint(ax::GC gc);
	};

	class WidgetMenuObj : public ax::Window::Backbone {
	public:
		WidgetMenuObj(const ax::Rect& rect, const std::string& builder_name, const std::string& file_path,
			const std::string& title, const std::string& info, const std::string& size,
			const std::string& img_path);
		
		void HideText();
		
		void ShowText();
		
		void SetSelectable(bool selectable);
		
	private:
		ax::Font _font;
		ax::Font _font_normal;
		std::string _builder_name, _file_path, _title, _info, _size_str;
		std::shared_ptr<ax::Image> _img;
		bool _show_text = true;
		bool _selectable;

		void OnMouseLeftDown(const ax::Point& pos);
		void OnMouseLeftDragging(const ax::Point& pos);
		void OnMouseLeftUp(const ax::Point& pos);
		void OnPaint(ax::GC gc);
	};

	class WidgetMenu : public ax::Window::Backbone {
	public:
		WidgetMenu(const ax::Rect& rect);
		
		enum : ax::Event::Id { SMALLER_MENU };
		
		void SetOnlyMainWindowWidgetSelectable();
		void SetAllSelectable();

	private:
		ax::Window* _panel;
		ax::ScrollBar::Ptr _scrollBar;
		std::vector<std::shared_ptr<WidgetMenuObj>> _objs;
		bool _dropped_smaller = false;
		
		static const int TOP_BAR_HEIGHT = 25;
	
		axEVENT_DECLARATION(ax::Button::Msg, OnSmallerMenu);
		
		void SetSmall();
		void OnMouseEnter(const ax::Point& pos);
		void OnMouseLeave(const ax::Point& pos);
		void OnMouseEnterChild(const ax::Point& pos);
		void OnMouseLeaveChild(const ax::Point& pos);
		void OnScrollWheel(const ax::Point& delta);
		void OnResize(const ax::Size& size);
		void OnPaint(ax::GC gc);
	};
}
}

#endif /* mdiWidgetMenu_hpp */
